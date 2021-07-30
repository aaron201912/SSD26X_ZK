#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

#include "mi_sys.h"
#include "mi_ai.h"
#include "AudioProcess.h"
#include "list.h"
#include "base_types.h"
#include "appconfig.h"
#include "kwsdetect.h"
#include <dlfcn.h>

#include "detect_paras.h"
#include "sstar_kws.h"
#include "sstar_impl.h"

#define APC_POINT_NUM 128
#define AI_PT_NUM_PER_FRM (APC_POINT_NUM * 8)
static ANR_HANDLE g_AnrHandle;
static AGC_HANDLE g_AgcHandle;
static char *g_AnrWorkingBuffer = NULL;
static char *g_AgcWorkingBuffer = NULL;

static sem_t g_AudioCheckSem;//信号量
static UsrData_t* g_pstUsrData = NULL;

static ThreadData_t g_stRecogThreadData;
static ThreadData_t g_stAudioInThreadData;
static void *_kws_VoiceAnalyzeProc_Once(void *pData);

// audio in thread
static void *_kws_AudioInGetDataProc_(void *pData)
{
    MI_AUDIO_DEV AiDevId = AI_DEV_ID;
    MI_AI_CHN AiChn = 0;
    MI_AUDIO_Frame_t stAudioFrame;
	MI_AUDIO_AecFrame_t stAecFrame;
    MI_S32 s32ToTalSize = 0;
    MI_S32 s32Ret = 0;
    int nCmdIdx = -1;
    FILE *pFile = NULL;
    char szFileName[64] = {0,};
    unsigned int anr_workingBufferSize;
	unsigned int agc_workingBufferSize;
    UsrData_t *pUsrData = g_pstUsrData;
    AudioProcessInit anr_init;
    AudioAnrConfig anr_config;
    AudioProcessInit agc_init;
    AudioAgcConfig agc_config;

    int anr_intensity_band[6] = {3,24,40,64,80,128};
	int anr_intensity[7] = {30,30,30,30,30,30,30};
	short agc_compression_ratio_input[7] = {-65,-55,-48,-25,-18,-12,0};
	short agc_compression_ratio_output[7] = {-65,-50,-27,-12,-1,-1,-1};

	anr_init.point_number = APC_POINT_NUM;
	anr_init.channel = 1;
	anr_init.sample_rate = IAA_APC_SAMPLE_RATE_16000;
	anr_config.anr_enable = 1;
	anr_config.user_mode = 2;
	anr_config.anr_smooth_level = 10;
	anr_config.anr_converge_speed = NR_SPEED_LOW;
	memcpy(anr_config.anr_intensity_band, anr_intensity_band, sizeof(anr_intensity_band));
	memcpy(anr_config.anr_intensity, anr_intensity, sizeof(anr_intensity));

	anr_workingBufferSize = IaaAnr_GetBufferSize();
	g_AnrWorkingBuffer = (char *)malloc(anr_workingBufferSize);
	if(NULL == g_AnrWorkingBuffer)
	{
		printf("malloc g_AnrWorkingBuffer failed !\n");
		return NULL;
	}
	printf("malloc g_AnrWorkingBuffer succeed !\n");
	g_AnrHandle = IaaAnr_Init(g_AnrWorkingBuffer, &anr_init);
	if(NULL == g_AnrHandle)
	{
		printf("IaaAnr_Init failed !\n");
		return NULL;
	}
	printf("IaaAnr_Init succeed !\n");
	s32Ret = IaaAnr_Config(g_AnrHandle, &anr_config);
	if(s32Ret)
	{
		printf("IaaAnr_Config failed !\n");
		return NULL;
	}
	printf("IaaAnr_Config succeed !\n");

	agc_init.point_number = APC_POINT_NUM;
	agc_init.channel = 1;
	agc_init.sample_rate = IAA_APC_SAMPLE_RATE_16000;
	agc_config.agc_enable = 1;
	agc_config.user_mode = 1;
	agc_config.gain_info.gain_max  = 40;
	agc_config.gain_info.gain_min  = -10;
	agc_config.gain_info.gain_init = 12;
	agc_config.drop_gain_max = 36;
	agc_config.attack_time = 1;
	agc_config.release_time = 1;
	agc_config.noise_gate_db = -80;
	agc_config.noise_gate_attenuation_db = 0;
	agc_config.drop_gain_threshold = -5;
	agc_config.gain_step = 1;
	memcpy(agc_config.compression_ratio_input, agc_compression_ratio_input, sizeof(agc_compression_ratio_input));
	memcpy(agc_config.compression_ratio_output, agc_compression_ratio_output, sizeof(agc_compression_ratio_output));

	agc_workingBufferSize = IaaAgc_GetBufferSize();
	g_AgcWorkingBuffer = (char *)malloc(agc_workingBufferSize);
	if(NULL == g_AgcWorkingBuffer)
	{
	   printf("malloc g_AgcWorkingBuffer failed !\n");
	   return NULL;
	}
	printf("malloc g_AgcWorkingBuffer succeed !\n");
	g_AgcHandle = IaaAgc_Init(g_AgcWorkingBuffer, &agc_init);
	if(NULL == g_AgcHandle)
	{
	   printf("IaaAgc_Init failed !\n");
	   return NULL;
	}
	printf("IaaAgc_Init succeed !\n");
	s32Ret = IaaAgc_Config(g_AgcHandle, &agc_config);
	if(s32Ret)
	{
	   printf("IaaAgc_Config failed !\n");
	   return NULL;
	}
	printf("IaaAgc_Config succeed !\n");

	memset(&stAudioFrame, 0, sizeof(MI_AUDIO_Frame_t));
    memset(&stAecFrame, 0, sizeof(MI_AUDIO_AecFrame_t));

    ST_DBG("pid=%ld\n", syscall(SYS_gettid));

    MI_SYS_ChnPort_t stChnPort;
    MI_S32 s32Fd = -1;
    fd_set read_fds;
    struct timeval TimeoutVal;

    memset(&stChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = E_MI_MODULE_ID_AI;
    stChnPort.u32DevId = AiDevId;
    stChnPort.u32ChnId = AiChn;
    stChnPort.u32PortId = 0;
    s32Ret = MI_SYS_GetFd(&stChnPort, &s32Fd);

    if(MI_SUCCESS != s32Ret)
    {
        ST_ERR("MI_SYS_GetFd err:%x, chn:%d\n", s32Ret, AiChn);
        return NULL;
    }

    while(!g_stAudioInThreadData.bExit)
    {
        FD_ZERO(&read_fds);
        FD_SET(s32Fd, &read_fds);

        TimeoutVal.tv_sec  = 1;
        TimeoutVal.tv_usec = 0;
        s32Ret = select(s32Fd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if(s32Ret < 0)
        {
            ST_ERR("select failed!\n");
            //  usleep(10 * 1000);
            continue;
        }
        else if(s32Ret == 0)
        {
            ST_ERR("get audio in frame time out\n");
            //usleep(10 * 1000);
            continue;
        }
        else
        {
            if(FD_ISSET(s32Fd, &read_fds))
            {
                MI_AI_GetFrame(AiDevId, AiChn, &stAudioFrame, &stAecFrame, AUDIO_SAMPLE_PER_FRAME*1000/E_MI_AUDIO_SAMPLE_RATE_8000);//1024 / 8000 = 128ms
                if (0 == stAudioFrame.u32Len)
                {
                    usleep(1 * 1000);
                    continue;
                }

                //PutVoiceFrameToQueue(&stAudioFrame);		// save

                //printf("=======> audio len:%d\n", stAudioFrame.u32Len[0]);
                for(MI_U8 i=0; i<AI_PT_NUM_PER_FRM/APC_POINT_NUM; i++)
				{
					s32Ret = IaaAnr_Run(g_AnrHandle, (short *)stAudioFrame.apVirAddr[0] + i * sizeof(short) * APC_POINT_NUM);
					if(s32Ret)
					{
						printf("IaaAnr_Run failed !\n");
						return NULL;
					}

					/*close AGC*/
//					s32Ret = IaaAgc_Run(g_AgcHandle, (short *)stAudioFrame.apVirAddr[0] + i * sizeof(short) * APC_POINT_NUM);
//					if(s32Ret)
//					{
//						printf("IaaAgc_Run failed !\n");
//						return NULL;
//					}
				}
                kws_senddata((short int *)stAudioFrame.apVirAddr[0], stAudioFrame.u32Len[0]);
                sem_post(&g_AudioCheckSem);
                //pthread_create(&g_stRecogThreadData.pt, NULL, _kws_VoiceAnalyzeProc_Once, (void*)g_pstUsrData);
#if 0
                nCmdIdx = -1;
                kws_predict(&nCmdIdx);
                if(nCmdIdx != -1)
                {
                	printf("=========> nCmdIdx:%d\n", nCmdIdx);
                	pUsrData->pfnCallback(E_TRIGGER_CMD, nCmdIdx);
                }
#endif

                MI_AI_ReleaseFrame(AiDevId,  AiChn, &stAudioFrame, &stAecFrame);
            }
        }
    }

    return NULL;
}


// aidev opt
static MI_S32 kws_AudioInStart()
{
    MI_S32 s32Ret = MI_SUCCESS, i;

    //Ai
    MI_AUDIO_DEV AiDevId = AI_DEV_ID;
    MI_AI_CHN AiChn = 0;
    MI_AUDIO_Attr_t stAiSetAttr;
    MI_SYS_ChnPort_t stAiChn0OutputPort0;
    MI_AI_ChnParam_t stAiChnParam;

    s32Ret = sem_init(&g_AudioCheckSem, 0, 0);
	if(s32Ret == -1)
	{
		perror("semaphore intitialization failed\n");
		return -1;
	}

    //set ai attr
    memset(&stAiSetAttr, 0, sizeof(MI_AUDIO_Attr_t));
    stAiSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stAiSetAttr.eSamplerate = AUDIO_SAMPLE_RATE;
    stAiSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
    stAiSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stAiSetAttr.u32ChnCnt = 1;
    stAiSetAttr.u32PtNumPerFrm = (int)stAiSetAttr.eSamplerate/5;//(int)stAiSetAttr.eSamplerate / (int)E_MI_AUDIO_SAMPLE_RATE_8000 * AUDIO_SAMPLE_PER_FRAME;
	stAiSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
	stAiSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;
	stAiSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = 1;
	stAiSetAttr.WorkModeSetting.stI2sConfig.u32TdmSlots = 0;
	stAiSetAttr.WorkModeSetting.stI2sConfig.eI2sBitWidth = E_MI_AUDIO_BIT_WIDTH_32;
	ExecFunc(MI_AI_SetPubAttr(AiDevId, &stAiSetAttr), MI_SUCCESS);
    ExecFunc(MI_AI_Enable(AiDevId), MI_SUCCESS);
	
    //set ai output port depth
    memset(&stAiChn0OutputPort0, 0, sizeof(MI_SYS_ChnPort_t));
    stAiChn0OutputPort0.eModId = E_MI_MODULE_ID_AI;
    stAiChn0OutputPort0.u32DevId = AiDevId;
    stAiChn0OutputPort0.u32ChnId = AiChn;
    stAiChn0OutputPort0.u32PortId = 0;

	ExecFunc(MI_SYS_SetChnOutputPortDepth(0, &stAiChn0OutputPort0, 4, 8), MI_SUCCESS);
	
    memset(&stAiChnParam, 0x0, sizeof(MI_AI_ChnParam_t));
	stAiChnParam.stChnGain.bEnableGainSet = TRUE;
	stAiChnParam.stChnGain.s16FrontGain = 15;
	stAiChnParam.stChnGain.s16RearGain = 0;
	ExecFunc(MI_AI_SetChnParam(AiDevId, AiChn, &stAiChnParam), MI_SUCCESS);
    ExecFunc(MI_AI_EnableChn(AiDevId, AiChn), MI_SUCCESS);

    g_stAudioInThreadData.bExit = false;
    s32Ret = pthread_create(&g_stAudioInThreadData.pt, NULL, _kws_AudioInGetDataProc_, (void*)g_pstUsrData);
    if(0 != s32Ret)
    {
         ST_ERR("create thread failed\n");
         return -1;
    }

    return MI_SUCCESS;
}

static MI_S32 kws_AudioInStop()
{
    MI_AUDIO_DEV AiDevId = AI_DEV_ID;
    MI_AI_CHN AiChn = 0;

    g_stAudioInThreadData.bExit = true;
    pthread_join(g_stAudioInThreadData.pt, NULL);
    IaaAnr_Free(g_AnrHandle);
	free(g_AnrWorkingBuffer);
	IaaAgc_Free(g_AgcHandle);
	free(g_AgcWorkingBuffer);

    ExecFunc(MI_AI_DisableChn(AiDevId, AiChn), MI_SUCCESS);
    ExecFunc(MI_AI_Disable(AiDevId), MI_SUCCESS);
    ExecFunc(MI_AI_DeInitDev(),MI_SUCCESS);

    sem_destroy(&g_AudioCheckSem);

    return MI_SUCCESS;
}


int kws_VoiceDetectDeinit()
{
	kws_free();
	return 0;
}

// init时会获取唤醒词条列表和指令词条列表，不再需要预先设置词条列表数组
int kws_VoiceDetectInit()
{
	int nErr = 0;
    float result[256];
    const char **word;
    int wordnum;
    int s32Ret;
    KWSInit_s kwsInit;
    int spotting = 0,skip = 0;
    int index = -1, count = 0;
	char modelfileName[512];
    float threshold[] = {
            20.0, //小慧小慧
            17.0, //打开空调
            17.0, //关闭空调
            20.0, //打开灯光
            20.0, //关闭灯光
            20.0, //打开灯带
            20.0, //关闭灯带
            20.0, //调高温度
            20.0, //调低温度
            16.0, //打开新风
            20.0, //关闭新风
            20.0, //打开地暖
            20.0, //关闭地暖
            20.0, //打开窗帘
            20.0, //关闭窗帘
            20.0, //停止窗帘
            20.0, //打开射灯
            20.0, //关闭射灯
            30.0, //打开筒灯
            30.0, //关闭筒灯
            14.0, //我出去了
            14.0, //我回来了
            14.0, //我起床了
            15.0, //客人来了
            15.0, //我要吃饭
            15.0, //我要看书
            25.0, //解除警报
			15.0, //我睡觉了
			20.0, //把门打开
			20.0, //接听对讲
			20.0  //关闭对讲
	};

	kwsInit.sample_rate = 16000;
	kwsInit.chunk_len = (32*16000*2)/1000;    //byte, 256ms
	kwsInit.nn_type = 0;
    memcpy(kwsInit.threshold, threshold, sizeof(float)*KWS_OUT_DIM);

    sprintf(modelfileName,"%s", "kws.model");
    s32Ret = kws_init(&kwsInit, modelfileName);
    if (s32Ret < 0)
    {
    	printf("kws init ret %d\n", s32Ret);
    	return s32Ret;
    }

    word = kws_getcmd(&wordnum);
    if (wordnum < 0)
    {
        printf("kws_getcmd ret %d \n", wordnum);
        return wordnum;
    }
    else
    {
        printf("Kws Init Successfully, Support word(%d):", wordnum);
        for(int i=0; i<wordnum; i++)
            printf(" %s", word[i]);
        printf("\n");
    }
	return 0;

exit_init:

	return -1;
}

static void *_kws_VoiceAnalyzeProc_Once(void *pData)
{
	UsrData_t *pUsrData = (UsrData_t*)pData;
	int nCmdIdx;

    AD_LOG("Enter _kws_VoiceAnalyzeProc_\n");

	nCmdIdx = -1;
	kws_predict(&nCmdIdx);
	if(nCmdIdx != -1)
	{
		pUsrData->pfnCallback(E_COMMON_CMD, nCmdIdx);
	}

    return NULL;
}

static void *_kws_VoiceAnalyzeProc(void *pData)
{
	UsrData_t *pUsrData = (UsrData_t*)pData;
    int ret = 0;
	int nCmdIdx, nWordDura, nEndSilDura, nNetworkLatency;
	int nGMM, nSG, nFIL;
	unsigned char pbyResult[COMMAND_LEN];
	CommandMode_e eMode = MODE_TRIGGER_WORD;
	struct timespec recogTime, curTime;
	static int lastCmdIdx = -1;

    AD_LOG("Enter _kws_VoiceAnalyzeProc_\n");

    while (!g_stRecogThreadData.bExit)
    {
    	sem_wait(&g_AudioCheckSem);
    	sem_wait(&g_AudioCheckSem);
        kws_predict(&nCmdIdx);

        if (nCmdIdx == -1)
        {
            //usleep(1000*10);
            continue;
        }
        if(lastCmdIdx != nCmdIdx)
        {
        	lastCmdIdx = nCmdIdx;
        	pUsrData->pfnCallback(E_COMMON_CMD, nCmdIdx);
        }
#if 0
        if(eMode == MODE_COMMAND)
		{
			unsigned long long timeInterval = 0;
			memset(&curTime, 0, sizeof(curTime));
			clock_gettime(CLOCK_MONOTONIC, &curTime);
			timeInterval = (unsigned long long)((curTime.tv_sec - recogTime.tv_sec) * 1000)
						   + (unsigned long long)((curTime.tv_nsec - recogTime.tv_nsec) / 1000000);
//			printf("curTime: %ld s, %ld ns\n", curTime.tv_sec, curTime.tv_nsec);
//			printf("recogTime: %ld s, %ld ns\n", recogTime.tv_sec, recogTime.tv_nsec);
//			printf("tmInterval:%lld, timeout:%d\n", timeInterval, g_configSetting.timeout);

			if (timeInterval >=  g_configSetting.timeout)
			{
				printf("Timeout!\n");
				eMode = MODE_TRIGGER_WORD;
				memset(&recogTime, 0, sizeof(recogTime));
				clock_gettime(CLOCK_MONOTONIC, &recogTime);
				pUsrData->pfnCallback(E_TRIGGER_CMD, -1);	// timeout
			}
		}

		if(eMode == MODE_TRIGGER_WORD)
		{
				eMode = MODE_COMMAND;
				memset(&recogTime, 0, sizeof(recogTime));
				clock_gettime(CLOCK_MONOTONIC, &recogTime);
		}
		else
		{
			printf("Get command!\n");

			pUsrData->pfnCallback(E_COMMON_CMD, nCmdIdx);

			memset(&recogTime, 0, sizeof(recogTime));
			clock_gettime(CLOCK_MONOTONIC, &recogTime);
		}
#endif
    }


    return NULL;
}

int kws_VoiceDetectStart(VoiceAnalyzeCallback pfnCallback)
{
	g_pstUsrData = (UsrData_t *)malloc(sizeof(UsrData_t));
	if (!g_pstUsrData)
	{
		printf("Error: g_pstUsrData has not inited\n");
		return -1;
	}
	g_pstUsrData->pfnCallback = pfnCallback;

	if (MI_SUCCESS != kws_AudioInStart())
	{
		printf("aidev init failed\n");
		return -1;
	}

	g_stRecogThreadData.bExit = false;
	pthread_create(&g_stRecogThreadData.pt, NULL, _kws_VoiceAnalyzeProc, (void*)g_pstUsrData);

	return 0;
}

void kws_VoiceDetectStop()
{
	if (!g_stRecogThreadData.bExit)
	{
		g_stRecogThreadData.bExit = true;
		pthread_join(g_stRecogThreadData.pt, NULL);
	}

	kws_AudioInStop();
}

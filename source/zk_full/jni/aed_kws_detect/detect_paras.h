/*
 * audio_paras.h
 *
 *  Created on: 2021年1月21日
 *      Author: shaojun.ke
 */

#ifndef JNI_AED_KWS_DETECT_DETECT_PARAS_H_
#define JNI_AED_KWS_DETECT_DETECT_PARAS_H_

#define AUDIO_SAMPLE_PER_FRAME 		(256)
#define AUDIO_SAMPLE_RATE 			(E_MI_AUDIO_SAMPLE_RATE_16000)
#define AUDIO_SOUND_MODE 			(E_MI_AUDIO_SOUND_MODE_MONO)

#define AUDIO_AO_DEV_ID_LINE_OUT 	0
#define AUDIO_AO_DEV_ID_I2S_OUT  	1

#define AUDIO_AI_DEV_ID_AMIC_IN   	0
#define AUDIO_AI_DEV_ID_DMIC_IN   	1
#define AUDIO_AI_DEV_ID_I2S_IN    	2

#if USE_AMIC
#define AI_DEV_ID (AUDIO_AI_DEV_ID_AMIC_IN)
#else
#define AI_DEV_ID (AUDIO_AI_DEV_ID_DMIC_IN)
#endif
#define AO_DEV_ID (AUDIO_AO_DEV_ID_LINE_OUT)

#ifndef ExecFunc
#define ExecFunc(_func_, _ret_) \
    do{ \
        MI_S32 s32Ret = MI_SUCCESS; \
        s32Ret = _func_; \
        if (s32Ret != _ret_) \
        { \
            printf("[%s %d]exec function failed, error:%x\n", __func__, __LINE__, s32Ret); \
            return s32Ret; \
        } \
        else \
        { \
            printf("[%s %d]exec function pass\n", __func__, __LINE__); \
        } \
    } while(0)
#endif

#define COLOR_NONE          "\033[0m"
#define COLOR_BLACK         "\033[0;30m"
#define COLOR_BLUE          "\033[0;34m"
#define COLOR_GREEN         "\033[0;32m"
#define COLOR_CYAN          "\033[0;36m"
#define COLOR_RED           "\033[0;31m"
#define COLOR_YELLOW        "\033[1;33m"
#define COLOR_WHITE         "\033[1;37m"

#define ST_DBG(fmt, args...) \
    do { \
        printf(COLOR_GREEN "[DBG]:%s[%d]: " COLOR_NONE, __FUNCTION__,__LINE__); \
        printf(fmt, ##args); \
    }while(0)

#define ST_ERR(fmt, args...) \
	do { \
		printf(COLOR_RED "[ERR]:%s[%d]: " COLOR_NONE, __FUNCTION__,__LINE__); \
		printf(fmt, ##args); \
	}while(0)
#define AD_LOG(fmt, args...) {printf("\033[1;34m");printf("%s[%d]:", __FUNCTION__, __LINE__);printf(fmt, ##args);printf("\033[0m");}

#define MAX_LINE_LEN				1024
#define MAX_BUF_LEN					256
#define MAX_FRAME_QUEUE_DEPTH       64
#define SAMPLE_RATE					16000
#define MAX_COMMAN_NUM				31

#define COMMAND_LEN					64

typedef struct
{
	bool bExit;
	pthread_t pt;
} ThreadData_t;

typedef enum
{
	E_TRIGGER_CMD = 0,
	E_COMMON_CMD
} CommandType_e;

typedef enum
{
	MODE_TRIGGER_WORD = 0,
	MODE_COMMAND
} CommandMode_e;

typedef struct
{
	unsigned char cmd[COMMAND_LEN];
} TrainedWord_t;

typedef void* (*VoiceAnalyzeCallback)(CommandType_e, int);

typedef struct
{
	VoiceAnalyzeCallback pfnCallback;
} UsrData_t;


#endif /* JNI_AED_KWS_DETECT_DETECT_PARAS_H_ */

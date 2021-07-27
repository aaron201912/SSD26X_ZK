/*
 * audio.c
 *
 *  Created on: 2020年11月2日
 *      Author: sigma
 */
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "video.h"
#include "mi_disp.h"
#include "panelconfig.h"

#if DELAY_LOAD
static VdecAssembly_t g_stVdecAssembly;
#endif

static int g_VdecRun = FALSE;
static pthread_t g_VdeStream_tid = 0;
static char g_VideoPath[256] = {0};

#define MI_U32VALUE(pu8Data, index) (pu8Data[index]<<24)|(pu8Data[index+1]<<16)|(pu8Data[index+2]<<8)|(pu8Data[index+3])
#define ST_DBG printf

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

#ifndef STCHECKRESULT
#define STCHECKRESULT(_func_)\
    do{ \
        MI_S32 s32Ret = MI_SUCCESS; \
        s32Ret = (MI_S32)_func_; \
        if (s32Ret != MI_SUCCESS)\
        { \
            printf("[%s %d]exec function failed, error:%x\n", __func__, __LINE__, s32Ret); \
            return s32Ret; \
        } \
        else \
        { \
            printf("(%s %d)exec function pass\n", __FUNCTION__,__LINE__); \
        } \
    } while(0)
#endif

#define ST_ERR(fmt, args...) \
    do { \
        printf(fmt, ##args); \
    }while(0)

//==============================================================================

typedef struct ST_Sys_BindInfo_s
{
    MI_SYS_ChnPort_t stSrcChnPort;
    MI_SYS_ChnPort_t stDstChnPort;
    MI_U32 u32SrcFrmrate;
    MI_U32 u32DstFrmrate;
} ST_Sys_BindInfo_T;

typedef struct ST_Sys_Rect_s
{
    MI_S32 s32X;
    MI_S32 s32Y;
    MI_S16 s16PicW;
    MI_S16 s16PicH;
} ST_Rect_T;

MI_U64 ST_Sys_GetPts(MI_U32 u32FrameRate) {
    return 0;
}

FILE *g_pStreamFile[32] = {NULL};
#define NALU_PACKET_SIZE 256*1024
#define ADD_HEADER_ES

typedef struct
{
    int startcodeprefix_len;

    unsigned int len;
    unsigned int max_size;
    char *buf;
    unsigned short lost_packets;
} NALU_t;

static int info2 = 0, info3 = 0;

static int FindStartCode2 (unsigned char *Buf)
{
    if((Buf[0] != 0) || (Buf[1] != 0) || (Buf[2] != 1))
        return 0;
    else
        return 1;
}

static int FindStartCode3 (unsigned char *Buf)
{
    if((Buf[0] != 0) || (Buf[1] != 0) || (Buf[2] != 0) || (Buf[3] != 1))
        return 0;
    else
        return 1;
}

NALU_t *AllocNALU(int buffersize)
{
    NALU_t *n;
    if ((n = (NALU_t*)calloc (1, sizeof (NALU_t))) == NULL)
    {
        printf("AllocNALU: n");
        exit(0);
    }
    n->max_size=buffersize;
    if ((n->buf = (char*)calloc (buffersize, sizeof (char))) == NULL)
    {
        free (n);
        printf ("AllocNALU: n->buf");
        exit(0);
    }
    return n;
}

void FreeNALU(NALU_t *n)
{
    if (n)
    {
        if (n->buf)
        {
            free(n->buf);
            n->buf=NULL;
        }
        free (n);
    }
}

int GetAnnexbNALU (NALU_t *nalu, MI_S32 chn)
{
    int pos = 0;
    int StartCodeFound, rewind;
    unsigned char *Buf;

    if ((Buf = (unsigned char*)calloc (nalu->max_size , sizeof(char))) == NULL)
        printf ("GetAnnexbNALU: Could not allocate Buf memory\n");
    nalu->startcodeprefix_len=3;
    if (3 != fread (Buf, 1, 3, g_pStreamFile[chn]))
    {
        free(Buf);
        return 0;
    }
    info2 = FindStartCode2 (Buf);
    if(info2 != 1)
    {
        if(1 != fread(Buf+3, 1, 1, g_pStreamFile[chn]))
        {
            free(Buf);
            return 0;
        }
        info3 = FindStartCode3 (Buf);
        if (info3 != 1)
        {
            free(Buf);
            return -1;
        }
        else
        {
            pos = 4;
            nalu->startcodeprefix_len = 4;
        }
    }
    else
    {
        nalu->startcodeprefix_len = 3;
        pos = 3;
    }
    StartCodeFound = 0;
    info2 = 0;
    info3 = 0;
    while (!StartCodeFound)
    {
        if (feof (g_pStreamFile[chn]))
        {
            nalu->len = (pos-1)-nalu->startcodeprefix_len;
            memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
            free(Buf);
            fseek(g_pStreamFile[chn], 0, 0);
            return pos-1;
        }
        Buf[pos++] = fgetc (g_pStreamFile[chn]);
        info3 = FindStartCode3(&Buf[pos-4]);
        if(info3 != 1)
            info2 = FindStartCode2(&Buf[pos-3]);
        StartCodeFound = (info2 == 1 || info3 == 1);
    }
    rewind = (info3 == 1) ? -4 : -3;
    if (0 != fseek (g_pStreamFile[chn], rewind, SEEK_CUR))
    {
        free(Buf);
        printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
    }
    nalu->len = (pos+rewind);
    memcpy (nalu->buf, &Buf[0], nalu->len);
    free(Buf);
    return (pos+rewind);
}

void dump(NALU_t *n)
{
    if (!n)
        return;
    //printf(" len: %d  ", n->len);
    //printf("nal_unit_type: %x\n", n->nal_unit_type);
}

MI_S32 SSTAR_ModuleBind(MI_S32 s32SrcMod, MI_S32 s32SrcDev, MI_S32 s32SrcChn,MI_S32 s32SrcPort,
    MI_S32 s32DstMod,MI_S32 s32DstDev,MI_S32 s32DstChn,MI_S32 s32DstPort)
{
    ST_Sys_BindInfo_T stBindInfo;
    memset(&stBindInfo, 0x0, sizeof(stBindInfo));

    stBindInfo.stSrcChnPort.eModId = (MI_ModuleId_e)s32SrcMod;
    stBindInfo.stSrcChnPort.u32DevId = s32SrcDev;
    stBindInfo.stSrcChnPort.u32ChnId = s32SrcChn;
    stBindInfo.stSrcChnPort.u32PortId = s32SrcPort;

    stBindInfo.stDstChnPort.eModId = (MI_ModuleId_e)s32DstMod;
    stBindInfo.stDstChnPort.u32DevId = s32DstDev;
    stBindInfo.stDstChnPort.u32ChnId = s32DstChn;
    stBindInfo.stDstChnPort.u32PortId = s32DstPort;

    stBindInfo.u32SrcFrmrate = 0;
    stBindInfo.u32DstFrmrate = 0;
    ExecFunc(MI_SYS_BindChnPort(0, &stBindInfo.stSrcChnPort, &stBindInfo.stDstChnPort, \
        stBindInfo.u32SrcFrmrate, stBindInfo.u32DstFrmrate), MI_SUCCESS);

    return 0;
}

MI_S32 SSTAR_ModuleUnBind(MI_S32 s32SrcMod, MI_S32 s32SrcDev, MI_S32 s32SrcChn,MI_S32 s32SrcPort,
    MI_S32 s32DstMod,MI_S32 s32DstDev,MI_S32 s32DstChn,MI_S32 s32DstPort)
{
    ST_Sys_BindInfo_T stBindInfo;
    memset(&stBindInfo, 0x0, sizeof(stBindInfo));

    stBindInfo.stSrcChnPort.eModId = (MI_ModuleId_e)s32SrcMod;
    stBindInfo.stSrcChnPort.u32DevId = s32SrcDev;
    stBindInfo.stSrcChnPort.u32ChnId = s32SrcChn;
    stBindInfo.stSrcChnPort.u32PortId = s32SrcPort;

    stBindInfo.stDstChnPort.eModId = (MI_ModuleId_e)s32DstMod;
    stBindInfo.stDstChnPort.u32DevId = s32DstDev;
    stBindInfo.stDstChnPort.u32ChnId = s32DstChn;
    stBindInfo.stDstChnPort.u32PortId = s32DstPort;

    stBindInfo.u32SrcFrmrate = 0;
    stBindInfo.u32DstFrmrate = 0;
    printf("xxxxxxxModule Unbind src(%d-%d-%d-%d) dst(%d-%d-%d-%d)...\n", s32SrcMod, s32SrcDev, s32SrcChn, s32SrcPort,
        s32DstMod, s32DstDev, s32DstChn, s32DstPort);
    ExecFunc(MI_SYS_UnBindChnPort(0, &stBindInfo.stSrcChnPort, &stBindInfo.stDstChnPort), MI_SUCCESS);

    return 0;
}

void *SSTAR_VdecSendStream(void *args)
{
    MI_SYS_ChnPort_t stChnPort;
    MI_SYS_BufConf_t stBufConf;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BUF_HANDLE hSysBuf;
    MI_S32 s32Ret = MI_SUCCESS;
    MI_VDEC_CHN vdecChn = 0;
    MI_S32 s32TimeOutMs = 20, s32ChannelId = 0, s32TempHeight = 0;
    MI_S32 s32Ms = 30;
    MI_BOOL bVdecChnEnable;
    MI_U16 u16Times = 20000;

    MI_S32 s32ReadCnt = 0;
    FILE *readfp = NULL;
    MI_U8 *pu8Buf = NULL;
    MI_S32 s32Len = 0;
    MI_U32 u32FrameLen = 0;
    MI_U64 u64Pts = 0;
    MI_U8 au8Header[32] = {0};
    MI_U32 u32Pos = 0;
    MI_VDEC_ChnStat_t stChnStat;
    MI_VDEC_VideoStream_t stVdecStream;

    MI_U32 u32FpBackLen = 0; // if send stream failed, file pointer back length

    char tname[32];
    memset(tname, 0, 32);

#ifndef ADD_HEADER_ES
    NALU_t *n;
    n = AllocNALU(2000000);
#endif

    vdecChn = 0;
    snprintf(tname, 32, "push_t_%u", vdecChn);

    memset(&stChnPort, 0x0, sizeof(MI_SYS_ChnPort_t));
    stChnPort.eModId = E_MI_MODULE_ID_VDEC;
    stChnPort.u32DevId = 0;
    stChnPort.u32ChnId = vdecChn;//0 1 2 3
    stChnPort.u32PortId = 0;

    if ((access(g_VideoPath, F_OK)) != -1)
    {
    	readfp = fopen(g_VideoPath, "rb"); //ES
    	ST_DBG("open %s err\n", g_VideoPath);
    }

    if (!readfp)
    {
        ST_ERR("Open es file failed!\n");
        return NULL;
    }
    else
    {
        g_pStreamFile[vdecChn] = readfp;
    }

    memset(&stBufConf, 0x0, sizeof(MI_SYS_BufConf_t));
    stBufConf.eBufType = E_MI_SYS_BUFDATA_RAW;
    stBufConf.u64TargetPts = 0;
    pu8Buf = (MI_U8 *)malloc(NALU_PACKET_SIZE);

    s32Ret = MI_SYS_SetChnOutputPortDepth(0, &stChnPort, 0, 3);
    if (MI_SUCCESS != s32Ret)
    {
        ST_ERR("MI_SYS_SetChnOutputPortDepth error, %X\n", s32Ret);
        return NULL;
    }

    s32Ms = 30;

    printf("----------------------%d----------g_VdecRun %d--------\n", stChnPort.u32ChnId, g_VdecRun);
    while (g_VdecRun)
    {
#ifdef ADD_HEADER_ES
        memset(au8Header, 0, 16);
        u32Pos = fseek(readfp, 0, SEEK_CUR);
        s32Len = fread(au8Header, 1, 16, readfp);
        if(s32Len <= 0)
        {
            fseek(readfp, 0, SEEK_SET);
            continue;
        }
        u32FrameLen = MI_U32VALUE(au8Header, 4);
        //printf("vdecChn:%d, u32FrameLen:%d, %d\n", vdecChn, u32FrameLen, NALU_PACKET_SIZE);
        if(u32FrameLen > NALU_PACKET_SIZE)
        {
            fseek(readfp, 0, SEEK_SET);
            continue;
        }
        s32Len = fread(pu8Buf, 1, u32FrameLen, readfp);
        if(s32Len <= 0)
        {
            fseek(readfp, 0, SEEK_SET);
            continue;
        }

        stVdecStream.pu8Addr = pu8Buf;
        stVdecStream.u32Len = s32Len;
        stVdecStream.u64PTS = u64Pts;
        stVdecStream.bEndOfFrame = 1;
        stVdecStream.bEndOfStream = 0;

        u32FpBackLen = stVdecStream.u32Len + 16; //back length
#else
        GetAnnexbNALU(n, vdecChn);
        dump(n);
        stVdecStream.pu8Addr = (MI_U8 *)n->buf;
        stVdecStream.u32Len = n->len;
        stVdecStream.u64PTS = u64Pts;
        stVdecStream.bEndOfFrame = 1;
        stVdecStream.bEndOfStream = 0;

        u32FpBackLen = stVdecStream.u32Len; //back length
#endif
        if(0x00 == stVdecStream.pu8Addr[0] && 0x00 == stVdecStream.pu8Addr[1]
            && 0x00 == stVdecStream.pu8Addr[2] && 0x01 == stVdecStream.pu8Addr[3]
            && 0x65 == stVdecStream.pu8Addr[4] || 0x61 == stVdecStream.pu8Addr[4]
            || 0x26 == stVdecStream.pu8Addr[4] || 0x02 == stVdecStream.pu8Addr[4]
            || 0x41 == stVdecStream.pu8Addr[4])
        {
            usleep(s32Ms * 1000);
        }
        if (MI_SUCCESS != (s32Ret = MI_VDEC_SendStream(0, vdecChn, &stVdecStream, s32TimeOutMs)))
        {
            //ST_ERR("MI_VDEC_SendStream fail, chn:%d, 0x%X\n", vdecChn, s32Ret);
            fseek(readfp, - u32FpBackLen, SEEK_CUR);
        }
        u64Pts = u64Pts + 33;
        if (0 == (s32ReadCnt++ % 30))
            ;// printf("vdec(%d) push buf cnt (%d)...\n", s32VoChannel, s32ReadCnt)
            ;//printf("###### ==> Chn(%d) push frame(%d) Frame Dec:%d  Len:%d\n", s32VoChannel, s32ReadCnt, stChnStat.u32DecodeStreamFrames, u32Len);
    }
    printf("\n\n");
    usleep(300000);
    free(pu8Buf);
    printf("End----------------------%d------------------End\n", stChnPort.u32ChnId);

    return NULL;
}

MI_S32 SSTAR_CreateVdecChannel(MI_S32 s32VdecChn, MI_S32 s32CodecType,
    MI_U32 u32Width, MI_U32 u32Height, MI_U32 u32OutWidth, MI_U32 u32OutHeight)
{
    MI_VDEC_ChnAttr_t stVdecChnAttr;
    MI_VDEC_OutputPortAttr_t stOutputPortAttr;
    MI_VDEC_InitParam_t stVdecInitParam;

    memset(&stVdecInitParam, 0, sizeof(MI_VDEC_InitParam_t));
    stVdecInitParam.bDisableLowLatency = false;
    //MI_VDEC_InitDev(&stVdecInitParam);
    MI_VDEC_CreateDev(0, &stVdecInitParam);

    memset(&stVdecChnAttr, 0, sizeof(MI_VDEC_ChnAttr_t));
    stVdecChnAttr.stVdecVideoAttr.u32RefFrameNum = 2;
    stVdecChnAttr.eVideoMode    = E_MI_VDEC_VIDEO_MODE_FRAME;
    stVdecChnAttr.u32BufSize    = 1 * 1024 * 1024;
    stVdecChnAttr.u32PicWidth   = u32Width;
    stVdecChnAttr.u32PicHeight  = u32Height;
    stVdecChnAttr.u32Priority   = 0;
    stVdecChnAttr.eCodecType    = (MI_VDEC_CodecType_e)s32CodecType;
    stVdecChnAttr.eDpbBufMode = (MI_VDEC_DPB_BufMode_e)0;

    STCHECKRESULT(MI_VDEC_CreateChn(0, s32VdecChn, &stVdecChnAttr));
    STCHECKRESULT(MI_VDEC_StartChn(0, s32VdecChn));

    if (u32OutWidth > u32Width)
    {
        u32OutWidth = u32Width;
    }
    if (u32OutHeight > u32Height)
    {
        u32OutHeight = u32Height;
    }
    stOutputPortAttr.u16Width = u32OutWidth;
    stOutputPortAttr.u16Height = u32OutHeight;
    MI_VDEC_SetOutputPortAttr(0, s32VdecChn, &stOutputPortAttr);

    return MI_SUCCESS;
}

MI_S32 SSTAR_DestroyVdecChannel(MI_S32 s32VdecChn)
{
    MI_S32 s32Ret = MI_SUCCESS;

    s32Ret = MI_VDEC_StopChn(0, s32VdecChn);
    if (MI_SUCCESS != s32Ret)
    {
        printf("%s %d, MI_VENC_StopRecvPic %d error, %X\n", __func__, __LINE__, s32VdecChn, s32Ret);
    }
    s32Ret |= MI_VDEC_DestroyChn(0, s32VdecChn);
    if (MI_SUCCESS != s32Ret)
    {
        printf("%s %d, MI_VENC_StopRecvPic %d error, %X\n", __func__, __LINE__, s32VdecChn, s32Ret);
    }

    return s32Ret;
}

MI_S32 SSTAR_CreateVdec2DispPipe(MI_S32 s32VdecChn, MI_S32 s32DivpChn, MI_U32 u32VdecW, MI_U32 u32VdecH, MI_S32 s32CodecType)
{
    ST_Rect_T stCrop= {0, 0, 0, 0};
    MI_DISP_InputPortAttr_t stInputPortAttr;
    SSTAR_CreateVdecChannel(s32VdecChn, s32CodecType, u32VdecW, u32VdecH, PANEL_WIDTH, PANEL_HEIGHT);

    MI_DISP_GetInputPortAttr(0, 0, &stInputPortAttr);
	stInputPortAttr.stDispWin.u16X      = 0;
	stInputPortAttr.stDispWin.u16Y      = 0;
	stInputPortAttr.stDispWin.u16Width  = PANEL_WIDTH;
	stInputPortAttr.stDispWin.u16Height = PANEL_HEIGHT;
	stInputPortAttr.u16SrcWidth = PANEL_WIDTH;
	stInputPortAttr.u16SrcHeight = PANEL_HEIGHT;

	printf("disp input: w=%d, h=%d\n", stInputPortAttr.u16SrcWidth, stInputPortAttr.u16SrcHeight);
	MI_DISP_DisableInputPort(0, 0);
	MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr);
	MI_DISP_GetInputPortAttr(0, 0, &stInputPortAttr);
	MI_DISP_EnableInputPort(0, 0);
	MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN);

    SSTAR_ModuleBind(E_MI_MODULE_ID_VDEC, 0, s32VdecChn, 0,
                    E_MI_MODULE_ID_DISP, 0, 0, 0); //DIVP->DISP

    return MI_SUCCESS;
}

MI_S32 SSTAR_DestroyVdec2DispPipe(MI_S32 s32VdecChn, MI_S32 s32DivpChn)
{
    SSTAR_ModuleUnBind(E_MI_MODULE_ID_VDEC, 0, s32VdecChn, 0,
                    E_MI_MODULE_ID_DISP, 0, 0, 0); //DIVP->DISP
    SSTAR_DestroyVdecChannel(s32VdecChn);
    MI_DISP_DisableInputPort(0, 0);

    //MI_VDEC_DeInitDev();
    MI_VDEC_DestroyDev(0);
    MI_DISP_DeInitDev();

    return MI_SUCCESS;
}

int SSTAR_StartPlayVideo(char *pVideoFile)
{
	memset(g_VideoPath, 0, sizeof(g_VideoPath));
	strncpy(g_VideoPath, pVideoFile, strlen(pVideoFile));
	SSTAR_CreateVdec2DispPipe(0, 0, 1280, 720, E_MI_VDEC_CODEC_TYPE_H264);
	g_VdecRun = TRUE;
	pthread_create(&g_VdeStream_tid, NULL, SSTAR_VdecSendStream, NULL);

	return 0;
}


int SSTAR_StopPlayVideo()
{
	g_VdecRun = FALSE;
	pthread_join(g_VdeStream_tid, NULL);
	SSTAR_DestroyVdec2DispPipe(0, 0);

	return 0;
}

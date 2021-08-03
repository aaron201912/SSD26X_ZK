#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <dlfcn.h>

#include <string>

#include "st_common.h"
#include "st_rgn.h"
#include "mi_scl.h"

#include "common_struct.h"
#include "FaceData.h"
#include "FaceRecognitionSDK.h"

//#define DBG_SAVE_FR_FILE

#define MAX_FR_INSTANCE 4
#define FACE_DETECT_IMG_WIDTH 320
#define FACE_DETECT_IMG_HEIGHT 256

#define ST_RECT_BORDER_WIDTH 4
#define ST_RECT_BORDER_LENGTH 30
#define ST_RECT_COLOR 7

#define IPU_FIRMWARE_PATH "/config/dla/ipu_firmware.bin"
#define ALG_MODEL_PATH "/customer/res/model/"
#define FR_SDK_LIB_PATH "/customer/lib/libFrSdk.so"

#define FACE_DB_FEAT_FILE_PATH "/customer/res/feat.bin"
#define FACE_DB_NAME_LIST_PATH "/customer/res/name.list"

typedef struct
{
    MI_BOOL bUsed;

    MI_SCL_DEV sclDev;
    MI_SCL_CHANNEL sclChn;
    MI_SCL_PORT rgnPort;
    MI_U16 u16SrcWidth;
    MI_U16 u16SrcHeight;
    MI_U16 u16RgnCropWidth;     /* 显示端口的宽高 */
    MI_U16 u16RgnCropHeight;
    MI_SCL_PORT capPort;
    MI_SYS_PixelFormat_e ePixelFormat;
    MI_RGN_HANDLE rgnHandle;
    ST_Rect_T rgnRect;

    long long frHandle = 0;
    /* 固定大小的FD小图 */
    MI_U32 u32Stride;
    MI_U32 u32BuffSize;
    MI_PHY u64FdBufPhyAddr;
    void *pFdBufVirAddr;
    pthread_t capTid;

    MI_BOOL bAddPerson;
    char personName[128];
} ST_FR_Instance_t;

typedef struct
{
    ST_Point_T stPointLU;
    ST_Point_T stPointRD;
    MI_U32 u32BorderLen;
    MI_U32 u32Color;
} ST_FaceFrame_t;

struct
{
	void *pHandle;

	/*************************************************
	*功能           初始化sdk环境
	*ipu_firwarepath  输入：ipu_firware.bin 的路径
	*modelpath        输入：模型存放的路劲
	*************************************************/
	int (*pfnXC_FaceRecognition_Init)(const char* ipu_firmwarepath,const char* modelpath);

	/*************************************************
	*功能           创建句柄
	*detectorID       输入：句柄指针
	*************************************************/
	int (*pfnXC_FaceRecognition_CreateHandle)(long long* detectorID);

	/*************************************************
	*功能           人脸检测接口（根据参数）
	*detectorID      输入：句柄指针
	*imagedata       输入：图像的数据（只支持BGRA）
	*width           输入：图像的宽 320
	*height          输入：图像的高 256
	*channel         输入：图像的通道（BGRA）
	*ParamSDK        输入：配置参数
	*DetectBox       输出：输出的人脸框
	*facecount       输出：人脸框个数
	*************************************************/
	int (*pfnXC_FaceRecognition_Detect)(long long detectID,const char* imagedata,const int width,const int height, const int channel, const ParamSDK* params, DetectBox** detectout,int* facecount);

	/*************************************************
     *功能           人脸检测接口（根据参数）
     *detectorID      输入：句柄指针
     *imagedata       输入：图像的数据（只支持BGRA）
     *width           输入：图像的宽 112
     *height          输入：图像的高 112
     *channel         输入：图像的通道（默认为BGRA 4通道）
     *DetectBox       输出：检测的人脸框
     *************************************************/
    int (*pfnXC_FaceLandmark_Detect)(long long detectID, const char* imagedata, const int width, const int height,const int channel, DetectBox* detectbox);

	/*************************************************
	*功能           将BGRA的数据crop出112x112的BGRA的数据
	*imagedata       输入：BGRA的数据指针
	*width           输入：图像的宽
	*height          输入：图像的高
	*channel		 输入：图像的通道
	*detectout       输入：检测框和关键点
	*outmatdata      输出：对齐后的BGRA图像
	*************************************************/
	int (*pfnXC_Crop112x112_BGRA)(unsigned char* imagedata,int width,int height,int channel, DetectBox detectout,unsigned char* outmatdata);

	/*************************************************
	*功能           活体检测（暂未实现）
	*detectorID      输入：句柄
	*imagedata       输入：图像buffer数据
	*width           输入：图像的宽
	*height          输入：图像的高
	* channel        输入：图像的通道数
	*detectRect      输入：检测框和关键点
	*spoofing        输出：是否是活体
	*************************************************/
	int (*pfnXC_FaceRecognition_Antispoofing)(long long detectID,const char* imagedata, const int width, const int height, const int channel, DetectBox detectRect,int* spoofing);

	/*************************************************
	*功能           人脸特征提取
	*detectorID      输入：句柄
	*imagedata       输入：BGR图像数据
	*width           输入：图像的宽
	*height          输入：图像的高
	*channel         输入：图像的通道数
	*detectRect      输入：检测框和关键点
	*featureout      输出：人脸特征
	*************************************************/
	int (*pfnXC_FaceRecegnition_FeatureExtract)(long long detectID, const char* imagedata, const int width, const int height, const int channel, DetectBox detectRect, int16_t* featureout);

	/*************************************************
	*功能           人脸特征比对
	*feature1        输入：第一个人脸特征
	*feature2        输入：第二个人脸特征
	*length          输入：人脸特征长度（130）
	*simility        输出：人脸相似度
	*************************************************/
	int (*pfnXC_FaceRecognition_FeatureCompare)(const int16_t* feature1,const int16_t* feature2,int length, float* simility);

	/*************************************************
	*功能           释放句柄
	*detectorID      输入：句柄
	*************************************************/
	int (*pfnXC_FaceRecognition_ReleaseHandle)(long long detectorID);

	/*************************************************
	*功能           清理人脸sdk的运行环境
	*************************************************/
	int (*pfnXC_FaceRecognition_Cleanup)();
} g_st_XC_FaceRecognition;

static MI_BOOL g_bIpuStart = FALSE;

static pthread_mutex_t gStFrLock = PTHREAD_MUTEX_INITIALIZER;
static MI_U32 gU32FrInstCnt = 0;
static ST_FR_Instance_t gStFrInstance[MAX_FR_INSTANCE];

extern MI_S32 ST_GetCropParam(MI_S32 srcWidth, MI_S32 srcHeight,
        MI_S32 dstWidth, MI_S32 dstHeight, MI_SYS_WindowRect_t *pStCropRect);

static MI_S32 ST_GetFrInstance()
{
    MI_S32 s32Frid = 0;

    pthread_mutex_lock(&gStFrLock);
    if(0 == gU32FrInstCnt)
    {
        memset(&gStFrInstance, 0, MAX_FR_INSTANCE * sizeof(ST_FR_Instance_t));
    }
    for(s32Frid = 0; s32Frid < MAX_FR_INSTANCE; s32Frid++)
    {
        if(!gStFrInstance[s32Frid].bUsed)
        {
            gStFrInstance[s32Frid].bUsed = TRUE;
            gU32FrInstCnt++;
            break;
        }
    }
    pthread_mutex_unlock(&gStFrLock);
    if(s32Frid >= MAX_FR_INSTANCE)
    {
        printf("Not more FR instance left!\n");
        return -1;
    }

    return s32Frid;
}

static MI_S32 ST_FreeFrInstance(MI_S32 s32Frid)
{
    MI_S32 s32Ret = -1;
    if(s32Frid < 0 || s32Frid >= MAX_FR_INSTANCE)
    {
        printf("FR instance ID[%d] error!\n", s32Frid);
        return -1;
    }

    pthread_mutex_lock(&gStFrLock);
    if(gU32FrInstCnt > 0)
    {
        s32Ret = --gU32FrInstCnt;
    }
    gStFrInstance[s32Frid].bUsed = FALSE;
    pthread_mutex_unlock(&gStFrLock);

    return s32Ret;
}

static MI_S32 ST_FaceFrameRendering(MI_RGN_HANDLE hHandle, ST_FaceFrame_t *pFaceFrame)
{
    ST_Point_T stPointStart, stPointEnd;

    if(NULL == pFaceFrame || 0 == pFaceFrame->u32BorderLen)
    {
        return -1;
    }

    //Left Up
    stPointStart = pFaceFrame->stPointLU;
    stPointEnd.u32X = stPointStart.u32X + pFaceFrame->u32BorderLen;
    stPointEnd.u32Y = stPointStart.u32Y;
    ST_OSD_DrawLine(hHandle, stPointStart, stPointEnd, ST_RECT_BORDER_WIDTH, pFaceFrame->u32Color);
    stPointEnd.u32X = stPointStart.u32X;
    stPointEnd.u32Y = stPointStart.u32Y + pFaceFrame->u32BorderLen;
    ST_OSD_DrawLine(hHandle, stPointStart, stPointEnd, ST_RECT_BORDER_WIDTH, pFaceFrame->u32Color);
    //Right Up
    stPointStart.u32X = pFaceFrame->stPointRD.u32X;
    stPointStart.u32Y = pFaceFrame->stPointLU.u32Y;
    stPointEnd.u32X = stPointStart.u32X - pFaceFrame->u32BorderLen;
    stPointEnd.u32Y = stPointStart.u32Y;
    ST_OSD_DrawLine(hHandle, stPointStart, stPointEnd, ST_RECT_BORDER_WIDTH, pFaceFrame->u32Color);
    stPointStart.u32X = pFaceFrame->stPointRD.u32X - ST_RECT_BORDER_WIDTH + 1;
    stPointStart.u32Y = pFaceFrame->stPointLU.u32Y;
    stPointEnd.u32X = stPointStart.u32X;
    stPointEnd.u32Y = stPointStart.u32Y + pFaceFrame->u32BorderLen;
    ST_OSD_DrawLine(hHandle, stPointStart, stPointEnd, ST_RECT_BORDER_WIDTH, pFaceFrame->u32Color);
    //Left Down
    stPointStart.u32X = pFaceFrame->stPointLU.u32X;
    stPointStart.u32Y = pFaceFrame->stPointRD.u32Y - ST_RECT_BORDER_WIDTH + 1;
    stPointEnd.u32X = stPointStart.u32X + pFaceFrame->u32BorderLen;
    stPointEnd.u32Y = stPointStart.u32Y;
    ST_OSD_DrawLine(hHandle, stPointStart, stPointEnd, ST_RECT_BORDER_WIDTH, pFaceFrame->u32Color);
    stPointStart.u32X = pFaceFrame->stPointLU.u32X;
    stPointStart.u32Y = pFaceFrame->stPointRD.u32Y;
    stPointEnd.u32X = stPointStart.u32X;
    stPointEnd.u32Y = stPointStart.u32Y - pFaceFrame->u32BorderLen;
    ST_OSD_DrawLine(hHandle, stPointStart, stPointEnd, ST_RECT_BORDER_WIDTH, pFaceFrame->u32Color);
    //Right Down
    stPointStart.u32X = pFaceFrame->stPointRD.u32X;
    stPointStart.u32Y = pFaceFrame->stPointRD.u32Y - ST_RECT_BORDER_WIDTH + 1;
    stPointEnd.u32X = stPointStart.u32X - pFaceFrame->u32BorderLen;
    stPointEnd.u32Y = stPointStart.u32Y;
    ST_OSD_DrawLine(hHandle, stPointStart, stPointEnd, ST_RECT_BORDER_WIDTH, pFaceFrame->u32Color);
    stPointStart.u32X = pFaceFrame->stPointRD.u32X - ST_RECT_BORDER_WIDTH + 1;
    stPointStart.u32Y = pFaceFrame->stPointRD.u32Y;
    stPointEnd.u32X = stPointStart.u32X;
    stPointEnd.u32Y = stPointStart.u32Y - pFaceFrame->u32BorderLen;
    ST_OSD_DrawLine(hHandle, stPointStart, stPointEnd, ST_RECT_BORDER_WIDTH, pFaceFrame->u32Color);

    return MI_SUCCESS;
}

MI_S32 ST_SaveFile(char *szFileName, void *pData, MI_U32 FileSize)
{
    int bSuccess = 0;
    FILE *pFile = NULL;
    unsigned char *p;
    //long lFileWsize;

    do
    {
        pFile = fopen(szFileName, "wb");
        if(pFile == NULL)
        {
            printf(" open \"%s\" failed \n", szFileName);
        }
        else
        {
            //printf(" open \"%s\" success \n", szFileName);
            fseek(pFile, 0, SEEK_END);
        }
        p = (unsigned char *)pData;
        //lFileWsize = fwrite(p, FileSize, sizeof(unsigned char), pFile);
        fwrite(p, FileSize, sizeof(unsigned char), pFile);

        fclose(pFile);

        bSuccess = 1;
    }
    while (0);

    return bSuccess;
}

static void ST_FrResFree(void *arg)
{
    MI_S32 s32Fd = -1;

    if(NULL != arg && (s32Fd = *(MI_S32 *)arg) > 0)
    {
        MI_SYS_CloseFd(s32Fd);
    }

    return;
}

static MI_S32 ST_FRFindName(FaceDatabase &face_db, int16_t * feat, char *name, int len)
{
    int num = face_db.persons.size();
    float score_max = 0.0;
    int person_index = -1;

    if(NULL == feat || NULL == name || len <= 0)
    {
        return -1;
    }

    // printf("====================================================\n");
    for(int i = 0; i < num; i++)
    {
        // printf("%s \n", face_db.persons[i_p].name.c_str());
        int feat_num = face_db.persons[i].features.size();
        for(int j = 0; j < feat_num; j++)
        {
            float score = 0;
            g_st_XC_FaceRecognition.pfnXC_FaceRecognition_FeatureCompare((int16_t *)feat, (int16_t*)face_db.persons[i].features[j].pData,
                                              FEATURE_SIZE, &score);
            if (score > score_max)
            {
                score_max = score;
                person_index = i;
            }
        }
    }

    // printf("the max sorce is:%f",score_max);
    printf("max~~~~~~~score: %f ~~~~~~~ \n", score_max);
    if(score_max > 0.7)
    {
        // printf("max~~~~~~~ %s ~~~~~~~~\n", face_db.persons[person_index].name.c_str());
        snprintf(name, len, "%s", face_db.persons[person_index].name.c_str());
        return 0;
    }
    else
    {
        snprintf(name, len, "unknown");
        return -1;
    }
}

static void *ST_FrTask(void * arg)
{
    MI_S32 i;
    MI_S32 s32Ret;
    MI_S32 s32Fd;
    MI_SYS_BUF_HANDLE hSysBufHandle;
    ST_FR_Instance_t *pstFrInstance;
    struct timeval TimeoutVal;
    fd_set read_fds;
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_ChnPort_t stSclChnOutput;
    DetectBox landmarkbox;
    float size, centerx, centery;

    // TODO: Only support 1 target now
    int curFaceID = -1;
    char name[128] = {"unknown"};
    FaceDatabase face_db;
    MI_SCL_DirectBuf_t stSrcBuf, stDstBuf;
    MI_SYS_WindowRect_t stSrcCrop;
    ParamSDK frParams;
    DetectBox *outputbox = NULL;
    DetectBox tofeature;
    int16_t feature[130];
    int facecount = 0;
    float fRgnSclTimesX, fRgnSclTimesY;
    MI_RGN_CanvasInfo_t *pstCanvasInfo;
    ST_Point_T stTextPoint;
    ST_FaceFrame_t stFaceFrame;
    MI_BOOL bOsdShowed = FALSE;
    time_t lastRecogTime = 0;
#ifdef DBG_SAVE_FR_FILE
    char frDbgFile[128] = {0};
#endif
    unsigned char outmatdata[112 * 112 * 4] = {0};

    if(0 == access(FACE_DB_FEAT_FILE_PATH, F_OK) &&\
        0 == access(FACE_DB_NAME_LIST_PATH, F_OK))
    {
        face_db.LoadFromFileBinay(FACE_DB_FEAT_FILE_PATH, FACE_DB_NAME_LIST_PATH); //加载数据库
    }

    pstFrInstance = (ST_FR_Instance_t *)arg;

    memset(&stSclChnOutput , 0, sizeof(stSclChnOutput));
    stSclChnOutput.eModId = E_MI_MODULE_ID_SCL;
    stSclChnOutput.u32DevId = pstFrInstance->sclDev;
    stSclChnOutput.u32ChnId = pstFrInstance->sclChn;
    stSclChnOutput.u32PortId = pstFrInstance->capPort;
    s32Ret = MI_SYS_GetFd(&stSclChnOutput, &s32Fd);
    if(MI_SUCCESS != s32Ret)
    {
        ST_ERR("MI_SYS_GetFd err:%x\n", s32Ret);
        return NULL;
    }

    frParams.datatype = 2;

    fRgnSclTimesX = (float)pstFrInstance->rgnRect.u16PicW / pstFrInstance->u16RgnCropWidth;
    fRgnSclTimesY = (float)pstFrInstance->rgnRect.u16PicH / pstFrInstance->u16RgnCropHeight;

    pthread_cleanup_push(ST_FrResFree, &s32Fd);
    while(1)
    {
        FD_ZERO(&read_fds); 
        FD_SET(s32Fd, &read_fds);
        TimeoutVal.tv_sec  = 1;
        TimeoutVal.tv_usec = 0;
        /*Waiting for frame*/
        s32Ret = select(s32Fd + 1, &read_fds, NULL, NULL, &TimeoutVal);
        if(s32Ret <= 0)
        {
            ST_ERR("select failed or get scl frame time out, s32Fd = %d, s32Ret = %d!\n", s32Fd, s32Ret);
            continue;
        }

        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        if(FD_ISSET(s32Fd, &read_fds))
        {
            /* Get a frame from scl */
            if(MI_SUCCESS  == MI_SYS_ChnOutputPortGetBuf(&stSclChnOutput, &stBufInfo, &hSysBufHandle))
            {
                if(E_MI_SYS_BUFDATA_FRAME != stBufInfo.eBufType)
                {
                    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
                    continue;
                }
            #if 0
                printf("FR src image(%d x %d) seq : %d\n", stBufInfo.stFrameData.u16Width, \
                                                        stBufInfo.stFrameData.u16Height, \
                                                        stBufInfo.u32SequenceNumber);
            #endif
                /* Source */
                stSrcBuf.ePixelFormat = pstFrInstance->ePixelFormat;
                stSrcBuf.u32Width = stBufInfo.stFrameData.u16Width;
                stSrcBuf.u32Height = stBufInfo.stFrameData.u16Height;
                stSrcBuf.u32Stride[0] = stBufInfo.stFrameData.u32Stride[0];
                stSrcBuf.u32Stride[1] = stBufInfo.stFrameData.u32Stride[1];
                stSrcBuf.phyAddr[0] = stBufInfo.stFrameData.phyAddr[0];
                stSrcBuf.phyAddr[1] = stBufInfo.stFrameData.phyAddr[1];
                stSrcBuf.u32BuffSize = stBufInfo.stFrameData.u32BufSize;
                do
                {
                    /* Source crop */
                    stSrcCrop.u16X = stSrcCrop.u16Y = 0;
                    stSrcCrop.u16Width = stSrcBuf.u32Width;
                    stSrcCrop.u16Height = stSrcBuf.u32Height;
                    /* Stretch destination buf */
                    stDstBuf.ePixelFormat = pstFrInstance->ePixelFormat;
                    stDstBuf.u32Width = FACE_DETECT_IMG_WIDTH;
                    stDstBuf.u32Height = FACE_DETECT_IMG_HEIGHT;
                    stDstBuf.phyAddr[0] = pstFrInstance->u64FdBufPhyAddr;
                    stDstBuf.phyAddr[1] = pstFrInstance->u64FdBufPhyAddr + stDstBuf.u32Height * pstFrInstance->u32Stride;
                    stDstBuf.u32Stride[0] = stDstBuf.u32Stride[1] = pstFrInstance->u32Stride;
                    stDstBuf.u32BuffSize = pstFrInstance->u32BuffSize;
                    if(MI_SUCCESS != MI_SCL_StretchBuf(&stSrcBuf, &stSrcCrop, &stDstBuf, E_MI_SCL_FILTER_TYPE_AUTO))
                    {
                        ST_ERR("MI_SCL_StretchBuf failed!\n");
                        break;
                    }

                    s32Ret = g_st_XC_FaceRecognition.pfnXC_FaceRecognition_Detect(pstFrInstance->frHandle, (const char*)pstFrInstance->pFdBufVirAddr, \
                                                        stDstBuf.u32Width, stDstBuf.u32Height, 4, \
                                                        &frParams, &outputbox, &facecount);
                    if(FR_SCECCSS != s32Ret || facecount <= 0)
                    {
                        //ST_DBG("XC_FaceRecognition_Detect ret = %d, facecount = %d\n", s32Ret, facecount);
                        if(bOsdShowed)
                        {
                            bOsdShowed = FALSE;
                            ST_OSD_GetCanvasInfo(pstFrInstance->rgnHandle, &pstCanvasInfo);
                            ST_OSD_Clear(pstFrInstance->rgnHandle, &pstFrInstance->rgnRect);
                            ST_OSD_Update(pstFrInstance->rgnHandle);
                        }
                        break;
                    }
                    //ST_DBG("XC_FaceRecognition_Detect OK\n");
                #ifdef DBG_SAVE_FR_FILE
                    ST_SaveFile("/tmp/fr_dbg_320_256.argb", pstFrInstance->pFdBufVirAddr, pstFrInstance->u32BuffSize);
                #endif

                    bOsdShowed = TRUE;
                    ST_OSD_GetCanvasInfo(pstFrInstance->rgnHandle, &pstCanvasInfo);
                    ST_OSD_Clear(pstFrInstance->rgnHandle, &pstFrInstance->rgnRect);

                    //printf("@@@Face detect ret = %d, facecount = %d\n", s32Ret, facecount);
                    for(i = 0; i < facecount; i++)
                    {
                        tofeature.faceID = outputbox[i].faceID;
                        tofeature.x1 = outputbox[i].x1 * pstFrInstance->u16RgnCropWidth / FACE_DETECT_IMG_WIDTH;
                        tofeature.y1 = outputbox[i].y1 * pstFrInstance->u16RgnCropHeight / FACE_DETECT_IMG_HEIGHT;
                        tofeature.x2 = outputbox[i].x2 * pstFrInstance->u16RgnCropWidth / FACE_DETECT_IMG_WIDTH;
                        tofeature.y2 = outputbox[i].y2 * pstFrInstance->u16RgnCropHeight / FACE_DETECT_IMG_HEIGHT;

                        stFaceFrame.stPointLU.u32X = tofeature.x1 * fRgnSclTimesX;
                        stFaceFrame.stPointLU.u32Y = tofeature.y1 * fRgnSclTimesY;
                        stFaceFrame.stPointRD.u32X = tofeature.x2 * fRgnSclTimesX;
                        stFaceFrame.stPointRD.u32Y = tofeature.y2 * fRgnSclTimesY;
                        stFaceFrame.u32BorderLen = ST_RECT_BORDER_LENGTH;
                        stFaceFrame.u32Color = ST_RECT_COLOR;
                        ST_FaceFrameRendering(pstFrInstance->rgnHandle, &stFaceFrame);

                        if(curFaceID != tofeature.faceID || abs(lastRecogTime - time(NULL)) > 1 ||\
                            TRUE == pstFrInstance->bAddPerson)
                        {
                            size = 1.2 * MAX(tofeature.x2 - tofeature.x1, tofeature.y2 - tofeature.y1);
                            centerx = (tofeature.x1 + tofeature.x2) / 2;
                            centery = (tofeature.y1 + tofeature.y2) / 2;
                            landmarkbox.x1 = MAX((centerx - size / 2), 0.f);
                            landmarkbox.y1 = MAX((centery - size / 2), 0.f);
                            landmarkbox.x2 = MIN((centerx + size / 2), stSrcBuf.u32Width * 1.f);
                            landmarkbox.y2 = MIN((centery + size / 2), stSrcBuf.u32Height * 1.f);
                            /* Source crop */
                            stSrcCrop.u16X = (MI_U16)landmarkbox.x1;
                            stSrcCrop.u16Y = (MI_U16)landmarkbox.y1;
                            stSrcCrop.u16Width = (MI_U16)(landmarkbox.x2 - landmarkbox.x1);
                            stSrcCrop.u16Height = (MI_U16)(landmarkbox.y2 - landmarkbox.y1);
                            /* Stretch destination buf */
                            stDstBuf.ePixelFormat = pstFrInstance->ePixelFormat;
                            stDstBuf.u32Width = 112;
                            stDstBuf.u32Height = 112;
                            stDstBuf.u32Stride[0] = stDstBuf.u32Stride[1] = stDstBuf.u32Width * 4;
                            stDstBuf.phyAddr[0] = pstFrInstance->u64FdBufPhyAddr;
                            stDstBuf.phyAddr[1] = pstFrInstance->u64FdBufPhyAddr + stDstBuf.u32Height * stDstBuf.u32Stride[0];
                            stDstBuf.u32BuffSize = pstFrInstance->u32BuffSize;
                            if(MI_SUCCESS != MI_SCL_StretchBuf(&stSrcBuf, &stSrcCrop, &stDstBuf, E_MI_SCL_FILTER_TYPE_AUTO))
                            {
                                ST_ERR("MI_SCL_StretchBuf failed!\n");
                                break;
                            }
                            g_st_XC_FaceRecognition.pfnXC_FaceLandmark_Detect(pstFrInstance->frHandle, (const char*)pstFrInstance->pFdBufVirAddr, \
                                                        stDstBuf.u32Width, stDstBuf.u32Height, 4, &landmarkbox);
                        #ifdef DBG_SAVE_FR_FILE
                            int j = 0;
                            for(; j < 5; j++)
                            {
                                printf("P[%d] @(%f, %f)\n", j, landmarkbox.Landmark[j].x, landmarkbox.Landmark[j].y);
                            }
                            ST_SaveFile("/tmp/fr_lm_dbg_112_112.argb", pstFrInstance->pFdBufVirAddr, stDstBuf.u32Width * stDstBuf.u32Height * 4);
                        #endif

                            if(tofeature.Befiltered)
                            {
                                continue;
                            }
                        #ifdef DBG_SAVE_FR_FILE
                            snprintf(frDbgFile, sizeof(frDbgFile), "/tmp/fr_dbg_%d_%d.argb", stSrcBuf.u32Width, stSrcBuf.u32Height);
                            ST_SaveFile(frDbgFile, (unsigned char*)stBufInfo.stFrameData.pVirAddr[0], stSrcBuf.u32Width * stSrcBuf.u32Height * 4);
                        #endif
                            g_st_XC_FaceRecognition.pfnXC_Crop112x112_BGRA((unsigned char*)stBufInfo.stFrameData.pVirAddr[0], stSrcBuf.u32Width, stSrcBuf.u32Height, 4, landmarkbox, outmatdata);
                        #ifdef DBG_SAVE_FR_FILE
                            ST_SaveFile("/tmp/fr_dbg_112_112.argb", outmatdata, sizeof(outmatdata));
                        #endif
                            s32Ret = g_st_XC_FaceRecognition.pfnXC_FaceRecegnition_FeatureExtract(pstFrInstance->frHandle, (const char *)outmatdata, 112, 112, 4, tofeature, feature);
                            if(s32Ret != 0)
                            {
                                printf("FR invoke error:%d\n", s32Ret);
                                continue;
                            }
                            lastRecogTime = time(NULL);
                            if(TRUE == pstFrInstance->bAddPerson)
                            {
                                face_db.AddPersonFeature(pstFrInstance->personName, feature);
                                face_db.SaveToFileBinary(FACE_DB_FEAT_FILE_PATH, FACE_DB_NAME_LIST_PATH);
                                memset(pstFrInstance->personName, 0, sizeof(pstFrInstance->personName));
                                pstFrInstance->bAddPerson = FALSE;
                            }
                            if(face_db.persons.size() > 0)
                            {
                                ST_FRFindName(face_db, feature, name, sizeof(name));
                            }
                            curFaceID = tofeature.faceID;
                        }
                        stTextPoint.u32X = stFaceFrame.stPointLU.u32X + 5;
                        stTextPoint.u32Y = stFaceFrame.stPointLU.u32Y + 5;
                        ST_OSD_DrawText(pstFrInstance->rgnHandle, stTextPoint, name, ST_RECT_COLOR, DMF_Font_Size_32x32);
                    }

                    ST_OSD_Update(pstFrInstance->rgnHandle);
                }while(0);
                /*Free frame*/
                s32Ret = MI_SYS_ChnOutputPortPutBuf(hSysBufHandle);
                if(s32Ret != MI_SUCCESS)
                {
                    ST_ERR("MI_SYS_ChnOutputPortPutBuf failed.s32Ret:0x%x !\n", s32Ret);
                }
            }
            else
            {
                ST_ERR("MI_SYS_ChnOutputPortGetBuf failed!\n");
            }
        }
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    }
    pthread_cleanup_pop(1);
    MI_SYS_CloseFd(s32Fd);

    return NULL;
}

#define ST_FR_DLSYM_DO(handle, name, pfunc) \
do \
{ \
    (pfunc) = (typeof(pfunc))dlsym((handle), (name)); \
    if(NULL == (pfunc)) \
    { \
        printf("[%s:%d] dlsym failed, %s\n", __func__, __LINE__, dlerror()); \
        goto errExit; \
    } \
}while(0);

int ST_FRInit()
{
    if(0 != access(IPU_FIRMWARE_PATH, F_OK) || \
        0 != access(ALG_MODEL_PATH, F_OK) || \
        0 != access(FR_SDK_LIB_PATH, F_OK))
    {
        return -1;
    }

    g_st_XC_FaceRecognition.pHandle = dlopen(FR_SDK_LIB_PATH, RTLD_NOW);
    if (NULL == g_st_XC_FaceRecognition.pHandle)
    {
        printf(" %s: Can not load %s!\n", __func__, FR_SDK_LIB_PATH);
        return -1;
    }
    ST_FR_DLSYM_DO(g_st_XC_FaceRecognition.pHandle, "XC_FaceRecognition_Init", g_st_XC_FaceRecognition.pfnXC_FaceRecognition_Init);
    ST_FR_DLSYM_DO(g_st_XC_FaceRecognition.pHandle, "XC_FaceRecognition_CreateHandle", g_st_XC_FaceRecognition.pfnXC_FaceRecognition_CreateHandle);
    ST_FR_DLSYM_DO(g_st_XC_FaceRecognition.pHandle, "XC_FaceRecognition_Detect", g_st_XC_FaceRecognition.pfnXC_FaceRecognition_Detect);
    ST_FR_DLSYM_DO(g_st_XC_FaceRecognition.pHandle, "XC_FaceLandmark_Detect", g_st_XC_FaceRecognition.pfnXC_FaceLandmark_Detect);
    ST_FR_DLSYM_DO(g_st_XC_FaceRecognition.pHandle, "XC_Crop112x112_BGRA", g_st_XC_FaceRecognition.pfnXC_Crop112x112_BGRA);
    ST_FR_DLSYM_DO(g_st_XC_FaceRecognition.pHandle, "XC_FaceRecognition_Antispoofing", g_st_XC_FaceRecognition.pfnXC_FaceRecognition_Antispoofing);
    ST_FR_DLSYM_DO(g_st_XC_FaceRecognition.pHandle, "XC_FaceRecegnition_FeatureExtract", g_st_XC_FaceRecognition.pfnXC_FaceRecegnition_FeatureExtract);
    ST_FR_DLSYM_DO(g_st_XC_FaceRecognition.pHandle, "XC_FaceRecognition_FeatureCompare", g_st_XC_FaceRecognition.pfnXC_FaceRecognition_FeatureCompare);
    ST_FR_DLSYM_DO(g_st_XC_FaceRecognition.pHandle, "XC_FaceRecognition_ReleaseHandle", g_st_XC_FaceRecognition.pfnXC_FaceRecognition_ReleaseHandle);
    ST_FR_DLSYM_DO(g_st_XC_FaceRecognition.pHandle, "XC_FaceRecognition_Cleanup", g_st_XC_FaceRecognition.pfnXC_FaceRecognition_Cleanup);

    return 0;

errExit:
    if(NULL != g_st_XC_FaceRecognition.pHandle)
    {
        dlclose(g_st_XC_FaceRecognition.pHandle);
        g_st_XC_FaceRecognition.pHandle = NULL;
    }
    return -1;
}

MI_S32 ST_FRStart(MI_SCL_DEV sclDev, MI_SCL_CHANNEL sclChn,
                        MI_U16 u16SrcWidth, MI_U16 u16SrcHeight,
                        MI_SCL_PORT rgnPort, MI_SCL_PORT capPort)
{
    MI_S32 s32Ret, s32Frid = 0;
    MI_SYS_ChnPort_t stChnPort;
    MI_SCL_OutPortParam_t stOutPortParam;
    MI_RGN_Attr_t stRegion;
    MI_RGN_ChnPort_t stRgnChnPort;
    MI_RGN_ChnPortParam_t stRgnChnPortParam;

    if(NULL == g_st_XC_FaceRecognition.pHandle)
    {
    	printf("Not support face recognition or g_st_XC_FaceRecognition do not be init yet.\n");
        return -1;
    }

    s32Frid = ST_GetFrInstance();
    if(s32Frid < 0)
    {
        return -1;
    }

    if(FALSE == g_bIpuStart)
    {
        s32Ret = g_st_XC_FaceRecognition.pfnXC_FaceRecognition_Init(IPU_FIRMWARE_PATH, ALG_MODEL_PATH);
        if(s32Ret != 0)
        {
            ST_ERR("XC_FaceRecognition init fail, s32Ret = %d\n", s32Ret);
            ST_FreeFrInstance(s32Frid);
            return s32Ret;
        }
        g_bIpuStart = TRUE;
    }
    printf("@@@ XC_FaceRecognition_Init OK!!!\n");
    s32Ret = g_st_XC_FaceRecognition.pfnXC_FaceRecognition_CreateHandle(&gStFrInstance[s32Frid].frHandle);
    if(0 != s32Ret)
    {
        ST_ERR("XC_FaceRecognition_CreateHandle err:%x\n", s32Ret);
        ST_FreeFrInstance(s32Frid);
        g_st_XC_FaceRecognition.pfnXC_FaceRecognition_Cleanup();
        return s32Ret;
    }

    STCHECKRESULT(MI_SCL_GetOutputPortParam(sclDev, sclChn, rgnPort, &stOutPortParam));
    gStFrInstance[s32Frid].rgnRect.u32X = 0;
    gStFrInstance[s32Frid].rgnRect.u32Y = 0;
    gStFrInstance[s32Frid].rgnRect.u16PicW = stOutPortParam.stSCLOutputSize.u16Width;
    gStFrInstance[s32Frid].rgnRect.u16PicH = stOutPortParam.stSCLOutputSize.u16Height;
    gStFrInstance[s32Frid].u16RgnCropWidth = stOutPortParam.stSCLOutCropRect.u16Width;
    gStFrInstance[s32Frid].u16RgnCropHeight = stOutPortParam.stSCLOutCropRect.u16Height;
    STCHECKRESULT(MI_SCL_DisableOutputPort(sclDev, sclChn, capPort));
    stOutPortParam.ePixelFormat = E_MI_SYS_PIXEL_FRAME_ARGB8888;
    stOutPortParam.stSCLOutputSize.u16Width = stOutPortParam.stSCLOutCropRect.u16Width;
    stOutPortParam.stSCLOutputSize.u16Height = stOutPortParam.stSCLOutCropRect.u16Height;
    STCHECKRESULT(MI_SCL_SetOutputPortParam(sclDev, sclChn, capPort, &stOutPortParam));
    STCHECKRESULT(MI_SCL_EnableOutputPort(sclDev, sclChn, capPort));
    stChnPort.eModId = E_MI_MODULE_ID_SCL;
    stChnPort.u32DevId = sclDev;
    stChnPort.u32ChnId = sclChn;
    stChnPort.u32PortId = capPort;
    STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(0, &stChnPort , 2, 4));

    gStFrInstance[s32Frid].rgnHandle = s32Frid;
    ST_OSD_Init();
    stRegion.eType = E_MI_RGN_TYPE_OSD;
    stRegion.stOsdInitParam.ePixelFmt = E_MI_RGN_PIXEL_FORMAT_I4;
    stRegion.stOsdInitParam.stSize.u32Width = gStFrInstance[s32Frid].rgnRect.u16PicW;
    stRegion.stOsdInitParam.stSize.u32Height = gStFrInstance[s32Frid].rgnRect.u16PicH;
    ST_OSD_Create(gStFrInstance[s32Frid].rgnHandle, &stRegion);
    stRgnChnPort.eModId = E_MI_MODULE_ID_SCL;
    stRgnChnPort.s32DevId = sclDev;
    stRgnChnPort.s32ChnId = sclChn;
    stRgnChnPort.s32PortId = rgnPort;
    memset(&stRgnChnPortParam, 0, sizeof(MI_RGN_ChnPortParam_t));
    stRgnChnPortParam.bShow = TRUE;
    stRgnChnPortParam.stPoint.u32X = 0;
    stRgnChnPortParam.stPoint.u32Y = 0;
    stRgnChnPortParam.unPara.stOsdChnPort.u32Layer = 99;
    STCHECKRESULT(MI_RGN_AttachToChn(0, gStFrInstance[s32Frid].rgnHandle, &stRgnChnPort, &stRgnChnPortParam));

    gStFrInstance[s32Frid].sclDev = sclDev;
    gStFrInstance[s32Frid].sclChn = sclChn;
    gStFrInstance[s32Frid].rgnPort = rgnPort;
    gStFrInstance[s32Frid].u16SrcWidth = u16SrcWidth;
    gStFrInstance[s32Frid].u16SrcHeight = u16SrcHeight;
    gStFrInstance[s32Frid].capPort = capPort;
    gStFrInstance[s32Frid].ePixelFormat = stOutPortParam.ePixelFormat;
    gStFrInstance[s32Frid].u32Stride = FACE_DETECT_IMG_WIDTH * 4;
    gStFrInstance[s32Frid].u32BuffSize = FACE_DETECT_IMG_HEIGHT * gStFrInstance[s32Frid].u32Stride;
    STCHECKRESULT(MI_SYS_MMA_Alloc(0, NULL, gStFrInstance[s32Frid].u32BuffSize, &gStFrInstance[s32Frid].u64FdBufPhyAddr));
    STCHECKRESULT(MI_SYS_Mmap(gStFrInstance[s32Frid].u64FdBufPhyAddr, gStFrInstance[s32Frid].u32BuffSize, &gStFrInstance[s32Frid].pFdBufVirAddr, FALSE));

    gStFrInstance[s32Frid].bAddPerson = FALSE;
    memset(gStFrInstance[s32Frid].personName, 0, sizeof(gStFrInstance[s32Frid].personName));

    pthread_create(&gStFrInstance[s32Frid].capTid, NULL, &ST_FrTask, (void *)&gStFrInstance[s32Frid]);

    return s32Frid;
}

MI_S32 ST_FRAddPerson(MI_S32 s32Frid, char *args)
{
    char *pName = NULL, *pPicPath = NULL;

    if(NULL == g_st_XC_FaceRecognition.pHandle)
    {
    	printf("Not support face recognition or g_st_XC_FaceRecognition do not be init yet.\n");
        return -1;
    }

    if(s32Frid < 0 || s32Frid > MAX_FR_INSTANCE || NULL == args)
    {
        return -1;
    }

    //printf("ST_FRAddPerson, frid = %d, args is: %s\n", s32Frid, args);
    pName = args;
    if(NULL == (pPicPath = strchr(args, ' ')))
    {
        //gStFrInstance[s32Frid].frHandle;
        printf("Add current person.\n");
        if(TRUE == gStFrInstance[s32Frid].bAddPerson)
        {
            printf("Adding person now.\n");
            return -1;
        }
        snprintf(gStFrInstance[s32Frid].personName, sizeof(gStFrInstance[s32Frid].personName), "%s", pName);
        gStFrInstance[s32Frid].bAddPerson = TRUE;
        return MI_SUCCESS;
    }

    *pPicPath = '\0';
    pPicPath++;
    if(0 != access(pPicPath, F_OK))
    {
        printf("File[%s] not exist.\n", pPicPath);
        return -1;
    }
    // TODO: Add person by picture.
    return MI_SUCCESS;
}

MI_S32 ST_FRStop(MI_S32 s32Frid)
{
    MI_RGN_ChnPort_t stRgnChnPort;

    if(NULL == g_st_XC_FaceRecognition.pHandle)
    {
    	printf("Not support face recognition or g_st_XC_FaceRecognition do not be init yet.\n");
        return -1;
    }

    pthread_mutex_lock(&gStFrLock);
    if(!gStFrInstance[s32Frid].bUsed)
    {
        pthread_mutex_unlock(&gStFrLock);
        return -1;
    }
    pthread_mutex_unlock(&gStFrLock);

    pthread_cancel(gStFrInstance[s32Frid].capTid);
    pthread_join(gStFrInstance[s32Frid].capTid, NULL);

    MI_SYS_MMA_Free(0, gStFrInstance[s32Frid].u64FdBufPhyAddr);

    STCHECKRESULT(MI_SCL_DisableOutputPort(gStFrInstance[s32Frid].sclDev, gStFrInstance[s32Frid].sclChn, gStFrInstance[s32Frid].capPort));

    stRgnChnPort.eModId = E_MI_MODULE_ID_SCL;
    stRgnChnPort.s32DevId = gStFrInstance[s32Frid].sclDev;
    stRgnChnPort.s32ChnId = gStFrInstance[s32Frid].sclChn;
    stRgnChnPort.s32PortId = gStFrInstance[s32Frid].rgnPort;
    STCHECKRESULT(MI_RGN_DetachFromChn(0, gStFrInstance[s32Frid].rgnHandle, &stRgnChnPort));
    ST_OSD_Destroy(gStFrInstance[s32Frid].rgnHandle);
    ST_OSD_Deinit();

    g_st_XC_FaceRecognition.pfnXC_FaceRecognition_ReleaseHandle(gStFrInstance[s32Frid].frHandle);
    if(0 == ST_FreeFrInstance(s32Frid))
    {
        g_st_XC_FaceRecognition.pfnXC_FaceRecognition_Cleanup();
    }
    g_bIpuStart = FALSE;

    return MI_SUCCESS;
}

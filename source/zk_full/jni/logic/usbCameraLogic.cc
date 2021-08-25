#pragma once
#include "uart/ProtocolSender.h"
/*
*此文件由GUI工具生成
*文件功能：用于处理用户的逻辑相应代码
*功能说明：
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数，XXX代表GUI工具里面的[ID值]名称，
如Button1,当返回值为false的时候系统将不再处理这个按键，返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index) 
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress) 
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX() 
当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[ID值]名称，
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[ID值]名称，
如List1;pListItem 是贴图中的单条目对象，index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXXPtr->setText("****") 在控件TextXXX上显示文字****
*mButton1Ptr->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBarPtr->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1Ptr->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1Ptr->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*
* 在Eclipse编辑器中  使用 “alt + /”  快捷键可以打开智能提示
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <dlfcn.h>
#include "mi_disp.h"
#include "mi_scl.h"
#include "panelconfig.h"
#include "jdec.h"
#include "v4l2.h"
#include "libyuv.h"
#include "sstar_dynamic_load.h"
#include "statusbarconfig.h"
#include "st_common.h"

#define VMAX(a,b) ((a) > (b) ? (a) : (b))
#define VMIN(a,b) ((a) < (b) ? (a) : (b))

#define STCHECKRESULT(_func_)\
    do{ \
        MI_S32 s32Ret = MI_SUCCESS; \
        s32Ret = _func_; \
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

#define ENABLE_SCL 1

#define YUV420      3/2
#define YUV422      2
#define YUV_TYPE    YUV422

#define TIMEUOT     100

#define TIME_DIFF_PRE_FRAME         33 * 1000

#define DIVP_CAP_MAX_WIDTH          1920
#define DIVP_CAP_MAX_HEIGHT         1080
#define DISP_OUT_MAX_WIDTH          PANEL_WIDTH
#define DISP_OUT_MAX_HEIGHT         PANEL_HEIGHT

#define CAMERA_VIDEO_WIDTH_MJPEG    1024
#define CAMERA_VIDEO_HEIGHT_MJPEG   576

#define SIZE_BUFFER_YUV             CAMERA_VIDEO_WIDTH_MJPEG * CAMERA_VIDEO_HEIGHT_MJPEG * YUV_TYPE

typedef struct
{
	void *pHandle;
	int (*pfnLibyuvI422ToYUY2)(const uint8_t* src_y,
							  int src_stride_y,
							  const uint8_t* src_u,
							  int src_stride_u,
							  const uint8_t* src_v,
							  int src_stride_v,
							  uint8_t* dst_yuy2,
							  int dst_stride_yuy2,
							  int width,
							  int height);
	int (*pfnLibyuvI420ToNV12)(const uint8_t* src_y,
							  int src_stride_y,
							  const uint8_t* src_u,
							  int src_stride_u,
							  const uint8_t* src_v,
							  int src_stride_v,
							  uint8_t* dst_y,
							  int dst_stride_y,
							  uint8_t* dst_uv,
							  int dst_stride_uv,
							  int width,
							  int height);
} LibyuvAssembly_t;

static LibyuvAssembly_t g_stLibyuvAssembly;

int SSTAR_LIBYUV_OpenLibrary(LibyuvAssembly_t *pstLibyuvAssembly)
{
	pstLibyuvAssembly->pHandle = dlopen("lib/libyuv.so", RTLD_NOW);		// libcus3a.so  ?
	if (NULL == pstLibyuvAssembly->pHandle)
	{
		printf(" %s: Can not load libyuv.so!\n", __func__);
		return -1;
	}

	pstLibyuvAssembly->pfnLibyuvI422ToYUY2 = (int(*)(const uint8_t* src_y,
													  int src_stride_y,
													  const uint8_t* src_u,
													  int src_stride_u,
													  const uint8_t* src_v,
													  int src_stride_v,
													  uint8_t* dst_yuy2,
													  int dst_stride_yuy2,
													  int width,
													  int height))dlsym(pstLibyuvAssembly->pHandle, "I422ToYUY2");	// "libyuv::I422UY2"
	if(NULL == pstLibyuvAssembly->pfnLibyuvI422ToYUY2)
	{
		printf(" %s: dlsym libyuv::I422ToYUY2 failed, %s\n", __func__, dlerror());
		return -1;
	}

	pstLibyuvAssembly->pfnLibyuvI420ToNV12 = (int(*)(const uint8_t* src_y,
											  int src_stride_y,
											  const uint8_t* src_u,
											  int src_stride_u,
											  const uint8_t* src_v,
											  int src_stride_v,
											  uint8_t* dst_y,
											  int dst_stride_y,
											  uint8_t* dst_uv,
											  int dst_stride_uv,
											  int width,
											  int height))dlsym(pstLibyuvAssembly->pHandle, "I420ToNV12");	// "libyuv::I420ToNV12"
	if(NULL == pstLibyuvAssembly->pfnLibyuvI420ToNV12)
	{
		printf(" %s: dlsym libyuv::I420ToNV12 failed, %s\n", __func__, dlerror());
		return -1;
	}

	return 0;
}

void SSTAR_LIBYUV_CloseLibrary(LibyuvAssembly_t *pstLibyuvAssembly)
{
	if(pstLibyuvAssembly->pHandle)
	{
		dlclose(pstLibyuvAssembly->pHandle);
		pstLibyuvAssembly->pHandle = NULL;
	}
	memset(pstLibyuvAssembly, 0, sizeof(*pstLibyuvAssembly));
}


int _sys_mma_alloc(jdecIMAGE *pImage, int size)
{
    if (0 != MI_SYS_MMA_Alloc(0, (unsigned char*)"#jdecI0", size, &(pImage->phyAddr)))
    {
        printf("MI_SYS_MMA_Alloc Failed\n");
        return -1;
    }
    if (0 != MI_SYS_Mmap(pImage->phyAddr, size, (void **)&(pImage->virtAddr), 1))
    {
        MI_SYS_MMA_Free(0, pImage->phyAddr);
        printf("MI_SYS_Mmap Failed\n");
        return -1;
    }

    return 0;
}

int _sys_mma_free(jdecIMAGE *pImage, int size)
{
    if (!pImage->virtAddr && !pImage->phyAddr)
        return -1;

    if (0 != MI_SYS_Munmap(pImage->virtAddr, size))
    {
    	printf("MI_SYS_Munmap Failed\n");
        return -1;
    }
    if (0 != MI_SYS_MMA_Free(0, pImage->phyAddr))
    {
    	printf("MI_SYS_MMA_Free Failed\n");
        return -1;
    }

    return 0;
}

int display_init(void)
{
	MI_SCL_DEV SclDevId = 1;
	MI_SCL_CHANNEL SclChnId = 0;
	MI_SCL_PORT SclOutPortId = 0;
	MI_SYS_ChnPort_t stChnPort;
	MI_SCL_DevAttr_t stCreateDevAttr;
	MI_SCL_ChannelAttr_t stSclChnAttr;
	MI_SCL_ChnParam_t stSclChnParam;
	MI_SCL_OutPortParam_t stSclOutputParam;
	MI_DISP_InputPortAttr_t stInputPortAttr;
	ST_Sys_BindInfo_T stBindInfo;

	STCHECKRESULT(ST_Sys_Init());

	memset(&stChnPort, 0, sizeof(MI_SYS_ChnPort_t));
	memset(&stCreateDevAttr, 0, sizeof(MI_SCL_DevAttr_t));
	memset(&stSclChnAttr, 0, sizeof(MI_SCL_ChannelAttr_t));
	memset(&stSclChnParam, 0, sizeof(MI_SCL_ChnParam_t));
	memset(&stSclOutputParam, 0, sizeof(MI_SCL_OutPortParam_t));
	stChnPort.eModId = E_MI_MODULE_ID_SCL;
	stChnPort.u32DevId = SclDevId;
	stChnPort.u32ChnId = SclChnId;
	stChnPort.u32PortId = SclOutPortId;
	stCreateDevAttr.u32NeedUseHWOutPortMask = E_MI_SCL_HWSCL0;
//	stSclOutputParam.stSCLOutCropRect.u16X = 0;
//	stSclOutputParam.stSCLOutCropRect.u16Y = 0;
//	stSclOutputParam.stSCLOutCropRect.u16Width = DISP_OUT_MAX_WIDTH;
//	stSclOutputParam.stSCLOutCropRect.u16Height = DISP_OUT_MAX_HEIGHT;
	stSclOutputParam.stSCLOutputSize.u16Width = DISP_OUT_MAX_WIDTH;
	stSclOutputParam.stSCLOutputSize.u16Height = DISP_OUT_MAX_HEIGHT;
	stSclOutputParam.bMirror = 0;
	stSclOutputParam.bFlip = 0;
	stSclOutputParam.eCompressMode= E_MI_SYS_COMPRESS_MODE_NONE;
	stSclOutputParam.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
	MI_SCL_CreateDevice(SclDevId, &stCreateDevAttr);
	MI_SCL_CreateChannel(SclDevId, SclChnId, &stSclChnAttr);
	MI_SCL_SetChnParam(SclDevId, SclChnId, &stSclChnParam);
	MI_SCL_StartChannel(SclDevId, SclChnId);
	MI_SCL_SetOutputPortParam(SclDevId, SclChnId, SclOutPortId, &stSclOutputParam);
	MI_SYS_SetChnOutputPortDepth(0, &stChnPort , 0, 4);
	MI_SCL_EnableOutputPort(SclDevId, SclChnId, SclOutPortId);

	memset(&stInputPortAttr, 0, sizeof(MI_DISP_InputPortAttr_t));
	MI_DISP_GetInputPortAttr(0, 0, &stInputPortAttr);
	stInputPortAttr.stDispWin.u16Height = DISP_OUT_MAX_HEIGHT;
	stInputPortAttr.stDispWin.u16Width = DISP_OUT_MAX_WIDTH;
	stInputPortAttr.stDispWin.u16X = 0;
	stInputPortAttr.stDispWin.u16Y = 0;
	stInputPortAttr.u16SrcHeight = DISP_OUT_MAX_HEIGHT;
	stInputPortAttr.u16SrcWidth = DISP_OUT_MAX_WIDTH;
	MI_DISP_DisableInputPort(0, 0);
    MI_DISP_SetInputPortAttr(0, 0, &stInputPortAttr);
    MI_DISP_EnableInputPort(0, 0);
    MI_DISP_SetInputPortSyncMode(0, 0, E_MI_DISP_SYNC_MODE_FREE_RUN);

    memset(&stBindInfo, 0, sizeof(ST_Sys_BindInfo_T));
	stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
	stBindInfo.stSrcChnPort.u32DevId = SclDevId;
	stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
	stBindInfo.stSrcChnPort.u32PortId = SclOutPortId;
	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
	stBindInfo.stDstChnPort.u32DevId = 0;
	stBindInfo.stDstChnPort.u32ChnId = 0;
	stBindInfo.stDstChnPort.u32PortId = 0;
	stBindInfo.u32SrcFrmrate = 30;
	stBindInfo.u32DstFrmrate = 30;
	stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
	ST_Sys_Bind(&stBindInfo);

    return 0;
}

int display_deinit()
{
    ST_Sys_BindInfo_T stBindInfo;

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
	stBindInfo.stSrcChnPort.u32DevId = 1;
	stBindInfo.stSrcChnPort.u32ChnId = 0;
	stBindInfo.stSrcChnPort.u32PortId = 0;
	stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
	stBindInfo.stDstChnPort.u32DevId = 0;
	stBindInfo.stDstChnPort.u32ChnId = 0;
	stBindInfo.stDstChnPort.u32PortId = 0;
	STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
	STCHECKRESULT(MI_DISP_DisableInputPort(0, 0));
	STCHECKRESULT(MI_SCL_DisableOutputPort(1, 0, 0));
	STCHECKRESULT(MI_SCL_StopChannel(1, 0));
	STCHECKRESULT(MI_SCL_DestroyChannel(1, 0));

    return 0;
}

struct timeval time_start, time_end;
int64_t time0;

int jdec_convert_yuv_fotmat(jdecIMAGE *pImage, unsigned char *pBuf[3])
{
    unsigned char* pu8STSrcU = NULL;
    unsigned char* pu8STSrcV = NULL;
    //FILE *jpg_fd = NULL;

    if(pImage->esubsamp == SAMP_422)        //divp 只支持YUV422_YUYV      YV16-->420
    {
        pu8STSrcU = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height;
        pu8STSrcV = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height + pImage->width * pImage->height / 2;

        g_stLibyuvAssembly.pfnLibyuvI422ToYUY2((unsigned char*)pImage->virtAddr, pImage->width,
							pu8STSrcU, pImage->width / 2,
							pu8STSrcV, pImage->width / 2,
							(uint8_t*)pBuf[0], pImage->width * 2,
							pImage->width, pImage->height);

        #if 0
        ST_INFO("YUV Type Is SAMP_422, Image Size = %d\n", pImage->width * pImage->height);

        jpg_fd = fopen("/mnt/mjpeg/dump_422.yuv", "w+");
        fwrite(pBuf[0], 1, pImage->width * pImage->height * 2, jpg_fd);
        fclose(jpg_fd);
        #endif

        return 0;
    }
    else if(pImage->esubsamp == SAMP_420)   //divp 只支持NV12（YUV420sp）
    {
        pu8STSrcU = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height;
        pu8STSrcV = (unsigned char*)pImage->virtAddr + pImage->width * pImage->height + pImage->width * pImage->height / 2 / 2;

        g_stLibyuvAssembly.pfnLibyuvI420ToNV12((unsigned char*)pImage->virtAddr, pImage->width,
							pu8STSrcU, pImage->width / 2,
							pu8STSrcV, pImage->width / 2,
							(uint8_t *)pBuf[0], pImage->width,
							(uint8_t *)pBuf[1], pImage->width,
							pImage->width, pImage->height);

        #if 0
        ST_INFO("YUV Type Is SAMP_420, Image Size = %d\n", pImage->width * pImage->height);

        jpg_fd = fopen("/mnt/mjpeg/dump_420.yuv", "w+");
        fwrite(pBuf[0], 1, pImage->width * pImage->height, jpg_fd);
        fwrite(pBuf[1], 1, pImage->width * pImage->height / 2, jpg_fd);
        fclose(jpg_fd);
        #endif

        return 0;
    }
    else
    {
        return -1;
    }
}

int send_yuv_to_display(jdecIMAGE *pImage)
{
    MI_SYS_BufInfo_t stBufInfo;
    MI_SYS_BufConf_t stBufConf;
    MI_SYS_BUF_HANDLE stBufHandle;
    MI_SYS_ChnPort_t stInputChnPort;
    struct timeval stTv;
    MI_S32 Res;

    memset(&stBufInfo, 0, sizeof(MI_SYS_BufInfo_t));
    memset(&stBufConf, 0, sizeof(MI_SYS_BufConf_t));
    memset(&stInputChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    memset(&stBufHandle, 0, sizeof(MI_SYS_BUF_HANDLE));

    stInputChnPort.eModId    = E_MI_MODULE_ID_SCL;
    stInputChnPort.u32ChnId  = 0;
    stInputChnPort.u32DevId  = 1;
    stInputChnPort.u32PortId = 0;

    gettimeofday(&stTv, NULL);
    stBufConf.u64TargetPts   = stTv.tv_sec*1000000 + stTv.tv_usec;
    stBufConf.eBufType       = E_MI_SYS_BUFDATA_FRAME;
    stBufConf.u32Flags       = MI_SYS_MAP_VA;
    stBufConf.stFrameCfg.u16Width       = pImage->width; //CAMERA_VIDEO_WIDTH_MJPEG;
    stBufConf.stFrameCfg.u16Height      = pImage->height;//CAMERA_VIDEO_HEIGHT_MJPEG;
    stBufConf.stFrameCfg.eFrameScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;

    switch(pImage->esubsamp)//divp only support YUYV422&&YUV420SP
    {
        case SAMP_422://divp 只支持YUV422_YUYV      YV16-->420
        {
            stBufConf.stFrameCfg.eFormat       = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
            stBufInfo.stFrameData.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
        }
        break;

        case SAMP_420://divp 只支持NV12（YUV420sp）
        {
            stBufConf.stFrameCfg.eFormat       = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            stBufInfo.stFrameData.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        }
        break;

        default:
        {
        	printf("Not Support YUV Yype, Esubsamp [%d]\n", pImage->esubsamp);
            return -1;
        }
        break;
    }

    Res = MI_SYS_ChnInputPortGetBuf(&stInputChnPort, &stBufConf, &stBufInfo, &stBufHandle, -1);
    if(MI_SUCCESS == Res)
    {
        stBufInfo.stFrameData.eCompressMode = E_MI_SYS_COMPRESS_MODE_NONE;
        stBufInfo.stFrameData.eFieldType    = E_MI_SYS_FIELDTYPE_NONE;
        stBufInfo.stFrameData.eTileMode     = E_MI_SYS_FRAME_TILE_MODE_NONE;
        stBufInfo.bEndOfStream              = 0;

        //ST_INFO("Get Buf Info: Width/Height/Stride = [%d %d %d]\n", stBufInfo.stFrameData.u16Width, stBufInfo.stFrameData.u16Height, stBufInfo.stFrameData.u32Stride[0]);

        //gettimeofday(&time_start, NULL);

        if (0 != jdec_convert_yuv_fotmat(pImage, &stBufInfo.stFrameData.pVirAddr[0]))
        {
        	printf("jdec_convert_yuv_fotmat error!\n");
        }

        //gettimeofday(&time_end, NULL);
        //time0 = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
        //ST_WARN("yuv fotmat convert time = %lld\n", time0);

        if(MI_SUCCESS != MI_SYS_ChnInputPortPutBuf(stBufHandle, &stBufInfo, 0))
        {
        	printf("MI_SYS_ChnInputPortPutBuf Failed!\n");
            return -1;
        }
    }
    else
    {
    	printf("MI_SYS_ChnInputPortGetBuf Failed!Error:%#X\n", Res);
         return -1;
    }

    return 0;
}

static pthread_t jdec_tid;
static bool b_exit = false;
static DeviceContex_t *ctx = NULL;
static Packet pkt;

static void * jpeg_decoding_thread(void * args)
{
    int ret;
    int timeout = 0;
    int decode_cnt = 0;
    int yuv_size = 0;
    int img_width = 0, img_height = 0;
    bool DispalyInited = false;
    jdecIMAGE image0 = {0};

	if (SSTAR_TurboJpeg_OpenLibrary())
	{
		printf("open libturbojpeg failed\n");
		return NULL;
	}

    if (0 != _sys_mma_alloc(&image0, SIZE_BUFFER_YUV))
    {
    	printf("_sys_mma_alloc failed\n");
        goto error;
    }

    while (!b_exit)
    {
        ret = v4l2_read_packet(ctx, &pkt);
        if(ret < 0)
        {
            if(++ timeout < TIMEUOT)
            {
                usleep(1 * 1000);
                continue;
            }
            else
            {
            	printf("v4l2_read_packet timeout exit\n");
                goto error;
            }
        }
        timeout = 0;

        //save_file((char *)pkt.data, pkt.size, 3);

        //gettimeofday(&time_start, NULL);

        //读取内存数据解码得到yuv图像
        yuv_size = jdec_decode_yuv_from_buf((char *)pkt.data, pkt.size, &image0, TANSFORM_NONE, SAMP_420);
        if(yuv_size < 0)
        {
            printf("Decode done, yuv_size=%d, image w/h=[%d %d], decode_cnt=%d \n", yuv_size, image0.width, image0.height, decode_cnt);
            goto try_again;
        }
        else
        {
            //根据jpg图像大小设置divp/disp
            if (image0.width > 0 && image0.height > 0)
            {
                if (!img_width && !img_height)
                {
                	if(DispalyInited == false)
                	{
                		if(!display_init())
                		{
                			DispalyInited = true;
                		}
                		else
                		{
                			printf("displey init error\n");
                		}
                	}
                    img_width  = image0.width;
                    img_height = image0.height;
                }
            }
            else
            {
            	printf("image w/h=[%d %d] parameter invalid!\n", image0.width, image0.height);
                v4l2_read_packet_end(ctx, &pkt);
                goto error;
            }
            //ST_INFO("image w/h=[%d %d], jpeg size=%ld, yuv type = %d\n", image0.width, image0.height, one_jepg_size, image0.esubsamp);
        }

        //gettimeofday(&time_end, NULL);
        //time0 = ((int64_t)time_end.tv_sec * 1000000 + time_end.tv_usec) - ((int64_t)time_start.tv_sec * 1000000 + time_start.tv_usec);
        //ST_WARN("jpeg decoded time = %lld\n", time0);

        //将yuv数据送到disp/divp,或放大或缩小显示
        if(0 != send_yuv_to_display(&image0))
        {
            printf("send_yuv_to_display error\n");
            v4l2_read_packet_end(ctx, &pkt);
            goto error;
        }

        //save_file((char *)pkt.data, pkt.size, 2);

        decode_cnt ++;
        //ST_INFO("had decoded jpeg num = [%d]\n", decode_cnt);

try_again:
        v4l2_read_packet_end(ctx, &pkt);
    }

error:
    _sys_mma_free(&image0, SIZE_BUFFER_YUV);

    if (img_width || img_height)
    {
        display_deinit();
    }

    SSTAR_TurbpJpeg_CloseLibrary();

    return NULL;
}

/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	//{1,  1000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init(){
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
    printf("welcome to uvc_player!\n");

    if (SSTAR_LIBYUV_OpenLibrary(&g_stLibyuvAssembly))
    {
    	printf("open libyuv failed\n");
    	return;
    }

    v4l2_dev_init(&ctx,  (char*)"/dev/video0");
    v4l2_dev_set_fmt(ctx, V4L2_PIX_FMT_MJPEG, CAMERA_VIDEO_WIDTH_MJPEG, CAMERA_VIDEO_HEIGHT_MJPEG);

    if (0 != v4l2_read_header(ctx))
    {
        printf("Can't find usb camera\n");
        mTextView_open_failPtr->setVisible(true);
        return;
    }

    b_exit = false;
    if (0 != pthread_create(&jdec_tid, NULL, jpeg_decoding_thread, NULL))
    {
        printf("pthread_create failed\n");
        return;
    }
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
    }
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {

}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {

}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
    LOGD(" onUI_quit !!!\n");
    if (!g_stLibyuvAssembly.pHandle)
    	return;

    b_exit = true;
	if (jdec_tid)
	{
		pthread_join(jdec_tid, NULL);
		jdec_tid = 0;
	}

	if (ctx)
	{
		v4l2_read_close(ctx);
		v4l2_dev_deinit(ctx);
		ctx = NULL;
	}

	//STCHECKRESULT(MI_DISP_DisableInputPort(0, 0));
	MI_DISP_DeInitDev();

	SSTAR_LIBYUV_CloseLibrary(&g_stLibyuvAssembly);

    ShowStatusBar(1, 0, 0);
}

/**
 * 串口数据回调接口
 */
static void onProtocolDataUpdate(const SProtocolData &data) {

}

/**
 * 定时器触发函数
 * 不建议在此函数中写耗时操作，否则将影响UI刷新
 * 参数： id
 *         当前所触发定时器的id，与注册时的id相同
 * 返回值: true
 *             继续运行当前定时器
 *         false
 *             停止运行当前定时器
 */
static bool onUI_Timer(int id){
	switch (id) {

		default:
			break;
	}
    return true;
}

/**
 * 有新的触摸事件时触发
 * 参数：ev
 *         新的触摸事件
 * 返回值：true
 *            表示该触摸事件在此被拦截，系统不再将此触摸事件传递到控件上
 *         false
 *            触摸事件将继续传递到控件上
 */
static bool onusbCameraActivityTouchEvent(const MotionEvent &ev) {
    switch (ev.mActionStatus) {
		case MotionEvent::E_ACTION_DOWN://触摸按下
			//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
			break;
		case MotionEvent::E_ACTION_MOVE://触摸滑动
			break;
		case MotionEvent::E_ACTION_UP:  //触摸抬起
			break;
		default:
			break;
	}
	return false;
}
static bool onButtonClick_sys_back(ZKButton *pButton) {
    LOGD(" ButtonClick sys_back !!!\n");
    return false;
}

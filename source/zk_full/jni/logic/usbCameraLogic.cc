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
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <signal.h>
#include <fcntl.h>
#include <dlfcn.h>
#include "st_common.h"
#include "mi_jpd.h"
#include "mi_vdec.h"
#include "mi_scl.h"
#include "mi_disp.h"
#include "mi_sys.h"
#include "panelconfig.h"
#include "statusbarconfig.h"

typedef enum
{
    UVC_DBG_NONE = 0,
    UVC_DBG_ERR,
    UVC_DBG_WRN,
    UVC_DBG_INFO,
    UVC_DBG_DEBUG,
    UVC_DBG_TRACE,
    UVC_DBG_ALL
} UVC_DBG_LEVEL_e;

#define V4L2_PIX_FMT_H265 	  v4l2_fourcc('H', '2', '6', '5')

#define MIN(a,b) ((a) > (b) ? (b) : (a))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

#define UVC_TRACE(dev, fmt, args...) ({do{if(uvc_debug_level>=UVC_DBG_TRACE)\
                {printf(ASCII_COLOR_GREEN"[APP TRACE][dev:%s]:%s[%d]: " fmt ASCII_COLOR_END"\n", dev?dev->path:NULL, __FUNCTION__,__LINE__,##args);}}while(0);})
#define UVC_DEBUG(dev, fmt, args...) ({do{if(uvc_debug_level>=UVC_DBG_DEBUG)\
                {printf(ASCII_COLOR_GREEN"[APP DEBUG][dev:%s]:%s[%d]: " fmt ASCII_COLOR_END"\n", dev?dev->path:NULL, __FUNCTION__,__LINE__,##args);}}while(0);})
#define UVC_INFO(dev, fmt, args...)     ({do{if(uvc_debug_level>=UVC_DBG_INFO)\
                {printf(ASCII_COLOR_GREEN"[APP INFO][dev:%s]:%s[%d]: " fmt ASCII_COLOR_END"\n", dev?dev->path:NULL, __FUNCTION__,__LINE__,##args);}}while(0);})
#define UVC_WRN(dev, fmt, args...)      ({do{if(uvc_debug_level>=UVC_DBG_WRN)\
                {printf(ASCII_COLOR_YELLOW"[APP WRN][dev:%s]: %s[%d]: " fmt ASCII_COLOR_END"\n", dev?dev->path:NULL, __FUNCTION__,__LINE__, ##args);}}while(0);})
#define UVC_ERR(dev, fmt, args...)      ({do{if(uvc_debug_level>=UVC_DBG_ERR)\
                {printf(ASCII_COLOR_RED"[APP ERR][dev:%s]: %s[%d]: " fmt ASCII_COLOR_END"\n", dev?dev->path:NULL, __FUNCTION__,__LINE__, ##args);}}while(0);})
#define UVC_EXIT_ERR(fmt, args...) ({do\
                {printf(ASCII_COLOR_RED"<<<%s[%d] " fmt ASCII_COLOR_END"\n",__FUNCTION__,__LINE__,##args);}while(0);})
#define UVC_ENTER()                ({do{if(uvc_func_trace)\
                {printf(ASCII_COLOR_BLUE">>>%s[%d] \n" ASCII_COLOR_END"\n",__FUNCTION__,__LINE__);}}while(0);})
#define UVC_EXIT_OK()              ({do{if(uvc_func_trace)\
                {printf(ASCII_COLOR_BLUE"<<<%s[%d] \n" ASCII_COLOR_END"\n",__FUNCTION__,__LINE__);}}while(0);})

#define JPD_DEV_ID 0
#define JPD_CHN_ID 0
#define JPD_PORT_ID 0

#define VDEC_DEV_ID 0
#define VDEC_CHN_ID 0
#define VDEC_PORT_ID 1

#define SCL_DEV_ID 1
#define SCL_CHN_ID 0
#define SCL_PORT_ID 0

#define DISP_DEV_ID 0
#define DISP_CHN_ID 0
#define DISP_PORT_ID 0
#define DISP_LAYER_ID 0

#define MAX_BUF_CNT 10
#define MAX_FMT_SUPPORT 500

typedef struct {
    int fd;
    char path[20];
    int pixelformat;
    int width, height;
    int frame_size;
    int frame_rate;

    int buf_cnt;
    void *buf_start[MAX_BUF_CNT];
    unsigned int buf_len[MAX_BUF_CNT];
} Device_Handle_t;

typedef struct {
    void *buf;
    int length;
} Stream_Buffer_t;

typedef struct {
    int pixelformat;
    int width, height;
    int frame_rate;
} Stream_Info_t;

UVC_DBG_LEVEL_e uvc_debug_level = UVC_DBG_ERR;
Device_Handle_t video_dev = {0};
static bool g_exit = false;
static pthread_t stream_tid;
static unsigned int g_buf_cnt = 3;
static unsigned int g_device_index = 0;
static int g_select_format = 0;

extern MI_S32 ST_GetCropParam(MI_S32 srcWidth, MI_S32 srcHeight,
        MI_S32 dstWidth, MI_S32 dstHeight, MI_SYS_WindowRect_t *pStCropRect);

static MI_S32 St_SclModuleInit(Device_Handle_t *video_dev)
{
    MI_SYS_ChnPort_t stSysChnPort;
    MI_SCL_DevAttr_t stSclDevAttr;
    MI_SCL_ChannelAttr_t stSclChnAttr;
    MI_SCL_ChnParam_t stSclChnParam;
    MI_SCL_OutPortParam_t stSclOutputParam;
    MI_SYS_WindowRect_t stSysWindowRect;

    memset(&stSysChnPort, 0, sizeof(stSysChnPort));
    memset(&stSclDevAttr, 0, sizeof(stSclDevAttr));
    memset(&stSclChnAttr, 0, sizeof(stSclChnAttr));
    memset(&stSclChnParam, 0, sizeof(stSclChnParam));
    memset(&stSclOutputParam, 0, sizeof(stSclOutputParam));

    stSysChnPort.eModId = E_MI_MODULE_ID_SCL;
    stSysChnPort.u32DevId = SCL_DEV_ID;
    stSysChnPort.u32ChnId = SCL_CHN_ID;
    stSysChnPort.u32PortId = SCL_PORT_ID;
    stSclDevAttr.u32NeedUseHWOutPortMask = E_MI_SCL_HWSCL0;
    ST_GetCropParam(video_dev->width, video_dev->height, PANEL_WIDTH, PANEL_HEIGHT, &stSclOutputParam.stSCLOutCropRect);
//    stSclOutputParam.stSCLOutCropRect.u16X = 0;
//    stSclOutputParam.stSCLOutCropRect.u16Y = 0;
//    stSclOutputParam.stSCLOutCropRect.u16Width = stSysWindowRect.u16Width;
//    stSclOutputParam.stSCLOutCropRect.u16Height = stSysWindowRect.u16Height;
    stSclOutputParam.stSCLOutputSize.u16Width = PANEL_WIDTH;
    stSclOutputParam.stSCLOutputSize.u16Height = PANEL_HEIGHT;
    stSclOutputParam.bMirror = false;
    stSclOutputParam.bFlip = false;
    stSclOutputParam.eCompressMode= E_MI_SYS_COMPRESS_MODE_NONE;
    stSclOutputParam.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;

    ExecFunc(MI_SCL_CreateDevice(SCL_DEV_ID, &stSclDevAttr), MI_SUCCESS);
    ExecFunc(MI_SCL_CreateChannel(SCL_DEV_ID, SCL_CHN_ID, &stSclChnAttr), MI_SUCCESS);
    ExecFunc(MI_SCL_SetChnParam(SCL_DEV_ID, SCL_CHN_ID, &stSclChnParam), MI_SUCCESS);
    ExecFunc(MI_SCL_StartChannel(SCL_DEV_ID, SCL_CHN_ID), MI_SUCCESS);
    ExecFunc(MI_SCL_SetOutputPortParam(SCL_DEV_ID, SCL_CHN_ID, SCL_PORT_ID, &stSclOutputParam), MI_SUCCESS);
    ExecFunc(MI_SYS_SetChnOutputPortDepth(0, &stSysChnPort, 0, 4), MI_SUCCESS);
    ExecFunc(MI_SCL_EnableOutputPort(SCL_DEV_ID, SCL_CHN_ID, SCL_PORT_ID), MI_SUCCESS);

    return MI_SUCCESS;
}

static MI_S32 St_JpdModuleInit(Device_Handle_t *video_dev)
{
    MI_SYS_ChnPort_t stSysChnPort;
    MI_JPD_ChnCreatConf_t stJpdChnCreatConf;

    memset(&stSysChnPort, 0, sizeof(stSysChnPort));
    memset(&stJpdChnCreatConf, 0, sizeof(stJpdChnCreatConf));

    stSysChnPort.eModId = E_MI_MODULE_ID_JPD;
    stSysChnPort.u32DevId = JPD_DEV_ID;
    stSysChnPort.u32ChnId = JPD_CHN_ID;
    stSysChnPort.u32PortId = JPD_PORT_ID;
    stJpdChnCreatConf.u32MaxPicWidth   = 8192;
    stJpdChnCreatConf.u32MaxPicHeight  = 8192;
    stJpdChnCreatConf.u32StreamBufSize = 2*1024*1024;

    ExecFunc(MI_JPD_CreateChn(JPD_DEV_ID, JPD_CHN_ID, &stJpdChnCreatConf), MI_SUCCESS);
    ExecFunc(MI_JPD_StartChn(JPD_DEV_ID, JPD_CHN_ID), MI_SUCCESS);
    ExecFunc(MI_SYS_SetChnOutputPortDepth(0, &stSysChnPort, 0, 4), MI_SUCCESS);

    return MI_SUCCESS;
}

static MI_S32 St_VdecModuleInit(Device_Handle_t *video_dev)
{
    MI_SYS_ChnPort_t stSysChnPort;
    MI_VDEC_InitParam_t stVdecInitParam;
    MI_VDEC_ChnAttr_t stVdecChnAttr;
    MI_VDEC_OutputPortAttr_t stVdecOutputPortAttr;
    MI_SYS_WindowRect_t stSysWindowRect;

    memset(&stSysChnPort, 0, sizeof(stSysChnPort));
    memset(&stVdecInitParam, 0, sizeof(stVdecInitParam));
    memset(&stVdecChnAttr, 0, sizeof(stVdecChnAttr));
    memset(&stVdecOutputPortAttr, 0, sizeof(stVdecOutputPortAttr));

    stSysChnPort.eModId = E_MI_MODULE_ID_VDEC;
    stSysChnPort.u32DevId = VDEC_DEV_ID;
    stSysChnPort.u32ChnId = VDEC_CHN_ID;
    stSysChnPort.u32PortId = VDEC_PORT_ID;
    stVdecInitParam.bDisableLowLatency = false;
    stVdecChnAttr.eCodecType = (video_dev->pixelformat == V4L2_PIX_FMT_H264) ? E_MI_VDEC_CODEC_TYPE_H264 : E_MI_VDEC_CODEC_TYPE_H265;
    stVdecChnAttr.eDpbBufMode = E_MI_VDEC_DPB_MODE_NORMAL;
    stVdecChnAttr.eVideoMode = E_MI_VDEC_VIDEO_MODE_FRAME;
    stVdecChnAttr.stVdecVideoAttr.stErrHandlePolicy.bUseCusPolicy = false;
    stVdecChnAttr.stVdecVideoAttr.u32RefFrameNum = 16;
    stVdecChnAttr.u32BufSize = 2*1024*1024;
    stVdecChnAttr.u32PicHeight = 4096;
    stVdecChnAttr.u32PicWidth = 4096;
    stVdecChnAttr.u32Priority = 0;
    stVdecOutputPortAttr.u16Height = video_dev->height;
    stVdecOutputPortAttr.u16Width = video_dev->width;

    ExecFunc(MI_VDEC_CreateDev(VDEC_DEV_ID, &stVdecInitParam), MI_SUCCESS);
    ExecFunc(MI_VDEC_CreateChn(VDEC_DEV_ID, VDEC_CHN_ID, &stVdecChnAttr), MI_SUCCESS);
    ExecFunc(MI_VDEC_SetDisplayMode(VDEC_DEV_ID, VDEC_CHN_ID, E_MI_VDEC_DISPLAY_MODE_PLAYBACK), MI_SUCCESS);
    ExecFunc(MI_VDEC_StartChn(VDEC_DEV_ID, VDEC_CHN_ID), MI_SUCCESS);
    ExecFunc(MI_VDEC_SetOutputPortAttr(VDEC_DEV_ID, VDEC_CHN_ID, &stVdecOutputPortAttr), MI_SUCCESS);
    ExecFunc(MI_SYS_SetChnOutputPortDepth(0, &stSysChnPort, 0, 4), MI_SUCCESS);

    return MI_SUCCESS;
}

static MI_S32 St_DispModuleInit(Device_Handle_t *video_dev)
{
    MI_DISP_InputPortAttr_t stDispInputPortAttr;

    memset(&stDispInputPortAttr, 0, sizeof(MI_DISP_InputPortAttr_t));

	MI_DISP_GetInputPortAttr(0, 0, &stDispInputPortAttr);
	stDispInputPortAttr.stDispWin.u16Height = PANEL_HEIGHT;
	stDispInputPortAttr.stDispWin.u16Width = PANEL_WIDTH;
	stDispInputPortAttr.stDispWin.u16X = 0;
	stDispInputPortAttr.stDispWin.u16Y = 0;
	stDispInputPortAttr.u16SrcHeight = PANEL_HEIGHT;
	stDispInputPortAttr.u16SrcWidth = PANEL_WIDTH;

	ExecFunc(MI_DISP_SetInputPortAttr(DISP_LAYER_ID, DISP_PORT_ID, &stDispInputPortAttr), MI_SUCCESS);
	ExecFunc(MI_DISP_EnableInputPort(DISP_LAYER_ID, DISP_PORT_ID), MI_SUCCESS);
	ExecFunc(MI_DISP_SetInputPortSyncMode(DISP_LAYER_ID, DISP_PORT_ID, E_MI_DISP_SYNC_MODE_FREE_RUN), MI_SUCCESS);

    return MI_SUCCESS;
}

static MI_S32 St_BaseModuleInit(Device_Handle_t *video_dev)
{
    ST_Sys_BindInfo_T stSysBindInfo;

    memset(&stSysBindInfo, 0x0, sizeof(stSysBindInfo));

    switch(video_dev->pixelformat)
    {
        case V4L2_PIX_FMT_YUYV:
        case V4L2_PIX_FMT_NV12:
            /* scl start */
            ExecFunc(St_SclModuleInit(video_dev), MI_SUCCESS);
            /* disp start */
            ExecFunc(St_DispModuleInit(video_dev), MI_SUCCESS);
            /* bind scl and disp */
            stSysBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stSysBindInfo.stSrcChnPort.u32DevId = SCL_DEV_ID;
            stSysBindInfo.stSrcChnPort.u32ChnId = SCL_CHN_ID;
            stSysBindInfo.stSrcChnPort.u32PortId = SCL_PORT_ID;
            stSysBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
            stSysBindInfo.stDstChnPort.u32DevId = DISP_DEV_ID;
            stSysBindInfo.stDstChnPort.u32ChnId = DISP_CHN_ID;
            stSysBindInfo.stDstChnPort.u32PortId = DISP_PORT_ID;
            stSysBindInfo.u32SrcFrmrate = video_dev->frame_rate;
            stSysBindInfo.u32DstFrmrate = video_dev->frame_rate;
            stSysBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            ExecFunc(ST_Sys_Bind(&stSysBindInfo), MI_SUCCESS);
            break;

        case V4L2_PIX_FMT_MJPEG:
            /* jpd start */
            ExecFunc(St_JpdModuleInit(video_dev), MI_SUCCESS);
            /* scl start */
            ExecFunc(St_SclModuleInit(video_dev), MI_SUCCESS);
            /* disp start */
            ExecFunc(St_DispModuleInit(video_dev), MI_SUCCESS);
            /* bind jpd and scl */
            stSysBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_JPD;
            stSysBindInfo.stSrcChnPort.u32DevId = JPD_DEV_ID;
            stSysBindInfo.stSrcChnPort.u32ChnId = JPD_CHN_ID;
            stSysBindInfo.stSrcChnPort.u32PortId = JPD_PORT_ID;
            stSysBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_SCL;
            stSysBindInfo.stDstChnPort.u32DevId = SCL_DEV_ID;
            stSysBindInfo.stDstChnPort.u32ChnId = SCL_CHN_ID;
            stSysBindInfo.stDstChnPort.u32PortId = SCL_PORT_ID;
            stSysBindInfo.u32SrcFrmrate = video_dev->frame_rate;
            stSysBindInfo.u32DstFrmrate = video_dev->frame_rate;
            stSysBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            ExecFunc(ST_Sys_Bind(&stSysBindInfo), MI_SUCCESS);
            /* bind scl and disp */
            stSysBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stSysBindInfo.stSrcChnPort.u32DevId = SCL_DEV_ID;
            stSysBindInfo.stSrcChnPort.u32ChnId = SCL_CHN_ID;
            stSysBindInfo.stSrcChnPort.u32PortId = SCL_PORT_ID;
            stSysBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
            stSysBindInfo.stDstChnPort.u32DevId = DISP_DEV_ID;
            stSysBindInfo.stDstChnPort.u32ChnId = DISP_CHN_ID;
            stSysBindInfo.stDstChnPort.u32PortId = DISP_PORT_ID;
            stSysBindInfo.u32SrcFrmrate = video_dev->frame_rate;
            stSysBindInfo.u32DstFrmrate = video_dev->frame_rate;
            stSysBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            ExecFunc(ST_Sys_Bind(&stSysBindInfo), MI_SUCCESS);
            break;

        case V4L2_PIX_FMT_H264:
        case V4L2_PIX_FMT_H265:
            /* vdec start */
            ExecFunc(St_VdecModuleInit(video_dev), MI_SUCCESS);
            /* scl start */
			ExecFunc(St_SclModuleInit(video_dev), MI_SUCCESS);
            /* disp start */
            ExecFunc(St_DispModuleInit(video_dev), MI_SUCCESS);
            /* bind vdec and scl */
			stSysBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VDEC;
			stSysBindInfo.stSrcChnPort.u32DevId = VDEC_DEV_ID;
			stSysBindInfo.stSrcChnPort.u32ChnId = VDEC_CHN_ID;
			stSysBindInfo.stSrcChnPort.u32PortId = VDEC_PORT_ID;
			stSysBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_SCL;
			stSysBindInfo.stDstChnPort.u32DevId = SCL_DEV_ID;
			stSysBindInfo.stDstChnPort.u32ChnId = SCL_CHN_ID;
			stSysBindInfo.stDstChnPort.u32PortId = SCL_PORT_ID;
			stSysBindInfo.u32SrcFrmrate = video_dev->frame_rate;
			stSysBindInfo.u32DstFrmrate = video_dev->frame_rate;
			stSysBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
			ExecFunc(ST_Sys_Bind(&stSysBindInfo), MI_SUCCESS);
			/* bind scl and disp */
			stSysBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
			stSysBindInfo.stSrcChnPort.u32DevId = SCL_DEV_ID;
			stSysBindInfo.stSrcChnPort.u32ChnId = SCL_CHN_ID;
			stSysBindInfo.stSrcChnPort.u32PortId = SCL_PORT_ID;
			stSysBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
			stSysBindInfo.stDstChnPort.u32DevId = DISP_DEV_ID;
			stSysBindInfo.stDstChnPort.u32ChnId = DISP_CHN_ID;
			stSysBindInfo.stDstChnPort.u32PortId = DISP_PORT_ID;
			stSysBindInfo.u32SrcFrmrate = video_dev->frame_rate;
			stSysBindInfo.u32DstFrmrate = video_dev->frame_rate;
			stSysBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
			ExecFunc(ST_Sys_Bind(&stSysBindInfo), MI_SUCCESS);
            break;

        default:
            break;
    }

    return MI_SUCCESS;
}

static MI_S32 St_SclModuleDeinit(void)
{
    ExecFunc(MI_SCL_DisableOutputPort(SCL_DEV_ID, SCL_CHN_ID, SCL_PORT_ID), MI_SUCCESS);
    ExecFunc(MI_SCL_StopChannel(SCL_DEV_ID, SCL_CHN_ID), MI_SUCCESS);
    ExecFunc(MI_SCL_DestroyChannel(SCL_DEV_ID, SCL_CHN_ID), MI_SUCCESS);
    ExecFunc(MI_SCL_DestroyDevice(SCL_DEV_ID), MI_SUCCESS);

    return MI_SUCCESS;
}

static MI_S32 St_JpdModuleDeinit(void)
{
    ExecFunc(MI_JPD_StopChn(JPD_DEV_ID, JPD_CHN_ID), MI_SUCCESS);
    ExecFunc(MI_JPD_DestroyChn(JPD_DEV_ID, JPD_CHN_ID), MI_SUCCESS);
    ExecFunc(MI_JPD_DestroyDev(JPD_DEV_ID), MI_SUCCESS);
    return MI_SUCCESS;
}

static MI_S32 St_VdecModuleDeinit(void)
{
    ExecFunc(MI_VDEC_StopChn(VDEC_DEV_ID, VDEC_CHN_ID), MI_SUCCESS);
    ExecFunc(MI_VDEC_DestroyChn(VDEC_DEV_ID, VDEC_CHN_ID), MI_SUCCESS);
    ExecFunc(MI_VDEC_DestroyDev(VDEC_DEV_ID), MI_SUCCESS);

    return MI_SUCCESS;
}

static MI_S32 St_DispModuleDeinit(void)
{
    ExecFunc(MI_DISP_DisableInputPort(DISP_LAYER_ID, DISP_PORT_ID), MI_SUCCESS);
    return MI_SUCCESS;
}

static MI_S32 St_BaseModuleDeinit(Device_Handle_t *video_dev)
{
    ST_Sys_BindInfo_T stSysBindInfo;
    memset(&stSysBindInfo, 0, sizeof(stSysBindInfo));

    switch(video_dev->pixelformat)
    {
        case V4L2_PIX_FMT_YUYV:
        case V4L2_PIX_FMT_NV12:
            stSysBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stSysBindInfo.stSrcChnPort.u32DevId = SCL_DEV_ID;
            stSysBindInfo.stSrcChnPort.u32ChnId = SCL_CHN_ID;
            stSysBindInfo.stSrcChnPort.u32PortId = SCL_PORT_ID;
            stSysBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
            stSysBindInfo.stDstChnPort.u32DevId = DISP_DEV_ID;
            stSysBindInfo.stDstChnPort.u32ChnId = DISP_CHN_ID;
            stSysBindInfo.stDstChnPort.u32PortId = DISP_PORT_ID;
            stSysBindInfo.u32SrcFrmrate = video_dev->frame_rate;
            stSysBindInfo.u32DstFrmrate = video_dev->frame_rate;
            stSysBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            ExecFunc(ST_Sys_UnBind(&stSysBindInfo), MI_SUCCESS);

            ExecFunc(St_DispModuleDeinit(), MI_SUCCESS);
            ExecFunc(St_SclModuleDeinit(), MI_SUCCESS);
            break;

        case V4L2_PIX_FMT_MJPEG:
            stSysBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_JPD;
            stSysBindInfo.stSrcChnPort.u32DevId = JPD_DEV_ID;
            stSysBindInfo.stSrcChnPort.u32ChnId = JPD_CHN_ID;
            stSysBindInfo.stSrcChnPort.u32PortId = JPD_PORT_ID;
            stSysBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_SCL;
            stSysBindInfo.stDstChnPort.u32DevId = SCL_DEV_ID;
            stSysBindInfo.stDstChnPort.u32ChnId = SCL_CHN_ID;
            stSysBindInfo.stDstChnPort.u32PortId = SCL_PORT_ID;
            stSysBindInfo.u32SrcFrmrate = video_dev->frame_rate;
            stSysBindInfo.u32DstFrmrate = video_dev->frame_rate;
            stSysBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            ExecFunc(ST_Sys_UnBind(&stSysBindInfo), MI_SUCCESS);

            stSysBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stSysBindInfo.stSrcChnPort.u32DevId = SCL_DEV_ID;
            stSysBindInfo.stSrcChnPort.u32ChnId = SCL_CHN_ID;
            stSysBindInfo.stSrcChnPort.u32PortId = SCL_PORT_ID;
            stSysBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
            stSysBindInfo.stDstChnPort.u32DevId = DISP_DEV_ID;
            stSysBindInfo.stDstChnPort.u32ChnId = DISP_CHN_ID;
            stSysBindInfo.stDstChnPort.u32PortId = DISP_PORT_ID;
            stSysBindInfo.u32SrcFrmrate = video_dev->frame_rate;
            stSysBindInfo.u32DstFrmrate = video_dev->frame_rate;
            stSysBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            ExecFunc(ST_Sys_UnBind(&stSysBindInfo), MI_SUCCESS);

            ExecFunc(St_DispModuleDeinit(), MI_SUCCESS);
            ExecFunc(St_SclModuleDeinit(), MI_SUCCESS);
            ExecFunc(St_JpdModuleDeinit(), MI_SUCCESS);
            break;

        case V4L2_PIX_FMT_H264:
        case V4L2_PIX_FMT_H265:
        	stSysBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VDEC;
			stSysBindInfo.stSrcChnPort.u32DevId = VDEC_DEV_ID;
			stSysBindInfo.stSrcChnPort.u32ChnId = VDEC_CHN_ID;
			stSysBindInfo.stSrcChnPort.u32PortId = VDEC_PORT_ID;
			stSysBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_SCL;
			stSysBindInfo.stDstChnPort.u32DevId = SCL_DEV_ID;
			stSysBindInfo.stDstChnPort.u32ChnId = SCL_CHN_ID;
			stSysBindInfo.stDstChnPort.u32PortId = SCL_PORT_ID;
			stSysBindInfo.u32SrcFrmrate = video_dev->frame_rate;
			stSysBindInfo.u32DstFrmrate = video_dev->frame_rate;
			stSysBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
			ExecFunc(ST_Sys_UnBind(&stSysBindInfo), MI_SUCCESS);

			stSysBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
			stSysBindInfo.stSrcChnPort.u32DevId = SCL_DEV_ID;
			stSysBindInfo.stSrcChnPort.u32ChnId = SCL_CHN_ID;
			stSysBindInfo.stSrcChnPort.u32PortId = SCL_PORT_ID;
			stSysBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
			stSysBindInfo.stDstChnPort.u32DevId = DISP_DEV_ID;
			stSysBindInfo.stDstChnPort.u32ChnId = DISP_CHN_ID;
			stSysBindInfo.stDstChnPort.u32PortId = DISP_PORT_ID;
			stSysBindInfo.u32SrcFrmrate = video_dev->frame_rate;
			stSysBindInfo.u32DstFrmrate = video_dev->frame_rate;
			stSysBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
			ExecFunc(ST_Sys_UnBind(&stSysBindInfo), MI_SUCCESS);

            ExecFunc(St_DispModuleDeinit(), MI_SUCCESS);
            ExecFunc(St_SclModuleDeinit(), MI_SUCCESS);
            ExecFunc(St_VdecModuleDeinit(), MI_SUCCESS);
            break;

        default:
            break;
    }

    return MI_SUCCESS;
}

static MI_S32 St_Start_Display(Device_Handle_t *video_dev, Stream_Buffer_t *video_buf)
{
    MI_S32 s32Ret;
    struct timeval stTv;

    MI_SYS_ChnPort_t stSysChnPort;
    MI_SYS_BufConf_t stSysBufConf;
    MI_SYS_BufInfo_t stSysBufInfo;
    MI_SYS_BUF_HANDLE stSysBufHandle;
    MI_JPD_StreamBuf_t stJpdStreamBuf;
    MI_VDEC_VideoStream_t stVdecVideoStream;

    memset(&stSysChnPort, 0, sizeof(stSysChnPort));
    memset(&stSysBufConf, 0, sizeof(stSysBufConf));
    memset(&stSysBufInfo, 0, sizeof(stSysBufInfo));
    memset(&stSysBufHandle, 0, sizeof(stSysBufHandle));
    memset(&stJpdStreamBuf, 0, sizeof(stJpdStreamBuf));
    memset(&stVdecVideoStream, 0, sizeof(stVdecVideoStream));

    switch(video_dev->pixelformat)
    {
        case V4L2_PIX_FMT_YUYV:
            stSysChnPort.eModId = E_MI_MODULE_ID_SCL;
            stSysChnPort.u32DevId = SCL_DEV_ID;
            stSysChnPort.u32ChnId = SCL_CHN_ID;
            stSysChnPort.u32PortId = SCL_PORT_ID;

            stSysBufConf.eBufType = E_MI_SYS_BUFDATA_FRAME;
            gettimeofday(&stTv, NULL);
            stSysBufConf.u64TargetPts = stTv.tv_sec*1000000 + stTv.tv_usec;
            stSysBufConf.stFrameCfg.eFormat = E_MI_SYS_PIXEL_FRAME_YUV422_YUYV;
            stSysBufConf.stFrameCfg.eFrameScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;
            stSysBufConf.stFrameCfg.u16Width = video_dev->width;
            stSysBufConf.stFrameCfg.u16Height = video_dev->height;

            s32Ret = MI_SYS_ChnInputPortGetBuf(&stSysChnPort, &stSysBufConf, &stSysBufInfo, &stSysBufHandle, -1);
            if(MI_SUCCESS != s32Ret)
            {
                UVC_ERR(video_dev, "MI_SYS_ChnInputPortGetBuf: 0x%x.\n", s32Ret);
                return -1;
            }

            stSysBufInfo.bEndOfStream = true;
            memcpy(stSysBufInfo.stFrameData.pVirAddr[0], video_buf->buf, video_buf->length);

            s32Ret = MI_SYS_ChnInputPortPutBuf(stSysBufHandle ,&stSysBufInfo , false);
            if(MI_SUCCESS != s32Ret)
            {
                UVC_ERR(video_dev, "MI_SYS_ChnInputPortPutBuf: 0x%x.\n", s32Ret);
                return -1;
            }
            break;

        case V4L2_PIX_FMT_NV12:
            stSysChnPort.eModId = E_MI_MODULE_ID_DISP;
            stSysChnPort.u32DevId = DISP_DEV_ID;
            stSysChnPort.u32ChnId = DISP_CHN_ID;
            stSysChnPort.u32PortId = DISP_PORT_ID;

            stSysBufConf.eBufType = E_MI_SYS_BUFDATA_FRAME;
            gettimeofday(&stTv, NULL);
            stSysBufConf.u64TargetPts = stTv.tv_sec*1000000 + stTv.tv_usec;
            stSysBufConf.stFrameCfg.eFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            stSysBufConf.stFrameCfg.eFrameScanMode = E_MI_SYS_FRAME_SCAN_MODE_PROGRESSIVE;
            stSysBufConf.stFrameCfg.u16Width = video_dev->width;
            stSysBufConf.stFrameCfg.u16Height = video_dev->height;

            s32Ret = MI_SYS_ChnInputPortGetBuf(&stSysChnPort,&stSysBufConf,&stSysBufInfo,&stSysBufHandle, -1);
            if(MI_SUCCESS != s32Ret)
            {
                UVC_ERR(video_dev, "MI_SYS_ChnInputPortGetBuf: 0x%x.\n", s32Ret);
                return -1;
            }

            stSysBufInfo.bEndOfStream = true;
            memcpy(stSysBufInfo.stFrameData.pVirAddr[0], video_buf->buf, video_buf->length);

            s32Ret = MI_SYS_ChnInputPortPutBuf(stSysBufHandle ,&stSysBufInfo , false);
            if(MI_SUCCESS != s32Ret)
            {
                UVC_ERR(video_dev, "MI_SYS_ChnInputPortPutBuf: 0x%x.\n", s32Ret);
                return -1;
            }
            break;

        case V4L2_PIX_FMT_MJPEG:
            s32Ret = MI_JPD_GetStreamBuf(JPD_DEV_ID, JPD_CHN_ID, video_buf->length, &stJpdStreamBuf, -1);
            if(MI_SUCCESS != s32Ret)
            {
                UVC_ERR(video_dev, "MI_JPD_GetStreamBuf: 0x%x.\n", s32Ret);
                return -1;
            }

            memcpy(stJpdStreamBuf.pu8HeadVirtAddr, video_buf->buf, MIN(stJpdStreamBuf.u32HeadLength, video_buf->length));

            if(stJpdStreamBuf.u32HeadLength + stJpdStreamBuf.u32TailLength < video_buf->length)
            {
                UVC_ERR(video_dev, "MI_JPD_GetStreamBuf return wrong value: HeadLen%u TailLen%u RequiredLength%u.\n",
                        stJpdStreamBuf.u32HeadLength, stJpdStreamBuf.u32TailLength, video_buf->length);

                s32Ret = MI_JPD_DropStreamBuf(JPD_DEV_ID, JPD_CHN_ID, &stJpdStreamBuf);
                if(MI_SUCCESS != s32Ret)
                {
                    UVC_ERR(video_dev, "MI_JPD_DropStreamBuf: 0x%x.", s32Ret);
                    return -1;
                }

                return -1;
            }
            else if(stJpdStreamBuf.u32TailLength > 0)
            {
                memcpy(stJpdStreamBuf.pu8TailVirtAddr, video_buf->buf + stJpdStreamBuf.u32HeadLength,
                       MIN(stJpdStreamBuf.u32TailLength, video_buf->length - stJpdStreamBuf.u32HeadLength));
            }

            stJpdStreamBuf.u32ContentLength = video_buf->length;
            s32Ret = MI_JPD_PutStreamBuf(JPD_DEV_ID, JPD_CHN_ID, &stJpdStreamBuf);
            if(MI_SUCCESS != s32Ret)
            {
                UVC_ERR(video_dev, "MI_JPD_PutStreamBuf: 0x%x.\n", s32Ret);
                return -1;
            }
            break;

        case V4L2_PIX_FMT_H264:
        case V4L2_PIX_FMT_H265:
            stVdecVideoStream.bEndOfFrame = true;
            stVdecVideoStream.bEndOfStream = false;
            stVdecVideoStream.pu8Addr = video_buf->buf;
            stVdecVideoStream.u32Len = video_buf->length;
            stVdecVideoStream.u64PTS = -1;
            s32Ret = MI_VDEC_SendStream(VDEC_DEV_ID, VDEC_CHN_ID, &stVdecVideoStream, -1);
            if(MI_SUCCESS != s32Ret)
            {
                UVC_ERR(video_dev, "MI_VDEC_SendStream: 0x%x.\n", s32Ret);
                return -1;
            }
            break;

        default:
            break;
    }

    return MI_SUCCESS;
}

static char* format_fcc_to_str(int pixelformat)
{
    switch(pixelformat)
    {
        case V4L2_PIX_FMT_YUYV:
            return "YUYV";
        case V4L2_PIX_FMT_NV12:
            return "NV12";
        case V4L2_PIX_FMT_MJPEG:
            return "MJPEG";
        case V4L2_PIX_FMT_H264:
            return "H264";
        case V4L2_PIX_FMT_H265:
            return "H265";
        default:
            return "Not Known!";
    }
}

int video_enum_format(Device_Handle_t *video_dev)
{
    int index = 0;
    int default_resolution = 0;
    Stream_Info_t info[MAX_FMT_SUPPORT];
    struct v4l2_fmtdesc fmt = {0};
    struct v4l2_frmsizeenum frmsize = {0};
    struct v4l2_frmivalenum framival = {0};

    for(int i = 0; ; i++)
    {
        fmt.index = i;
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if(ioctl(video_dev->fd, VIDIOC_ENUM_FMT, &fmt) < 0)
            break;

        for(int j = 0; ; j++)
        {
            frmsize.index = j;
            frmsize.pixel_format = fmt.pixelformat;

            if(ioctl(video_dev->fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) < 0)
                break;

            /* don't enum frameinterval, use default value */
            for(int k = 0; ; k++)
            {
                framival.index = k;
                framival.pixel_format = fmt.pixelformat;
                framival.width = frmsize.discrete.width;
                framival.height = frmsize.discrete.height;

                if(ioctl(video_dev->fd, VIDIOC_ENUM_FRAMEINTERVALS, &framival) < 0)
                    break;

                info[index].pixelformat = frmsize.pixel_format;
                info[index].width = frmsize.discrete.width;
                info[index].height = frmsize.discrete.height;
                info[index].frame_rate = framival.discrete.denominator;
                index++;
            }
        }
    }

    if(index == 0)
    {
        UVC_ERR(video_dev, "Cannot enum video format.\n");
        goto fail;
    }

    printf("====================Support These Formats====================\n");

    for(int i = 0; i < index; i++)
    {
    	if(info[i].frame_rate == 30)
    	{
    		int tempres = info[i].width * info[i].height;
    		if(default_resolution < tempres && info[i].width <= 1920 && info[i].height <= 1920)
    		{
    			default_resolution = tempres;
    			g_select_format = i;
    		}
    	}
        printf("[%d]    Format: %s    Resolution: %d x %d    Fps: %d\n",
                i,
                format_fcc_to_str(info[i].pixelformat),
                info[i].width,
                info[i].height,
                info[i].frame_rate);
    }

    printf("The default index is [%d]\n", g_select_format);
	if(g_select_format >= index)
	{
		UVC_ERR(video_dev, "Invalid choice.\n");
		goto fail;
	}

    video_dev->pixelformat = info[g_select_format].pixelformat;
    video_dev->width = info[g_select_format].width;
    video_dev->height = info[g_select_format].height;
    video_dev->frame_rate = info[g_select_format].frame_rate;

    return 0;

fail:
    close(video_dev->fd);
    return -1;
}

int video_set_format(Device_Handle_t *video_dev)
{
    int err;

    struct v4l2_format fmt = {};
    struct v4l2_streamparm param = {};

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = video_dev->width;
    fmt.fmt.pix.height = video_dev->height;
    fmt.fmt.pix.pixelformat = video_dev->pixelformat;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;

    if(ioctl(video_dev->fd, VIDIOC_S_FMT, &fmt) < 0)
    {
        UVC_ERR(video_dev, "ioctl(VIDIOC_S_FMT): %s.\n", strerror(errno));
        err = -errno;
        goto fail;
    }

    param.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    param.parm.capture.timeperframe.numerator = 1;
    param.parm.capture.timeperframe.denominator = video_dev->frame_rate;

    if(ioctl(video_dev->fd, VIDIOC_S_PARM, &param) < 0)
    {
        UVC_ERR(video_dev, "ioctl(VIDIOC_S_PARM): %s.\n", strerror(errno));
        err = -errno;
        goto fail;
    }

    video_dev->width = fmt.fmt.pix.width;
    video_dev->height = fmt.fmt.pix.height;
    video_dev->pixelformat = fmt.fmt.pix.pixelformat;

    switch(video_dev->pixelformat)
    {
        case V4L2_PIX_FMT_YUYV:
            video_dev->frame_size = video_dev->width * video_dev->height * 2.0;
            break;
        case V4L2_PIX_FMT_NV12:
            video_dev->frame_size = video_dev->width * video_dev->height * 1.5;
            break;
        case V4L2_PIX_FMT_MJPEG:
            video_dev->frame_size = video_dev->width * video_dev->height * 1.0;
            break;
        case V4L2_PIX_FMT_H264:
            video_dev->frame_size = video_dev->width * video_dev->height * 1.0;
            break;
        case V4L2_PIX_FMT_H265:
            video_dev->frame_size = video_dev->width * video_dev->height * 1.0;
            break;
    }

    printf("Format: %s    Resolution: %d x %d    Fps: %d    Expect Frame Size: %d\n",
        format_fcc_to_str(video_dev->pixelformat), video_dev->width, video_dev->height, video_dev->frame_rate, video_dev->frame_size);

    return 0;

fail:
    close(video_dev->fd);
    return err;
}

int video_streamon(Device_Handle_t *video_dev, unsigned int buf_cnt)
{
    int err;

    struct v4l2_requestbuffers req = {};
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.count = buf_cnt;
    req.memory = V4L2_MEMORY_MMAP;

    if(ioctl(video_dev->fd, VIDIOC_REQBUFS, &req) < 0)
    {
        UVC_ERR(video_dev, "ioctl(VIDIOC_REQBUFS): %s.\n", strerror(errno));
        err = -errno;
        goto fail;
    }

    if(req.count < 2)
    {
        UVC_ERR(video_dev, "Insufficient buffer memory.\n");
        err = -ENOMEM;
        goto fail;
    }

    video_dev->buf_cnt = req.count;

    for(int i = 0; i < video_dev->buf_cnt; i++)
    {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.index = i;
        buf.memory = V4L2_MEMORY_MMAP;

        if(ioctl(video_dev->fd, VIDIOC_QUERYBUF, &buf) < 0)
        {
            UVC_ERR(video_dev, "ioctl(VIDIOC_QUERYBUF): %s.\n", strerror(errno));
            err = -errno;
            goto fail;
        }

        video_dev->buf_len[i] = buf.length;
        if(video_dev->frame_size > 0 && video_dev->buf_len[i] < video_dev->frame_size)
        {
            UVC_ERR(video_dev, "buf_len[%d] = %d < expected frame size %d.\n",
                    i, video_dev->buf_len[i], video_dev->frame_size);
            err = -ENOMEM;
            goto fail;
        }

        video_dev->buf_start[i] = mmap(NULL, buf.length,
                                    PROT_READ | PROT_WRITE, MAP_SHARED,
                                    video_dev->fd, buf.m.offset);
        if(video_dev->buf_start[i] == MAP_FAILED)
        {
            UVC_ERR(video_dev, "mmap: %s.\n", strerror(errno));
            err = -errno;
            goto fail;
        }

        UVC_INFO(video_dev, "mmap: index%d start0x%p.\n", i, video_dev->buf_start[i]);

        if(ioctl(video_dev->fd, VIDIOC_QBUF, &buf) < 0)
        {
            UVC_ERR(video_dev, "ioctl(VIDIOC_QBUF): %s.\n", strerror(errno));
            err = -errno;
            goto fail;
        }
    }

    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(ioctl(video_dev->fd, VIDIOC_STREAMON, &type) < 0)
    {
        UVC_ERR(video_dev, "ioctl(VIDIOC_STREAMON): %s.\n", strerror(errno));
        err = -errno;
        goto fail;
    }

    return 0;

fail:
    close(video_dev->fd);
    return err;
}

int video_streamoff(Device_Handle_t *video_dev)
{
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(ioctl(video_dev->fd, VIDIOC_STREAMOFF, &type) < 0)
        UVC_ERR(video_dev, "ioctl(VIDIOC_STREAMOFF): %s.\n", strerror(errno));

    for(int i = 0; i < video_dev->buf_cnt; i++)
        munmap(video_dev->buf_start[i], video_dev->buf_len[i]);

    return 0;
}

int video_put_buf(Device_Handle_t *video_dev, Stream_Buffer_t *video_buf)
{
    video_buf->length = 0;
    if(video_buf->buf)
        free(video_buf->buf);

    return 0;
}

int video_get_buf(Device_Handle_t *video_dev, Stream_Buffer_t *video_buf)
{
    int err;

    struct v4l2_buffer buf = {};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    err = ioctl(video_dev->fd, VIDIOC_DQBUF, &buf);
    if(err < 0)
    {
        if(errno != EAGAIN)
            UVC_ERR(video_dev, "ioctl(VIDIOC_DQBUF): %s.\n", strerror(errno));
        return -errno;
    }

    if(buf.index >= video_dev->buf_cnt)
    {
        UVC_ERR(video_dev, "Invalid buffer index received.\n");
        err = -EAGAIN;
        goto fail;
    }

    if(buf.flags & V4L2_BUF_FLAG_ERROR)
    {
        UVC_ERR(video_dev, "Dequeued v4l2 buffer contains corrupted data (%d bytes).\n", buf.bytesused);
        err = -EAGAIN;
        goto fail;
    }

    if(!buf.bytesused || (video_dev->frame_size > 0 && buf.bytesused > video_dev->frame_size))
    {
        UVC_ERR(video_dev, "Dequeued v4l2 buffer contains %d bytes, but %d were expected. Flags: 0x%08X.\n",
                    buf.bytesused, video_dev->frame_size, buf.flags);
        err = -EAGAIN;
        goto fail;
    }

    /* copy to user buffer */
    video_buf->length = buf.bytesused;
    video_buf->buf = calloc(1, buf.bytesused);
    if(!video_buf->buf)
    {
        UVC_ERR(video_dev, "Error allocating a packet.\n");
        err = -ENOMEM;
        goto fail;
    }

    memcpy(video_buf->buf, video_dev->buf_start[buf.index], buf.bytesused);

    if(ioctl(video_dev->fd, VIDIOC_QBUF, &buf) < 0)
    {
        UVC_ERR(video_dev, "ioctl(VIDIOC_QBUF): %s.\n", strerror(errno));
        video_put_buf(video_dev, video_buf);
        return -errno;
    }

    return 0;

fail:
    if(ioctl(video_dev->fd, VIDIOC_QBUF, &buf) < 0)
    {
        UVC_ERR(video_dev, "ioctl(VIDIOC_QBUF): %s.\n", strerror(errno));
        err = -errno;
    }

    return err;
}

int video_init(Device_Handle_t *video_dev)
{
    int fd;
    int err;
    int flags = O_RDWR | O_NONBLOCK;

    struct v4l2_capability cap;

    fd = open(video_dev->path, flags, 0);
    if(fd < 0)
    {
        UVC_ERR(video_dev, "Cannot open video device: %s.\n", strerror(errno));
        mTextView_open_failPtr->setVisible(true);
        return -errno;
    }

    if(ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0)
    {
        UVC_ERR(video_dev, "ioctl(VIDIOC_QUERYCAP): %s.\n", strerror(errno));
        err = -errno;
        goto fail;
    }

    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        UVC_ERR(video_dev, "Not a video capture device.\n");
        err = -ENODEV;
        goto fail;
    }

    if(!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        UVC_ERR(video_dev, "The device does not support the streaming I/O method.\n");
        err = -ENOSYS;
        goto fail;
    }

    video_dev->fd = fd;

    return 0;

fail:
    close(fd);
    return err;
}

int video_deinit(Device_Handle_t *video_dev)
{
    close(video_dev->fd);
    return 0;
}

static void *uvc_stream(void *arg)
{
    int ret = 0;
    int frame_cnt = 0;
	struct timeval tv_before, tv_after;
    Device_Handle_t *video_dev = (Device_Handle_t *)arg;
    Stream_Buffer_t video_buf;
    memset(&video_buf, 0, sizeof(Stream_Buffer_t));

    ret = video_enum_format(video_dev);
    if(ret != 0)
        pthread_exit(&ret);

    ret = video_set_format(video_dev);
    if(ret != 0)
        pthread_exit(&ret);

	ret = St_BaseModuleInit(video_dev);
	if(ret != 0)
		pthread_exit(&ret);

	ret = video_streamon(video_dev, g_buf_cnt);
	if(ret != 0)
		pthread_exit(&ret);

	if(gettimeofday(&tv_before, NULL))
		UVC_WRN(video_dev, "gettimeofday begin: %s.\n", strerror(errno));

	while(!g_exit)
	{
		ret = video_get_buf(video_dev, &video_buf);
		if(ret == 0)
		{
			St_Start_Display(video_dev, &video_buf);
			video_put_buf(video_dev, &video_buf);
		}
		else if(ret == -EAGAIN)
			continue;
		else
			break;

		if(gettimeofday(&tv_after, NULL))
			UVC_WRN(video_dev, "gettimeofday end: %s.\n", strerror(errno));

		if((tv_after.tv_sec - tv_before.tv_sec)*1000*1000 + (tv_after.tv_usec - tv_before.tv_usec) >= 1*1000*1000)
		{
			printf("%s Current Frame Rate:%d fps.\n", video_dev->path, frame_cnt);
			frame_cnt = 0;

			if(gettimeofday(&tv_before, NULL))
				UVC_WRN(video_dev, "gettimeofday update: %s.\n", strerror(errno));
		}
		else
			frame_cnt++;
	}

	ret = video_streamoff(video_dev);
	if(ret != 0)
		pthread_exit(&ret);

	ret = St_BaseModuleDeinit(video_dev);
	if(ret != 0)
		pthread_exit(&ret);

    pthread_exit(NULL);
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

    char path[20] = {0};
    sprintf(path, "/dev/video%d", g_device_index);
    memcpy(video_dev.path, path, 20);
    if(video_init(&video_dev) != 0)
    	return -1;

    g_exit = false;
    pthread_create(&stream_tid, NULL, uvc_stream, &video_dev);

    return;
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

	g_exit = true;
	if (stream_tid)
	{
		pthread_join(stream_tid, NULL);
		stream_tid = 0;
	}

	video_deinit(&video_dev);

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

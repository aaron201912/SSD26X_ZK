/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _MI_VIF_DATATYPE_H_
#define _MI_VIF_DATATYPE_H_

#include "mi_sys_datatype.h"

#pragma pack(push)
#pragma pack(4)

typedef enum
{
    E_MI_VIF_ERR_INVALID_PORTID = E_MI_ERR_MAX + 1,
    E_MI_VIF_ERR_FAILED_DEVNOTENABLE,   /* device not enable*/
    E_MI_VIF_ERR_FAILED_DEVNOTDISABLE,       /* device not disable*/
    E_MI_VIF_ERR_FAILED_PORTNOTENABLE,   /* port not enable*/
    E_MI_VIF_ERR_FAILED_PORTNOTDISABLE,     /* port not disable*/
    E_MI_VIF_ERR_CFG_TIMEOUT,             /* config timeout*/
    E_MI_VIF_ERR_NORM_UNMATCH,            /* video norm of ADC and VIU is unmatch*/
    E_MI_VIF_ERR_INVALID_WAYID,           /* invlalid way ID     */
    E_MI_VIF_ERR_INVALID_PHYCHNID,        /* invalid phychn id*/
    E_MI_VIF_ERR_FAILED_NOTBIND,          /* device or channel not bind */
    E_MI_VIF_ERR_FAILED_BINDED,           /* device or channel not unbind */
} MI_VIF_ErrCode_e;

#define MI_VIF_SUCCESS (0)
#define MI_VIF_FAIL (1)

#define MI_ERR_VIF_INVALID_DEVID         MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_DEVID)
#define MI_ERR_VIF_INVALID_CHNID         MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_INVALID_CHNID)
#define MI_ERR_VIF_INVALID_PARA          MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_ILLEGAL_PARAM)
#define MI_ERR_VIF_INVALID_NULL_PTR      MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NULL_PTR)
#define MI_ERR_VIF_FAILED_NOTCONFIG      MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_CONFIG)
#define MI_ERR_VIF_NOT_SUPPORT           MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_SUPPORT)
#define MI_ERR_VIF_NOT_PERM              MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOT_PERM)
#define MI_ERR_VIF_NOMEM                 MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_NOMEM)
#define MI_ERR_VIF_BUF_EMPTY             MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_EMPTY)
#define MI_ERR_VIF_BUF_FULL              MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUF_FULL)
#define MI_ERR_VIF_SYS_NOTREADY          MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_SYS_NOTREADY)

#define MI_ERR_VIF_BUSY                  MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_ERR_BUSY)
#define MI_ERR_VIF_INVALID_PORTID        MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_VIF_ERR_INVALID_PORTID)/* 0xA0108040*/
#define MI_ERR_VIF_FAILED_DEVNOTENABLE      MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_VIF_ERR_FAILED_DEVNOTENABLE)/* 0xA0108040*/
#define MI_ERR_VIF_FAILED_DEVNOTDISABLE     MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_VIF_ERR_FAILED_DEVNOTDISABLE)/* 0xA0108041*/
#define MI_ERR_VIF_FAILED_PORTNOTENABLE    MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_VIF_ERR_FAILED_PORTNOTENABLE)/* 0xA0108042*/
#define MI_ERR_VIF_FAILED_PORTNOTDISABLE    MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_VIF_ERR_FAILED_PORTNOTDISABLE)/* 0xA0108042*/
#define MI_ERR_VIF_CFG_TIMEOUT           MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_VIF_ERR_CFG_TIMEOUT)/* 0xA0108043*/
#define MI_ERR_VIF_NORM_UNMATCH          MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_VIF_ERR_NORM_UNMATCH)/* 0xA0108044*/
#define MI_ERR_VIF_INVALID_WAYID         MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_VIF_ERR_INVALID_WAYID)/* 0xA0108045*/
#define MI_ERR_VIF_INVALID_PHYCHNID      MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_VIF_ERR_INVALID_PHYCHNID)/* 0xA0108046*/
#define MI_ERR_VIF_FAILED_NOTBIND        MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_VIF_ERR_FAILED_NOTBIND)/* 0xA0108047*/
#define MI_ERR_VIF_FAILED_BINDED         MI_DEF_ERR(E_MI_MODULE_ID_VIF, E_MI_ERR_LEVEL_ERROR, E_MI_VIF_ERR_FAILED_BINDED)/* 0xA0108048*/

typedef MI_U32 MI_VIF_GROUP;
typedef MI_U32 MI_VIF_DEV;
typedef MI_U32 MI_VIF_PORT;

#define MI_VIF_MAX_GROUP_DEV_CNT 4

/* interface mode of video input */
/*
BT 601: 16位数据传输；21芯；Y、U、V信号同时传输。并行数据，行场同步有单独输出;
BT 656:  8位数据传输； 9芯，不需要同步信号；串行视频传输；传输速率是601的2倍；
         先传Y，后传UV。656输出的是串行数据，行场同步信号嵌入在数据流中

656只是数据传输接口而已，可以说是作为601的一个传输方式。
简单的说ITU-R BT.601是"演播室数字电视编码参数"标准,而ITU-R BT.656 则是ITU-R BT.601附件A中的数字接口标准,
用于主要数字视频设备(包括芯 片)之间采用27Mhz/s并口或243Mb/s串行接口的数字传输接口标准.

601号建议

    它规定了取样频率与取样结构。
        例如：在4：2：2等级的编码中，规定亮度信号和色差信号的取样频率分别为13.5MHZ和6.75MHZ   ,
        取样结构为正交结构,即按行、场、帧重复,每行中的R-Y和B-Y取样与奇次(1,3,5……)Y的取样同位置，
        即取样结构是固定的，取样点在电视屏幕上的相对位置不变。
    它规定了编码方式。
        对亮度信号和两个色差信号进行线性PCM编码，每个取样点取8比特量化。
        同时，规定在数字编码时，不使用A/D转换的整个动态范围，只给亮度信号分配220个量化级，        黑电平对应于量化级16，白电平对应于量化级235。
        为每个色差信号分配224个量化级，色差信号的零电平对应于量化级128。

以4：2：2编码标准为例，其比特流为：13.5×8+6.75×8×2 = 216Mb/S。
若采用4：4：4编码方式，即对复合信号直接编码，其抽样频率取为13.3×8=106.4 Mb/S
*/

typedef enum
{
    /*
    输入数据的协议符合标准BT.656 协议，端口    数据输入模式为亮度色度复合模式，分量模    式为单分量。
    */
    E_MI_VIF_MODE_BT656,
    /*
    如果是Sensor，则为VI_MODE_DIGITAL_CAMERA    输入数据的协议为Digital camera 协议，
    端口  数据输入模式为亮度色度复合模式，分量模    式为单分量
    */
    E_MI_VIF_MODE_DIGITAL_CAMERA,
    /*
    BT.1120 协议（BT.656+双分量），端口数据输入模式为亮度色度分离模式，分量模式为双分量。
    */
    E_MI_VIF_MODE_BT1120_STANDARD,
    /*
    BT.1120 interleave 模式，端口数据输入模式为亮度色度分离模式，分量模式为双分量。
    */
    E_MI_VIF_MODE_BT1120_INTERLEAVED,
    E_MI_VIF_MODE_MIPI,
    E_MI_VIF_MODE_MAX
} MI_VIF_IntfMode_e;

typedef enum
{
    E_MI_VIF_WORK_MODE_1MULTIPLEX,
    E_MI_VIF_WORK_MODE_2MULTIPLEX,
    E_MI_VIF_WORK_MODE_4MULTIPLEX,

    E_MI_VIF_WORK_MODE_MAX
} MI_VIF_WorkMode_e;


typedef enum
{
    E_MI_VIF_FRAMERATE_FULL,
    E_MI_VIF_FRAMERATE_HALF,
    E_MI_VIF_FRAMERATE_QUARTER,
    E_MI_VIF_FRAMERATE_OCTANT,
    E_MI_VIF_FRAMERATE_THREE_QUARTERS,
    E_MI_VIF_FRAMERATE_MAX
} MI_VIF_FrameRate_e;

typedef enum
{
    // 时钟单沿模式，且VI 设备在上升沿采样
    E_MI_VIF_CLK_EDGE_SINGLE_UP,
    // 时钟单沿模式，且VI 设备在下降沿采样
    E_MI_VIF_CLK_EDGE_SINGLE_DOWN,
    // 前端送过来双沿数据时，VI 进行双沿采样
    E_MI_VIF_CLK_EDGE_DOUBLE,
    E_MI_VIF_CLK_EDGE_MAX
} MI_VIF_ClkEdge_e;

typedef enum
{
    E_MI_VIF_HDR_TYPE_OFF,
    E_MI_VIF_HDR_TYPE_VC,                 //virtual channel mode HDR,vc0->long, vc1->short
    E_MI_VIF_HDR_TYPE_DOL,
    E_MI_VIF_HDR_TYPE_EMBEDDED,  //compressed HDR mode
    E_MI_VIF_HDR_TYPE_LI,                //Line interlace HDR
    E_MI_VIF_HDR_TYPE_MAX
} MI_VIF_HDRType_e;

typedef enum
{
    E_MI_VIF_MCLK_12MHZ,
    E_MI_VIF_MCLK_18MHZ,
    E_MI_VIF_MCLK_27MHZ,
    E_MI_VIF_MCLK_36MHZ,
    E_MI_VIF_MCLK_54MHZ,
    E_MI_VIF_MCLK_108MHZ,
    E_MI_VIF_MCLK_MAX
}MI_VIF_MclkSource_e;

typedef enum
{
    E_MI_VIF_GROUPMASK_ID0  = 0x0001,
    E_MI_VIF_GROUPMASK_ID1  = 0x0002,
    E_MI_VIF_GROUPMASK_ID2  = 0x0004,
    E_MI_VIF_GROUPMASK_ID3  = 0x0008,
    E_MI_VIF_GROUPMASK_ID_MAX = 0xffff
} MI_VIF_GroupIdMask_e;


typedef enum
{
    E_MI_VIF_SNRPAD_ID_0 = 0,
    E_MI_VIF_SNRPAD_ID_1 = 1,
    E_MI_VIF_SNRPAD_ID_2 = 2,
    E_MI_VIF_SNRPAD_ID_3 = 3,
    E_MI_VIF_SNRPAD_ID_MAX,
    E_MI_VIF_SNRPAD_ID_NA = 0xFF,
}MI_VIF_SNRPad_e;

typedef struct MI_VIF_GroupAttr_s
{
    MI_VIF_IntfMode_e       eIntfMode;
    MI_VIF_WorkMode_e       eWorkMode;
    MI_VIF_HDRType_e        eHDRType;
    MI_VIF_ClkEdge_e        eClkEdge; //BT656
    MI_VIF_MclkSource_e     eMclk;
    MI_SYS_FrameScanMode_e  eScanMode;
    MI_U32                 u32GroupStitchMask; //multi vif dev bitmask by MI_VIF_GroupIdMask_e
} MI_VIF_GroupAttr_t;

typedef struct MI_VIF_GroupExtAttr_s
{
    MI_VIF_SNRPad_e eSensorPadID;   //sensor Pad id
    MI_U32 u32PlaneID[MI_VIF_MAX_GROUP_DEV_CNT];              //For HDR,0 is short exposure, 1 is long exposure
} MI_VIF_GroupExtAttr_t;

typedef struct MI_VIF_DevAttr_s
{
    MI_SYS_PixelFormat_e   eInputPixel;
    MI_SYS_WindowRect_t    stInputRect;
    MI_SYS_FieldType_e     eField;
    MI_BOOL                bEnH2T1PMode;
} MI_VIF_DevAttr_t;

typedef struct MI_VIF_OutputPortAttr_s
{
    /*
    采集区域起始坐标用于配置需要采集的矩形图像相对于原始图像起始点的位置。
    stCapRect 的宽与高为静态属性， 其他项为动态属性；无次属性
    stCapRect 是针对原始图像进行裁剪；
    stCapRect 中 s32X 和u32Width 必须 2 对齐；
                s32Y 和u32Height 逐行采集时必须 2 对齐
                s32Y 和u32Height 隔行采集时必须 4 对齐
    */
    MI_SYS_WindowRect_t              stCapRect;

    MI_SYS_WindowSize_t              stDestSize;

    MI_SYS_PixelFormat_e   ePixFormat;

    MI_VIF_FrameRate_e     eFrameRate;
} MI_VIF_OutputPortAttr_t;

typedef struct MI_VIF_DevPortStat_s
{
    // 通道是否使能
    MI_BOOL bEnable;
    // 中断计数
    // can not support ?
    MI_U32  u32IntCnt;
    // 每10 秒的平均帧率，该值不一定精确
    MI_U32  u32FrameRate;
    // 中断丢失计数,该结构体的中断丢失计数一直在增加，说明 VI 工作出现异常
    // can not support?
    MI_U32  u32LostInt;
    // 获取VB 失败计数
    MI_U32  u32VbFail;
    // 图像宽度
    MI_U32  u32PicWidth;
    // 图像高度
    MI_U32  u32PicHeight;
} MI_VIF_DevPortStat_t;

typedef struct MI_VIF_VIFDevStatus_s
{
    MI_VIF_GROUP  GroupId;
    MI_BOOL  bGroupCreated;
    MI_U32 bDevEn;
    MI_VIF_SNRPad_e eSensorPadID;
    MI_U32 u32PlaneID;
} MI_VIF_DevStatus_t;

typedef MI_S32 (*MI_VIF_CALLBK_FUNC)(MI_U64 u64Data);

typedef enum
{
    E_MI_VIF_CALLBACK_ISR,
    E_MI_VIF_CALLBACK_MAX,
} MI_VIF_CallBackMode_e;

typedef enum
{
    E_MI_VIF_IRQ_FRAMESTART,  //frame start irq
    E_MI_VIF_IRQ_FRAMEEND,  //frame end irq
    E_MI_VIF_IRQ_LINEHIT,  //frame line hit irq
    E_MI_VIF_IRQ_MAX,
} MI_VIF_IrqType_e;

typedef struct MI_VIF_CallBackParam_s
{
    MI_VIF_CallBackMode_e eCallBackMode;
    MI_VIF_IrqType_e  eIrqType;
    MI_VIF_CALLBK_FUNC  pfnCallBackFunc;
    MI_U64    u64Data;
} MI_VIF_CallBackParam_t;

#pragma pack(pop)

#endif //_MI_VIF_DATATYPE_H_



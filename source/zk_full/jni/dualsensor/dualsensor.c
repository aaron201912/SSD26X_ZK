#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/syscall.h>

#include "mi_sensor.h"
#include "mi_sensor_datatype.h"
#include "mi_vif.h"
#include "mi_ldc.h"
#include "mi_isp.h"
#include "mi_scl.h"
#include "mi_vdec.h"
#include "mi_disp.h"
#include "mi_panel.h"
#include "mi_isp_iq.h"
#include "mi_isp_cus3a_api.h"
#include "st_common.h"
#include "st_hdmi.h"
#include "st_disp.h"

#include "dualsensor.h"

#define  ASCII_CR       0x0d
#define  ASCII_LF       0x0a

#define ST_MAX_SENSOR_NUM (3)

#define USE_HDMI_4_SCREEN 0

#define GPIO_PANEL_PWR 12
#define GPIO_PANEL_BL 13
#define GPIO_PANEL_RST 135

#ifndef ALIGN_UP
#define ALIGN_UP(val, alignment) ((((val)+(alignment)-1)/(alignment))*(alignment))
#endif

#ifndef ALIGN_DOWN
#define ALIGN_DOWN(val, alignment) (((val)/(alignment))*(alignment))
#endif

#if defined(USE_ONE_SENSOR)
#if !defined(SENSOR_PAD)
MI_VIF_SNRPad_e g_eSensorPad = 0;
#else
MI_VIF_SNRPad_e g_eSensorPad = SENSOR_PAD;
#endif
#endif

#define TO_SYS_CHN_ID(layerid,portid) \
({\
    int chnid = 0;\
    if((layerid == 0 || layerid == 2) && portid <= 15)\
    {\
        chnid = portid;\
    }\
    else if((layerid == 1 || layerid == 3) && portid == 0)\
    {\
        chnid = 16;\
    }\
    else\
    {\
        chnid = 0;\
    }\
    chnid;\
})

#define TO_DISP_LAYER_ID(devid,chnid)\
({\
    int layerid=0;\
    if(devid == 0 && chnid <= 15)\
    {\
        layerid = 0;\
    }\
    else if(devid == 0 && chnid == 16)\
    {\
        layerid = 1;\
    }\
    else if(devid == 1 && chnid <= 15)\
    {\
        layerid = 2;\
    }\
    else if(devid == 1 && chnid == 16)\
    {\
        layerid = 3;\
    }\
    else\
    {\
        layerid = 0;\
    }\
    layerid;\
})

#define TO_DISP_PORT_ID(chnid)\
({\
    int portid = 0;\
    if(chnid <= 15)\
    {\
        portid = chnid;\
    }\
    else if(chnid == 16)\
    {\
        portid = 0;\
    }\
    else\
    {\
        portid = 0;\
    }\
    portid;\
})

typedef struct ST_Sensor_Attr_s
{
    MI_BOOL bUsed;
    MI_BOOL bDoFr;
    MI_VIF_SNRPad_e eSensorPadID;
    MI_BOOL bCreate;
    MI_VIF_GROUP u32VifGroupID;
    MI_U32 u32BindVifDev;
    MI_BOOL bPlaneMode;
    MI_U8 u8ResIndex;
    MI_U16 u16Width;
    MI_U16 u16Height;
    char *s8IqPath;
}ST_Sensor_Attr_t;

typedef struct ST_Display_Opt_s
{
    MI_U32 u32DispPort;
    MI_U8 u8SensorCnt;   /* hdmi inputport use 0 ~ cnt */
    /* Panel parameter */
    MI_BOOL bUsedPanel;
    MI_DISP_DEV s32PanelDev;
    MI_DISP_LAYER s32PanelLayer;
    MI_PANEL_IntfType_e ePanelType;
    MI_DISP_OutputTiming_e u32PanelDispTiming;
    MI_U8 u8PanelInputPort;
    MI_U8 res0[3];
    MI_U32 u32PanelW;
    MI_U32 u32PanelH;
    MI_BOOL bPanelMirror;
    MI_BOOL bPanelFlip;
    MI_SYS_Rotate_e ePanelRot;

    /* HDMI parameter */
    MI_BOOL bUsedHdmi;
    MI_DISP_DEV s32HdmiDev;
    MI_DISP_LAYER s32HdmiLayer;
    MI_U32 u32HdmiW;
    MI_U32 u32HdmiH;
    MI_U8 res1[3];
    MI_HDMI_TimingType_e eCurHdmiTiming;
    MI_DISP_OutputTiming_e u32HdmiDispTiming;
}ST_Display_Opt_t;

#ifdef USE_ONE_SENSOR
int g_s32SensorIdxForPanel = 0;
int g_s32SensorIdxForHdmi = 0;
#else
int g_s32SensorIdxForPanel = 1;
int g_s32SensorIdxForHdmi = 0;
#endif

static ST_Sensor_Attr_t gstSensorAttr[ST_MAX_SENSOR_NUM] =
{
    [0] =
    {
        .bUsed = 1,
        .bDoFr = 0,
    #ifdef USE_ONE_SENSOR
        .eSensorPadID = (MI_VIF_SNRPad_e)g_eSensorPad,
        .u32VifGroupID = 2 * g_eSensorPad,
        .u32BindVifDev = 8 * g_eSensorPad,
    #else
        .eSensorPadID = E_MI_VIF_SNRPAD_ID_0,
        .u32VifGroupID = 0,
        .u32BindVifDev = 0,
    #endif
        .bPlaneMode = 1,
        .u8ResIndex = 0/* 3840*2160,20fps @imx415 res 0 */,
        .s8IqPath = (char*)"/customer/mi_demo/imx415_api.bin",
    },
    [1] =
    {
    #ifdef USE_ONE_SENSOR
        .bUsed = 0,
    #else
        .bUsed = 1,
    #endif
        .bDoFr = 1,
        .eSensorPadID = E_MI_VIF_SNRPAD_ID_1,
        .u32VifGroupID = 2,
        .u32BindVifDev = 8,
        .bPlaneMode = 1,
        .u8ResIndex = 0/* 1920*1080,30fps @imx307 res 0 */,
        .s8IqPath = (char*)"/customer/mi_demo/imx307_api.bin",
    },
    [2] =
    {
        .bUsed = 0,
        .bDoFr = 0,
        .eSensorPadID = E_MI_VIF_SNRPAD_ID_2,
        .u32VifGroupID = 1,
        .u32BindVifDev = 4,
        .bPlaneMode = 1,
        .u8ResIndex = 0xff,
        .s8IqPath = NULL,
    },
};

static MI_U32 u32IspRef = 0;
static MI_U32 u32SclRef = 0;

static ST_Display_Opt_t gstDisplayOpt =
{
    .u32DispPort = 0,
    .u8SensorCnt = 0,
    /* Panel parameter */
    .bUsedPanel = 1,
    .s32PanelDev = 0,
    .s32PanelLayer = 0,
    .ePanelType = E_MI_PNL_INTF_TTL,
    .u32PanelDispTiming = E_MI_DISP_OUTPUT_USER,
    .u8PanelInputPort = 0,
    .u32PanelW = 0,
    .u32PanelH = 0,
    .bPanelMirror = 0,
    .bPanelFlip = 0,
    .ePanelRot = E_MI_SYS_ROTATE_NONE,

    /* HDMI parameter */
    .bUsedHdmi = 1,
    .s32HdmiDev = 1,
    .s32HdmiLayer = 2, /* Disp 1, layer should be 2 */
    .u32HdmiW = 0,
    .u32HdmiH = 0,
    .eCurHdmiTiming = E_MI_HDMI_TIMING_1080_60P,
};

static MI_BOOL bExit = FALSE;

//static pthread_t tidPollTsk;

#ifdef ENABLE_FR
const char *g_paramList = "s:S:d:D:p:P:r:R:f:F:h";
#else
const char *g_paramList = "s:S:d:D:p:P:r:R:h";
#endif

static void ST_Usage()
{
    printf("Usage: eg. prog_panel -S 2 -D 1080,60 -P mipi\n");
    printf("Sigmastar panel demo.\n\n");
    printf("   -S,-s: sensor count, default 2 sensor.\n");
    //printf("   -D,-d: hdmi output, if not parameter use default 1080p@60fps. eg. 1080,60\n");
    printf("   -P,-p: panel output, default mipi. eg. mipi ttl or none\n");
    printf("   -R,-r: need rotation for panel display. 0/90/180/270\n");
#ifdef ENABLE_FR
    printf("   -F,-f: select which sensor for face recognize, default sensor pad 1\n");
#endif
    printf("      -h: help\n");
}

#if 1//def ENABLE_FR
#ifdef __cplusplus
extern "C" {
#endif
extern MI_S32 ST_FRStart(MI_SCL_DEV sclDev, MI_SCL_CHANNEL sclChn,
                        MI_U16 u16SrcWidth, MI_U16 u16SrcHeight,
                        MI_SCL_PORT rgnPort, MI_SCL_PORT capPort);
extern MI_S32 ST_FRAddPerson(MI_S32 s32Frid, char *args);
extern MI_S32 ST_FRStop(MI_S32 s32Frid);
#ifdef __cplusplus
}
#endif
#endif

static MI_S32 ST_ParserArgs(int argc, char *argv[])
{
    int i, ch = 0;
#ifdef ENABLE_FR
    int sensorPad;
#endif
    int sensorCnt, rot;
    while(-1 != (ch = getopt(argc, argv, g_paramList)))
    {
        switch(ch)
        {
            case 's':
            case 'S':
            {
                sensorCnt = atoi(optarg);
                if(sensorCnt > 0 && sensorCnt <= 3)
                {
                    for(i = 0; i < sensorCnt; i++)
                    {
                        gstSensorAttr[i].bUsed = 1;
                    }
                    for(; i < ST_MAX_SENSOR_NUM; i++)
                    {
                        gstSensorAttr[i].bUsed = 0;
                    }
                }
                else
                {
                    printf("Sensor parameter error.\n");
                }
                break;
            }
            case 'd':
            case 'D':
            {
                break;
            }
            case 'p':
            case 'P':
            {
                if(0 == strcasecmp(optarg, "ttl"))
                {
                    gstDisplayOpt.ePanelType = E_MI_PNL_INTF_TTL;
                }
                break;
            }
            case 'r':
            case 'R':
            {
                rot = atoi(optarg);
                switch(rot)
                {
                    case 0:
                    case 360:
                    {
                        gstDisplayOpt.ePanelRot = E_MI_SYS_ROTATE_NONE;
                        break;
                    }
                    case 90:
                    {
                        gstDisplayOpt.ePanelRot = E_MI_SYS_ROTATE_90;
                        break;
                    }
                    case 180:
                    {
                        gstDisplayOpt.ePanelRot = E_MI_SYS_ROTATE_180;
                        break;
                    }
                    case 270:
                    {
                        gstDisplayOpt.ePanelRot = E_MI_SYS_ROTATE_270;
                        break;
                    }
                    default :
                    {
                        printf("Rotate parameter error.\n");
                        break;
                    }
                }
                break;
            }
        #ifdef ENABLE_FR
            case 'F':
            case 'f':
            {
                sensorPad = atoi(optarg);
                if(sensorPad < E_MI_VIF_SNRPAD_ID_0 || sensorPad >= E_MI_VIF_SNRPAD_ID_MAX)
                {
                    printf("Sensor pad[%d] for face recognize error.\n", sensorPad);
                    break;
                }
                for(i = 0; i < ST_MAX_SENSOR_NUM; i++)
                {
                    gstSensorAttr[i].bDoFr = (sensorPad == gstSensorAttr[i].eSensorPadID) ? 1 : 0;
                }
                break;
            }
        #endif
            case 'h':
            default:
            {
                ST_Usage();
                return -1;
            }
        }
    }

    return 0;
}

static void ST_Flush(void)
{
    char c;

    while((c = getchar()) != '\n' && c != EOF);
}

static void ST_Quit(int signo)
{
    if(SIGINT == signo)
    {
        printf("Get SIGINT, exit!\n");
        bExit = 1;
    }
}

MI_S32 ST_GetCropParam(MI_S32 srcWidth, MI_S32 srcHeight,
        MI_S32 dstWidth, MI_S32 dstHeight, MI_SYS_WindowRect_t *pStCropRect)
{
    float scale = 0.0;

    if(NULL == pStCropRect)
    {
        return -1;
    }

    memset(pStCropRect, 0, sizeof(MI_SYS_WindowRect_t));
    pStCropRect->u16Width = ALIGN_DOWN(srcWidth, 16);
    pStCropRect->u16Height = ALIGN_DOWN(srcHeight, 16);
    scale = dstWidth * 1.0 / dstHeight;
    if(scale * pStCropRect->u16Height > pStCropRect->u16Width)
    {
        pStCropRect->u16Height = ALIGN_DOWN((MI_U16)(pStCropRect->u16Width * dstHeight / dstWidth), 16);
        pStCropRect->u16Y = (srcHeight - pStCropRect->u16Height) / 2;
    }
    else
    {
        pStCropRect->u16Width = ALIGN_DOWN((MI_U16)(pStCropRect->u16Height * dstWidth / dstHeight), 16);
        pStCropRect->u16X = (srcWidth - pStCropRect->u16Width) / 2;
    }

    return 0;
}

MI_S32 ST_SetGpioOut(MI_U32 gpio, MI_U8 val)
{
    char cmd[100] = {0};

    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/export", gpio);
    system(cmd);
    usleep(100 * 1000);
    snprintf(cmd, sizeof(cmd), "echo out > /sys/class/gpio/gpio%d/direction", gpio);
    system(cmd);
    usleep(100 * 1000);
    snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio%d/value", val, gpio);
    system(cmd);
    usleep(100 * 1000);

    return MI_SUCCESS;
}

MI_S32 ST_MiscInit()
{
    ST_SetGpioOut(GPIO_PANEL_PWR, 1);
    ST_SetGpioOut(GPIO_PANEL_RST, 1);
    ST_SetGpioOut(GPIO_PANEL_BL, 1);

    return MI_SUCCESS;
}

MI_S32 ST_MiscUnInit()
{
    return MI_SUCCESS;
}

MI_S32 ST_SysModuleInit()
{
    ST_Sys_Init();
    return MI_SUCCESS;
}

static ST_DispoutTiming_e ST_TimingConvert(MI_HDMI_TimingType_e eHdmiTiming)
{
    switch(eHdmiTiming)
    {
        case E_MI_HDMI_TIMING_720_50P:
        {
            return E_ST_TIMING_720P_50;
        }
        case E_MI_HDMI_TIMING_720_60P:
        {
            return E_ST_TIMING_720P_60;
        }
        case E_MI_HDMI_TIMING_1080_50P:
        {
            return E_ST_TIMING_1080P_50;
        }
        case E_MI_HDMI_TIMING_1080_60P:
        {
            return E_ST_TIMING_1080P_60;
        }
        case E_MI_HDMI_TIMING_1600x1200_60P:
        {
            return E_ST_TIMING_1600x1200_60;
        }
        case E_MI_HDMI_TIMING_1440x900_60P:
        {
            return E_ST_TIMING_1440x900_60;
        }
        case E_MI_HDMI_TIMING_1280x1024_60P:
        {
            return E_ST_TIMING_1280x1024_60;
        }
        case E_MI_HDMI_TIMING_1024x768_60P:
        {
            return E_ST_TIMING_1024x768_60;
        }
        case E_MI_HDMI_TIMING_1440_30P:
        {
            return E_ST_TIMING_2560x1440_30;
        }
        case E_MI_HDMI_TIMING_4K2K_30P:
        {
            return E_ST_TIMING_4k2k_30;
        }

        default :
        {
            return E_ST_TIMING_MAX;
        }
    }
}

MI_S32 ST_ResetHdmiDispFlow(ST_DispoutTiming_e eStHdmiDispTiming);

static MI_S32 ST_HdmiCallback(MI_HDMI_DeviceId_e eHdmi, MI_HDMI_EventType_e Event, void *pEventParam, void *pUsrParam)
{
    ST_DispoutTiming_e eStHdmiDispTiming;
    MI_HDMI_SinkInfo_t stSinkInfo;

    switch (Event)
    {
        case E_MI_HDMI_EVENT_HOTPLUG:
            printf("E_MI_HDMI_EVENT_HOTPLUG.\n");
            MI_HDMI_Start(eHdmi);
            sleep(1);
            MI_HDMI_GetSinkInfo(E_MI_HDMI_ID_0, &stSinkInfo);
            if(TRUE == stSinkInfo.bConnected/* && \
                gstDisplayOpt.eCurHdmiTiming != stSinkInfo.eNativeTimingType*/)
            {
                printf("HDMI timing change last[%d] current[%d]\n", gstDisplayOpt.eCurHdmiTiming, stSinkInfo.eNativeTimingType);
                MI_HDMI_Stop(eHdmi);
                gstDisplayOpt.eCurHdmiTiming = stSinkInfo.eNativeTimingType;
                eStHdmiDispTiming = ST_TimingConvert(stSinkInfo.eNativeTimingType);
                ST_ResetHdmiDispFlow(eStHdmiDispTiming);
                STCHECKRESULT(ST_Hdmi_Start(E_MI_HDMI_ID_0, stSinkInfo.eNativeTimingType));
                STCHECKRESULT(MI_HDMI_SetAvMute(E_MI_HDMI_ID_0, FALSE));
            }
            break;
        case E_MI_HDMI_EVENT_NO_PLUG:
            printf("E_MI_HDMI_EVENT_NO_PLUG.\n");
            ExecFunc(MI_HDMI_Stop(eHdmi), MI_SUCCESS);
            break;
        default:
            printf("Unsupport event.\n");
            break;
    }

    return MI_SUCCESS;
}

MI_S32 ST_HdmiInit(MI_HDMI_EventCallBack pfnHdmiEventCallback)
{
    MI_HDMI_InitParam_t stInitParam;
    MI_HDMI_DeviceId_e eHdmi = E_MI_HDMI_ID_0;

    stInitParam.pCallBackArgs = NULL;
    stInitParam.pfnHdmiEventCallback = pfnHdmiEventCallback;

    STCHECKRESULT(MI_HDMI_Init(&stInitParam));
    STCHECKRESULT(MI_HDMI_Open(eHdmi));

    return MI_SUCCESS;
}

/* Init panel and/or hdmi */
MI_S32 ST_DispDevInit(ST_Display_Opt_t *pstDisplayOpt)
{
    MI_DISP_LAYER s32DispLayer;
    MI_U32 u32LayerW = 0;
    MI_U32 u32LayerH = 0;
    MI_DISP_PubAttr_t stDispPubAttr;
    MI_DISP_VideoLayerAttr_t stLayerAttr;
    MI_DISP_OutputTiming_e u32DispTiming;
    MI_PANEL_ParamConfig_t stParamCfg;
    MI_PANEL_MipiDsiConfig_t stMipiDsiCfg;
    MI_HDMI_TimingType_e u32HdmiTiming;
    MI_HDMI_SinkInfo_t stSinkInfo;
    ST_DispoutTiming_e eStHdmiDispTiming;

    if(NULL == pstDisplayOpt)
    {
        return -1;
    }

    /* Init Panel */
    if(0)//pstDisplayOpt->bUsedPanel)
    {
        s32DispLayer = pstDisplayOpt->s32PanelLayer;
        memset(&stParamCfg, 0x0, sizeof(MI_PANEL_ParamConfig_t));
        memset(&stMipiDsiCfg, 0x0, sizeof(MI_PANEL_MipiDsiConfig_t));
        /* set disp pub */
        memset(&stDispPubAttr, 0, sizeof(MI_DISP_PubAttr_t));
        stDispPubAttr.u32BgColor = YUYV_BLACK;
        stDispPubAttr.eIntfType = (E_MI_PNL_INTF_MIPI_DSI == \
            pstDisplayOpt->ePanelType) ? E_MI_DISP_INTF_MIPIDSI : E_MI_DISP_INTF_TTL;
        stDispPubAttr.eIntfSync =  E_MI_DISP_OUTPUT_USER;
        STCHECKRESULT(MI_DISP_SetPubAttr(pstDisplayOpt->s32PanelDev, &stDispPubAttr));
        STCHECKRESULT(MI_DISP_Enable(pstDisplayOpt->s32PanelDev));
    #if 0
        if(E_MI_PNL_INTF_MIPI_DSI == pstDisplayOpt->ePanelType)
        {
            MI_PANEL_SetMipiDsiConfig(pstDisplayOpt->ePanelType, stMipiDsiCfg);
        }
    #endif
        STCHECKRESULT(MI_PANEL_Init(pstDisplayOpt->ePanelType));
        STCHECKRESULT(MI_PANEL_GetPanelParam(pstDisplayOpt->ePanelType, &stParamCfg));
        u32LayerW = stParamCfg.u16Width;
        u32LayerH = stParamCfg.u16Height;
        if(E_MI_SYS_ROTATE_90 == pstDisplayOpt->ePanelRot ||\
            E_MI_SYS_ROTATE_270 == pstDisplayOpt->ePanelRot)
        {
            pstDisplayOpt->u32PanelW = u32LayerH;
            pstDisplayOpt->u32PanelH = u32LayerW;
        }
        else
        {
            pstDisplayOpt->u32PanelW = u32LayerW;
            pstDisplayOpt->u32PanelH = u32LayerH;
        }

        /* set layer */
        memset(&stLayerAttr, 0x0, sizeof(MI_DISP_VideoLayerAttr_t));
        stLayerAttr.stVidLayerDispWin.u16X = 0;
        stLayerAttr.stVidLayerDispWin.u16Y = 0;
        stLayerAttr.stVidLayerDispWin.u16Width = u32LayerW;
        stLayerAttr.stVidLayerDispWin.u16Height = u32LayerH;
        stLayerAttr.stVidLayerSize.u16Width = u32LayerW;
        stLayerAttr.stVidLayerSize.u16Height = u32LayerH;
        stLayerAttr.ePixFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        STCHECKRESULT(MI_DISP_BindVideoLayer(s32DispLayer, pstDisplayOpt->s32PanelDev));
        STCHECKRESULT(MI_DISP_SetVideoLayerAttr(s32DispLayer, &stLayerAttr));
        STCHECKRESULT(MI_DISP_EnableVideoLayer(s32DispLayer));

        pstDisplayOpt->u32DispPort++;
    }
    else
    {
    	STCHECKRESULT(MI_PANEL_GetPanelParam(pstDisplayOpt->ePanelType, &stParamCfg));
		u32LayerW = stParamCfg.u16Width;
		u32LayerH = stParamCfg.u16Height;
		if(E_MI_SYS_ROTATE_90 == pstDisplayOpt->ePanelRot ||\
			E_MI_SYS_ROTATE_270 == pstDisplayOpt->ePanelRot)
		{
			pstDisplayOpt->u32PanelW = u32LayerH;
			pstDisplayOpt->u32PanelH = u32LayerW;
		}
		else
		{
			pstDisplayOpt->u32PanelW = u32LayerW;
			pstDisplayOpt->u32PanelH = u32LayerH;
		}
		pstDisplayOpt->u32DispPort++;
    }

    /* Init HDMI */
    if(pstDisplayOpt->bUsedHdmi)
    {
        s32DispLayer = pstDisplayOpt->s32HdmiLayer;
        MI_HDMI_GetSinkInfo(E_MI_HDMI_ID_0, &stSinkInfo);
        if(TRUE == stSinkInfo.bConnected)
        {
            pstDisplayOpt->eCurHdmiTiming = stSinkInfo.eNativeTimingType;
        }
        eStHdmiDispTiming = ST_TimingConvert(pstDisplayOpt->eCurHdmiTiming);
        ST_GetTimingInfo(eStHdmiDispTiming, &u32HdmiTiming, &u32DispTiming, &u32LayerW, &u32LayerH);
        ST_DBG("eStDispTiming: %d\n", eStHdmiDispTiming);
        ST_DBG("u32HdmiTiming: %d\n", u32HdmiTiming);
        ST_DBG("u32DispTiming: %d\n", u32DispTiming);
        ST_DBG("u32LayerW: %d u32LayerH: %d\n", u32LayerW, u32LayerH);
        pstDisplayOpt->u32HdmiW = u32LayerW;
        pstDisplayOpt->u32HdmiH = u32LayerH;
        memset(&stDispPubAttr, 0, sizeof(MI_DISP_PubAttr_t));
        stDispPubAttr.eIntfSync = u32DispTiming;
        stDispPubAttr.eIntfType = E_MI_DISP_INTF_HDMI;
        stDispPubAttr.u32BgColor = YUYV_BLACK;
        ST_DBG("eIntfSync: %d eIntfType: %d\n", stDispPubAttr.eIntfSync, stDispPubAttr.eIntfType);
        STCHECKRESULT(MI_DISP_SetPubAttr(pstDisplayOpt->s32HdmiDev, &stDispPubAttr));
        if(stDispPubAttr.eIntfType == E_MI_DISP_INTF_HDMI
            && stDispPubAttr.eIntfSync != E_MI_DISP_OUTPUT_2560x1440_30
            && stDispPubAttr.eIntfSync != E_MI_DISP_OUTPUT_2560x1600_60
            && stDispPubAttr.eIntfSync != E_MI_DISP_OUTPUT_3840x2160_30
            && stDispPubAttr.eIntfSync != E_MI_DISP_OUTPUT_3840x2160_60)
        {
            stDispPubAttr.eIntfType = E_MI_DISP_INTF_VGA;
            ST_DBG("eIntfSync: %d eIntfType: %d\n", stDispPubAttr.eIntfSync, stDispPubAttr.eIntfType);
            STCHECKRESULT(MI_DISP_SetPubAttr(pstDisplayOpt->s32HdmiDev, &stDispPubAttr));
        }
        STCHECKRESULT(MI_DISP_Enable(pstDisplayOpt->s32HdmiDev));

        STCHECKRESULT(ST_HdmiInit(&ST_HdmiCallback));
        if(1)//TRUE == stSinkInfo.bConnected)
        {
            STCHECKRESULT(ST_Hdmi_Start(E_MI_HDMI_ID_0, u32HdmiTiming));
            STCHECKRESULT(MI_HDMI_SetAvMute(E_MI_HDMI_ID_0, FALSE));
        }

        /* set layer */
        memset(&stLayerAttr, 0x0, sizeof(MI_DISP_VideoLayerAttr_t));
        stLayerAttr.stVidLayerDispWin.u16X = 0;
        stLayerAttr.stVidLayerDispWin.u16Y = 0;
        stLayerAttr.stVidLayerDispWin.u16Width = u32LayerW;
        stLayerAttr.stVidLayerDispWin.u16Height = u32LayerH;
        stLayerAttr.stVidLayerSize.u16Width = u32LayerW;
        stLayerAttr.stVidLayerSize.u16Height = u32LayerH;
        stLayerAttr.ePixFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        STCHECKRESULT(MI_DISP_BindVideoLayer(s32DispLayer, pstDisplayOpt->s32HdmiDev));
        STCHECKRESULT(MI_DISP_SetVideoLayerAttr(s32DispLayer, &stLayerAttr));
        STCHECKRESULT(MI_DISP_EnableVideoLayer(s32DispLayer));

        pstDisplayOpt->u32DispPort++;
    }

    return MI_SUCCESS;
}


MI_S32 ST_DispDevUnInit(ST_Display_Opt_t *pstDisplayOpt)
{
    MI_DISP_LAYER s32DispLayer;

    if(NULL == pstDisplayOpt)
    {
        return -1;
    }

    /* Uninit Panel */
    if(0)//pstDisplayOpt->bUsedPanel)
    {
        s32DispLayer = pstDisplayOpt->s32PanelLayer;
        STCHECKRESULT(MI_DISP_UnBindVideoLayer(s32DispLayer, pstDisplayOpt->s32PanelDev));
        STCHECKRESULT(MI_DISP_DisableVideoLayer(s32DispLayer));
        STCHECKRESULT(MI_DISP_Disable(pstDisplayOpt->s32PanelDev));
        STCHECKRESULT(MI_PANEL_DeInit());
    }
    else
    {
        pstDisplayOpt->u32DispPort--;
    }

    /* Uninit HDMI */
    if(pstDisplayOpt->bUsedHdmi)
    {
        s32DispLayer = pstDisplayOpt->s32HdmiLayer;
        STCHECKRESULT(MI_DISP_UnBindVideoLayer(s32DispLayer, pstDisplayOpt->s32HdmiDev));
        STCHECKRESULT(MI_DISP_DisableVideoLayer(s32DispLayer));
        STCHECKRESULT(MI_DISP_Disable(pstDisplayOpt->s32HdmiDev));
        STCHECKRESULT(ST_Hdmi_DeInit(E_MI_HDMI_ID_0));
        pstDisplayOpt->u32DispPort--;
    }

    return MI_SUCCESS;
}

MI_S32 ST_SensorModuleInit(MI_U32 eSnrPad)
{
    MI_SNR_PADID eSnrPadId = eSnrPad;
    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    MI_U32 u32ResCount =0;
    MI_U8 u8ResIndex =0;
    MI_U8 u8ChocieRes = gstSensorAttr[eSnrPad].u8ResIndex;
    MI_S32 s32Input =0;
    MI_SNR_Res_t stRes;
    ST_Sensor_Attr_t *pstSensorAttr = NULL;
    memset(&stRes, 0x0, sizeof(MI_SNR_Res_t));
    memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));

    pstSensorAttr = &gstSensorAttr[eSnrPadId];

    if(pstSensorAttr->bPlaneMode == TRUE)
    {
        STCHECKRESULT(MI_SNR_SetPlaneMode(eSnrPad, FALSE));
    }
    else
    {
        STCHECKRESULT(MI_SNR_SetPlaneMode(eSnrPad, TRUE));
    }

    STCHECKRESULT(MI_SNR_QueryResCount(eSnrPadId, &u32ResCount));
    for(u8ResIndex=0; u8ResIndex < u32ResCount; u8ResIndex++)
    {
        STCHECKRESULT(MI_SNR_GetRes(eSnrPadId, u8ResIndex, &stRes));
        printf("index %d, Crop(%d,%d,%d,%d), outputsize(%d,%d), maxfps %d, minfps %d, ResDesc %s\n",
        u8ResIndex,
        stRes.stCropRect.u16X, stRes.stCropRect.u16Y, stRes.stCropRect.u16Width,stRes.stCropRect.u16Height,
        stRes.stOutputSize.u16Width, stRes.stOutputSize.u16Height,
        stRes.u32MaxFps,stRes.u32MinFps,
        stRes.strResDesc);
    }
    if((u8ChocieRes == 0xff) || (u8ChocieRes >= u32ResCount && u8ChocieRes != 0xff))
    {
        printf("choice which resolution use, cnt %d\n", u32ResCount);
        do
        {
            scanf("%d", &s32Input);
            u8ChocieRes = (MI_U8)s32Input;
            ST_Flush();
            STCHECKRESULT(MI_SNR_QueryResCount(eSnrPadId, &u32ResCount));
            if(u8ChocieRes >= u32ResCount)
            {
                printf("choice err res %d > =cnt %d\n", u8ChocieRes, u32ResCount);
            }
        }while(u8ChocieRes >= u32ResCount);
        printf("You select %d res\n", u8ChocieRes);
    }
    printf("Rest %d\n", u8ChocieRes);

    STCHECKRESULT(MI_SNR_SetRes(eSnrPadId, u8ChocieRes));
    STCHECKRESULT(MI_SNR_Enable(eSnrPadId));

    return MI_SUCCESS;
}

MI_S32 ST_SensorModuleUnInit(MI_SNR_PADID eSnrPad)
{
    MI_SNR_PADID eSnrPadId = eSnrPad;

    STCHECKRESULT(MI_SNR_Disable(eSnrPadId));

    return MI_SUCCESS;
}

MI_S32 ST_VifModuleInit(ST_Sensor_Attr_t *pstSensorAttr)
{
    MI_VIF_DEV vifDev =0;
    MI_VIF_PORT vifPort = 0;
    MI_SNR_PADInfo_t  stPad0Info;
    MI_SNR_PlaneInfo_t stSnrPlane0Info;
    MI_VIF_GroupAttr_t stGroupAttr;
    MI_VIF_DevAttr_t stVifDevAttr;
    MI_VIF_OutputPortAttr_t stVifPortInfo;
    MI_S32 eSnrPadId;
    MI_U32 u32PlaneId;
    MI_VIF_GROUP GroupId;

    if(NULL == pstSensorAttr || 0 == pstSensorAttr->bUsed)
    {
        return -1;
    }

    vifDev = pstSensorAttr->u32BindVifDev;
    eSnrPadId = pstSensorAttr->eSensorPadID;
    u32PlaneId = 0;
    GroupId = pstSensorAttr->u32VifGroupID;
    memset(&stGroupAttr, 0x0, sizeof(MI_VIF_GroupAttr_t));
    memset(&stPad0Info, 0x0, sizeof(MI_SNR_PADInfo_t));
    memset(&stSnrPlane0Info, 0x0, sizeof(MI_SNR_PlaneInfo_t));
    memset(&stVifDevAttr, 0x0, sizeof(MI_VIF_DevAttr_t));
    memset(&stVifPortInfo, 0, sizeof(MI_VIF_OutputPortAttr_t));

    STCHECKRESULT(MI_SNR_GetPadInfo(eSnrPadId, &stPad0Info));
    STCHECKRESULT(MI_SNR_GetPlaneInfo(eSnrPadId, u32PlaneId, &stSnrPlane0Info));
    pstSensorAttr->u16Width = stSnrPlane0Info.stCapRect.u16Width;
    pstSensorAttr->u16Height = stSnrPlane0Info.stCapRect.u16Height;

    if(pstSensorAttr->bCreate == FALSE)
    {
        stGroupAttr.eIntfMode = (MI_VIF_IntfMode_e)stPad0Info.eIntfMode;
        stGroupAttr.eWorkMode = E_MI_VIF_WORK_MODE_1MULTIPLEX;
        stGroupAttr.eHDRType = E_MI_VIF_HDR_TYPE_OFF;
        if(stGroupAttr.eIntfMode == E_MI_VIF_MODE_BT656)
            stGroupAttr.eClkEdge = (MI_VIF_ClkEdge_e)stPad0Info.unIntfAttr.stBt656Attr.eClkEdge;
        else
            stGroupAttr.eClkEdge = E_MI_VIF_CLK_EDGE_DOUBLE;
        ST_DBG("GroupId: %d eIntfMode: %d eWorkMode: %d eHDRType: %d eClkEdge: %d\n", GroupId, stGroupAttr.eIntfMode, stGroupAttr.eWorkMode, stGroupAttr.eHDRType, stGroupAttr.eClkEdge);
        STCHECKRESULT(MI_VIF_CreateDevGroup(GroupId, &stGroupAttr));
        pstSensorAttr->bCreate = TRUE;
    }

    stVifDevAttr.stInputRect.u16X = stSnrPlane0Info.stCapRect.u16X;
    stVifDevAttr.stInputRect.u16Y = stSnrPlane0Info.stCapRect.u16Y;
    stVifDevAttr.stInputRect.u16Width = stSnrPlane0Info.stCapRect.u16Width;
    stVifDevAttr.stInputRect.u16Height = stSnrPlane0Info.stCapRect.u16Height;
    if(stSnrPlane0Info.eBayerId >= E_MI_SYS_PIXEL_BAYERID_MAX)
        stVifDevAttr.eInputPixel = stSnrPlane0Info.ePixel;
    else
        stVifDevAttr.eInputPixel = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);
    ST_DBG("dev=%d eInputPixel=%d \n", vifDev, stVifDevAttr.eInputPixel);
    ST_DBG("stInputRect (%d,%d,%d,%d), H2T1P %d, capsel %d \n", stVifDevAttr.stInputRect.u16X, stVifDevAttr.stInputRect.u16Y, stVifDevAttr.stInputRect.u16Width, stVifDevAttr.stInputRect.u16Height,
        stVifDevAttr.bEnH2T1PMode, stVifDevAttr.eField);
    STCHECKRESULT(MI_VIF_SetDevAttr(vifDev, &stVifDevAttr));
    STCHECKRESULT(MI_VIF_EnableDev(vifDev));

    stVifPortInfo.stCapRect.u16X = stSnrPlane0Info.stCapRect.u16X;
    stVifPortInfo.stCapRect.u16Y = stSnrPlane0Info.stCapRect.u16Y;
    stVifPortInfo.stCapRect.u16Width = stSnrPlane0Info.stCapRect.u16Width;
    stVifPortInfo.stCapRect.u16Height = stSnrPlane0Info.stCapRect.u16Height;
    stVifPortInfo.stDestSize.u16Width = stSnrPlane0Info.stCapRect.u16Width;
    stVifPortInfo.stDestSize.u16Height = stSnrPlane0Info.stCapRect.u16Height;
    if(stSnrPlane0Info.eBayerId >= E_MI_SYS_PIXEL_BAYERID_MAX)
        stVifPortInfo.ePixFormat = stSnrPlane0Info.ePixel;
    else
        stVifPortInfo.ePixFormat = (MI_SYS_PixelFormat_e)RGB_BAYER_PIXEL(stSnrPlane0Info.ePixPrecision, stSnrPlane0Info.eBayerId);
    //stVifPortInfo.u32FrameModeLineCount for lowlantancy mode
    stVifPortInfo.eFrameRate = E_MI_VIF_FRAMERATE_FULL;
    ST_DBG("vifPort: %d sensor bayerid %d, ePixPrecision %d \n", vifPort, stSnrPlane0Info.eBayerId, stSnrPlane0Info.ePixPrecision);
    ST_DBG("ePixFormat: %d\n", stVifPortInfo.ePixFormat);
    ST_DBG("stVifPortInfo.stCapRect: %d %d %d %d\n", stVifPortInfo.stCapRect.u16X, stVifPortInfo.stCapRect.u16Y, stVifPortInfo.stCapRect.u16Width, stVifPortInfo.stCapRect.u16Height);
    ST_DBG("stVifPortInfo.stDestSize: %d %d\n", stVifPortInfo.stDestSize.u16Width, stVifPortInfo.stDestSize.u16Height);
    STCHECKRESULT(MI_VIF_SetOutputPortAttr(vifDev, vifPort, &stVifPortInfo));
    STCHECKRESULT(MI_VIF_EnableOutputPort(vifDev, vifPort));

    return MI_SUCCESS;
}

MI_S32 ST_VifModuleUnInit(ST_Sensor_Attr_t *pstSensorAttr)
{
    MI_VIF_DEV vifDev =0;
    MI_VIF_PORT vifPort = 0;
    MI_VIF_GROUP GroupId;

    if(NULL == pstSensorAttr || 0 == pstSensorAttr->bUsed)
    {
        return -1;
    }

    vifDev = pstSensorAttr->u32BindVifDev;
    GroupId = pstSensorAttr->u32VifGroupID;
    STCHECKRESULT(MI_VIF_DisableOutputPort(vifDev, vifPort));

    STCHECKRESULT(MI_VIF_DisableDev(vifDev));

    STCHECKRESULT(MI_VIF_DestroyDevGroup(GroupId));

    pstSensorAttr->bCreate = FALSE;

    return MI_SUCCESS;
}

static MI_S32 ST_TransMISnrPadToMIIspBindSensorId(MI_SNR_PADID eMiSnrPadId, MI_ISP_BindSnrId_e *peMiIspSnrBindId)
{
    switch(eMiSnrPadId)
    {
        case 0:
            *peMiIspSnrBindId = E_MI_ISP_SENSOR0;
            break;
        case 1:
            *peMiIspSnrBindId = E_MI_ISP_SENSOR1;
            break;
        case 2:
            *peMiIspSnrBindId = E_MI_ISP_SENSOR2;
            break;
        case 3:
            *peMiIspSnrBindId = E_MI_ISP_SENSOR3;
            break;
        default:
            *peMiIspSnrBindId = E_MI_ISP_SENSOR0;
            printf("[%s]%d snrPad%d fail \n", __FUNCTION__, __LINE__, eMiSnrPadId);
            break;
    }

    return MI_SUCCESS;
}

MI_BOOL ST_DoSetIqBin(MI_ISP_DEV IspDev, MI_ISP_CHANNEL Vpechn, char *pConfigPath)
{
    MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t status;
    MI_U8 u8ispreadycnt = 0;
    if (NULL == pConfigPath || strlen(pConfigPath) == 0)
    {
        printf("IQ Bin File path NULL!\n");
        return FALSE;
    }

    do
    {
        if(u8ispreadycnt > 100)
        {
            printf("%s:%d, isp ready time out \n", __FUNCTION__, __LINE__);
            u8ispreadycnt = 0;
            break;
        }

        memset(&status, 0, sizeof(status));
        MI_ISP_IQ_GetParaInitStatus(IspDev, Vpechn, &status);
        if(status.stParaAPI.bFlag != 1)
        {
            usleep(300*1000);
            u8ispreadycnt++;
            continue;
        }

        u8ispreadycnt = 0;

        printf("loading api bin...path:%s\n",pConfigPath);
        STCHECKRESULT(MI_ISP_API_CmdLoadBinFile(IspDev, Vpechn, (char *)pConfigPath, 1234));

        usleep(10*1000);
    }while(!status.stParaAPI.bFlag);

    return 0;
}

MI_S32 ST_IspModuleInit(ST_Sensor_Attr_t *pstSensorAttr)
{
    MI_ISP_DEV ispDevId = 0;
    MI_ISP_CHANNEL ispChnId = 0;
    MI_ISP_PORT ispOutPortId =0;
    MI_ISP_DevAttr_t stCreateDevAttr;
    MI_ISP_ChannelAttr_t stIspChnAttr;
    MI_ISP_ChnParam_t stIspChnParam;
    MI_ISP_OutPortParam_t stIspOutputParam;
    MI_SYS_ChnPort_t stChnPort;
    ST_Sys_BindInfo_T stBindInfo;

    if(NULL == pstSensorAttr || 0 == pstSensorAttr->bUsed)
    {
        return -1;
    }

    if(0 == u32IspRef)
    {
        memset(&stCreateDevAttr, 0x0, sizeof(MI_ISP_DevAttr_t));
        stCreateDevAttr.u32DevStitchMask = E_MI_ISP_DEVICEMASK_ID0;
        STCHECKRESULT(MI_ISP_CreateDevice(ispDevId, &stCreateDevAttr));
    }
    u32IspRef++;

    ispChnId = pstSensorAttr->eSensorPadID;
    memset(&stIspChnAttr, 0x0, sizeof(MI_ISP_ChannelAttr_t));
    ST_TransMISnrPadToMIIspBindSensorId(pstSensorAttr->eSensorPadID, (MI_ISP_BindSnrId_e *)&stIspChnAttr.u32SensorBindId);
    STCHECKRESULT(MI_ISP_CreateChannel(ispDevId, ispChnId, &stIspChnAttr));
    //STCHECKRESULT(MI_ISP_SetInputPortCrop(IspDevId, IspChnId, &pstIspChnAttr->stIspInPortAttr[0].stInputCropWin));

    memset(&stIspChnParam, 0x0, sizeof(MI_ISP_ChnParam_t));
    stIspChnParam.eHDRType = E_MI_ISP_HDR_TYPE_OFF;
    stIspChnParam.e3DNRLevel = E_MI_ISP_3DNR_LEVEL2;
    stIspChnParam.eRot = E_MI_SYS_ROTATE_NONE;
    stIspChnParam.bMirror = 0;
    stIspChnParam.bFlip = 0;
    STCHECKRESULT(MI_ISP_SetChnParam(ispDevId, ispChnId, &stIspChnParam));
    STCHECKRESULT(MI_ISP_StartChannel(ispDevId, ispChnId));

    memset(&stIspOutputParam, 0x0, sizeof(MI_ISP_OutPortParam_t));
    stIspOutputParam.stCropRect.u16X = 0;
    stIspOutputParam.stCropRect.u16Y = 0;
    stIspOutputParam.stCropRect.u16Width = pstSensorAttr->u16Width;
    stIspOutputParam.stCropRect.u16Height = pstSensorAttr->u16Height;
    stIspOutputParam.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    STCHECKRESULT(MI_ISP_SetOutputPortParam(ispDevId, ispChnId, ispOutPortId, &stIspOutputParam));
    STCHECKRESULT(MI_ISP_EnableOutputPort(ispDevId, ispChnId, ispOutPortId));
    stChnPort.eModId = E_MI_MODULE_ID_ISP;
    stChnPort.u32DevId = ispDevId;
    stChnPort.u32ChnId = ispChnId;
    stChnPort.u32PortId = ispOutPortId;
    STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(0, &stChnPort , 0, 4));
    printf("isp module Dev%d, chn%d, port%d\n", stChnPort.u32DevId, stChnPort.u32ChnId, stChnPort.u32PortId);

    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = pstSensorAttr->u32BindVifDev;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_ISP;
    stBindInfo.stDstChnPort.u32DevId = ispDevId;
    stBindInfo.stDstChnPort.u32ChnId = ispChnId;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    if(NULL != pstSensorAttr->s8IqPath && \
        0 == access(pstSensorAttr->s8IqPath, F_OK))
    {
        ST_DoSetIqBin(ispDevId, ispChnId, pstSensorAttr->s8IqPath);
    }

    return MI_SUCCESS;
}

MI_S32 ST_IspModuleUnInit(ST_Sensor_Attr_t *pstSensorAttr)
{
    MI_ISP_DEV ispDevId = 0;
    MI_ISP_CHANNEL ispChnId = 0;
    MI_ISP_PORT ispOutPortId =0;
    ST_Sys_BindInfo_T stBindInfo;

    if(NULL == pstSensorAttr || 0 == pstSensorAttr->bUsed)
    {
        return -1;
    }

    ispChnId = pstSensorAttr->eSensorPadID;
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_VIF;
    stBindInfo.stSrcChnPort.u32DevId = pstSensorAttr->u32BindVifDev;
    stBindInfo.stSrcChnPort.u32ChnId = 0;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_ISP;
    stBindInfo.stDstChnPort.u32DevId = ispDevId;
    stBindInfo.stDstChnPort.u32ChnId = ispChnId;
    stBindInfo.stDstChnPort.u32PortId = ispOutPortId;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    STCHECKRESULT(MI_ISP_DisableOutputPort(ispDevId, ispChnId, ispOutPortId));

    STCHECKRESULT(MI_ISP_StopChannel(ispDevId, ispChnId));

    STCHECKRESULT(MI_ISP_DestroyChannel(ispDevId, ispChnId));

    u32IspRef--;
    if(0 == u32IspRef)
    {
        STCHECKRESULT(MI_ISP_DestoryDevice(ispDevId));
    }

    return MI_SUCCESS;
}

MI_S32 ST_SclModuleInit(ST_Sensor_Attr_t *pstSensorAttr)
{
    MI_SCL_DEV SclDevId = 0;
    MI_SCL_CHANNEL SclChnId = 0;
    MI_SCL_PORT SclOutPortId = 0;
    MI_SCL_DevAttr_t stCreateDevAttr;
    MI_SCL_ChannelAttr_t stSclChnAttr;
    MI_SCL_ChnParam_t stSclChnParam;
    MI_SCL_OutPortParam_t stSclOutputParam;
    MI_SYS_ChnPort_t stChnPort;
    ST_Sys_BindInfo_T stBindInfo;

    if(NULL == pstSensorAttr || 0 == pstSensorAttr->bUsed)
    {
        return -1;
    }

    if(0 == u32SclRef)
    {
        memset(&stCreateDevAttr, 0x0, sizeof(MI_SCL_DevAttr_t));
        stCreateDevAttr.u32NeedUseHWOutPortMask = E_MI_SCL_HWSCL0 | E_MI_SCL_HWSCL1 | E_MI_SCL_HWSCL2 | E_MI_SCL_HWSCL3 | E_MI_SCL_HWSCL4;
        STCHECKRESULT(MI_SCL_CreateDevice(SclDevId, &stCreateDevAttr));
        /* Create SCL 3 for Stretch or Rotate */
        memset(&stCreateDevAttr, 0x0, sizeof(MI_SCL_DevAttr_t));
        stCreateDevAttr.u32NeedUseHWOutPortMask = E_MI_SCL_HWSCL5;
        STCHECKRESULT(MI_SCL_CreateDevice(3, &stCreateDevAttr));
    }
    u32SclRef++;

    SclChnId = pstSensorAttr->eSensorPadID;
    memset(&stSclChnAttr, 0x0, sizeof(MI_SCL_ChannelAttr_t));
    STCHECKRESULT(MI_SCL_CreateChannel(SclDevId, SclChnId, &stSclChnAttr));
#if 0
    if(pstSclChnAttr->stSclInPortAttr[0].stInputCropWin.u16Width !=0
        && pstSclChnAttr->stSclInPortAttr[0].stInputCropWin.u16Height !=0)
    {
        MI_SCL_SetInputPortCrop((MI_SCL_DEV)SclDevId, SclChnId, &pstSclChnAttr->stSclInPortAttr[0].stInputCropWin);
    }
#endif
    memset(&stSclChnParam, 0x0, sizeof(MI_SCL_ChnParam_t));
    //stSclChnParam.eRot = pstSclChnAttr->eRotate;  //useless for SCL 0~4
    STCHECKRESULT(MI_SCL_SetChnParam(SclDevId, SclChnId, &stSclChnParam));
    STCHECKRESULT(MI_SCL_StartChannel(SclDevId, SclChnId));
    for(SclOutPortId = 0; SclOutPortId < gstDisplayOpt.u32DispPort; SclOutPortId++)
    {
        memset(&stSclOutputParam, 0x0, sizeof(MI_SCL_OutPortParam_t));
        /* Crop parameter */
        if(0/* SCL port 0 for panel display */ == SclOutPortId && gstDisplayOpt.bUsedPanel)
        {
            ST_GetCropParam(pstSensorAttr->u16Width, pstSensorAttr->u16Height, \
                            gstDisplayOpt.u32PanelW, gstDisplayOpt.u32PanelH, \
                            &stSclOutputParam.stSCLOutCropRect);
            /* Scaling parameter */
            stSclOutputParam.stSCLOutputSize.u16Width = gstDisplayOpt.u32PanelW;
            stSclOutputParam.stSCLOutputSize.u16Height = gstDisplayOpt.u32PanelH;
        }
        else
        {
            ST_GetCropParam(pstSensorAttr->u16Width, pstSensorAttr->u16Height, \
                            gstDisplayOpt.u32HdmiW, gstDisplayOpt.u32HdmiH, \
                            &stSclOutputParam.stSCLOutCropRect);
            /* Scaling parameter */
            stSclOutputParam.stSCLOutputSize.u16Width = gstDisplayOpt.u32HdmiW;
            stSclOutputParam.stSCLOutputSize.u16Height = gstDisplayOpt.u32HdmiH;
        }
        stSclOutputParam.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
        stSclOutputParam.bMirror = 0;
        stSclOutputParam.bFlip = 0;
        STCHECKRESULT(MI_SCL_SetOutputPortParam(SclDevId, SclChnId, SclOutPortId, &stSclOutputParam));
        STCHECKRESULT(MI_SCL_EnableOutputPort(SclDevId, SclChnId, SclOutPortId));
        stChnPort.eModId = E_MI_MODULE_ID_SCL;
        stChnPort.u32DevId = SclDevId;
        stChnPort.u32ChnId = SclChnId;
        stChnPort.u32PortId = SclOutPortId;
        STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(0, &stChnPort , 2, 4));
    }
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_ISP;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = pstSensorAttr->eSensorPadID;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_SCL;
    stBindInfo.stDstChnPort.u32DevId = SclDevId;
    stBindInfo.stDstChnPort.u32ChnId = SclChnId;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_REALTIME;
    STCHECKRESULT(ST_Sys_Bind(&stBindInfo));

    return MI_SUCCESS;
}

MI_S32 ST_SclModuleUnInit(ST_Sensor_Attr_t *pstSensorAttr)
{
    MI_SCL_DEV SclDevId = 0;
    MI_SCL_CHANNEL SclChnId = 0;
    MI_SCL_PORT SclOutPortId =0;
    ST_Sys_BindInfo_T stBindInfo;

    if(NULL == pstSensorAttr || 0 == pstSensorAttr->bUsed)
    {
        return -1;
    }

    SclChnId = pstSensorAttr->eSensorPadID;
    memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
    stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_ISP;
    stBindInfo.stSrcChnPort.u32DevId = 0;
    stBindInfo.stSrcChnPort.u32ChnId = pstSensorAttr->eSensorPadID;
    stBindInfo.stSrcChnPort.u32PortId = 0;
    stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_SCL;
    stBindInfo.stDstChnPort.u32DevId = SclDevId;
    stBindInfo.stDstChnPort.u32ChnId = SclChnId;
    stBindInfo.stDstChnPort.u32PortId = 0;
    stBindInfo.u32SrcFrmrate = 30;
    stBindInfo.u32DstFrmrate = 30;
    STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

    for(SclOutPortId = 0; SclOutPortId < gstDisplayOpt.u32DispPort; SclOutPortId++)
    {
        STCHECKRESULT(MI_SCL_DisableOutputPort(SclDevId, SclChnId, SclOutPortId));
    }

    STCHECKRESULT(MI_SCL_StopChannel(SclDevId, SclChnId));

    STCHECKRESULT(MI_SCL_DestroyChannel(SclDevId, SclChnId));

    u32SclRef--;
    if(0 == u32SclRef)
    {
        STCHECKRESULT(MI_SCL_DestroyDevice(SclDevId));
        /* SCL 3 for Stretch or Rotate */
        STCHECKRESULT(MI_SCL_DestroyDevice(3));
    }

    return MI_SUCCESS;
}

/* Init disp and do bind */
MI_S32 ST_DisplayFlowInit(ST_Display_Opt_t *pstDisplayOpt, int inputCnt)
{
    MI_U8 u8DispInport;
    MI_DISP_LAYER s32DispLayer;
    MI_U32 u32DispWidth, u32DispHeight;
    MI_DISP_InputPortAttr_t stInputPortAttr;
    MI_DISP_VidWinRect_t stWinRect;
    ST_Sys_BindInfo_T stBindInfo;

    MI_SCL_DEV SclDevId = 0;
    MI_SCL_CHANNEL SclChnId = 0;
    MI_SCL_PORT SclOutPortId =0;
    MI_SCL_ChannelAttr_t stSclChnAttr;
    MI_SCL_ChnParam_t stSclChnParam;
    MI_SCL_OutPortParam_t stSclOutputParam;
    MI_SYS_ChnPort_t stChnPort;

    if(NULL == pstDisplayOpt || inputCnt <= 0)
    {
        return -1;
    }

    /* Init Panel */
    if(pstDisplayOpt->bUsedPanel)
    {
        SclDevId = 0;
        SclChnId = gstSensorAttr[g_s32SensorIdxForPanel].eSensorPadID;
        SclOutPortId =0;
        s32DispLayer = pstDisplayOpt->s32PanelLayer;
        u8DispInport = pstDisplayOpt->u8PanelInputPort;
        u32DispWidth = pstDisplayOpt->u32PanelW;
        u32DispHeight = pstDisplayOpt->u32PanelH;
        if(E_MI_SYS_ROTATE_90 == pstDisplayOpt->ePanelRot ||\
            E_MI_SYS_ROTATE_270 == pstDisplayOpt->ePanelRot)
        {
            SclDevId = 3;    /* SCL device 3(Create in func ST_SclModuleInit) for rotate */
            // Swap W H
            u32DispWidth = pstDisplayOpt->u32PanelH;
            u32DispHeight = pstDisplayOpt->u32PanelW;
            memset(&stSclChnAttr, 0x0, sizeof(MI_SCL_ChannelAttr_t));
            STCHECKRESULT(MI_SCL_CreateChannel(SclDevId, SclChnId, &stSclChnAttr));
        #if 0
            if(pstSclChnAttr->stSclInPortAttr[0].stInputCropWin.u16Width !=0
                && pstSclChnAttr->stSclInPortAttr[0].stInputCropWin.u16Height !=0)
            {
                MI_SCL_SetInputPortCrop((MI_SCL_DEV)SclDevId, SclChnId, &pstSclChnAttr->stSclInPortAttr[0].stInputCropWin);
            }
        #endif
            memset(&stSclChnParam, 0x0, sizeof(MI_SCL_ChnParam_t));
            stSclChnParam.eRot = pstDisplayOpt->ePanelRot;
            STCHECKRESULT(MI_SCL_SetChnParam(SclDevId, SclChnId, &stSclChnParam));
            STCHECKRESULT(MI_SCL_StartChannel(SclDevId, SclChnId));

            memset(&stSclOutputParam, 0x0, sizeof(MI_SCL_OutPortParam_t));
            /* Crop parameter */
            /* Scaling parameter */
            stSclOutputParam.stSCLOutputSize.u16Width = u32DispWidth;
            stSclOutputParam.stSCLOutputSize.u16Height = u32DispHeight;
            stSclOutputParam.ePixelFormat = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
            stSclOutputParam.bMirror = 0;
            stSclOutputParam.bFlip = 0;
            STCHECKRESULT(MI_SCL_SetOutputPortParam(SclDevId, SclChnId, SclOutPortId, &stSclOutputParam));
            STCHECKRESULT(MI_SCL_EnableOutputPort(SclDevId, SclChnId, SclOutPortId));
            stChnPort.eModId = E_MI_MODULE_ID_SCL;
            stChnPort.u32DevId = SclDevId;
            stChnPort.u32ChnId = SclChnId;
            stChnPort.u32PortId = SclOutPortId;
            STCHECKRESULT(MI_SYS_SetChnOutputPortDepth(0, &stChnPort , 2, 4));

            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stBindInfo.stSrcChnPort.u32DevId = 0;
            stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
            stBindInfo.stSrcChnPort.u32PortId = 0;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_SCL;
            stBindInfo.stDstChnPort.u32DevId = SclDevId;
            stBindInfo.stDstChnPort.u32ChnId = SclChnId;
            stBindInfo.stDstChnPort.u32PortId = SclOutPortId;
            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = 30;
            stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
        }
        /* set inputport */
        memset(&stWinRect, 0x0, sizeof(MI_DISP_VidWinRect_t));
        memset(&stInputPortAttr, 0x0, sizeof(MI_DISP_InputPortAttr_t));
        stInputPortAttr.stDispWin.u16X = 0;
        stInputPortAttr.stDispWin.u16Y = 0;
        stInputPortAttr.stDispWin.u16Width = u32DispWidth;
        stInputPortAttr.stDispWin.u16Height = u32DispHeight;
        stInputPortAttr.u16SrcWidth = u32DispWidth;
        stInputPortAttr.u16SrcHeight = u32DispHeight;
        stWinRect.u16Width = u32DispWidth;
        stWinRect.u16Height = u32DispHeight;
        STCHECKRESULT(MI_DISP_SetInputPortAttr(s32DispLayer, u8DispInport, &stInputPortAttr));
        STCHECKRESULT(MI_DISP_EnableInputPort(s32DispLayer, u8DispInport));
        STCHECKRESULT(MI_DISP_SetInputPortSyncMode(s32DispLayer, u8DispInport, E_MI_DISP_SYNC_MODE_FREE_RUN));
        //if(1 == inputCnt)   // Only one sensor, bind directly
        {
            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stBindInfo.stSrcChnPort.u32DevId = SclDevId;
            stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
            stBindInfo.stSrcChnPort.u32PortId = SclOutPortId;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
            stBindInfo.stDstChnPort.u32DevId = pstDisplayOpt->s32PanelDev;
            stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(pstDisplayOpt->s32PanelLayer, u8DispInport);
            stBindInfo.stDstChnPort.u32PortId = 0;
            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = 30;
            stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
        }
    }

    /* Init HDMI */
    if(pstDisplayOpt->bUsedHdmi)
    {
        s32DispLayer = pstDisplayOpt->s32HdmiLayer;
        /* set inputport */
        if(1 == inputCnt)
        {
            u8DispInport = 0;
            SclChnId = gstSensorAttr[0].eSensorPadID;
            u32DispWidth = pstDisplayOpt->u32HdmiW;
            u32DispHeight = pstDisplayOpt->u32HdmiH;
            memset(&stWinRect, 0x0, sizeof(MI_DISP_VidWinRect_t));
            memset(&stInputPortAttr, 0x0, sizeof(MI_DISP_InputPortAttr_t));
            stInputPortAttr.stDispWin.u16X = 0;
            stInputPortAttr.stDispWin.u16Y = 0;
            stInputPortAttr.stDispWin.u16Width = u32DispWidth;
            stInputPortAttr.stDispWin.u16Height = u32DispHeight;
            stInputPortAttr.u16SrcWidth = u32DispWidth;
            stInputPortAttr.u16SrcHeight = u32DispHeight;
            stWinRect.u16Width = u32DispWidth;
            stWinRect.u16Height = u32DispHeight;
            STCHECKRESULT(MI_DISP_SetInputPortAttr(s32DispLayer, u8DispInport, &stInputPortAttr));
            STCHECKRESULT(MI_DISP_EnableInputPort(s32DispLayer, u8DispInport));
            STCHECKRESULT(MI_DISP_SetInputPortSyncMode(s32DispLayer, u8DispInport, E_MI_DISP_SYNC_MODE_FREE_RUN));
            // Only one sensor, bind directly
            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stBindInfo.stSrcChnPort.u32DevId = 0;
            stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
            stBindInfo.stSrcChnPort.u32PortId = 1;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
            stBindInfo.stDstChnPort.u32DevId = pstDisplayOpt->s32HdmiDev;
            stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(pstDisplayOpt->s32HdmiLayer, u8DispInport);
            stBindInfo.stDstChnPort.u32PortId = 0;
            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = 30;
            stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
        }
        else
        {
        #if USE_HDMI_4_SCREEN
            pstDisplayOpt->u8SensorCnt = inputCnt;
            u32DispWidth = pstDisplayOpt->u32HdmiW / 2;
            u32DispHeight = pstDisplayOpt->u32HdmiH / 2;
            SclDevId = 0;
            SclOutPortId = 1;
            for(u8DispInport = 0; u8DispInport < inputCnt; u8DispInport++)
            {
                SclChnId = gstSensorAttr[u8DispInport].eSensorPadID;
                memset(&stSclOutputParam, 0x0, sizeof(MI_SCL_OutPortParam_t));
                STCHECKRESULT(MI_SCL_GetOutputPortParam(SclDevId, SclChnId, SclOutPortId, &stSclOutputParam));
                STCHECKRESULT(MI_SCL_DisableOutputPort(SclDevId, SclChnId, SclOutPortId));
                /* Scaling parameter */
                stSclOutputParam.stSCLOutputSize.u16Width = u32DispWidth;
                stSclOutputParam.stSCLOutputSize.u16Height = u32DispHeight;
                STCHECKRESULT(MI_SCL_SetOutputPortParam(SclDevId, SclChnId, SclOutPortId, &stSclOutputParam));
                STCHECKRESULT(MI_SCL_EnableOutputPort(SclDevId, SclChnId, SclOutPortId));
                memset(&stWinRect, 0x0, sizeof(MI_DISP_VidWinRect_t));
                memset(&stInputPortAttr, 0x0, sizeof(MI_DISP_InputPortAttr_t));
                stInputPortAttr.stDispWin.u16X = (u8DispInport % 2) * u32DispWidth;
                stInputPortAttr.stDispWin.u16Y = (u8DispInport / 2) * u32DispHeight;
                stInputPortAttr.stDispWin.u16Width = u32DispWidth;
                stInputPortAttr.stDispWin.u16Height = u32DispHeight;
                stInputPortAttr.u16SrcWidth = u32DispWidth;
                stInputPortAttr.u16SrcHeight = u32DispHeight;
                stWinRect.u16Width = u32DispWidth;
                stWinRect.u16Height = u32DispHeight;
                STCHECKRESULT(MI_DISP_SetInputPortAttr(s32DispLayer, u8DispInport, &stInputPortAttr));
                STCHECKRESULT(MI_DISP_EnableInputPort(s32DispLayer, u8DispInport));
                STCHECKRESULT(MI_DISP_SetInputPortSyncMode(s32DispLayer, u8DispInport, E_MI_DISP_SYNC_MODE_FREE_RUN));

                memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
                stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
                stBindInfo.stSrcChnPort.u32DevId = SclDevId;
                stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
                stBindInfo.stSrcChnPort.u32PortId = SclOutPortId;
                stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
                stBindInfo.stDstChnPort.u32DevId = pstDisplayOpt->s32HdmiDev;
                stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(pstDisplayOpt->s32HdmiLayer, u8DispInport);
                stBindInfo.stDstChnPort.u32PortId = 0;
                stBindInfo.u32SrcFrmrate = 30;
                stBindInfo.u32DstFrmrate = 30;
                stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
                STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
            }
        #else
            pstDisplayOpt->u8SensorCnt = inputCnt;
            u32DispWidth = pstDisplayOpt->u32HdmiW;
            u32DispHeight = pstDisplayOpt->u32HdmiH;
            SclDevId = 0;
            SclOutPortId = 1;
            u8DispInport = 0;
            //for(u8DispInport = 0; u8DispInport < inputCnt; u8DispInport++)
            {
                SclChnId = gstSensorAttr[g_s32SensorIdxForHdmi].eSensorPadID;
                memset(&stWinRect, 0x0, sizeof(MI_DISP_VidWinRect_t));
                memset(&stInputPortAttr, 0x0, sizeof(MI_DISP_InputPortAttr_t));
                stInputPortAttr.stDispWin.u16X = 0;
                stInputPortAttr.stDispWin.u16Y = 0;
                stInputPortAttr.stDispWin.u16Width = u32DispWidth;
                stInputPortAttr.stDispWin.u16Height = u32DispHeight;
                stInputPortAttr.u16SrcWidth = u32DispWidth;
                stInputPortAttr.u16SrcHeight = u32DispHeight;
                stWinRect.u16Width = u32DispWidth;
                stWinRect.u16Height = u32DispHeight;
                STCHECKRESULT(MI_DISP_SetInputPortAttr(s32DispLayer, u8DispInport, &stInputPortAttr));
                STCHECKRESULT(MI_DISP_EnableInputPort(s32DispLayer, u8DispInport));
                STCHECKRESULT(MI_DISP_SetInputPortSyncMode(s32DispLayer, u8DispInport, E_MI_DISP_SYNC_MODE_FREE_RUN));

                memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
                stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
                stBindInfo.stSrcChnPort.u32DevId = SclDevId;
                stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
                stBindInfo.stSrcChnPort.u32PortId = SclOutPortId;
                stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
                stBindInfo.stDstChnPort.u32DevId = pstDisplayOpt->s32HdmiDev;
                stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(pstDisplayOpt->s32HdmiLayer, u8DispInport);
                stBindInfo.stDstChnPort.u32PortId = 0;
                stBindInfo.u32SrcFrmrate = 30;
                stBindInfo.u32DstFrmrate = 30;
                stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
                STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
            }
        #endif
        }
    }

    return MI_SUCCESS;
}

MI_S32 ST_DisplayFlowUnInit(ST_Display_Opt_t *pstDisplayOpt)
{
    int inputCnt;
    MI_U8 u8DispInport;
    MI_DISP_LAYER s32DispLayer;
    MI_SCL_DEV SclDevId = 0;
    MI_SCL_CHANNEL SclChnId = 0;
    MI_SCL_PORT SclOutPortId = 0;
    ST_Sys_BindInfo_T stBindInfo;

    if(NULL == pstDisplayOpt || pstDisplayOpt->u8SensorCnt <= 0)
    {
        return -1;
    }

    inputCnt = pstDisplayOpt->u8SensorCnt;

    /* Deinit Panel */
    if(pstDisplayOpt->bUsedPanel)
    {
        SclDevId = 0;
        SclChnId = gstSensorAttr[g_s32SensorIdxForPanel].eSensorPadID;
        SclOutPortId =0;
        s32DispLayer = pstDisplayOpt->s32PanelLayer;
        u8DispInport = pstDisplayOpt->u8PanelInputPort;
        if(E_MI_SYS_ROTATE_90 == pstDisplayOpt->ePanelRot ||\
            E_MI_SYS_ROTATE_270 == pstDisplayOpt->ePanelRot)
        {
            SclDevId = 3;    /* SCL device 3(scl5) for rotate */
        }
        //if(1 == inputCnt)   // Only one sensor, bind directly
        {
            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stBindInfo.stSrcChnPort.u32DevId = SclDevId;
            stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
            stBindInfo.stSrcChnPort.u32PortId = SclOutPortId;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
            stBindInfo.stDstChnPort.u32DevId = pstDisplayOpt->s32PanelDev;
            stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(pstDisplayOpt->s32PanelLayer, u8DispInport);
            stBindInfo.stDstChnPort.u32PortId = 0;
            STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));
        }
        STCHECKRESULT(MI_DISP_DisableInputPort(s32DispLayer, u8DispInport));
        if(E_MI_SYS_ROTATE_90 == pstDisplayOpt->ePanelRot ||\
            E_MI_SYS_ROTATE_270 == pstDisplayOpt->ePanelRot)
        {
            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stBindInfo.stSrcChnPort.u32DevId = 0;
            stBindInfo.stSrcChnPort.u32ChnId = gstSensorAttr[g_s32SensorIdxForPanel].eSensorPadID;
            stBindInfo.stSrcChnPort.u32PortId = 0;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_SCL;
            stBindInfo.stDstChnPort.u32DevId = SclDevId;
            stBindInfo.stDstChnPort.u32ChnId = SclChnId;
            stBindInfo.stDstChnPort.u32PortId = SclOutPortId;
            STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

            STCHECKRESULT(MI_SCL_DisableOutputPort(SclDevId, SclChnId, SclOutPortId));

            STCHECKRESULT(MI_SCL_StopChannel(SclDevId, SclChnId));

            STCHECKRESULT(MI_SCL_DestroyChannel(SclDevId, SclChnId));
        }
    }

    /* Deinit HDMI */
    if(pstDisplayOpt->bUsedHdmi)
    {
        s32DispLayer = pstDisplayOpt->s32HdmiLayer;
        /* set inputport */
        if(1 == inputCnt)
        {
            u8DispInport = 0;
            SclChnId = gstSensorAttr[0].eSensorPadID;
            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stBindInfo.stSrcChnPort.u32DevId = 0;
            stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
            stBindInfo.stSrcChnPort.u32PortId = 1;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
            stBindInfo.stDstChnPort.u32DevId = pstDisplayOpt->s32HdmiDev;
            stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(pstDisplayOpt->s32HdmiLayer, u8DispInport);
            stBindInfo.stDstChnPort.u32PortId = 0;
            STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

            STCHECKRESULT(MI_DISP_DisableInputPort(s32DispLayer, u8DispInport));
        }
        else
        {
        #if USE_HDMI_4_SCREEN
            SclDevId = 0;
            SclOutPortId = 1;
            for(u8DispInport = 0; u8DispInport < inputCnt; u8DispInport++)
            {
                SclChnId = gstSensorAttr[u8DispInport].eSensorPadID;
                memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
                stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
                stBindInfo.stSrcChnPort.u32DevId = SclDevId;
                stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
                stBindInfo.stSrcChnPort.u32PortId = SclOutPortId;
                stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
                stBindInfo.stDstChnPort.u32DevId = pstDisplayOpt->s32HdmiDev;
                stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(pstDisplayOpt->s32HdmiLayer, u8DispInport);
                stBindInfo.stDstChnPort.u32PortId = 0;
                STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

                STCHECKRESULT(MI_DISP_DisableInputPort(s32DispLayer, u8DispInport));
            }
        #else
            SclDevId = 0;
            SclOutPortId = 1;
            u8DispInport = 0;
            //for(u8DispInport = 0; u8DispInport < inputCnt; u8DispInport++)
            {
                SclChnId = gstSensorAttr[g_s32SensorIdxForHdmi].eSensorPadID;
                memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
                stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
                stBindInfo.stSrcChnPort.u32DevId = SclDevId;
                stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
                stBindInfo.stSrcChnPort.u32PortId = SclOutPortId;
                stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
                stBindInfo.stDstChnPort.u32DevId = pstDisplayOpt->s32HdmiDev;
                stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(pstDisplayOpt->s32HdmiLayer, u8DispInport);
                stBindInfo.stDstChnPort.u32PortId = 0;
                STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

                STCHECKRESULT(MI_DISP_DisableInputPort(s32DispLayer, u8DispInport));
            }
        #endif
        }
    }

    return MI_SUCCESS;
}

MI_S32 ST_ResetHdmiDispFlow(ST_DispoutTiming_e eStHdmiDispTiming)
{
    int inputCnt;
    MI_U8 u8DispInport;
    MI_DISP_LAYER s32DispLayer;
    MI_U32 u32DispWidth, u32DispHeight;
    MI_SCL_DEV SclDevId = 0;        //For HDMI
    MI_SCL_CHANNEL SclChnId = 0;
    MI_SCL_PORT SclOutPortId = 1;    // For HDMI
    MI_U32 u32LayerW = 0;
    MI_U32 u32LayerH = 0;
    MI_SCL_OutPortParam_t stSclOutputParam;
    MI_DISP_InputPortAttr_t stInputPortAttr;
    MI_DISP_OutputTiming_e u32DispTiming;
    MI_DISP_VidWinRect_t stWinRect;
    MI_HDMI_TimingType_e u32HdmiTiming;
    ST_Sys_BindInfo_T stBindInfo;

    inputCnt = gstDisplayOpt.u8SensorCnt;

    ST_GetTimingInfo(eStHdmiDispTiming, &u32HdmiTiming, &u32DispTiming, &u32LayerW, &u32LayerH);
    ST_DBG("eStDispTiming: %d\n", eStHdmiDispTiming);
    ST_DBG("u32HdmiTiming: %d\n", u32HdmiTiming);
    ST_DBG("u32DispTiming: %d\n", u32DispTiming);
    ST_DBG("u32LayerW: %d u32LayerH: %d\n", u32LayerW, u32LayerH);
    gstDisplayOpt.u32HdmiW = u32LayerW;
    gstDisplayOpt.u32HdmiH = u32LayerH;
    //SclChnId = pstSensorAttr->eSensorPadID;

    s32DispLayer = gstDisplayOpt.s32HdmiLayer;
    /* set inputport */
    if(1 == inputCnt)
    {
        u8DispInport = 0;
        SclChnId = gstSensorAttr[0].eSensorPadID;
        u32DispWidth = gstDisplayOpt.u32HdmiW;
        u32DispHeight = gstDisplayOpt.u32HdmiH;
        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
        stBindInfo.stSrcChnPort.u32DevId = 0;
        stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
        stBindInfo.stSrcChnPort.u32PortId = 1;
        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
        stBindInfo.stDstChnPort.u32DevId = gstDisplayOpt.s32HdmiDev;
        stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(gstDisplayOpt.s32HdmiLayer, u8DispInport);
        stBindInfo.stDstChnPort.u32PortId = 0;
        STCHECKRESULT(ST_Sys_UnBind(&stBindInfo));

        /* Config SCL */
        memset(&stSclOutputParam, 0x0, sizeof(MI_SCL_OutPortParam_t));
        STCHECKRESULT(MI_SCL_GetOutputPortParam(SclDevId, SclChnId, SclOutPortId, &stSclOutputParam));
        STCHECKRESULT(MI_SCL_DisableOutputPort(SclDevId, SclChnId, SclOutPortId));
        ST_GetCropParam(gstSensorAttr[0].u16Width, gstSensorAttr[0].u16Height, \
                        gstDisplayOpt.u32HdmiW, gstDisplayOpt.u32HdmiH, \
                        &stSclOutputParam.stSCLOutCropRect);
        /* Scaling parameter */
        stSclOutputParam.stSCLOutputSize.u16Width = gstDisplayOpt.u32HdmiW;
        stSclOutputParam.stSCLOutputSize.u16Height = gstDisplayOpt.u32HdmiH;
        STCHECKRESULT(MI_SCL_SetOutputPortParam(SclDevId, SclChnId, SclOutPortId, &stSclOutputParam));
        STCHECKRESULT(MI_SCL_EnableOutputPort(SclDevId, SclChnId, SclOutPortId));

        /* Config disp */
        memset(&stInputPortAttr, 0x0, sizeof(MI_DISP_InputPortAttr_t));
        STCHECKRESULT(MI_DISP_GetInputPortAttr(s32DispLayer, u8DispInport, &stInputPortAttr));
        STCHECKRESULT(MI_DISP_DisableInputPort(s32DispLayer, u8DispInport));
        stInputPortAttr.stDispWin.u16Width = u32DispWidth;
        stInputPortAttr.stDispWin.u16Height = u32DispHeight;
        stInputPortAttr.u16SrcWidth = u32DispWidth;
        stInputPortAttr.u16SrcHeight = u32DispHeight;
        STCHECKRESULT(MI_DISP_SetInputPortAttr(s32DispLayer, u8DispInport, &stInputPortAttr));
        STCHECKRESULT(MI_DISP_EnableInputPort(s32DispLayer, u8DispInport));
        STCHECKRESULT(MI_DISP_SetInputPortSyncMode(s32DispLayer, u8DispInport, E_MI_DISP_SYNC_MODE_FREE_RUN));
        // Only one sensor, bind directly
        memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
        stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
        stBindInfo.stSrcChnPort.u32DevId = 0;
        stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
        stBindInfo.stSrcChnPort.u32PortId = 1;
        stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
        stBindInfo.stDstChnPort.u32DevId = gstDisplayOpt.s32HdmiDev;
        stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(gstDisplayOpt.s32HdmiLayer, u8DispInport);
        stBindInfo.stDstChnPort.u32PortId = 0;
        stBindInfo.u32SrcFrmrate = 30;
        stBindInfo.u32DstFrmrate = 30;
        stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
        STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
    }
    else
    {
#if USE_HDMI_4_SCREEN
        u32DispWidth = gstDisplayOpt.u32HdmiW / 2;
        u32DispHeight = gstDisplayOpt.u32HdmiH / 2;
        SclDevId = 0;
        SclOutPortId = 1;
        for(u8DispInport = 0; u8DispInport < inputCnt; u8DispInport++)
        {
            SclChnId = gstSensorAttr[u8DispInport].eSensorPadID;
            memset(&stSclOutputParam, 0x0, sizeof(MI_SCL_OutPortParam_t));
            STCHECKRESULT(MI_SCL_GetOutputPortParam(SclDevId, SclChnId, SclOutPortId, &stSclOutputParam));
            STCHECKRESULT(MI_SCL_DisableOutputPort(SclDevId, SclChnId, SclOutPortId));
            ST_GetCropParam(gstSensorAttr[u8DispInport].u16Width, gstSensorAttr[u8DispInport].u16Height, \
                            gstDisplayOpt.u32HdmiW, gstDisplayOpt.u32HdmiH, \
                            &stSclOutputParam.stSCLOutCropRect);
            stSclOutputParam.stSCLOutputSize.u16Width = u32DispWidth;
            stSclOutputParam.stSCLOutputSize.u16Height = u32DispHeight;
            STCHECKRESULT(MI_SCL_SetOutputPortParam(SclDevId, SclChnId, SclOutPortId, &stSclOutputParam));
            STCHECKRESULT(MI_SCL_EnableOutputPort(SclDevId, SclChnId, SclOutPortId));
            memset(&stWinRect, 0x0, sizeof(MI_DISP_VidWinRect_t));
            memset(&stInputPortAttr, 0x0, sizeof(MI_DISP_InputPortAttr_t));
            stInputPortAttr.stDispWin.u16X = (u8DispInport % 2) * u32DispWidth;
            stInputPortAttr.stDispWin.u16Y = (u8DispInport / 2) * u32DispHeight;
            stInputPortAttr.stDispWin.u16Width = u32DispWidth;
            stInputPortAttr.stDispWin.u16Height = u32DispHeight;
            stInputPortAttr.u16SrcWidth = u32DispWidth;
            stInputPortAttr.u16SrcHeight = u32DispHeight;
            stWinRect.u16Width = u32DispWidth;
            stWinRect.u16Height = u32DispHeight;
            STCHECKRESULT(MI_DISP_SetInputPortAttr(s32DispLayer, u8DispInport, &stInputPortAttr));
            STCHECKRESULT(MI_DISP_EnableInputPort(s32DispLayer, u8DispInport));
            STCHECKRESULT(MI_DISP_SetInputPortSyncMode(s32DispLayer, u8DispInport, E_MI_DISP_SYNC_MODE_FREE_RUN));

            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stBindInfo.stSrcChnPort.u32DevId = SclDevId;
            stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
            stBindInfo.stSrcChnPort.u32PortId = SclOutPortId;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
            stBindInfo.stDstChnPort.u32DevId = gstDisplayOpt.s32HdmiDev;
            stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(gstDisplayOpt.s32HdmiLayer, u8DispInport);
            stBindInfo.stDstChnPort.u32PortId = 0;
            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = 30;
            stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
        }
#else
        u32DispWidth = gstDisplayOpt.u32HdmiW;
        u32DispHeight = gstDisplayOpt.u32HdmiH;
        SclDevId = 0;
        SclOutPortId = 1;
        u8DispInport = 0;
        //for(u8DispInport = 0; u8DispInport < inputCnt; u8DispInport++)
        {
            SclChnId = gstSensorAttr[g_s32SensorIdxForHdmi].eSensorPadID;
            memset(&stSclOutputParam, 0x0, sizeof(MI_SCL_OutPortParam_t));
            STCHECKRESULT(MI_SCL_GetOutputPortParam(SclDevId, SclChnId, SclOutPortId, &stSclOutputParam));
            STCHECKRESULT(MI_SCL_DisableOutputPort(SclDevId, SclChnId, SclOutPortId));
            ST_GetCropParam(gstSensorAttr[g_s32SensorIdxForHdmi].u16Width, gstSensorAttr[g_s32SensorIdxForHdmi].u16Height, \
                            gstDisplayOpt.u32HdmiW, gstDisplayOpt.u32HdmiH, \
                            &stSclOutputParam.stSCLOutCropRect);
            stSclOutputParam.stSCLOutputSize.u16Width = u32DispWidth;
            stSclOutputParam.stSCLOutputSize.u16Height = u32DispHeight;
            STCHECKRESULT(MI_SCL_SetOutputPortParam(SclDevId, SclChnId, SclOutPortId, &stSclOutputParam));
            STCHECKRESULT(MI_SCL_EnableOutputPort(SclDevId, SclChnId, SclOutPortId));
            memset(&stWinRect, 0x0, sizeof(MI_DISP_VidWinRect_t));
            memset(&stInputPortAttr, 0x0, sizeof(MI_DISP_InputPortAttr_t));
            stInputPortAttr.stDispWin.u16X = 0;
            stInputPortAttr.stDispWin.u16Y = 0;
            stInputPortAttr.stDispWin.u16Width = u32DispWidth;
            stInputPortAttr.stDispWin.u16Height = u32DispHeight;
            stInputPortAttr.u16SrcWidth = u32DispWidth;
            stInputPortAttr.u16SrcHeight = u32DispHeight;
            stWinRect.u16Width = u32DispWidth;
            stWinRect.u16Height = u32DispHeight;
            STCHECKRESULT(MI_DISP_SetInputPortAttr(s32DispLayer, u8DispInport, &stInputPortAttr));
            STCHECKRESULT(MI_DISP_EnableInputPort(s32DispLayer, u8DispInport));
            STCHECKRESULT(MI_DISP_SetInputPortSyncMode(s32DispLayer, u8DispInport, E_MI_DISP_SYNC_MODE_FREE_RUN));

            memset(&stBindInfo, 0x0, sizeof(ST_Sys_BindInfo_T));
            stBindInfo.stSrcChnPort.eModId = E_MI_MODULE_ID_SCL;
            stBindInfo.stSrcChnPort.u32DevId = SclDevId;
            stBindInfo.stSrcChnPort.u32ChnId = SclChnId;
            stBindInfo.stSrcChnPort.u32PortId = SclOutPortId;
            stBindInfo.stDstChnPort.eModId = E_MI_MODULE_ID_DISP;
            stBindInfo.stDstChnPort.u32DevId = gstDisplayOpt.s32HdmiDev;
            stBindInfo.stDstChnPort.u32ChnId = TO_SYS_CHN_ID(gstDisplayOpt.s32HdmiLayer, u8DispInport);
            stBindInfo.stDstChnPort.u32PortId = 0;
            stBindInfo.u32SrcFrmrate = 30;
            stBindInfo.u32DstFrmrate = 30;
            stBindInfo.eBindType = E_MI_SYS_BIND_TYPE_FRAME_BASE;
            STCHECKRESULT(ST_Sys_Bind(&stBindInfo));
        }
#endif
    }

    return MI_SUCCESS;
}

#if 1//def ENABLE_FR
    static MI_S32 g_s32Frid = -1;
#endif

int SSTAR_DualSensorInit(MI_BOOL bEnableFr, int doFrPad)
{
    int sensorIdx, totalCnt;
    ST_Sensor_Attr_t *pstSensorAttr = NULL;
#ifdef ENABLE_FR
    MI_S32 i, s32Frid = -1;
    char *pCmd, *pArgs, argStr[256];
    int argLen = 0;
#endif

#ifndef USE_ONE_SENSOR
    static MI_VIF_SNRPad_e eSensorPadID = gstSensorAttr[0].eSensorPadID;
    static MI_VIF_GROUP u32VifGroupID = gstSensorAttr[0].u32VifGroupID;
    static MI_U32 u32BindVifDev = gstSensorAttr[0].u32BindVifDev;
    static MI_BOOL bSensor1Used = gstSensorAttr[1].bUsed;

    if(bEnableFr)
    {
        gstSensorAttr[0].eSensorPadID = (MI_VIF_SNRPad_e)doFrPad;
        gstSensorAttr[0].u32VifGroupID = 2 * doFrPad;
        gstSensorAttr[0].u32BindVifDev = 8 * doFrPad;
        gstSensorAttr[0].bDoFr = 1;
        gstSensorAttr[1].bUsed = 0;
        g_s32SensorIdxForPanel = 0;
        g_s32SensorIdxForHdmi = 0;
     }
    else
    {
        gstSensorAttr[0].eSensorPadID = eSensorPadID;
        gstSensorAttr[0].u32VifGroupID = u32VifGroupID;
        gstSensorAttr[0].u32BindVifDev = u32BindVifDev;
        gstSensorAttr[0].bDoFr = 0;
        gstSensorAttr[1].bUsed = bSensor1Used;
        g_s32SensorIdxForPanel = 1;
        g_s32SensorIdxForHdmi = 0;
     }
#endif

    //ST_MiscInit();

    STCHECKRESULT(ST_SysModuleInit());

    STCHECKRESULT(ST_DispDevInit(&gstDisplayOpt));

    for(sensorIdx = 0; sensorIdx < ST_MAX_SENSOR_NUM; sensorIdx++)
    {
        pstSensorAttr = &gstSensorAttr[sensorIdx];
        if(0 == pstSensorAttr->bUsed)
        {
            continue;
        }

        STCHECKRESULT(ST_SensorModuleInit(pstSensorAttr->eSensorPadID));
        STCHECKRESULT(ST_VifModuleInit(pstSensorAttr));
        STCHECKRESULT(ST_IspModuleInit(pstSensorAttr));
        STCHECKRESULT(ST_SclModuleInit(pstSensorAttr));
        totalCnt++;
    }

    STCHECKRESULT(ST_DisplayFlowInit(&gstDisplayOpt, totalCnt));

#ifdef ENABLE_FR
    if(bEnableFr)
    {
        pstSensorAttr = NULL;
        for(sensorIdx = 0; sensorIdx < ST_MAX_SENSOR_NUM; sensorIdx++)
        {
            pstSensorAttr = &gstSensorAttr[sensorIdx];
            if(0 == pstSensorAttr->bUsed)
            {
                continue;
            }
            if(TRUE == pstSensorAttr->bDoFr)
            {
            	g_s32Frid = ST_FRStart(0/* scl device */, pstSensorAttr->eSensorPadID, \
                        pstSensorAttr->u16Width, pstSensorAttr->u16Height, \
                        0/* Port for RGN, 0(panel) or 1(hdmi) */, \
                        2/* Port 2 for capture */);
                break;
            }
        }
    }
#endif

#if 0
    while(!bExit)
    {
    #ifdef ENABLE_FR
        printf(">");
        fflush(stdout);
        memset(argStr, 0, sizeof(argStr));
        argLen = read(0, argStr, sizeof(argStr) - 1);
        if(argLen <= 1)
        {
            continue;
        }
        else if(2 == argLen)
        {
            if('q' == argStr[0] || 'Q' == argStr[0])
            {
                break;
            }
        }
        pCmd = argStr;
        if(NULL == (pArgs = strchr(argStr, ' ')))
        {
            continue;
        }
        *pArgs = '\0';
        pArgs++;
        i = strlen(pArgs) - 1;
        for(i = strlen(pArgs) - 1; i >= 0; i--)
        {
            if(ASCII_CR == pArgs[i] || ASCII_LF == pArgs[i])
            {
                pArgs[i] = 0;
            }
        }
        if(0 == strcasecmp(pCmd, "add"))
        {
            //Add face to db
            ST_FRAddPerson(g_s32Frid, pArgs);
        }
    #else
        usleep(100 * 1000);
    #endif
    }

#ifdef ENABLE_FR
    if(g_s32Frid >= 0)
    {
        ST_FRStop(g_s32Frid);
    }
#endif
#endif

    return 0;
}

void SSTAR_DualSensorDeinit()
{
    int sensorIdx;
    ST_Sensor_Attr_t *pstSensorAttr = NULL;

#ifdef ENABLE_FR
    if(g_s32Frid >= 0)
    {
        ST_FRStop(g_s32Frid);
        g_s32Frid = -1;
    }
#endif

	ST_DisplayFlowUnInit(&gstDisplayOpt);
	ST_DispDevUnInit(&gstDisplayOpt);
	for(sensorIdx = ST_MAX_SENSOR_NUM; sensorIdx > 0; sensorIdx--)
	{
		pstSensorAttr = &gstSensorAttr[sensorIdx - 1];
		if(0 == pstSensorAttr->bUsed)
		{
			continue;
		}

		ST_SclModuleUnInit(pstSensorAttr);
		ST_IspModuleUnInit(pstSensorAttr);
		ST_VifModuleUnInit(pstSensorAttr);
		ST_SensorModuleUnInit(pstSensorAttr->eSensorPadID);
	}


	//ST_MiscUnInit();
	return;
}

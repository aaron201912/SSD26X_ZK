#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>

#include "mi_sys.h"
#include "sstardisp.h"
#include "mi_panel_datatype.h"
#include "mi_panel.h"
#include "mi_disp_datatype.h"
#include "mi_disp.h"


#if defined(__cplusplus)||defined(c_plusplus)
extern "C"{
#endif

#define DISP_INPUT_WIDTH    1024
#define DISP_INPUT_HEIGHT   600

#define MAKE_YUYV_VALUE(y,u,v) ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK MAKE_YUYV_VALUE(0,128,128)

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

}ST_Display_Opt_t;

static ST_Display_Opt_t gstDisplayOpt =
{
    .u32DispPort = 0,
    .u8SensorCnt = 0,
    /* Panel parameter */
    .bUsedPanel = 1,
    .s32PanelDev = 1,
    .s32PanelLayer = 2, /* Disp 1, layer should be 2 */
    .ePanelType = E_MI_PNL_INTF_TTL,
    .u32PanelDispTiming = E_MI_DISP_OUTPUT_USER,
    .u8PanelInputPort = 0,
    .u32PanelW = 0,
    .u32PanelH = 0,
    .bPanelMirror = 0,
    .bPanelFlip = 0,
    .ePanelRot = E_MI_SYS_ROTATE_NONE,
};

int sstar_disp_init()
{
    MI_DISP_LAYER s32DispLayer;
    MI_U32 u32LayerW = 0;
    MI_U32 u32LayerH = 0;
    MI_DISP_PubAttr_t stDispPubAttr;
    MI_DISP_VideoLayerAttr_t stLayerAttr;
    MI_DISP_OutputTiming_e u32DispTiming;
    MI_DISP_InputPortAttr_t stInputPortAttr;
    MI_PANEL_ParamConfig_t stParamCfg;
    MI_PANEL_MipiDsiConfig_t stMipiDsiCfg;
    MI_DISP_RotateConfig_t stRotateConfig;

    MI_SYS_Init(0);

    ST_Display_Opt_t *pstDisplayOpt = &gstDisplayOpt;

    s32DispLayer = pstDisplayOpt->s32PanelLayer;
    memset(&stParamCfg, 0x0, sizeof(MI_PANEL_ParamConfig_t));
    memset(&stMipiDsiCfg, 0x0, sizeof(MI_PANEL_MipiDsiConfig_t));
    /* set disp pub */
    memset(&stDispPubAttr, 0, sizeof(MI_DISP_PubAttr_t));
    stDispPubAttr.u32BgColor = YUYV_BLACK;
    stDispPubAttr.eIntfType = (E_MI_PNL_INTF_MIPI_DSI == \
        pstDisplayOpt->ePanelType) ? E_MI_DISP_INTF_MIPIDSI : E_MI_DISP_INTF_TTL;
    stDispPubAttr.eIntfSync =  E_MI_DISP_OUTPUT_USER;
    MI_DISP_SetPubAttr(pstDisplayOpt->s32PanelDev, &stDispPubAttr);
    MI_DISP_Enable(pstDisplayOpt->s32PanelDev);
#if 0
    if(E_MI_PNL_INTF_MIPI_DSI == pstDisplayOpt->ePanelType)
    {
        MI_PANEL_SetMipiDsiConfig(pstDisplayOpt->ePanelType, stMipiDsiCfg);
    }
#endif
    MI_PANEL_Init(pstDisplayOpt->ePanelType);
    MI_PANEL_GetPanelParam(pstDisplayOpt->ePanelType, &stParamCfg);
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
    MI_DISP_BindVideoLayer(s32DispLayer, pstDisplayOpt->s32PanelDev);
    MI_DISP_SetVideoLayerAttr(s32DispLayer, &stLayerAttr);
    MI_DISP_EnableVideoLayer(s32DispLayer);

    stRotateConfig.eRotateMode = E_MI_DISP_ROTATE_NONE;
    MI_DISP_SetVideoLayerRotateMode(s32DispLayer, &stRotateConfig);

    memset(&stDispPubAttr, 0, sizeof(MI_DISP_PubAttr_t));
    stDispPubAttr.eIntfType = E_MI_DISP_INTF_TTL;
    MI_DISP_GetPubAttr(pstDisplayOpt->s32PanelDev, &stDispPubAttr);
    memset(&stInputPortAttr, 0, sizeof(MI_DISP_InputPortAttr_t));
    stInputPortAttr.u16SrcWidth = DISP_INPUT_WIDTH;
    stInputPortAttr.u16SrcHeight = DISP_INPUT_HEIGHT;
    stInputPortAttr.stDispWin.u16X = 0;
    stInputPortAttr.stDispWin.u16Y = 0;
    stInputPortAttr.stDispWin.u16Width = stDispPubAttr.stSyncInfo.u16Hact;
    stInputPortAttr.stDispWin.u16Height = stDispPubAttr.stSyncInfo.u16Vact;
    MI_DISP_SetInputPortAttr(s32DispLayer, 0, &stInputPortAttr);
    MI_DISP_EnableInputPort(s32DispLayer, 0);

    return 0;
}

int sstar_disp_Deinit()
{
    MI_DISP_LAYER s32DispLayer;
    ST_Display_Opt_t *pstDisplayOpt = &gstDisplayOpt;

    s32DispLayer = pstDisplayOpt->s32PanelLayer;
    MI_DISP_UnBindVideoLayer(s32DispLayer, pstDisplayOpt->s32PanelDev);
    MI_DISP_DisableVideoLayer(s32DispLayer);
    MI_DISP_Disable(pstDisplayOpt->s32PanelDev);
    MI_PANEL_DeInit();

    MI_DISP_DeInitDev();
    MI_PANEL_DeInitDev();
    MI_SYS_Exit(0);
    printf("sstar_disp_Deinit...\n");

    return 0;
}

#if defined(__cplusplus)||defined(c_plusplus)
}
#endif


/*
 * sstar_dynamic_load.h
 *
 *  Created on: 2021年2月8日
 *      Author: koda.xu
 */

#ifndef JNI_SDKDIR_SSTAR_DYNAMIC_LOAD_H_
#define JNI_SDKDIR_SSTAR_DYNAMIC_LOAD_H_

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include "mi_sys.h"
#include "mi_ai.h"
#include "mi_ao.h"
#include "mi_rgn.h"
#include "mi_ive.h"
#include "mi_sensor.h"
#include "mi_vif.h"
#include "mi_vpe.h"
#include "mi_isp.h"
#include "mi_isp_iq.h"
#include "mi_iqserver.h"
#include "mi_vdisp.h"
#include "mi_vdec.h"

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnAiSetPubAttr)(MI_AUDIO_DEV AiDevId, MI_AUDIO_Attr_t *pstAttr);
	MI_S32 (*pfnAiGetPubAttr)(MI_AUDIO_DEV AiDevId, MI_AUDIO_Attr_t*pstAttr);
	MI_S32 (*pfnAiEnable)(MI_AUDIO_DEV AiDevId);
	MI_S32 (*pfnAiDisable)(MI_AUDIO_DEV AiDevId);
	MI_S32 (*pfnAiEnableChn)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
	MI_S32 (*pfnAiDisableChn)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn);
	MI_S32 (*pfnAiGetFrame)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_Frame_t*pstFrm, MI_AUDIO_AecFrame_t *pstAecFrm , MI_S32 s32MilliSec);
	MI_S32 (*pfnAiReleaseFrame)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AUDIO_Frame_t *pstFrm, MI_AUDIO_AecFrame_t *pstAecFrm);
	MI_S32 (*pfnAiSetChnParam)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_ChnParam_t *pstChnParam);
	MI_S32 (*pfnAiGetChnParam)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_AI_ChnParam_t *pstChnParam);
	MI_S32 (*pfnAiSetMute)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_BOOL bMute);
	MI_S32 (*pfnAiGetMute)(MI_AUDIO_DEV AiDevId, MI_AI_CHN AiChn, MI_BOOL *pbMute);
	MI_S32 (*pfnAiInitDev)(MI_AI_InitParam_t *pstInitParam);
	MI_S32 (*pfnAiDeInitDev)(void);
} AudioInAssembly_t;


typedef struct
{
	void *pHandle;
	MI_S32 (*pfnAoSetPubAttr)(MI_AUDIO_DEV AoDevId, MI_AUDIO_Attr_t *pstAttr);
	MI_S32 (*pfnAoGetPubAttr)(MI_AUDIO_DEV AoDevId, MI_AUDIO_Attr_t *pstAttr);
	MI_S32 (*pfnAoEnable)(MI_AUDIO_DEV AoDevId);
	MI_S32 (*pfnAoDisable)(MI_AUDIO_DEV AoDevId);
	MI_S32 (*pfnAoEnableChn)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 (*pfnAoDisableChn)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 (*pfnAoSendFrame)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AUDIO_Frame_t *pstData, MI_S32 s32MilliSec);
	MI_S32 (*pfnAoPauseChn)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 (*pfnAoResumeChn)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn);
	MI_S32 (*pfnAoSetVolume)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_S32 s32VolumeDb, MI_AO_GainFading_e eFading);
	MI_S32 (*pfnAoGetVolume)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_S32 *ps32VolumeDb);
	MI_S32 (*pfnAoSetMute)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_BOOL bEnable);
	MI_S32 (*pfnAoGetMute)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_BOOL *pbEnable);
	MI_S32 (*pfnAoSetChnParam)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_ChnParam_t *pstChnParam);
	MI_S32 (*pfnAoGetChnParam)(MI_AUDIO_DEV AoDevId, MI_AO_CHN AoChn, MI_AO_ChnParam_t *pstChnParam);
	MI_S32 (*pfnAoSetSrcGain)(MI_AUDIO_DEV AoDevId, MI_S32 s32VolumeDb);
	MI_S32 (*pfnAoInitDev)(MI_AO_InitParam_t *pstInitParam);
	MI_S32 (*pfnAoDeInitDev)(void);
} AudioOutAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnRgnInit)(MI_RGN_PaletteTable_t *pstPaletteTable);
	MI_S32 (*pfnRgnDeInit)(void);
	MI_S32 (*pfnRgnCreate)(MI_RGN_HANDLE hHandle, MI_RGN_Attr_t *pstRegion);
	MI_S32 (*pfnRgnDestroy) (MI_RGN_HANDLE hHandle);
	MI_S32 (*pfnRgnGetAttr)(MI_RGN_HANDLE hHandle, MI_RGN_Attr_t *pstRegion);
	MI_S32 (*pfnRgnSetBitMap)(MI_RGN_HANDLE hHandle, MI_RGN_Bitmap_t *pstBitmap);
	MI_S32 (*pfnRgnAttachToChn)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t* pstChnPort, MI_RGN_ChnPortParam_t *pstChnAttr);
	MI_S32 (*pfnRgnDetachFromChn)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort);
	MI_S32 (*pfnRgnSetDisplayAttr)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort, MI_RGN_ChnPortParam_t *pstChnPortAttr);
	MI_S32 (*pfnRgnGetDisplayAttr)(MI_RGN_HANDLE hHandle, MI_RGN_ChnPort_t *pstChnPort, MI_RGN_ChnPortParam_t *pstChnPortAttr);
	MI_S32 (*pfnRgnGetCanvasInfo)(MI_RGN_HANDLE hHandle, MI_RGN_CanvasInfo_t* pstCanvasInfo);
	MI_S32 (*pfnRgnUpdateCanvas)(MI_RGN_HANDLE hHandle);
	MI_S32 (*pfnRgnScaleRect)(MI_RGN_ChnPort_t *pstChnPort, MI_RGN_Size_t *pstCanvasSize, MI_RGN_Size_t *pstScreenSize);
	MI_S32 (*pfnRgnInitDev)(MI_RGN_InitParam_t *pstInitParam);
	MI_S32 (*pfnRgnDeInitDev)(void);
} RgnAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnIveCreate)(MI_IVE_HANDLE hHandle);
	MI_S32 (*pfnIveDestroy)(MI_IVE_HANDLE hHandle);
	MI_S32 (*pfnIveCsc)(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc, MI_IVE_DstImage_t *pstDst, MI_IVE_CscCtrl_t *pstCscCtrl, MI_BOOL bInstant);
	MI_S32 (*pfnIveSad)(MI_IVE_HANDLE hHandle, MI_IVE_SrcImage_t *pstSrc1, MI_IVE_SrcImage_t *pstSrc2, MI_IVE_DstImage_t *pstSad, MI_IVE_DstImage_t *pstThr,
					  MI_IVE_SadCtrl_t *pstSadCtrl, MI_BOOL bInstant);
} IveAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnSnrEnable)(MI_SNR_PADID ePADId);
	MI_S32 (*pfnSnrDisable)(MI_SNR_PADID ePADId);  // Stop the sensor operation, streaming, mclk
	MI_S32 (*pfnSnrGetPadInfo)(MI_SNR_PADID ePADId, MI_SNR_PADInfo_t  *pstPadInfo);
	MI_S32 (*pfnSnrGetPlaneInfo)(MI_SNR_PADID ePADId, MI_U32  u32PlaneID, MI_SNR_PlaneInfo_t *pstPlaneInfo);
	MI_S32 (*pfnSnrGetFps)(MI_SNR_PADID ePADId, MI_U32 *pFps);
	MI_S32 (*pfnSnrSetFps)(MI_SNR_PADID ePADId, MI_U32  u32Fps);
	MI_S32 (*pfnSnrSetAnadecSrcAttr)(MI_SNR_PADID ePADId, MI_U32 u32PlaneID, MI_SNR_Anadec_SrcAttr_t *pstSrcAttr);
	MI_S32 (*pfnSnrGetAnadecSrcAttr)(MI_SNR_PADID ePADId, MI_U32 u32PlaneID, MI_SNR_Anadec_SrcAttr_t *pstSrcAttr);
	MI_S32 (*pfnSnrQueryResCount)(MI_SNR_PADID ePADId, MI_U32 *pu32ResCount);
	MI_S32 (*pfnSnrGetRes)(MI_SNR_PADID ePADId, MI_U8 u8ResIdx, MI_SNR_Res_t *pstRes);
	MI_S32 (*pfnSnrGetCurRes)(MI_SNR_PADID ePADId, MI_U8 *pu8CurResIdx, MI_SNR_Res_t  *pstCurRes);
	MI_S32 (*pfnSnrSetRes)(MI_SNR_PADID ePADId, MI_U8 u8ResIdx);
	MI_S32 (*pfnSnrSetOrien)(MI_SNR_PADID ePADId, MI_BOOL bMirror, MI_BOOL bFlip);
	MI_S32 (*pfnSnrGetOrien)(MI_SNR_PADID ePADId, MI_BOOL *pbMirror, MI_BOOL *pbFlip);
	MI_S32 (*pfnSnrSetPlaneMode)(MI_SNR_PADID ePADId, MI_BOOL bEnable);
	MI_S32 (*pfnSnrGetPlaneMode)(MI_SNR_PADID ePADId, MI_BOOL *pbEnable);
	MI_S32 (*pfnSnrCustFunction)(MI_SNR_PADID ePADId, MI_U32 u32CmdId, MI_U32 u32DataSize, void *pCustData, MI_SNR_CUST_DIR_e eDir);
	MI_S32 (*pfnSnrInitDev)(MI_SNR_InitParam_t *pstInitParam);
	MI_S32 (*pfnSnrDeInitDev)(void);
} SensorAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnVifCreateDevGroup)(MI_VIF_GROUP GroupId, MI_VIF_GroupAttr_t *pstGroupAttr);
	MI_S32 (*pfnVifCreateDevGroupExt)(MI_VIF_GROUP GroupId, MI_VIF_GroupAttr_t *pstGroupAttr, MI_VIF_GroupExtAttr_t *pstGroupOptionExtAttr);
	MI_S32 (*pfnVifDestroyDevGroup)(MI_VIF_GROUP GroupId);
	MI_S32 (*pfnVifGetDevGroupAttr)(MI_VIF_GROUP GroupId, MI_VIF_GroupAttr_t *pstGroupAttr);

	MI_S32 (*pfnVifSetDevAttr)(MI_VIF_DEV DevId, MI_VIF_DevAttr_t *pstDevAttr);
	MI_S32 (*pfnVifGetDevAttr)(MI_VIF_DEV DevId, MI_VIF_DevAttr_t *pstDevAttr);
	MI_S32 (*pfnVifEnableDev)(MI_VIF_DEV DevId);
	MI_S32 (*pfnVifDisableDev)(MI_VIF_DEV DevId);
	MI_S32 (*pfnVifGetDevStatus)(MI_VIF_DEV DevId,MI_VIF_DevStatus_t *pstVifDevStatus);

	MI_S32 (*pfnVifSetOutputPortAttr)(MI_VIF_DEV DevId, MI_VIF_PORT PortId, MI_VIF_OutputPortAttr_t *pstAttr);
	MI_S32 (*pfnVifGetOutputPortAttr)(MI_VIF_DEV DevId, MI_VIF_PORT PortId, MI_VIF_OutputPortAttr_t *pstAttr);
	MI_S32 (*pfnVifEnableOutputPort)(MI_VIF_DEV DevId, MI_VIF_PORT PortId);
	MI_S32 (*pfnVifDisableOutputPort)(MI_VIF_DEV DevId, MI_VIF_PORT PortId);

	MI_S32 (*pfnVifQuery)(MI_VIF_DEV DevId, MI_VIF_PORT PortId, MI_VIF_DevPortStat_t *pstStat);
	MI_S32 (*pfnVifCallBackTask_Register)(MI_VIF_DEV DevId, MI_VIF_CallBackParam_t *pstCallBackParam);
	MI_S32 (*pfnVifCallBackTask_UnRegister)(MI_VIF_DEV DevId, MI_VIF_CallBackParam_t *pstCallBackParam);
} VifAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnVpeCreateChannel)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);
	MI_S32 (*pfnVpeDestroyChannel)(MI_VPE_CHANNEL VpeCh);
	MI_S32 (*pfnVpeGetChannelAttr)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);
	MI_S32 (*pfnVpeSetChannelAttr)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelAttr_t *pstVpeChAttr);
	MI_S32 (*pfnVpeStartChannel)(MI_VPE_CHANNEL VpeCh);
	MI_S32 (*pfnVpeStopChannel)(MI_VPE_CHANNEL VpeCh);
	MI_S32 (*pfnVpeSetChannelParam)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelPara_t *pstVpeParam);
	MI_S32 (*pfnVpeGetChannelParam)(MI_VPE_CHANNEL VpeCh, MI_VPE_ChannelPara_t *pstVpeParam);
	MI_S32 (*pfnVpeSetChannelCrop)(MI_VPE_CHANNEL VpeCh,  MI_SYS_WindowRect_t *pstCropInfo);
	MI_S32 (*pfnVpeGetChannelCrop)(MI_VPE_CHANNEL VpeCh,  MI_SYS_WindowRect_t *pstCropInfo);
	MI_S32 (*pfnVpeGetChannelRegionLuma)(MI_VPE_CHANNEL VpeCh, MI_VPE_RegionInfo_t *pstRegionInfo, MI_U32 *pu32LumaData,MI_S32 s32MilliSec);
	MI_S32 (*pfnVpeSetChannelRotation)(MI_VPE_CHANNEL VpeCh,  MI_SYS_Rotate_e eType);
	MI_S32 (*pfnVpeGetChannelRotation)(MI_VPE_CHANNEL VpeCh,  MI_SYS_Rotate_e *pType);
	MI_S32 (*pfnVpeEnablePort)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort);
	MI_S32 (*pfnVpeDisablePort)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort);
	MI_S32 (*pfnVpeSetPortMode)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_VPE_PortMode_t *pstVpeMode);
	MI_S32 (*pfnVpeGetPortMode)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_VPE_PortMode_t *pstVpeMode);
	MI_S32 (*pfnVpeSetPortCrop)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstOutCropInfo);
	MI_S32 (*pfnVpeGetPortCrop)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstOutCropInfo);
	MI_S32 (*pfnVpeSetPortShowPosition)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstPortPositionInfo);
	MI_S32 (*pfnVpeGetPortShowPosition)(MI_VPE_CHANNEL VpeCh, MI_VPE_PORT VpePort, MI_SYS_WindowRect_t *pstPortPositionInfo);
	MI_S32 (*pfnVpeSkipFrame)(MI_VPE_CHANNEL VpeCh, MI_U32 u32FrameNum);
	MI_S32 (*pfnVpeLDCBegViewConfig)(MI_VPE_CHANNEL VpeCh);
	MI_S32 (*pfnVpeLDCEndViewConfig)(MI_VPE_CHANNEL VpeCh);
	MI_S32 (*pfnVpeLDCSetViewConfig)(MI_VPE_CHANNEL VpeCh, void *pConfigAddr, MI_U32 u32ConfigSize);
	MI_S32 (*pfnVpeAlloc_IspDataBuf)(MI_U32 u32Size,void **pUserVirAddr);
	MI_S32 (*pfnVpeFree_IspDataBuf)(void *pUserBuf);
	MI_S32 (*pfnVpeCreateMultiChannelStitch)(MI_VPE_CHANNEL VpeCh, MI_VPE_MultiChannelStitchAttr_t *pstVpeChAttr);
	MI_S32 (*pfnVpeInitDev)(MI_VPE_InitParam_t *pstInitParam);
	MI_S32 (*pfnVpeDeInitDev)(void);
} VpeAssembly_t;

//typedef struct
//{
//	void *pHandle;
//	MI_S32 (*pfnDivpEnable)(MI_DISP_DEV DispDev);
//	MI_S32 (*pfnDivpDisable)(MI_DISP_DEV DispDev);
//	MI_S32 (*pfnDivpSetPubAttr)(MI_DISP_DEV DispDev, const MI_DISP_PubAttr_t *pstPubAttr);
//	MI_S32 (*pfnDivpGetPubAttr)(MI_DISP_DEV DispDev, MI_DISP_PubAttr_t *pstPubAttr);
//	MI_S32 (*pfnDivpDeviceAttach)(MI_DISP_DEV DispSrcDev, MI_DISP_DEV DispDstDev, const MI_DISP_DevAttachAttr_t *pstAttachAttr);
//	MI_S32 (*pfnDivpDeviceDetach)(MI_DISP_DEV DispSrcDev, MI_DISP_DEV DispDstDev);
//	MI_S32 (*pfnDivpEnableVideoLayer)(MI_DISP_LAYER DispLayer);
//	MI_S32 (*pfnDivpDisableVideoLayer)(MI_DISP_LAYER DispLayer);
//	MI_S32 (*pfnDivpSetVideoLayerAttr)(MI_DISP_LAYER DispLayer, const MI_DISP_VideoLayerAttr_t *pstLayerAttr);
//	MI_S32 (*pfnDivpGetVideoLayerAttr)(MI_DISP_LAYER DispLayer, MI_DISP_VideoLayerAttr_t *pstLayerAttr);
//	MI_S32 (*pfnDivpBindVideoLayer)(MI_DISP_LAYER DispLayer, MI_DISP_DEV DispDev);
//	MI_S32 (*pfnDivpUnBindVideoLayer)(MI_DISP_LAYER DispLayer, MI_DISP_DEV DispDev);
//	MI_S32 (*pfnDivpSetPlayToleration)(MI_DISP_LAYER DispLayer, MI_U32 u32Toleration);
//	MI_S32 (*pfnDivpGetPlayToleration)(MI_DISP_LAYER DispLayer, MI_U32 *pu32Toleration);
//	MI_S32 (*pfnDivpGetScreenFrame)(MI_DISP_LAYER DispLayer, MI_DISP_VideoFrame_t *pstVFrame);
//	MI_S32 (*pfnDivpReleaseScreenFrame)(MI_DISP_LAYER DispLayer, MI_DISP_VideoFrame_t *pstVFrame);
//	MI_S32 (*pfnDivpSetVideoLayerAttrBegin)(MI_DISP_LAYER DispLayer);
//	MI_S32 (*pfnDivpSetVideoLayerAttrEnd)(MI_DISP_LAYER DispLayer);
//	MI_S32 (*pfnDivpSetInputPortAttr)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort, const MI_DISP_InputPortAttr_t *pstInputPortAttr);
//	MI_S32 (*pfnDivpGetInputPortAttr)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort, MI_DISP_InputPortAttr_t *pstInputPortAttr);
//	MI_S32 (*pfnDivpEnableInputPort)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort);
//	MI_S32 (*pfnDivpDisableInputPort)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort);
//	MI_S32 (*pfnDivpSetInputPortDispPos)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort, const MI_DISP_Position_t *pstDispPos);
//	MI_S32 (*pfnDivpGetInputPortDispPos)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort, MI_DISP_Position_t *pstDispPos);
//	MI_S32 (*pfnDivpPauseInputPort)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort);
//	MI_S32 (*pfnDivpResumeInputPort)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort);
//	MI_S32 (*pfnDivpStepInputPort)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort);
//	MI_S32 (*pfnDivpShowInputPort)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort);
//	MI_S32 (*pfnDivpHideInputPort)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort);
//	MI_S32 (*pfnDivpSetInputPortSyncMode)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort, MI_DISP_SyncMode_e eMode);
//	MI_S32 (*pfnDivpQueryInputPortStat)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort, MI_DISP_QueryChannelStatus_t *pstStatus);
//	MI_S32 (*pfnDivpSetZoomInWindow)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort, MI_DISP_VidWinRect_t* pstZoomRect);
//	MI_S32 (*pfnDivpGetVgaParam)(MI_DISP_DEV DispDev, MI_DISP_VgaParam_t *pstVgaParam);
//	MI_S32 (*pfnDivpSetVgaParam)(MI_DISP_DEV DispDev, MI_DISP_VgaParam_t *pstVgaParam);
//	MI_S32 (*pfnDivpGetHdmiParam)(MI_DISP_DEV DispDev, MI_DISP_HdmiParam_t *pstHdmiParam);
//	MI_S32 (*pfnDivpSetHdmiParam)(MI_DISP_DEV DispDev, MI_DISP_HdmiParam_t *pstHdmiParam);
//	MI_S32 (*pfnDivpGetLcdParam)(MI_DISP_DEV DispDev, MI_DISP_LcdParam_t *pstLcdParam);
//	MI_S32 (*pfnDivpSetLcdParam)(MI_DISP_DEV DispDev, MI_DISP_LcdParam_t *pstLcdParam);
//	MI_S32 (*pfnDivpGetCvbsParam)(MI_DISP_DEV DispDev, MI_DISP_CvbsParam_t *pstCvbsParam);
//	MI_S32 (*pfnDivpSetCvbsParam)(MI_DISP_DEV DispDev, MI_DISP_CvbsParam_t *pstCvbsParam);
//	MI_S32 (*pfnDivpDeviceSetColorTempeture)(MI_DISP_DEV DispDev, MI_DISP_ColorTemperature_t *pstColorTempInfo);
//	MI_S32 (*pfnDivpDeviceGetColorTempeture)(MI_DISP_DEV DispDev, MI_DISP_ColorTemperature_t *pstColorTempInfo);
//	MI_S32 (*pfnDivpDeviceSetGammaParam)(MI_DISP_DEV DispDev, MI_DISP_GammaParam_t * pstGammaParam);
//	MI_S32 (*pfnDivpClearInputPortBuffer)(MI_DISP_LAYER DispLayer, MI_DISP_INPUTPORT LayerInputPort, MI_BOOL bClrAll);
//	MI_S32 (*pfnDivpSetVideoLayerRotateMode)(MI_DISP_LAYER DispLayer, MI_DISP_RotateConfig_t *pstRotateConfig);
//	MI_S32 (*pfnDivpSetWBCSource)(MI_DISP_WBC DispWbc, const MI_DISP_WBC_Source_t *pstWbcSource);
//	MI_S32 (*pfnDivpGetWBCSource)(MI_DISP_WBC DispWbc, MI_DISP_WBC_Source_t *pstWbcSource);
//	MI_S32 (*pfnDivpSetWBCAttr)(MI_DISP_WBC DispWbc, const MI_DISP_WBC_Attr_t *pstWbcAttr);
//	MI_S32 (*pfnDivpGetWBCAttr)(MI_DISP_WBC DispWbc, MI_DISP_WBC_Attr_t *pstWbcAttr);
//	MI_S32 (*pfnDivpEnableWBC)(MI_DISP_WBC DispWbc);
//	MI_S32 (*pfnDivpDisableWBC)(MI_DISP_WBC DispWbc);
//
//	MI_S32 (*pfnDivpInitDev)(MI_DISP_InitParam_t *pstInitParam);
//	MI_S32 (*pfnDivpDeInitDev)(void);
//} DivpAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnIspApiCmdLoadBinFile)(MI_U32 Channel, char* filepath, MI_U32 user_key);
	//MI_S32 (*pfnIspIqGetParaInitStatus)(MI_U32 Channel, MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t *data);
	MI_S32 (*pfnIspIqGetParaInitStatus)(MI_U32 DevId, MI_U32 Channel, MI_ISP_IQ_PARAM_INIT_INFO_TYPE_t *data);
} IspAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnIqServerOpen)(MI_U16 width, MI_U16 height, MI_S32 vpeChn);
	MI_S32 (*pfnIqServerSetDataPath)(char* path);
	MI_S32 (*pfnIqServerSetCustFunc)(MI_S32(* func)(MI_U16 data_type, MI_U32 length, MI_U8 * data));
	MI_S32 (*pfnIqServerClose)();
} IqServerAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnVdispInit)(void);
	MI_S32 (*pfnVdispExit)(void);
	MI_S32 (*pfnVdispOpenDevice)(MI_VDISP_DEV DevId);
	MI_S32 (*pfnVdispCloseDevice)(MI_VDISP_DEV DevId);
	MI_S32 (*pfnVdispSetOutputPortAttr)(MI_VDISP_DEV DevId, MI_VDISP_PORT PortId, MI_VDISP_OutputPortAttr_t *pstOutputPortAttr);
	MI_S32 (*pfnVdispGetOutputPortAttr)(MI_VDISP_DEV DevId, MI_VDISP_PORT PortId, MI_VDISP_OutputPortAttr_t *pstOutputPortAttr);
	MI_S32 (*pfnVdispSetInputChannelAttr)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId, MI_VDISP_InputChnAttr_t *pstInputChnAttr);
	MI_S32 (*pfnVdispGetInputChannelAttr)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId, MI_VDISP_InputChnAttr_t *pstInputChnAttr);
	MI_S32 (*pfnVdispEnableInputChannel)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId);
	MI_S32 (*pfnVdispDisableInputChannel)(MI_VDISP_DEV DevId, MI_VDISP_CHN ChnId);
	MI_S32 (*pfnVdispStartDev)(MI_VDISP_DEV DevId);
	MI_S32 (*pfnVdispStopDev)(MI_VDISP_DEV DevId);
	MI_S32 (*pfnVdispInitDev)(MI_VDISP_InitParam_t *pstInitParam);
	MI_S32 (*pfnVdispDeInitDev)(void);
} VdispAssembly_t;

typedef struct
{
	void *pHandle;
	MI_S32 (*pfnVdecCreateDev)(MI_VDEC_DEV VdecDev, MI_VDEC_InitParam_t *pstInitParam);
	MI_S32 (*pfnVdecDestroyDev)(MI_VDEC_DEV VdecDev);
	MI_S32 (*pfnVdecCreateChn)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_ChnAttr_t *pstChnAttr);
	MI_S32 (*pfnVdecDestroyChn)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn);
	MI_S32 (*pfnVdecGetChnAttr)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_ChnAttr_t *pstChnAttr);
	MI_S32 (*pfnVdecStartChn)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn);
	MI_S32 (*pfnVdecStopChn)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn);
	MI_S32 (*pfnVdecGetChnStat)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_ChnStat_t *pstChnStat);
	MI_S32 (*pfnVdecFlushChn)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn);
	MI_S32 (*pfnVdecResetChn)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn);
	MI_S32 (*pfnVdecSetChnParam)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_ChnParam_t *pstChnParam);
	MI_S32 (*pfnVdecGetChnParam)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_ChnParam_t *pstChnParam);
	MI_S32 (*pfnVdecSendStream)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_VideoStream_t *pstVideoStream, MI_S32 s32MilliSec);
	MI_S32 (*pfnVdecPauseChn)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn);
	MI_S32 (*pfnVdecRefreshChn)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn);
	MI_S32 (*pfnVdecResumeChn)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn);
	MI_S32 (*pfnVdecStepChn)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn);
	MI_S32 (*pfnVdecGetUserData)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_UserData_t *pstUserData, MI_S32 s32MilliSec);
	MI_S32 (*pfnVdecReleaseUserData)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_UserData_t *pstUserData);
	MI_S32 (*pfnVdecSetDisplayMode)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_DisplayMode_e eDisplayMode);
	MI_S32 (*pfnVdecGetDisplayMode)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_DisplayMode_e *peDisplayMode);
	MI_S32 (*pfnVdecSetOutputPortAttr)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_OutputPortAttr_t *pstOutputPortAttr);
	MI_S32 (*pfnVdecSetOutputPortAttrEx)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_U32 u32Port, MI_VDEC_OutputPortAttr_t *pstOutputPortAttr);
	MI_S32 (*pfnVdecGetOutputPortAttr)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_OutputPortAttr_t *pstOutputPortAttr);
	MI_S32 (*pfnVdecGetOutputPortAttrEx)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_U32 u32Port, MI_VDEC_OutputPortAttr_t *pstOutputPortAttr);
	MI_S32 (*pfnVdecSetOutputPortLayoutMode)(MI_VDEC_DEV VdecDev, MI_VDEC_OutbufLayoutMode_e eBufTileMode);
	MI_S32 (*pfnVdecGetOutputPortLayoutMode)(MI_VDEC_DEV VdecDev, MI_VDEC_OutbufLayoutMode_e *peBufTileMode);
	MI_S32 (*pfnVdecSetDestCrop)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_CropCfg_t *pstCropCfg);
	MI_S32 (*pfnVdecSetDestCropEx)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_U32 u32Port, MI_VDEC_CropCfg_t *pstCropCfg);
	MI_S32 (*pfnVdecGetDestCrop)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_CropCfg_t *pstCropCfg);
	MI_S32 (*pfnVdecGetDestCropEx)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_U32 u32Port, MI_VDEC_CropCfg_t *pstCropCfg);
	MI_S32 (*pfnVdecSetChnErrHandlePolicy)(MI_VDEC_DEV VdecDev, MI_VDEC_CHN VdecChn, MI_VDEC_ErrHandlePolicy_t *pstErrHandlePolicy);
} VdecAssembly_t;



int SSTAR_AI_OpenLibrary(AudioInAssembly_t *pstAudioInAssembly);
void SSTAR_AI_CloseLibrary(AudioInAssembly_t *pstAudioInAssembly);

int SSTAR_AO_OpenLibrary(AudioOutAssembly_t *pstAudioOutAssembly);
void SSTAR_AO_CloseLibrary(AudioOutAssembly_t *pstAudioOutAssembly);

int SSTAR_RGN_OpenLibrary(RgnAssembly_t *pstRgnAssembly);
void SSTAR_RGN_CloseLibrary(RgnAssembly_t *pstRgnAssembly);

int SSTAR_IVE_OpenLibrary(IveAssembly_t *pstIveAssembly);
void SSTAR_IVE_CloseLibrary(IveAssembly_t *pstIveAssembly);

int SSTAR_SNR_OpenLibrary(SensorAssembly_t *pstSnrAssembly);
void SSTAR_SNR_CloseLibrary(SensorAssembly_t *pstSnrAssembly);

int SSTAR_VIF_OpenLibrary(VifAssembly_t *pstVifAssembly);
void SSTAR_VIF_CloseLibrary(VifAssembly_t *pstVifAssembly);

int SSTAR_VPE_OpenLibrary(VpeAssembly_t *pstVpeAssembly);
void SSTAR_VPE_CloseLibrary(VpeAssembly_t *pstVpeAssembly);

//int SSTAR_DIVP_OpenLibrary(DivpAssembly_t *pstDivpAssembly);
//void SSTAR_DIVP_CloseLibrary(DivpAssembly_t *pstDivpAssembly);

int SSTAR_ISP_OpenLibrary(IspAssembly_t *pstIspAssembly);
void SSTAR_ISP_CloseLibrary(IspAssembly_t *pstIspAssembly);

int SSTAR_IQSERVER_OpenLibrary(IqServerAssembly_t *pstIqServerAssembly);
void SSTAR_IQSERVER_CloseLibrary(IqServerAssembly_t *pstIqServerAssembly);

int SSTAR_VDISP_OpenLibrary(VdispAssembly_t *pstVdispAssembly);
void SSTAR_VDISP_CloseLibrary(VdispAssembly_t *pstVdispAssembly);

int SSTAR_VDEC_OpenLibrary(VdecAssembly_t *pstVdecAssembly);
void SSTAR_VDEC_CloseLibrary(VdecAssembly_t *pstVdecAssembly);
#ifdef __cplusplus
}
#endif // __cplusplus


#endif /* JNI_SDKDIR_SSTAR_DYNAMIC_LOAD_H_ */

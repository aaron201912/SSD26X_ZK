#ifndef __DUALSENSOR__H__
#define __DUALSENSOR__H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include "st_common.h"

int SSTAR_DualSensorInit(MI_BOOL bEnableFr, int doFrPad);
void SSTAR_DualSensorDeinit();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //__SENSOR2PANEL__H__

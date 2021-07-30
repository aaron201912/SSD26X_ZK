#ifndef __AUDIODETECT_H__
#define __AUDIODETECT_H__
#include "../aed_kws_detect/base_types.h"
#include "detect_paras.h"

#ifdef __cplusplus
extern "C" {
#include "detect_paras.h"
#endif

int aed_VoiceDetectInit();
int aed_VoiceDetectDeinit();
int aed_VoiceDetectStart(VoiceAnalyzeCallback pfnCallback);
void aed_VoiceDetectStop();

#ifdef __cplusplus
}
#endif

#endif

#ifndef __AUDIODETECT_H__
#define __AUDIODETECT_H__
#include "base_types.h"

#ifdef __cplusplus
extern "C" {
#include "detect_paras.h"
#endif

int kws_VoiceDetectInit();
int kws_VoiceDetectDeinit();
int kws_VoiceDetectStart(VoiceAnalyzeCallback pfnCallback);
void kws_VoiceDetectStop();

#ifdef __cplusplus
}
#endif

#endif

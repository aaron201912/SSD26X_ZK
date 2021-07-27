/*
 * audio.h
 *
 *  Created on: 2020年11月2日
 *      Author: sigma
 */

#ifndef JNI_VIDEO_VIDEO_H_
#define JNI_VIDEO_VIDEO_H_

#ifdef  __cplusplus
extern "C"
{
#endif

#include "sstar_dynamic_load.h"

int SSTAR_StartPlayVideo(char *pVideoFile);
int SSTAR_StopPlayVideo();

#ifdef  __cplusplus
}
#endif
#endif /* JNI_VIDEO_VIDEO_H_ */

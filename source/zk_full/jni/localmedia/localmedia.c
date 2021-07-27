/*
 * localmedia.c
 *
 *  Created on: 2021年7月22日
 *      Author: koda.xu
 */
#include "localmedia.h"
#include "audio.h"
#include "video.h"

#define VIDEO_SRC_PATH	"/customer/res/media/720P25.h264"
#define AUDIO_SRC_PATH	"/customer/res/media/8K_16bit_MONO.wav"

int SSTAR_LocalMedia_StartPlay()
{
	SSTAR_StartPlayVideo(VIDEO_SRC_PATH);
	SSTAR_AO_StartPlayFile(0, AUDIO_SRC_PATH, 2);

	return 0;
}

void SSTAR_LocalMedia_StopPlay()
{
	SSTAR_StopPlayVideo();
	SSTAR_AO_StopPlayFile(0);
}

int SSTAR_LocalMedia_SetVolume(int vol)
{
	SSTAR_AO_SetVolume(0, vol);

	return 0;
}

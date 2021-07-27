/*
 * localmedia.h
 *
 *  Created on: 2021年7月22日
 *      Author: koda.xu
 */

#ifndef JNI_LOCALMEDIA_LOCALMEDIA_H_
#define JNI_LOCALMEDIA_LOCALMEDIA_H_

#ifdef  __cplusplus
extern "C"
{
#endif

int SSTAR_LocalMedia_StartPlay();
void SSTAR_LocalMedia_StopPlay();
int SSTAR_LocalMedia_SetVolume(int vol);

#ifdef  __cplusplus
}
#endif

#endif /* JNI_LOCALMEDIA_LOCALMEDIA_H_ */

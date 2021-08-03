#pragma once
#ifndef SVTCLIENT_EXTERN_C
#ifdef __cplusplus
#define SVTCLIENT_EXTERN_C extern "C"
#else
#define SVTCLIENT_EXTERN_C
#endif
#endif

#define SVTCLIENT_API_EXPORTS

#ifndef SVTCLIENT_STDCALL
#if (defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64) && (defined _MSC_VER)
#define SVTCLIENT_STDCALL __stdcall
#else
#define SVTCLIENT_STDCALL
#endif
#endif

#define SVTCLIENT_IMPL SVTCLIENT_EXTERN_C
#include "common_struct.h"
#define SVTCLIENT_API(rettype) SVTCLIENT_EXTERN_C SVTCLIENT_API_EXPORTS rettype SVTCLIENT_STDCALL




#ifdef __cplusplus
extern "C" {
#endif
	/*************************************************
	*����           ��ʼ��sdk����
	*ipu_firwarepath  ���룺ipu_firware.bin ��·��
	*modelpath        ���룺ģ�ʹ�ŵ�·��
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_Init(const char* ipu_firmwarepath,const char* modelpath);

	/*************************************************
	*����           �������
	*detectorID       ���룺���ָ��
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_CreateHandle(long long* detectorID);

	/*************************************************
	*����           �������ӿڣ����ݲ�����
	*detectorID      ���룺���ָ��
	*imagedata       ���룺ͼ������ݣ�ֻ֧��BGRA��
	*width           ���룺ͼ��Ŀ� 320
	*height          ���룺ͼ��ĸ� 256
	*channel         ���룺ͼ���ͨ����BGRA��
	*ParamSDK        ���룺���ò���
	*DetectBox       ����������������
	*facecount       ��������������
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_Detect(long long detectID,const char* imagedata,const int width,const int height, const int channel, const ParamSDK* params, DetectBox** detectout,int* facecount);
	
	/*************************************************
     *����           �������ӿڣ����ݲ�����
     *detectorID      ���룺���ָ��
     *imagedata       ���룺ͼ������ݣ�ֻ֧��BGRA��
     *width           ���룺ͼ��Ŀ� 112
     *height          ���룺ͼ��ĸ� 112
     *channel         ���룺ͼ���ͨ����Ĭ��ΪBGRA 4ͨ����
     *DetectBox       ���������������
     *************************************************/
    SVTCLIENT_API(int) XC_FaceLandmark_Detect(long long detectID, const char* imagedata, const int width, const int height,const int channel, DetectBox* detectbox);
	/*************************************************
	*����           ��BGRA������crop��112x112��BGRA������
	*imagedata       ���룺BGRA������ָ��
	*width           ���룺ͼ��Ŀ�
	*height          ���룺ͼ��ĸ�
	*channel		 ���룺ͼ���ͨ��
	*detectout       ���룺����͹ؼ���
	*outmatdata      �����������BGRAͼ��
	*************************************************/
	SVTCLIENT_API(int) XC_Crop112x112_BGRA(unsigned char* imagedata,int width,int height,int channel, DetectBox detectout,unsigned char* outmatdata); 
	
	/*************************************************
	*����           �����⣨��δʵ�֣�
	*detectorID      ���룺���
	*imagedata       ���룺ͼ��buffer����
	*width           ���룺ͼ��Ŀ�
	*height          ���룺ͼ��ĸ�
	* channel        ���룺ͼ���ͨ����
	*detectRect      ���룺����͹ؼ���
	*spoofing        ������Ƿ��ǻ���
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_Antispoofing(long long detectID,const char* imagedata, const int width, const int height, const int channel, DetectBox detectRect,int* spoofing);
	
	/*************************************************
	*����           ����������ȡ
	*detectorID      ���룺���
	*imagedata       ���룺BGRͼ������
	*width           ���룺ͼ��Ŀ�
	*height          ���룺ͼ��ĸ�
	*channel         ���룺ͼ���ͨ����
	*detectRect      ���룺����͹ؼ���
	*featureout      �������������
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecegnition_FeatureExtract(long long detectID, const char* imagedata, const int width, const int height, const int channel, DetectBox detectRect, int16_t* featureout);
	
	/*************************************************
	*����           ���������ȶ�
	*feature1        ���룺��һ����������
	*feature2        ���룺�ڶ�����������
	*length          ���룺�����������ȣ�130��
	*simility        ������������ƶ�
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_FeatureCompare(const int16_t* feature1,const int16_t* feature2,int length, float* simility);
	
	/*************************************************
	*����           �ͷž��
	*detectorID      ���룺���
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_ReleaseHandle(long long detectorID);

	/*************************************************
	*����           ��������sdk�����л���
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_Cleanup();


#ifdef __cplusplus
}
#endif

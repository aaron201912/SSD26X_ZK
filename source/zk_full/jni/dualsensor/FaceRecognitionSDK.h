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
	*功能           初始化sdk环境
	*ipu_firwarepath  输入：ipu_firware.bin 的路径
	*modelpath        输入：模型存放的路劲
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_Init(const char* ipu_firmwarepath,const char* modelpath);

	/*************************************************
	*功能           创建句柄
	*detectorID       输入：句柄指针
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_CreateHandle(long long* detectorID);

	/*************************************************
	*功能           人脸检测接口（根据参数）
	*detectorID      输入：句柄指针
	*imagedata       输入：图像的数据（只支持BGRA）
	*width           输入：图像的宽 320
	*height          输入：图像的高 256
	*channel         输入：图像的通道（BGRA）
	*ParamSDK        输入：配置参数
	*DetectBox       输出：输出的人脸框
	*facecount       输出：人脸框个数
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_Detect(long long detectID,const char* imagedata,const int width,const int height, const int channel, const ParamSDK* params, DetectBox** detectout,int* facecount);
	
	/*************************************************
     *功能           人脸检测接口（根据参数）
     *detectorID      输入：句柄指针
     *imagedata       输入：图像的数据（只支持BGRA）
     *width           输入：图像的宽 112
     *height          输入：图像的高 112
     *channel         输入：图像的通道（默认为BGRA 4通道）
     *DetectBox       输出：检测的人脸框
     *************************************************/
    SVTCLIENT_API(int) XC_FaceLandmark_Detect(long long detectID, const char* imagedata, const int width, const int height,const int channel, DetectBox* detectbox);
	/*************************************************
	*功能           将BGRA的数据crop出112x112的BGRA的数据
	*imagedata       输入：BGRA的数据指针
	*width           输入：图像的宽
	*height          输入：图像的高
	*channel		 输入：图像的通道
	*detectout       输入：检测框和关键点
	*outmatdata      输出：对齐后的BGRA图像
	*************************************************/
	SVTCLIENT_API(int) XC_Crop112x112_BGRA(unsigned char* imagedata,int width,int height,int channel, DetectBox detectout,unsigned char* outmatdata); 
	
	/*************************************************
	*功能           活体检测（暂未实现）
	*detectorID      输入：句柄
	*imagedata       输入：图像buffer数据
	*width           输入：图像的宽
	*height          输入：图像的高
	* channel        输入：图像的通道数
	*detectRect      输入：检测框和关键点
	*spoofing        输出：是否是活体
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_Antispoofing(long long detectID,const char* imagedata, const int width, const int height, const int channel, DetectBox detectRect,int* spoofing);
	
	/*************************************************
	*功能           人脸特征提取
	*detectorID      输入：句柄
	*imagedata       输入：BGR图像数据
	*width           输入：图像的宽
	*height          输入：图像的高
	*channel         输入：图像的通道数
	*detectRect      输入：检测框和关键点
	*featureout      输出：人脸特征
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecegnition_FeatureExtract(long long detectID, const char* imagedata, const int width, const int height, const int channel, DetectBox detectRect, int16_t* featureout);
	
	/*************************************************
	*功能           人脸特征比对
	*feature1        输入：第一个人脸特征
	*feature2        输入：第二个人脸特征
	*length          输入：人脸特征长度（130）
	*simility        输出：人脸相似度
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_FeatureCompare(const int16_t* feature1,const int16_t* feature2,int length, float* simility);
	
	/*************************************************
	*功能           释放句柄
	*detectorID      输入：句柄
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_ReleaseHandle(long long detectorID);

	/*************************************************
	*功能           清理人脸sdk的运行环境
	*************************************************/
	SVTCLIENT_API(int) XC_FaceRecognition_Cleanup();


#ifdef __cplusplus
}
#endif

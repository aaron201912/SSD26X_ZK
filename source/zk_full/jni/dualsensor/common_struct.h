#pragma once
#define FR_POINT_LEN 5

#define FR_SDK_ERROR(fmt,args...)  do {printf("[ERROR ] [%-4d] [%10s] ", __LINE__, __func__); printf(fmt, ##args);} while(0)
#define FR_SDK_INFO(fmt,args...)  // do {printf("[INFO ] [%-4d] [%10s] ", __LINE__, __func__); printf(fmt, ##args);} while(0)

typedef struct FR_POINTF_
{
	float x;
	float y;

}FR_POINTF;

typedef struct DetectBox_
{
	float x1;
	float y1;
	float x2;
	float y2;
	float score;
	FR_POINTF Landmark[5];
	int faceID;
	bool Befiltered = false;
}DetectBox;

typedef enum FR_ERRORCODE_ {
	FR_SCECCSS = 0,
	FR_INVAILD_PARAM,
	FR_LOADMODEL_FAIL,
	FR_INPUT_NULL,
	FR_NOT_INIT,
	FR_INERENCE_FAIL,
	FR_UNREGISTER,
	FR_INVALID_DEVICEID,
	FR_UNKNOWN,
}FR_ERRORCODE;

typedef struct ParamSDK_
{
	int datatype=0;              //datatype:0��ʾע���գ�ȡ�������;1��ʾ���,ȡ��������;2��ʾ��Ƶ����,�������id����Ŀ�����;3��ʾ��Ƶ���룬��Ŀ�����
}ParamSDK;





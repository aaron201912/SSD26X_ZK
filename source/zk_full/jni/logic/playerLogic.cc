#pragma once
#include "uart/ProtocolSender.h"
/*
*此文件由GUI工具生成
*文件功能：用于处理用户的逻辑相应代码
*功能说明：
*========================onButtonClick_XXXX
当页面中的按键按下后系统会调用对应的函数，XXX代表GUI工具里面的[ID值]名称，
如Button1,当返回值为false的时候系统将不再处理这个按键，返回true的时候系统将会继续处理此按键。比如SYS_BACK.
*========================onSlideWindowItemClick_XXXX(int index) 
当页面中存在滑动窗口并且用户点击了滑动窗口的图标后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如slideWindow1;index 代表按下图标的偏移值
*========================onSeekBarChange_XXXX(int progress) 
当页面中存在滑动条并且用户改变了进度后系统会调用此函数,XXX代表GUI工具里面的[ID值]名称，
如SeekBar1;progress 代表当前的进度值
*========================ogetListItemCount_XXXX() 
当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表的总数目,XXX代表GUI工具里面的[ID值]名称，
如List1;返回值为当前列表的总条数
*========================oobtainListItemData_XXXX(ZKListView::ZKListItem *pListItem, int index)
 当页面中存在滑动列表的时候，更新的时候系统会调用此接口获取列表当前条目下的内容信息,XXX代表GUI工具里面的[ID值]名称，
如List1;pListItem 是贴图中的单条目对象，index是列表总目的偏移量。具体见函数说明
*========================常用接口===============
*LOGD(...)  打印调试信息的接口
*mTextXXXPtr->setText("****") 在控件TextXXX上显示文字****
*mButton1Ptr->setSelected(true); 将控件mButton1设置为选中模式，图片会切换成选中图片，按钮文字会切换为选中后的颜色
*mSeekBarPtr->setProgress(12) 在控件mSeekBar上将进度调整到12
*mListView1Ptr->refreshListView() 让mListView1 重新刷新，当列表数据变化后调用
*mDashbroadView1Ptr->setTargetAngle(120) 在控件mDashbroadView1上指针显示角度调整到120度
*
* 在Eclipse编辑器中  使用 “alt + /”  快捷键可以打开智能提示
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct shared_use_st
{
    int  written;    //作为一个标志，非0：表示可读，0表示可写
    bool flag;
};

struct shared_use_st *shm_addr = NULL;
int shm_id = 0;

#define CLT_IPC         "/customer/client_input"
#define SVC_IPC         "/customer/server_input"
#define MYPLAYER_PATH   "/customer/MyPlayer &"

typedef enum
{
  IPC_COMMAND_OPEN,
  IPC_COMMAND_CLOSE,
  IPC_COMMAND_PAUSE,
  IPC_COMMAND_RESUME,
  IPC_COMMAND_SEEK,
  IPC_COMMAND_SEEK2TIME,
  IPC_COMMAND_GET_POSITION,
  IPC_COMMAND_GET_DURATION,
  IPC_COMMAND_MAX,
  IPC_COMMAND_ACK,
  IPC_COMMAND_SET_VOLUMN,
  IPC_COMMAND_SET_MUTE,
  IPC_COMMAND_ERROR,
  IPC_COMMAND_COMPLETE,
  IPC_COMMAND_CREATE,
  IPC_COMMAND_DESTORY,
  IPC_COMMAND_EXIT,
  IPC_COMMAND_PANT
} IPC_COMMAND_TYPE;

typedef struct{
    int x;
    int y;
    int width;
    int height;
    double misc;
    int aodev, volumn;
    int status;
    int rotate;
    bool mute;
    bool audio_only, video_only;
    int  play_mode;
    char filePath[512];
}stPlayerData;

typedef struct {
    unsigned int EventType;
    stPlayerData stPlData;
} IPCEvent;

class IPCOutput {
public:
    IPCOutput(const std::string& file):m_fd(-1), m_file(file) {
    }

    virtual ~IPCOutput() {
        Term();
    }

    bool Init() {
        if (m_fd < 0) {
            m_fd = open(m_file.c_str(), O_WRONLY | O_NONBLOCK, S_IWUSR | S_IWOTH);
            printf("IPCOutput m_fd = %d\n", m_fd);
        }
        return m_fd >= 0;
    }

    void Term() {
        if (m_fd >= 0) {
            close(m_fd);
        }
        m_fd = -1;
        printf("%s term!\n", m_file.c_str());
    }

    int Send(const IPCEvent& evt) {
        if (m_fd >= 0) {
            return write(m_fd, &evt, sizeof(IPCEvent));
        }
        printf("write %s failed!\n", m_file.c_str());
        return -1;
    }

private:
    int m_fd, ret;
    std::string m_file;
};

class IPCNameFifo {
public:
    IPCNameFifo(const char* file): m_file(file) {
        unlink(m_file.c_str());
        m_valid = !mkfifo(m_file.c_str(), 0777);
    }

    ~IPCNameFifo() {
    unlink(m_file.c_str());
}

inline const std::string& Path() { return m_file; }
inline bool IsValid() { return m_valid; }

private:
    bool m_valid;
    std::string m_file;
};

class IPCInput {
public:
    IPCInput(const std::string& file):m_fd(-1),m_file(file),m_fifo(file.c_str()){}

    virtual ~IPCInput() {
        Term();
    }

    bool Init() {
        if (!m_fifo.IsValid()){
            printf("%s non-existent!!!! \n",m_fifo.Path().c_str());
            return false;
        }
        if (m_fd < 0) {
            m_fd = open(m_file.c_str(), O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU | S_IWOTH);
            printf("IPCInput m_fd = %d\n", m_fd);
        }
        return m_fd >= 0;
    }

    int Read(IPCEvent& evt) {
        if (m_fd >= 0) {
            return read(m_fd, &evt, sizeof(IPCEvent));
        }
        printf("read %s failed!\n", m_file.c_str());
        return 0;
    }

    void Term() {
        if (m_fd >= 0) {
            close(m_fd);
        }
        m_fd = -1;
        printf("%s term!\n", m_file.c_str());
    }

private:
    int m_fd, ret;
    std::string m_file;
    IPCNameFifo m_fifo;
};

IPCEvent recvevt;
IPCEvent sendevt;
IPCInput  i_server(SVC_IPC);
IPCOutput o_client(CLT_IPC);

#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "mi_sys.h"
#include "mi_scl.h"
#include "mi_disp.h"
#include "mi_gfx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#include "frame.h"
//#include "demux.h"
//#include "videostream.h"
//#include "audiostream.h"
//#include "player.h"
//#include "blitutil.h"

#include "panelconfig.h"
#include "hotplugdetect.h"
#include "imageplayer.h"

#define UI_MAX_WIDTH			800
#define UI_MAX_HEIGHT			480

#define ALIGN_DOWN(x, n)        (((x) / (n)) * (n))
#define MAKE_YUYV_VALUE(y,u,v)  ((y) << 24) | ((u) << 16) | ((y) << 8) | (v)
#define YUYV_BLACK              MAKE_YUYV_VALUE(0,128,128)
#define YUYV_WHITE              MAKE_YUYV_VALUE(255,128,128)
#define YUYV_RED                MAKE_YUYV_VALUE(76,84,255)
#define YUYV_GREEN              MAKE_YUYV_VALUE(149,43,21)
#define YUYV_BLUE               MAKE_YUYV_VALUE(29,225,107)

#define DIVP_CHN        		0
#define DISP_DEV        		0
#define DISP_LAYER      		0
#define DISP_INPUTPORT  		0
#define AUDIO_DEV       		0
#define AUDIO_CHN       		0
#define AUDIO_SAMPLE_PER_FRAME  1024
#define AUDIO_MAX_DATA_SIZE     25000
#define MIN_AO_VOLUME           -60
#define MAX_AO_VOLUME           30
#define MIN_ADJUST_AO_VOLUME    -10
#define MAX_ADJUST_AO_VOLUME    20

#define VOL_ADJUST_FACTOR		2
#define PROGRESS_UPDATE_TIME_INTERVAL	500000		// 0.5s

#define DISPLAY_PIC_DURATION	5000000				// 5s

#define PLAY_INIT_POS -1

typedef enum
{
    E_PLAY_FORWARD,
    E_PLAY_BACKWARD
}PlayDirection_e;

typedef enum
{
    E_PLAY_NORMAL_MODE,
    E_PLAY_FAST_MODE,
    E_PLAY_SLOW_MODE
}PlayMode_e;

typedef enum
{
    E_NORMAL_SPEED = 0,
    E_2X_SPEED,
    E_4X_SPEED,
    E_8X_SPEED,
    E_16X_SPEED,
    E_32X_SPEED
}PlaySpeedMode_e;

typedef enum
{
	FILE_REPEAT_MODE,
	LIST_REPEAT_MODE
}RepeatMode_e;

typedef enum
{
	NO_SKIP,
	SKIP_NEXT,
	SKIP_PREV
}SkipMode_e;

// playing page
static bool g_bShowPlayToolBar = FALSE;         // select file list page or playing page
static bool g_bPlaying = FALSE;					// 正在播放状态
static bool g_bPause = FALSE;					// 播放暂停状态
static bool g_bMute = FALSE;
static int g_s32VolValue = 20;
static bool g_ePlayDirection = E_PLAY_FORWARD;
static PlayMode_e g_ePlayMode = E_PLAY_NORMAL_MODE;
static PlaySpeedMode_e g_eSpeedMode = E_NORMAL_SPEED;
static unsigned int g_u32SpeedNumerator = 1;
static unsigned int g_u32SpeedDenomonator = 1;

// playViewer size
static int g_playViewWidth = PANEL_WIDTH;
static int g_playViewHeight = PANEL_HEIGHT;

// streamplayer & imagePlayer
static std::string g_fileName;
static ImagePlayer_t *g_pstImagePlayer = NULL;

// play pos
static long long g_firstPlayPos = PLAY_INIT_POS;
static long long g_duration = 0;
static long long int g_currentPos = 0;

// play video/audio or picture
static int g_playStream = 0;
static pthread_t g_playFileThread = 0;
static bool g_bPlayFileThreadExit = false;
static bool g_bPlayCompleted = false;
static bool g_bPlayError = false;
static RepeatMode_e g_eRepeatMode = LIST_REPEAT_MODE;
static SkipMode_e g_eSkipMode = NO_SKIP;
static pthread_mutex_t g_playFileMutex;
static bool g_bPantStatus = false;

extern void GetPrevFile(char *pCurFileFullName, char *pPrevFileFullName, int prevFilePathLen);
extern void GetNextFile(char *pCurFileFullName, char *pNextFileFullName, int nextFilePathLen);
extern int IsMediaStreamFile(char *pCurFileFullName);

void ShowToolbar(bool bShow)
{
	mWindow_playBarPtr->setVisible(bShow);
	mWindow_mediaInfoPtr->setVisible(bShow);

	if (g_playStream)
	{
		mTextview_volTitlePtr->setVisible(true);
		mTextview_volumePtr->setVisible(true);
		mSeekbar_progressPtr->setVisible(true);
		mButton_voicePtr->setVisible(true);
		mSeekbar_volumnPtr->setVisible(true);
		mTextview_slashPtr->setVisible(true);
		mTextview_durationPtr->setVisible(true);
		mTextview_curtimePtr->setVisible(true);
	}
	else
	{
		mTextview_volTitlePtr->setVisible(false);
		mTextview_volumePtr->setVisible(false);
		mSeekbar_progressPtr->setVisible(false);
		mButton_voicePtr->setVisible(false);
		mSeekbar_volumnPtr->setVisible(false);
		mTextview_slashPtr->setVisible(false);
		mTextview_durationPtr->setVisible(false);
		mTextview_curtimePtr->setVisible(false);
	}
}

class ToolbarHideThread : public Thread {
public:
	void setCycleCnt(int cnt, int sleepMs) { nCycleCnt = cnt; nSleepMs = sleepMs; }

protected:
	virtual bool threadLoop() {
		if (!nCycleCnt)
		{
			ShowToolbar(false);
			return false;
		}

		sleep(nSleepMs);
		nCycleCnt--;

		return true;
	}

private:
	int nCycleCnt;
	int nSleepMs;
};

static ToolbarHideThread g_hideToolbarThread;

// auto hide toolbar after displaying 5s
void AutoDisplayToolbar()
{
	if (!g_hideToolbarThread.isRunning())
	{
		printf("start hide toolbar thread\n");
		g_hideToolbarThread.setCycleCnt(100, 50);
		g_hideToolbarThread.run("hideToolbar");
	}
	else
	{
		printf("wait hideToolBarthread exit\n");
		g_hideToolbarThread.requestExitAndWait();
		g_hideToolbarThread.setCycleCnt(100, 50);
		g_hideToolbarThread.run("hideToolbar");
	}

	ShowToolbar(true);
}

void SetPlayingStatus(bool bPlaying)
{
	mButton_playPtr->setSelected(bPlaying);
}

void SetMuteStatus(bool bMute)
{
	mButton_voicePtr->setSelected(bMute);
}

int SetPlayerVolumn(int vol)
{
	char volInfo[8];

	memset(volInfo, 0, sizeof(volInfo));
	sprintf(volInfo, "%d%%", vol);
	mSeekbar_volumnPtr->setProgress(vol);
	mTextview_volumePtr->setText(volInfo);
	return 0;
}

int GetPlayerVolumn()
{
	int vol = mSeekbar_volumnPtr->getProgress();
	char volInfo[8];

	memset(volInfo, 0, sizeof(volInfo));
	sprintf(volInfo, "%d%%", vol);
	mTextview_volumePtr->setText(volInfo);
	return vol;
}
#if 0
MI_S32 StartPlayAudio()
{
    MI_AUDIO_Attr_t stSetAttr;
    MI_AUDIO_Attr_t stGetAttr;
    MI_AUDIO_DEV AoDevId = AUDIO_DEV;
    MI_AO_CHN AoChn = AUDIO_CHN;

    MI_S32 s32SetVolumeDb;
    MI_S32 s32GetVolumeDb;

    MI_AO_InitParam_t stInitParam;

    system("echo 1 > /sys/class/gpio/gpio12/value");

    memset(&stInitParam, 0x0, sizeof(MI_AO_InitParam_t));
    stInitParam.u32DevId = AoDevId;
    stInitParam.u8Data = NULL;
    MI_AO_InitDev(&stInitParam);

    //set Ao Attr struct
    memset(&stSetAttr, 0, sizeof(MI_AUDIO_Attr_t));
    stSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    stSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    stSetAttr.u32FrmNum = 6;
    stSetAttr.u32PtNumPerFrm = AUDIO_SAMPLE_PER_FRAME;
    stSetAttr.u32ChnCnt = 1;

    if(stSetAttr.u32ChnCnt == 2)
    {
        stSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_STEREO;
    }
    else if(stSetAttr.u32ChnCnt == 1)
    {
        stSetAttr.eSoundmode = E_MI_AUDIO_SOUND_MODE_MONO;
    }

    stSetAttr.eSamplerate = E_MI_AUDIO_SAMPLE_RATE_48000;

    /* set ao public attr*/
    MI_AO_SetPubAttr(AoDevId, &stSetAttr);

    /* get ao device*/
    MI_AO_GetPubAttr(AoDevId, &stGetAttr);

    /* enable ao device */
    MI_AO_Enable(AoDevId);

    /* enable ao channel of device*/
    MI_AO_EnableChn(AoDevId, AoChn);


    /* if test AO Volume */
    if (g_s32VolValue)
    	s32SetVolumeDb = g_s32VolValue * (MAX_ADJUST_AO_VOLUME-MIN_ADJUST_AO_VOLUME) / 100 + MIN_ADJUST_AO_VOLUME;
    else
    	s32SetVolumeDb = MIN_AO_VOLUME;

    MI_AO_SetVolume(AoDevId, s32SetVolumeDb);
    MI_AO_SetMute(AoDevId, g_bMute);
    SetMuteStatus(g_bMute);

    /* get AO volume */
    MI_AO_GetVolume(AoDevId, &s32GetVolumeDb);

    return 0;
}

void StopPlayAudio()
{
    MI_AUDIO_DEV AoDevId = AUDIO_DEV;
    MI_AO_CHN AoChn = AUDIO_CHN;

    system("echo 0 > /sys/class/gpio/gpio12/value");

    /* disable ao channel of */
    MI_AO_DisableChn(AoDevId, AoChn);

    /* disable ao device */
    MI_AO_Disable(AoDevId);

    MI_AO_DeInitDev();
}

MI_S32 StartPlayVideo()
{
    MI_DISP_ShowInputPort(DISP_LAYER, DISP_INPUTPORT);
    return 0;
}

void StopPlayVideo()
{
    MI_DISP_ClearInputPortBuffer(DISP_LAYER, DISP_INPUTPORT, TRUE);
    MI_DISP_HideInputPort(DISP_LAYER, DISP_INPUTPORT);
}

MI_S32 CreatePlayerDev()
{
    /*MI_SYS_ChnPort_t stDivpChnPort;
    MI_DIVP_ChnAttr_t stDivpChnAttr;
    MI_DIVP_OutputPortAttr_t stOutputPortAttr;

    MI_U32 u32InputPort = DISP_INPUTPORT;
    MI_DISP_DEV dispDev = DISP_DEV;
    MI_DISP_LAYER dispLayer = DISP_LAYER;
    MI_SYS_ChnPort_t stDispChnPort;
    MI_DISP_RotateConfig_t stRotateConfig;*/

    MI_DISP_InputPortAttr_t stInputPortAttr;
    MI_DISP_InitParam_t stInitDispParam;
    memset(&stInitDispParam, 0x0, sizeof(MI_DISP_InitParam_t));
    stInitDispParam.u32DevId = 0;
    stInitDispParam.u8Data = NULL;
    MI_DISP_InitDev(&stInitDispParam);

    system("echo 12 > /sys/class/gpio/export");
    system("echo out > /sys/class/gpio/gpio12/direction");
    //system("echo 1 > /sys/class/gpio/gpio12/value");

    // 1.初始化DISP模块
    memset(&stInputPortAttr, 0, sizeof(MI_DISP_InputPortAttr_t));
    MI_DISP_GetInputPortAttr(0, 0, &stInputPortAttr);
    stInputPortAttr.u16SrcWidth         = ALIGN_DOWN(g_playViewWidth , 32);
    stInputPortAttr.u16SrcHeight        = ALIGN_DOWN(g_playViewHeight, 32);
    stInputPortAttr.stDispWin.u16X      = 0;
    stInputPortAttr.stDispWin.u16Y      = 0;
    stInputPortAttr.stDispWin.u16Width  = g_playViewWidth;
    stInputPortAttr.stDispWin.u16Height = g_playViewHeight;

    printf("disp input: w=%d, h=%d\n", stInputPortAttr.u16SrcWidth, stInputPortAttr.u16SrcHeight);

    // 2.初始化DIVP模块
    /*memset(&stDivpChnAttr, 0, sizeof(MI_DIVP_ChnAttr_t));
    stDivpChnAttr.bHorMirror            = FALSE;
    stDivpChnAttr.bVerMirror            = FALSE;
    stDivpChnAttr.eDiType               = E_MI_DIVP_DI_TYPE_OFF;
    stDivpChnAttr.eRotateType           = E_MI_SYS_ROTATE_NONE;
    stDivpChnAttr.eTnrLevel             = E_MI_DIVP_TNR_LEVEL_OFF;
    stDivpChnAttr.stCropRect.u16X       = 0;
    stDivpChnAttr.stCropRect.u16Y       = 0;
    stDivpChnAttr.stCropRect.u16Width   = 0;
    stDivpChnAttr.stCropRect.u16Height  = 0;
    stDivpChnAttr.u32MaxWidth           = 1920;
    stDivpChnAttr.u32MaxHeight          = 1080;

    MI_DIVP_CreateChn(DIVP_CHN, &stDivpChnAttr);
    MI_DIVP_StartChn(DIVP_CHN);
    
    memset(&stOutputPortAttr, 0, sizeof(MI_DIVP_OutputPortAttr_t));
    stOutputPortAttr.eCompMode          = E_MI_SYS_COMPRESS_MODE_NONE;
    stOutputPortAttr.ePixelFormat       = E_MI_SYS_PIXEL_FRAME_YUV_SEMIPLANAR_420;
    stOutputPortAttr.u32Width           = ALIGN_DOWN(g_playViewWidth , 32);
    stOutputPortAttr.u32Height          = ALIGN_DOWN(g_playViewHeight, 32);

    // 3.配置旋转属性
    stRotateConfig.eRotateMode          = E_MI_DISP_ROTATE_NONE;
    MI_DISP_SetVideoLayerRotateMode(dispLayer, &stRotateConfig);

    if (stRotateConfig.eRotateMode == E_MI_DISP_ROTATE_NONE || stRotateConfig.eRotateMode == E_MI_DISP_ROTATE_180)
    {
        stInputPortAttr.u16SrcWidth     = ALIGN_DOWN(g_playViewWidth , 32);
        stInputPortAttr.u16SrcHeight    = ALIGN_DOWN(g_playViewHeight, 32);
        stOutputPortAttr.u32Width       = ALIGN_DOWN(g_playViewWidth , 32);
        stOutputPortAttr.u32Height      = ALIGN_DOWN(g_playViewHeight, 32);
    }
    else
    {
        stInputPortAttr.u16SrcWidth     = ALIGN_DOWN(g_playViewHeight, 32);
        stInputPortAttr.u16SrcHeight    = ALIGN_DOWN(g_playViewWidth , 32);
        stOutputPortAttr.u32Width       = ALIGN_DOWN(g_playViewHeight, 32);
        stOutputPortAttr.u32Height      = ALIGN_DOWN(g_playViewWidth , 32);
    }*/

    MI_DISP_DisableInputPort(DISP_LAYER, DISP_INPUTPORT);
    MI_DISP_SetInputPortAttr(DISP_LAYER, DISP_INPUTPORT, &stInputPortAttr);
    MI_DISP_EnableInputPort(DISP_LAYER, DISP_INPUTPORT);
    MI_DISP_SetInputPortSyncMode(DISP_LAYER, DISP_INPUTPORT, E_MI_DISP_SYNC_MODE_FREE_RUN);

    //MI_DIVP_SetOutputPortAttr(0, &stOutputPortAttr);

    //4.绑定DIVP与DISP
    /*memset(&stDispChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stDispChnPort.eModId                = E_MI_MODULE_ID_DISP;
    stDispChnPort.u32DevId              = DISP_DEV;
    stDispChnPort.u32ChnId              = 0;
    stDispChnPort.u32PortId             = DISP_INPUTPORT;
    
    memset(&stDivpChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stDivpChnPort.eModId                = E_MI_MODULE_ID_DIVP;
    stDivpChnPort.u32DevId              = 0;
    stDivpChnPort.u32ChnId              = DIVP_CHN;
    stDivpChnPort.u32PortId             = 0;

    MI_SYS_SetChnOutputPortDepth(&stDivpChnPort, 0, 3);
    MI_SYS_BindChnPort(&stDivpChnPort, &stDispChnPort, 30, 30);*/

    return 0;
}

void DestroyPlayerDev()
{
    /*MI_DISP_LAYER dispLayer = DISP_LAYER;
    MI_U32 u32InputPort = DISP_INPUTPORT;
    MI_SYS_ChnPort_t stDivpChnPort;
    MI_SYS_ChnPort_t stDispChnPort;
    
    memset(&stDispChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stDispChnPort.eModId                = E_MI_MODULE_ID_DISP;
    stDispChnPort.u32DevId              = DISP_DEV;
    stDispChnPort.u32ChnId              = 0;
    stDispChnPort.u32PortId             = DISP_INPUTPORT;
    
    memset(&stDivpChnPort, 0, sizeof(MI_SYS_ChnPort_t));
    stDivpChnPort.eModId                = E_MI_MODULE_ID_DIVP;
    stDivpChnPort.u32DevId              = 0;
    stDivpChnPort.u32ChnId              = DIVP_CHN;
    stDivpChnPort.u32PortId             = 0;
    
    MI_SYS_UnBindChnPort(&stDivpChnPort, &stDispChnPort);

    MI_DIVP_StopChn(0);
    MI_DIVP_DestroyChn(0);*/

    MI_DISP_DisableInputPort(DISP_LAYER, DISP_INPUTPORT);
    MI_DISP_DeInitDev();
}
#endif

int GetImageFileDuration(long long duration)
{
	g_duration = IMAGE_DISPLAY_DURATION;
	return 0;
}

int SetImageFileMaxProgres(long long duration)
{
	char totalTime[32] = {0};
	long long durationSec = 0;

	g_duration = duration;
	durationSec = g_duration / 1000000;	//AV_TIME_BASE;
	sprintf(totalTime, "%02lld:%02lld:%02lld", durationSec/3600, (durationSec%3600)/60, durationSec%60);
	mTextview_durationPtr->setText(totalTime);
	mTextview_curtimePtr->setText("00:00:00");
	mSeekbar_progressPtr->setProgress(0);

	return 0;
}

int GetImageFilePlayPos(long long currentPos)
{
	g_currentPos = currentPos;
	return 0;
}

int SetImageFileCurrentProgress(long long currentPos)
{
	char curTime[32];
	long long curSec = 0;
	int trackPos = 0;

	printf("currentPos is %lld\n", currentPos);
	curSec = currentPos / 1000000;	//AV_TIME_BASE;
	trackPos  = (currentPos * mSeekbar_progressPtr->getMax()) / g_duration;
	memset(curTime, 0, sizeof(curTime));
	sprintf(curTime, "%02lld:%02lld:%02lld", curSec/3600, (curSec%3600)/60, curSec%60);
	printf("image curTime: %s\n", curTime);
	mTextview_curtimePtr->setText(curTime);
	mSeekbar_progressPtr->setProgress(trackPos);

	return 0;
}

int PlayImageFileComplete()
{
	pthread_mutex_lock(&g_playFileMutex);
	g_bPlayCompleted = true;
	pthread_mutex_unlock(&g_playFileMutex);
	return 0;
}

int ResetPlayImageFileProgres()
{
	SetPlayingStatus(false);
	mTextView_picPtr->setVisible(false);
	mTextview_curtimePtr->setText("00:00:00");
	mSeekbar_progressPtr->setProgress(0);

	return 0;
}

static void ResetSpeedMode()
{
    g_ePlayDirection = E_PLAY_FORWARD;
    g_ePlayMode = E_PLAY_NORMAL_MODE;
    g_eSpeedMode = E_NORMAL_SPEED;
    g_u32SpeedNumerator = 1;
    g_u32SpeedDenomonator = 1;
}

static void AdjustVolumeByTouch(int startPos, int endPos)
{
	int progress = mSeekbar_volumnPtr->getProgress();
	char volInfo[16];

	memset(volInfo, 0, sizeof(volInfo));
	// move up, vol++; move down, vol--
	progress -= (endPos - startPos) / VOL_ADJUST_FACTOR;

	progress = (progress > mSeekbar_volumnPtr->getMax())? mSeekbar_volumnPtr->getMax() : progress;
	progress = (progress < 0)? 0 : progress;
	mSeekbar_volumnPtr->setProgress(progress);

	sprintf(volInfo, "%d%%", progress);
	mTextview_volumePtr->setText(volInfo);
	printf("set progress: %d\n", progress);
}

void DetectUsbHotplug(UsbParam_t *pstUsbParam)		// action 0, connect; action 1, disconnect
{
	if (!pstUsbParam->action)
	{
		g_bPlaying = FALSE;
		g_bPause = FALSE;

		if (g_hideToolbarThread.isRunning())
		{
			printf("stop hideToolBarthread\n");
			g_hideToolbarThread.requestExitAndWait();
		}

		g_bPlayFileThreadExit = true;
		if (g_playFileThread)
		{
			pthread_join(g_playFileThread, NULL);
			g_playFileThread = NULL;
		}

		SetPlayingStatus(false);
		ResetSpeedMode();
		mTextview_speedPtr->setText("");
		g_bShowPlayToolBar = FALSE;

		EASYUICONTEXT->goHome();
	}
}

#define USE_POPEN       1
#define PANT_TIME       5
FILE *player_fd = NULL;
extern int errno;

static void StartPlayStreamFile(char *pFileName)
{
    printf("Start to StartPlayStreamFile\n");

    int ret;
    struct timeval time_start, time_wait;
    void *shm = NULL;

    mWindow_errMsgPtr->setVisible(false);
    ResetSpeedMode();
    //system("echo 1 > /sys/class/gpio/gpio12/value");

    memset(&recvevt, 0, sizeof(IPCEvent));

    if(!i_server.Init()) {
        printf("[%s %d]create i_server fail!\n", __FILE__, __LINE__);
        fprintf(stderr, "Error：%s\n", strerror(errno));
        return;
    }

    //创建共享内存
    shm_id = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
    if(shm_id < 0) {
        fprintf(stderr, "shmget failed\n");
        goto next;
    }

    //将共享内存连接到当前进程的地址空间
    shm = shmat(shm_id, (void*)NULL, 0);
    if(shm < 0) {
        fprintf(stderr, "shmat failed\n");
        goto next;
    }

    shm_addr = (struct shared_use_st *)shm;
    memset(shm_addr, 0x0, sizeof(struct shared_use_st));
    printf("shared memory attached at %x\n", (int)shm);

    player_fd = popen(MYPLAYER_PATH, "w");
    if (NULL == player_fd) {
        printf("my_player is not exit!\n");
        goto next;
    }
    printf("popen myplayer progress done!\n");

    gettimeofday(&time_start, NULL);
    while (i_server.Read(recvevt) <= 0
           || (recvevt.EventType != IPC_COMMAND_CREATE)) {
        usleep(10 * 1000);
        gettimeofday(&time_wait, NULL);
        if (time_wait.tv_sec - time_start.tv_sec > 2) {
            printf("myplayer progress create failed!\n");
            break;
        }
    }
next:
    if (recvevt.EventType == IPC_COMMAND_CREATE) {
        printf("myplayer progress create success!\n");
    } else {
        if (shm_addr) {
            //把共享内存从当前进程中分离
            ret = shmdt((void *)shm_addr);
            if (ret < 0) {
                fprintf(stderr, "shmdt failed\n");
            }

            //删除共享内存
            ret = shmctl(shm_id, IPC_RMID, NULL);
            if(ret < 0) {
                fprintf(stderr, "shmctl(IPC_RMID) failed\n");
            }
        }
        shm_addr = NULL;
        shm_id = 0;

        mTextview_msgPtr->setText("Other Error Occur!");
        mWindow_errMsgPtr->setVisible(true);

        pthread_mutex_lock(&g_playFileMutex);
        g_bPlayError = true;
        pthread_mutex_unlock(&g_playFileMutex);

        return;
    }

    if(!o_client.Init()) {
        printf("[%s %d]my_player process not start!\n", __FILE__, __LINE__);
        fprintf(stderr, "Error：%s\n", strerror(errno));
        return;
    }

    memset(&sendevt, 0, sizeof(IPCEvent));
    sendevt.EventType = IPC_COMMAND_OPEN;
    strcpy(sendevt.stPlData.filePath, pFileName);
    printf("list file name to play = %s\n", sendevt.stPlData.filePath);

    // 旋转开关
    #if ENABLE_ROTATE
    sendevt.stPlData.rotate = E_MI_DISP_ROTATE_270;
    #else
    sendevt.stPlData.rotate = E_MI_DISP_ROTATE_NONE;
    #endif
    sendevt.stPlData.x = 0;
    sendevt.stPlData.y = 0;
    sendevt.stPlData.width  = g_playViewWidth;
    sendevt.stPlData.height = g_playViewHeight;
    sendevt.stPlData.aodev = AUDIO_DEV;
    sendevt.stPlData.audio_only = false;
    sendevt.stPlData.video_only = false;
    sendevt.stPlData.play_mode  = 0;    // 0: 单次播放,1: 循环播放(seek to start)
    o_client.Send(sendevt);
    printf("try to open file: %s\n", pFileName);

    memset(&recvevt, 0, sizeof(IPCEvent));
    gettimeofday(&time_start, NULL);
    while (i_server.Read(recvevt) <= 0
           || ((recvevt.EventType != IPC_COMMAND_ACK)
           && (recvevt.EventType != IPC_COMMAND_ERROR)))
    {
        usleep(10 * 1000);
        gettimeofday(&time_wait, NULL);
        if (time_wait.tv_sec - time_start.tv_sec > 10)
        {
            memset(&sendevt, 0, sizeof(IPCEvent));
            sendevt.EventType = IPC_COMMAND_EXIT;
            o_client.Send(sendevt);
            break;
        }
    }
    if (recvevt.EventType == IPC_COMMAND_ACK) {
        printf("receive ack from my_player!\n");

        memset(&sendevt, 0, sizeof(IPCEvent));
        sendevt.EventType = IPC_COMMAND_GET_DURATION;
        o_client.Send(sendevt);
    } else if(recvevt.EventType == IPC_COMMAND_ERROR) {
        if (recvevt.stPlData.status == -101)
            mTextview_msgPtr->setText("请检查网络连接！");
        else if (recvevt.stPlData.status == -2)
            mTextview_msgPtr->setText("不支持播放720P以上的视频！");
        else if (recvevt.stPlData.status == -3)
            mTextview_msgPtr->setText("解码速度不够，请降低视频帧率！");
        else if (recvevt.stPlData.status == -4)
            mTextview_msgPtr->setText("读取网络超时！");
        else
            mTextview_msgPtr->setText("Other Error Occur!");

        mWindow_errMsgPtr->setVisible(true);

        pthread_mutex_lock(&g_playFileMutex);
        g_bPlayError = true;
        pthread_mutex_unlock(&g_playFileMutex);
    }

    SetPlayerVolumn(g_s32VolValue);
    printf("End to StartPlayStreamFile\n");
}

static void StopPlayStreamFile()
{
    printf("Start to StopPlayStreamFile\n");
	
    int ret;
    struct timeval time_start, time_wait;

    //system("echo 0 > /sys/class/gpio/gpio12/value");

    if(o_client.Init())
    {
        memset(&sendevt, 0, sizeof(IPCEvent));
        sendevt.EventType = IPC_COMMAND_EXIT;
        o_client.Send(sendevt);

        memset(&recvevt, 0, sizeof(IPCEvent));
        gettimeofday(&time_start, NULL);
        while ((i_server.Read(recvevt) <= 0 || recvevt.EventType != IPC_COMMAND_DESTORY) &&
               (shm_addr && (shm_addr->written || !shm_addr->flag)))
        {
            usleep(10 * 1000);
            gettimeofday(&time_wait, NULL);
            if (time_wait.tv_sec - time_start.tv_sec > 2)
            {
                printf("myplayer progress destory failed!\n");
                break;
            }
        }
    }
    else
    {
        printf("my_player is not start!\n");
        fprintf(stderr, "Error：%s\n", strerror(errno));
    }

    if ((shm_addr && shm_addr->flag) || recvevt.EventType == IPC_COMMAND_DESTORY)
    {
        printf("myplayer progress destory done!\n");
    }

    if (shm_addr)
    {
        //把共享内存从当前进程中分离
        ret = shmdt((void *)shm_addr);
        if (ret < 0) {
            fprintf(stderr, "shmdt failed\n");
        }

        //删除共享内存
        ret = shmctl(shm_id, IPC_RMID, NULL);
        if(ret < 0) {
            fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        }
    }
    shm_addr = NULL;
    shm_id = 0;

    if (player_fd)
    {
        pclose(player_fd);
        player_fd = NULL;
    }

    i_server.Term();
    o_client.Term();

    char sysCmd[256];
    memset(sysCmd, 0, sizeof(sysCmd));
    snprintf(sysCmd, sizeof(sysCmd), "rm -rf %s", SVC_IPC);
    system(sysCmd);
    printf("remove server_input file\n");

    g_bPlaying = false;
    g_bPause = false;

    ResetSpeedMode();
    SetPlayingStatus(false);
    mTextview_speedPtr->setText("");
    mTextview_curtimePtr->setText("00:00:00");
    mSeekbar_progressPtr->setProgress(0);
    g_firstPlayPos = PLAY_INIT_POS;

    printf("End of StopPlayStreamFile\n");
}

static void TogglePlayStreamFile()
{
    if(!o_client.Init()) {
        printf("my_player is not start!\n");
        return;
    }

    memset(&sendevt, 0, sizeof(IPCEvent));
	if (!g_bPause) {
		sendevt.EventType = IPC_COMMAND_RESUME;
	} else {
		sendevt.EventType = IPC_COMMAND_PAUSE;
	}
    o_client.Send(sendevt);
}

static void StartDisplayImage(char *pFileName)
{
	ImagePlayerCtrl_t stPlayerCtrl;

	printf("Enter StartDisplayImage, fileName is %s\n", pFileName);
	stPlayerCtrl.fpGetDuration = GetImageFileDuration;
	stPlayerCtrl.fpGetCurrentPlayPos = GetImageFilePlayPos;
	stPlayerCtrl.fpDisplayComplete = PlayImageFileComplete;
	//StartPlayAudio();

	g_pstImagePlayer = ImagePlayer_Init(&stPlayerCtrl);
	if (!g_pstImagePlayer)
	{
		//StopPlayAudio();
		printf("ImagePlayer_Init exec failed\n");
		return;
	}

	int imgWidth = 0;
	int imgHeight = 0;
	int colorBits = 0;
	unsigned char *data = stbi_load(pFileName, &imgWidth, &imgHeight, &colorBits, 0);
	printf("img width=%d, height=%d, colorbits=%d\n", imgWidth, imgHeight, colorBits);
	stbi_image_free(data);

	LayoutPosition imgPosition = mTextView_picPtr->getPosition();
	imgPosition.mLeft = imgPosition.mLeft + (imgPosition.mWidth - imgWidth) / 2;
	imgPosition.mTop = imgPosition.mTop + (imgPosition.mHeight - imgHeight) / 2;
	imgPosition.mWidth = imgWidth;
	imgPosition.mHeight = imgHeight;
	mTextView_picPtr->setPosition(imgPosition);
	mTextView_picPtr->setBackgroundPic(pFileName);

	mTextView_picPtr->setVisible(true);
	printf("Leave StartDisplayImage\n");
}

static void StopDisplayImage()
{
	printf("Enter StopDisplayImage\n");
	ImagePlayer_Deinit(g_pstImagePlayer);
	//StopPlayAudio();

	mTextView_picPtr->setVisible(false);
	mTextview_curtimePtr->setText("00:00:00");
	mSeekbar_progressPtr->setProgress(0);

	printf("Leave StopDisplayImage\n");
}

static void TogglePlayImageFile()
{
	ImagePlayer_TogglePause(g_pstImagePlayer);
}

static void StopPlayFile()
{
	if (g_playStream)
		StopPlayStreamFile();
	else
		StopDisplayImage();
}

static void StartPlayFile(char *pFileName)
{
	g_bPlayCompleted = false;
	g_playStream = IsMediaStreamFile(pFileName);

	if (g_playStream)
		StartPlayStreamFile(pFileName);
	else
		StartDisplayImage(pFileName);

	g_bPlaying = TRUE;
	g_bPause   = FALSE;

	char filePath[256];
	char *p = NULL;
	memset(filePath, 0, sizeof(filePath));
	strcpy(filePath, pFileName);
	p = strrchr(filePath, '/');
	*p = 0;
	mTextview_fileNamePtr->setText(pFileName+strlen(filePath)+1);
	SetPlayingStatus(true);
	//AutoDisplayToolbar();
}

static void TogglePlayFile()
{
	if (g_bPlaying)
	{
		g_bPause = !g_bPause;
		SetPlayingStatus(!g_bPause);

		if (g_playStream)
			TogglePlayStreamFile();
		else
			TogglePlayImageFile();
	}
}

static void PlayNextFile()
{
	pthread_mutex_lock(&g_playFileMutex);
	g_eSkipMode = SKIP_NEXT;
	pthread_mutex_unlock(&g_playFileMutex);
}

static void PlayPrevFile()
{
	pthread_mutex_lock(&g_playFileMutex);
	g_eSkipMode = SKIP_PREV;
	pthread_mutex_unlock(&g_playFileMutex);
}

RepeatMode_e operator++(RepeatMode_e& cmd)
{
	RepeatMode_e t = cmd;
	cmd = static_cast<RepeatMode_e>(cmd+1);
	return t;
}

static void PollRepeatMode()
{
	RepeatMode_e eRepeatMode = FILE_REPEAT_MODE;
	pthread_mutex_lock(&g_playFileMutex);
	if (g_eRepeatMode < LIST_REPEAT_MODE)
		g_eRepeatMode++;
	else
		g_eRepeatMode = FILE_REPEAT_MODE;

	eRepeatMode = g_eRepeatMode;
	pthread_mutex_unlock(&g_playFileMutex);

	switch (eRepeatMode)
	{
	case FILE_REPEAT_MODE:
		mButton_circlemodePtr->setBackgroundPic("player/singlerepeat.png");
		break;
	case LIST_REPEAT_MODE:
		mButton_circlemodePtr->setBackgroundPic("player/listrepeat.png");
		break;
	default:
		printf("invalid repeat mode, mode=%d\n", (int)eRepeatMode);
		break;
	}
}

static void *PlayFileProc(void *pData)
{
	char *pFileName = (char*)pData;
	char curFileName[256] = {0};
	SkipMode_e eSkipMode = NO_SKIP;
	bool bPlayCompleted = false;
	bool bPlayError = false;
    struct timeval pant_start, pant_wait;
	printf("get in PlayFileProc!\n");
	strncpy(curFileName, pFileName, sizeof(curFileName));
	RepeatMode_e eRepeatMode = LIST_REPEAT_MODE;
	StartPlayFile(curFileName);
	AutoDisplayToolbar();

	if (!g_playStream)
		SetImageFileMaxProgres(IMAGE_DISPLAY_DURATION);

    gettimeofday(&pant_start, NULL);

	while (!g_bPlayFileThreadExit)
	{
		pthread_mutex_lock(&g_playFileMutex);
		eRepeatMode = g_eRepeatMode;
		if (eSkipMode != g_eSkipMode)
		{
			eSkipMode = g_eSkipMode;
		}
		g_eSkipMode = NO_SKIP;
		if (bPlayError != g_bPlayError)
		{
			bPlayError = g_bPlayError;
			g_bPlayError = false;
		}
		if (bPlayCompleted != g_bPlayCompleted)
		{
			bPlayCompleted = g_bPlayCompleted;
			g_bPlayCompleted = false;
		}
		pthread_mutex_unlock(&g_playFileMutex);

		if (bPlayError)
		{
			printf("occur error when playing file\n");
			break;
		}

		if (bPlayCompleted)
		{
			if (eRepeatMode == LIST_REPEAT_MODE)
			{
				char nextFileName[256] = {0};
				GetNextFile(curFileName, nextFileName, sizeof(nextFileName));
				memset(curFileName, 0, sizeof(curFileName));
				strncpy(curFileName, nextFileName, sizeof(curFileName));
			}

			StopPlayFile();
			StartPlayFile(curFileName);
		}
		else
		{
			if (eSkipMode == SKIP_NEXT)
			{
				char nextFileName[256] = {0};
				GetNextFile(curFileName, nextFileName, sizeof(nextFileName));
				if (strcmp(curFileName, nextFileName))	// if only one file, not change
				{
					memset(curFileName, 0, sizeof(curFileName));
					strncpy(curFileName, nextFileName, sizeof(curFileName));
					StopPlayFile();
					StartPlayFile(curFileName);
				}
			}
			else if (eSkipMode == SKIP_PREV)
			{
				char prevFileName[256] = {0};
				GetPrevFile(curFileName, prevFileName, sizeof(prevFileName));
				if (strcmp(curFileName, prevFileName))	// if only one file, not change
				{
					memset(curFileName, 0, sizeof(curFileName));
					strncpy(curFileName, prevFileName, sizeof(curFileName));
					StopPlayFile();
					StartPlayFile(curFileName);
				}
			}
		}

        if (g_playStream) {
            memset(&recvevt, 0, sizeof(IPCEvent));
            if (i_server.Read(recvevt) > 0) {
                switch (recvevt.EventType)
                {
                    case IPC_COMMAND_GET_DURATION : {
                        char totalTime[32];
                        long int durationSec = recvevt.stPlData.misc / 1.0;

                        if (durationSec / 3600 < 99) {
                            memset(totalTime, 0, sizeof(totalTime));
                            sprintf(totalTime, "%02d:%02d:%02d", durationSec/3600, (durationSec%3600)/60, durationSec%60);
                            mTextview_durationPtr->setText(totalTime);
                            g_duration = durationSec;
                            printf("file duration time = %lld\n", g_duration);
                        }
                    }
                    break;

                    case IPC_COMMAND_GET_POSITION : {
                        char curTime[32];
                        int curSec = recvevt.stPlData.misc / 1.0;
                        int trackPos;
                        //printf("get video current position time = %d\n", curSec);
                        memset(curTime, 0, sizeof(curTime));
                        sprintf(curTime, "%02d:%02d:%02d", curSec/3600, (curSec%3600)/60, curSec%60);
                        mTextview_curtimePtr->setText(curTime);

                        trackPos  = (curSec * mSeekbar_progressPtr->getMax()) / g_duration;
                        mSeekbar_progressPtr->setProgress(trackPos);
                    }
                    break;

                    case IPC_COMMAND_ERROR : {
                        if (recvevt.stPlData.status == -101)
                            mTextview_msgPtr->setText("请检查网络连接！");
                        else if (recvevt.stPlData.status == -2)
                            mTextview_msgPtr->setText("不支持播放720P以上的视频！");
                        else if (recvevt.stPlData.status == -3)
                            mTextview_msgPtr->setText("解码速度不够，请降低视频帧率！");
                        else if (recvevt.stPlData.status == -4)
                            mTextview_msgPtr->setText("读取网络超时！");
                        else
                            mTextview_msgPtr->setText("Other Error Occur!");

                        mWindow_errMsgPtr->setVisible(true);

                        pthread_mutex_lock(&g_playFileMutex);
                        g_bPlayError = true;
                        pthread_mutex_unlock(&g_playFileMutex);
                        printf("[%s] play error!\n", curFileName);
                    }
                    break;

                    case IPC_COMMAND_COMPLETE : {
                        SetPlayingStatus(false);
                        mTextview_speedPtr->setText("");
                        g_bShowPlayToolBar = FALSE;
                        g_bPantStatus = false;

                        pthread_mutex_lock(&g_playFileMutex);
                        g_bPlayCompleted = true;
                        pthread_mutex_unlock(&g_playFileMutex);
                        printf("[%s] play complete!\n", curFileName);
                    }
                    break;

                    case IPC_COMMAND_PANT : {
                        g_bPantStatus = true;
                        gettimeofday(&pant_start, NULL);
                        if(!o_client.Init()) {
                            printf("[%s %d]my_player process not start!\n", __FILE__, __LINE__);
                            fprintf(stderr, "Error：%s\n", strerror(errno));
                        } else {
                            memset(&sendevt, 0, sizeof(IPCEvent));
                            sendevt.EventType = IPC_COMMAND_PANT;
                            o_client.Send(sendevt);
                        }
                    }
                    break;

                    default : break;
                }
            }

            //心跳包判断
            gettimeofday(&pant_wait, NULL);
            if (pant_wait.tv_sec - pant_start.tv_sec > 2 * PANT_TIME && g_bPantStatus) {
                mTextview_msgPtr->setText("Other Error Occur!");
                mWindow_errMsgPtr->setVisible(true);
                pthread_mutex_lock(&g_playFileMutex);
                g_bPlayError = true;
                pthread_mutex_unlock(&g_playFileMutex);
                printf("myplayer has exit abnormallity!\n");
            }
        }
        else
        {
        	if (g_bPlayCompleted)
        		ResetPlayImageFileProgres();
        	else
        		SetImageFileCurrentProgress(g_currentPos);
        }

        usleep(100 * 1000);
    }

    StopPlayFile();
    g_fileName = curFileName;
    g_bPantStatus = false;
    printf("### PlayFileProc Exit ###\n");
    return NULL;
}


/**
 * 注册定时器
 * 填充数组用于注册定时器
 * 注意：id不能重复
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	//{0,  6000}, //定时器id=0, 时间间隔6秒
	//{1,  1000},
};

/**
 * 当界面构造时触发
 */
static void onUI_init(){
    //Tips :添加 UI初始化的显示代码到这里,如:mText1Ptr->setText("123");
    printf("create player dev\n");

    // init play view real size
    LayoutPosition layoutPos = mVideoview_videoPtr->getPosition();
    g_playViewWidth = layoutPos.mWidth * PANEL_WIDTH / UI_MAX_WIDTH;	// video width
    g_playViewHeight = ALIGN_DOWN(layoutPos.mHeight * PANEL_HEIGHT / UI_MAX_HEIGHT, 2);		// video height
    printf("play view size: w=%d, h=%d\n", g_playViewWidth, g_playViewHeight);

    SSTAR_RegisterUsbListener(DetectUsbHotplug);
    // init pts
    g_firstPlayPos = PLAY_INIT_POS;

    // divp use window max width & height default, when play file, the inputAttr of divp will be set refer to file size.
    //CreatePlayerDev();

    pthread_mutex_init(&g_playFileMutex, NULL);		// playFile mutex init
}

/**
 * 当切换到该界面时触发
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
    g_fileName = intentPtr->getExtra("filepath");

    g_bPlayFileThreadExit = false;
    pthread_create(&g_playFileThread, NULL, PlayFileProc, (void*)g_fileName.c_str());
    }
}

/*
 * 当界面显示时触发
 */
static void onUI_show() {

}

/*
 * 当界面隐藏时触发
 */
static void onUI_hide() {

}

/*
 * 当界面完全退出时触发
 */
static void onUI_quit() {
	printf("destroy player dev\n");
    pthread_mutex_destroy(&g_playFileMutex);
    //DestroyPlayerDev();
    g_firstPlayPos = PLAY_INIT_POS;

    printf("start to UnRegisterUsbListener\n");
    SSTAR_UnRegisterUsbListener(DetectUsbHotplug);
    printf("end of UnRegisterUsbListener\n");
}

/**
 * 串口数据回调接口
 */
static void onProtocolDataUpdate(const SProtocolData &data) {

}

/**
 * 定时器触发函数
 * 不建议在此函数中写耗时操作，否则将影响UI刷新
 * 参数： id
 *         当前所触发定时器的id，与注册时的id相同
 * 返回值: true
 *             继续运行当前定时器
 *         false
 *             停止运行当前定时器
 */
static bool onUI_Timer(int id){
	switch (id) {

		default:
			break;
	}
    return true;
}

/**
 * 有新的触摸事件时触发
 * 参数：ev
 *         新的触摸事件
 * 返回值：true
 *            表示该触摸事件在此被拦截，系统不再将此触摸事件传递到控件上
 *         false
 *            触摸事件将继续传递到控件上
 */
static bool onplayerActivityTouchEvent(const MotionEvent &ev) {
	static SZKPoint touchDown;
	static SZKPoint touchMove;
	static SZKPoint lastMove;
	static bool bValidMove = false;	// on the first move, delt y should be larger than delt x, or update touchDown point

    switch (ev.mActionStatus) {
		case MotionEvent::E_ACTION_DOWN://触摸按下
			//LOGD("时刻 = %ld 坐标  x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
			//printf("down: time=%ld, x=%d, y=%d\n", ev.mEventTime, ev.mX, ev.mY);
			touchDown.x = ev.mX;
			touchDown.y = ev.mY;
			bValidMove = false;

			// judge if the model window is visible
			if (mWindow_errMsgPtr->isVisible() && !mWindow_errMsgPtr->getPosition().isHit(ev.mX, ev.mY))
			{
				return true;
			}

			// show play bar when touch down
			AutoDisplayToolbar();

			break;
		case MotionEvent::E_ACTION_MOVE://触摸滑动
			//printf("move: time=%ld, x=%d, y=%d\n", ev.mEventTime, ev.mX, ev.mY);
			touchMove.x = ev.mX;
			touchMove.y = ev.mY;

			if (!bValidMove)
			{
				if (touchMove.y == touchDown.y)
				{
					touchDown = touchMove;
				}
				else if (touchMove.x == touchDown.x)
				{
					bValidMove = true;
					AdjustVolumeByTouch((int)touchDown.y, (int)touchMove.y);
					lastMove = touchMove;
				}
				else if ((touchMove.y-touchDown.y) > 0 && (touchMove.x-touchDown.x) > 0
						&& (touchMove.y-touchDown.y) >= (touchMove.x-touchDown.x))
				{
					bValidMove = true;
					AdjustVolumeByTouch((int)touchDown.y, (int)touchMove.y);
					lastMove = touchMove;
				}
				else if ((touchMove.y-touchDown.y) < 0 && (touchMove.x-touchDown.x) < 0
						&& (touchMove.y-touchDown.y) <= (touchMove.x-touchDown.x))
				{
					bValidMove = true;
					AdjustVolumeByTouch((int)touchDown.y, (int)touchMove.y);
					lastMove = touchMove;
				}
				else if ((touchMove.y-touchDown.y) > 0 && (touchMove.x-touchDown.x) < 0
						&& (touchMove.y-touchDown.y) >= (touchDown.x-touchMove.x))
				{
					bValidMove = true;
					AdjustVolumeByTouch((int)touchDown.y, (int)touchMove.y);
					lastMove = touchMove;
				}
				else if ((touchMove.y-touchDown.y) < 0 && (touchMove.x-touchDown.x) > 0
						&& (touchDown.y-touchMove.y) >= (touchMove.x-touchDown.x))
				{
					bValidMove = true;
					AdjustVolumeByTouch((int)touchDown.y, (int)touchMove.y);
					lastMove = touchMove;
				}
				else
				{
					touchDown = touchMove;
				}
			}
			else
			{
				//printf("lastY:%d, curY:%d\n", lastMove.y, touchMove.y);
				AdjustVolumeByTouch(lastMove.y, touchMove.y);
				lastMove = touchMove;
			}

			AutoDisplayToolbar();
			break;
		case MotionEvent::E_ACTION_UP:  //触摸抬起
			//printf("up: time=%ld, x=%d, y=%d\n", ev.mEventTime, ev.mX, ev.mY);
			break;
		default:
			break;
	}

	return false;
}
static void onProgressChanged_Seekbar_progress(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged Seekbar_progress %d !!!\n", progress);
}

static void onStartTrackingTouch_Seekbar_progress(ZKSeekBar *pSeekBar) {
    //LOGD(" ProgressChanged Seekbar_progress %d !!!\n", progress);

	printf("onStartTrackingTouch_Seekbar_progress\n");
	if (g_playStream)
	{
		//if (!g_bPause)
		//    toggle_pause(g_pstPlayStat);
	}
	else
	{
		if (!g_bPause)
			ImagePlayer_TogglePause(g_pstImagePlayer);
	}
}

static void onStopTrackingTouch_Seekbar_progress(ZKSeekBar *pSeekBar) {
    //LOGD(" ProgressChanged Seekbar_progress %d !!!\n", progress);
	printf("onStopTrackingTouch_Seekbar_progress\n");

	int progress = pSeekBar->getProgress();
	long long curPos = progress * g_duration / mSeekbar_progressPtr->getMax();
	printf("progress value is %d, max value is %d, duration is %lld, curPos is %lld\n", progress, mSeekbar_progressPtr->getMax(),
			g_duration, curPos);

	if (g_playStream)
	{
	    if(!o_client.Init()) {
	        printf("my_player is not start!\n");
	        return;
	    }
		memset(&sendevt, 0, sizeof(IPCEvent));
		sendevt.EventType = IPC_COMMAND_SEEK2TIME;
		sendevt.stPlData.misc = (double)curPos;
		printf("send seek gap time to myplayer = %.3f!\n", sendevt.stPlData.misc);
		o_client.Send(sendevt);
	}
	else
	{
		printf("UI: image seek pos is %lld\n", curPos);
		ImagePlayer_Seek(g_pstImagePlayer, curPos);
		if (!g_bPause)
			ImagePlayer_TogglePause(g_pstImagePlayer);
	}
}

static bool onButtonClick_Button_play(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_play !!!\n");
	TogglePlayFile();
    return false;
}

static bool onButtonClick_Button_stop(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_stop !!!\n");
	if (g_hideToolbarThread.isRunning())
	{
		printf("stop hideToolBarthread\n");
		g_hideToolbarThread.requestExitAndWait();
	}

	g_bPlayFileThreadExit = true;
	if (g_playFileThread)
	{
		pthread_join(g_playFileThread, NULL);
		g_playFileThread = NULL;
	}

	EASYUICONTEXT->goBack();
    return false;
}

static bool onButtonClick_Button_slow(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_slow !!!\n");
	char speedMode[16] = {0};

	if (!g_playStream)
		return false;

	if (g_bPlaying)
	{
		if (g_ePlayDirection == E_PLAY_FORWARD)
		{
			// slow down
			if (g_ePlayMode == E_PLAY_FAST_MODE)
			{
				g_eSpeedMode = (PlaySpeedMode_e)((int)g_eSpeedMode - 1);
				g_u32SpeedNumerator = 1 << (int)g_eSpeedMode;
				g_u32SpeedDenomonator = 1;

				if (g_eSpeedMode == E_NORMAL_SPEED)
					g_ePlayMode = E_PLAY_NORMAL_MODE;
			}
			else
			{
				if (g_eSpeedMode < E_32X_SPEED)
				{
					g_ePlayMode = E_PLAY_SLOW_MODE;
					g_eSpeedMode = (PlaySpeedMode_e)((int)g_eSpeedMode + 1);
				}
				else    // turn to play backward
				{
					g_ePlayDirection = E_PLAY_BACKWARD;
					g_ePlayMode = E_PLAY_NORMAL_MODE;
					g_eSpeedMode = E_NORMAL_SPEED;
				}

				g_u32SpeedNumerator = 1;
				g_u32SpeedDenomonator = 1 << (int)g_eSpeedMode;
			}
		}
		else
		{
			// speed up
			if (g_ePlayMode == E_PLAY_SLOW_MODE)
			{
				g_eSpeedMode = (PlaySpeedMode_e)((int)g_eSpeedMode - 1);
				g_u32SpeedNumerator = 1;
				g_u32SpeedDenomonator = 1 << (int)g_eSpeedMode;

				if (g_eSpeedMode == E_NORMAL_SPEED)
				{
					g_ePlayMode = E_PLAY_NORMAL_MODE;
				}
			}
			else
			{
				if (g_eSpeedMode < E_32X_SPEED)
				{
					g_ePlayMode = E_PLAY_FAST_MODE;
					g_eSpeedMode = (PlaySpeedMode_e)((int)g_eSpeedMode + 1);
					g_u32SpeedNumerator = 1 << (int)g_eSpeedMode;
					g_u32SpeedDenomonator = 1;
				}
			}
		}

		memset(speedMode, 0, sizeof(speedMode));
		if (g_u32SpeedNumerator == g_u32SpeedDenomonator)
			//sprintf(speedMode, "", g_u32SpeedNumerator);
			memset(speedMode, 0, sizeof(speedMode));
		else if (g_u32SpeedNumerator > g_u32SpeedDenomonator)
			sprintf(speedMode, "%s %dX", ((g_ePlayDirection == E_PLAY_FORWARD) ? ">>" : "<<"), g_u32SpeedNumerator);
		else
			sprintf(speedMode, "%s 1/%dX", ((g_ePlayDirection == E_PLAY_FORWARD) ? ">>" : "<<"), g_u32SpeedDenomonator);

		mTextview_speedPtr->setText(speedMode);

		// sendmessage to adjust speed
	}
    return false;
}

static bool onButtonClick_Button_fast(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_fast !!!\n");
	char speedMode[16] = {0};

	if (!g_playStream)
		return false;

	if (g_bPlaying)
	{
		if (g_ePlayDirection == E_PLAY_FORWARD)
		{
			// speed up
			if (g_ePlayMode == E_PLAY_SLOW_MODE)
			{
				g_eSpeedMode = (PlaySpeedMode_e)((int)g_eSpeedMode - 1);
				g_u32SpeedNumerator = 1;
				g_u32SpeedDenomonator = 1 << (int)g_eSpeedMode;

				if (g_eSpeedMode == E_NORMAL_SPEED)
					g_ePlayMode = E_PLAY_NORMAL_MODE;
			}
			else
			{
				if (g_eSpeedMode < E_32X_SPEED)
				{
					g_ePlayMode = E_PLAY_FAST_MODE;
					g_eSpeedMode = (PlaySpeedMode_e)((int)g_eSpeedMode + 1);
					g_u32SpeedNumerator = 1 << (int)g_eSpeedMode;
					g_u32SpeedDenomonator = 1;
				}
			}
		}
		else
		{
			// slow down
			if (g_ePlayMode == E_PLAY_FAST_MODE)
			{
				g_eSpeedMode = (PlaySpeedMode_e)((int)g_eSpeedMode - 1);
				g_u32SpeedNumerator = 1 << (int)g_eSpeedMode;
				g_u32SpeedDenomonator = 1;

				if (g_eSpeedMode == E_NORMAL_SPEED)
					g_ePlayMode = E_PLAY_NORMAL_MODE;
			}
			else
			{
				// 1/32X speed backward to normal speed forward
				if (g_eSpeedMode == E_32X_SPEED)
				{
					g_eSpeedMode = E_NORMAL_SPEED;
					g_ePlayMode = E_PLAY_NORMAL_MODE;
					g_ePlayDirection = E_PLAY_FORWARD;
				}
				else
				{
					g_ePlayMode = E_PLAY_SLOW_MODE;
					g_eSpeedMode = (PlaySpeedMode_e)((int)g_eSpeedMode + 1);
				}

				g_u32SpeedNumerator = 1;
				g_u32SpeedDenomonator = 1 << (int)g_eSpeedMode;
			}
		}

		memset(speedMode, 0, sizeof(speedMode));
		if (g_u32SpeedNumerator == g_u32SpeedDenomonator)
			memset(speedMode, 0, sizeof(speedMode));
		else if (g_u32SpeedNumerator > g_u32SpeedDenomonator)
			sprintf(speedMode, "%dX %s", g_u32SpeedNumerator, ((g_ePlayDirection == E_PLAY_FORWARD) ? ">>" : "<<"));
		else
			sprintf(speedMode, "1/%dX %s", g_u32SpeedDenomonator, ((g_ePlayDirection == E_PLAY_FORWARD) ? ">>" : "<<"));

		mTextview_speedPtr->setText(speedMode);

		// sendmessage to adjust speed
	}

    return false;
}
static bool onButtonClick_Button_voice(ZKButton *pButton) {
    //LOGD(" ButtonClick Button_voice !!!\n");
    if(!o_client.Init()) {
        printf("my_player is not start!\n");
        return;
    }
	g_bMute = !g_bMute;
	memset(&sendevt, 0, sizeof(IPCEvent));
	sendevt.EventType = IPC_COMMAND_SET_MUTE;
	sendevt.stPlData.mute = g_bMute;
	o_client.Send(sendevt);
	SetMuteStatus(g_bMute);
	printf("set mute to %d\n", g_bMute);

    return false;
}

static void onProgressChanged_Seekbar_volumn(ZKSeekBar *pSeekBar, int progress) {
    //LOGD(" ProgressChanged Seekbar_volumn %d !!!\n", progress);
    if(!o_client.Init()) {
        printf("my_player is not start!\n");
        return;
    }
	memset(&sendevt, 0, sizeof(IPCEvent));
	sendevt.EventType = IPC_COMMAND_SET_VOLUMN;
	sendevt.stPlData.volumn = GetPlayerVolumn();
	printf("set voice volumn = [%d]\n", sendevt.stPlData.volumn);
	o_client.Send(sendevt);

	SetMuteStatus(g_bMute);
}
static bool onButtonClick_Button_confirm(ZKButton *pButton) {
    LOGD(" ButtonClick Button_confirm !!!\n");
	mWindow_errMsgPtr->setVisible(false);

	if (g_hideToolbarThread.isRunning())
	{
		printf("stop hideToolBarthread\n");
		g_hideToolbarThread.requestExitAndWait();
	}

	g_bPlayFileThreadExit = true;
	if (g_playFileThread)
	{
		pthread_join(g_playFileThread, NULL);
		g_playFileThread = NULL;
	}

	EASYUICONTEXT->goBack();
    return false;
}
static bool onButtonClick_Button_prev(ZKButton *pButton) {
    LOGD(" ButtonClick Button_prev !!!\n");
    PlayPrevFile();
    return false;
}

static bool onButtonClick_Button_next(ZKButton *pButton) {
    LOGD(" ButtonClick Button_next !!!\n");
    PlayNextFile();
    return false;
}

static bool onButtonClick_Button_circlemode(ZKButton *pButton) {
    LOGD(" ButtonClick Button_circlemode !!!\n");
    PollRepeatMode();
    return false;
}

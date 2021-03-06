#include "media/ZKMediaPlayer.h"
#include "statusbarconfig.h"

static ZKMediaPlayer *sAudioPlayerPtr = NULL;
static std::vector<string> sMediaFileList;
static int sPlayIndex;


static bool sIsTracking = false;
static int sTrackProgress = -1;

extern "C" void SSTAR_PlayAudio();
extern "C" void SSTAR_StopAudio();
extern "C" void SSTAR_SetVolume(int vol);

class SeekbarChangeListener : public ZKSeekBar::ISeekBarChangeListener {
public:
    virtual void onProgressChanged(ZKSeekBar *pSeekBar, int progress) {
        sTrackProgress = progress;
    }
    virtual void onStartTrackingTouch(ZKSeekBar *pSeekBar) {
        sIsTracking = true;
    }
    virtual void onStopTrackingTouch(ZKSeekBar *pSeekBar) {
        sIsTracking = false;
        if (sTrackProgress >= 0) {
            if (sAudioPlayerPtr) {
                sAudioPlayerPtr->seekTo(sTrackProgress * 1000);
            } else {
                mVideoviewTTPtr->seekTo(sTrackProgress * 1000);
            }
            sTrackProgress = -1;
        }
    }
};
static SeekbarChangeListener progressbar;

static int removeCharFromString(string& nString, char c) {
    string::size_type pos;
    while (1) {
        pos = nString.find(c);
        if (pos != string::npos) {
            nString.erase(pos, 1);
        } else {
            break;
        }
    }
    return (int) nString.size();
}

static bool parseVideoFileList(const char *pFileListPath, std::vector<string>& mediaFileList) {
    mediaFileList.clear();
    if (NULL == pFileListPath || 0 == strlen(pFileListPath)) {
        LOGD("video file list is null!");
        return false;
    }

    ifstream is(pFileListPath, ios_base::in);
    if (!is.is_open()) {
        LOGD("cann't open file %s \n", pFileListPath);
        return false;
    }
    char tmp[1024] = {0};
    while (is.getline(tmp, sizeof(tmp))) {
        string str = tmp;
        removeCharFromString(str, '\"');
        removeCharFromString(str, '\r');
        removeCharFromString(str, '\n');
        if (str.size() > 1) {
            mediaFileList.push_back(str.c_str());
        }
    }
    LOGD("(f:%s, l:%d) parse fileList[%s], get [%d]files\n", __FUNCTION__,
            __LINE__, pFileListPath, mediaFileList.size());
    for (size_t i = 0; i < mediaFileList.size(); i++) {
        LOGD("file[%d]:[%s]\n", i, mediaFileList[i].c_str());
    }
    is.close();

    return true;
}

static void next() {
    if (!sMediaFileList.empty()) {
        LOGD("! empty");
        sPlayIndex = (sPlayIndex + 1) % sMediaFileList.size();
        if (sAudioPlayerPtr) {
            sAudioPlayerPtr->play(sMediaFileList[sPlayIndex].c_str());
        } else {
            mVideoviewTTPtr->play(sMediaFileList[sPlayIndex].c_str());
        }
    }
}

static void prev() {
    if (!sMediaFileList.empty()) {
        sPlayIndex = (sPlayIndex - 1 + sMediaFileList.size()) % sMediaFileList.size();
        if (sAudioPlayerPtr) {
            sAudioPlayerPtr->play(sMediaFileList[sPlayIndex].c_str());
        } else {
            mVideoviewTTPtr->play(sMediaFileList[sPlayIndex].c_str());
        }
    }
}

/**
 * ???????????????
 * ?????????????????????????????????
 * ?????????id????????????
 */
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
    //{0,  6000}, //?????????id=0, ????????????6???
    { 1, 1000 },
};

/**
 * ????????????????????????
 */
static void onUI_init() {
    //Tips :?????? UI?????????????????????????????????,???:mText1Ptr->setText("123");

    SSTAR_PlayAudio();
}

/**
 * ??????????????????????????????
 */
static void onUI_intent(const Intent *intentPtr) {
    mSoundSeekbarPtr->setProgress(55);
}

/*
 * ????????????????????????
 */
static void onUI_show() {

}

/*
 * ????????????????????????
 */
static void onUI_hide() {
}

/*
 * ??????????????????????????????
 */
static void onUI_quit() {
    system("echo 3 > /proc/sys/vm/drop_caches");

    SSTAR_StopAudio();
    ShowStatusBar(1, 0, 0);
}

/**
 * ????????????????????????
 */
static void onProtocolDataUpdate(const SProtocolData &data) {

}

/**
 * ?????????????????????
 * ?????????????????????????????????????????????????????????UI??????
 * ????????? id
 *         ???????????????????????????id??????????????????id??????
 * ?????????: true
 *             ???????????????????????????
 *         false
 *             ???????????????????????????
 */
static bool onUI_Timer(int id){
	switch (id) {

		default:
			break;
	}
    return true;
}

/**
 * ??????????????????????????????
 * ?????????ev
 *         ??????????????????
 * ????????????true
 *            ???????????????????????????????????????????????????????????????????????????????????????
 *         false
 *            ???????????????????????????????????????
 */
static bool onplayPcmFileActivityTouchEvent(const MotionEvent &ev) {
    return false;
}

static void onProgressChanged_SoundSeekbar(ZKSeekBar *pSeekBar, int progress) {
	SSTAR_SetVolume(progress);
}

static bool onButtonClick_PlayButton(ZKButton *pButton) {
    return false;
}

static bool onButtonClick_PrevButton(ZKButton *pButton) {
    prev();
    return false;
}

static bool onButtonClick_NextButton(ZKButton *pButton) {
    next();
    return false;
}

static void onProgressChanged_PlayProgressSeekbar(ZKSeekBar *pSeekBar, int progress) {

}

static bool onButtonClick_VoiceButton(ZKButton *pButton) {
    //LOGD(" ButtonClick VoiceButton !!!\n");
    mSoundWindowPtr->showWnd();
    return false;
}

static bool onButtonClick_sys_back(ZKButton *pButton) {
    return false;
}

static void onVideoViewPlayerMessageListener_VideoviewTT(ZKVideoView *pVideoView, int msg) {
	switch (msg) {
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_STARTED:
	    {
            int max = pVideoView->getDuration() / 1000;
            char timeStr[12] = {0};
            snprintf(timeStr, sizeof(timeStr), "%02d:%02d", max / 60, max % 60);
            mDurationTextViewPtr->setText(timeStr);
            mPlayProgressSeekbarPtr->setMax(max);
            mPlayButtonPtr->setSelected(true);
	    }
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_COMPLETED:
	    next();
		break;
	case ZKVideoView::E_MSGTYPE_VIDEO_PLAY_ERROR:
		break;
	}
}

static bool onButtonClick_ButtonZoom(ZKButton *pButton) {
    return false;
}

static bool onButtonClick_Button4(ZKButton *pButton) {
    EASYUICONTEXT->openActivity("helpVideoActivity");
    return false;
}

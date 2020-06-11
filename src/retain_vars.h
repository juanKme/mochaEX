#ifndef _RETAIN_VARS_H_
#define _RETAIN_VARS_H_

//for pre-compiling the app
#define LITE     1    // 1 = lite version (no tcpgecko nor cafiine)


#include "common/types.h"
#include "utils/voice_info.hpp"

extern struct fs_patcher_utils fspatchervars __attribute__((section(".data")));

extern int isSplatoon;          //delete?
extern int drcMode;
extern uint32_t gAppStatus;
extern int gCoolDown;
extern int gLCDDelay;
extern s32 gLCDMode;

// AX
extern VoiceInfo gVoiceInfos[VOICE_INFO_MAX];

#endif // _RETAIN_VARS_H_

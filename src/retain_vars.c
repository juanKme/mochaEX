#include "common/types.h"
#include "retain_vars.h"

int isSplatoon __attribute__((section(".data"))) = 0;
int drcMode __attribute__((section(".data"))) = 0;
uint32_t gAppStatus __attribute__((section(".data"))) = 0;
int gCoolDown __attribute__((section(".data"))) = 0;
int gLCDDelay __attribute__((section(".data"))) = 0;
s32 gLCDMode __attribute__((section(".data"))) = 0;

VoiceInfo gVoiceInfos[VOICE_INFO_MAX] __attribute__((section(".data")));
struct fs_patcher_utils fspatchervars __attribute__((section(".data")));


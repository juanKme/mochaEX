#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/fs_functions.h"
#include "dynamic_libs/gx2_functions.h"
#include "dynamic_libs/ax_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "dynamic_libs/sys_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "dynamic_libs/proc_ui_functions.h"
#include "fs/fs_utils.h"
#include "fs/sd_fat_devoptab.h"
#include "system/memory.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include "common/common.h"
#include "patcher/voice_swapping_function_patcher.h"
#include "patcher/video_swapping_function_patcher.h"
#include "patcher/vpad_function_patcher.h"
#include "patcher/proc_ui_function_patcher.h"
#include "kernel/kernel_functions.h"
#include "retain_vars.h"
#include "menu.h"
#include "main.h"
#include "ios_exploit.h"

#define PRINT_TEXT1(x, y, str) { OSScreenPutFontEx(1, x, y, str); }
#define PRINT_TEXT2(x, y, ...) { snprintf(msg, 80, __VA_ARGS__); OSScreenPutFontEx(0, x, y, msg); OSScreenPutFontEx(1, x, y, msg); }
#define PRINT_TEXT3(x, y, _fmt, ...) { __os_snprintf(msg, 80, _fmt, __VA_ARGS__); OSScreenPutFontEx(1, x, y, msg); }

#define BUILD "1.5.0"

u8 isFirstBoot __attribute__((section(".data"))) = 1;

/* IP union */
typedef union u_serv_ip
{
    uint8_t  digit[4];
    uint32_t full;
} u_serv_ip;

static int exitToHBLOnLaunch = 0;

int run_SwapDRC()
{
    log_init("192.168.2.18");
    SetupKernelCallback();
    log_printf("SWAP DRC started\n");
    memset(gVoiceInfos, 0, sizeof(gVoiceInfos));
    memset(&fspatchervars,0,sizeof(fspatchervars));

        //Reset everything when were going back to the Mii Maker
    if(!isFirstBoot && isInMiiMakerHBL()){
        log_printf("Returing to the Homebrew Launcher!\n");
        isFirstBoot = 0;
        deInit();
        return EXIT_SUCCESS;
    }

    ApplyPatches();

    if(isFirstBoot){ // First boot back to SysMenu
        memoryInitialize();
        VPADInit();

            // Init screen and screen buffers
        OSScreenInit();
        int screen_buf0_size = OSScreenGetBufferSizeEx(0);
        int screen_buf1_size = OSScreenGetBufferSizeEx(1);
        unsigned char *screenBuffer = MEM1_alloc(screen_buf0_size + screen_buf1_size, 0x100);
        char msg[80];
        OSScreenSetBufferEx(0, screenBuffer);
        OSScreenSetBufferEx(1, (screenBuffer + screen_buf0_size));
        OSScreenEnableEx(0, 1);
        OSScreenEnableEx(1, 1);

#if LITE
        OSScreenClearBufferEx(0, 0);
        OSScreenClearBufferEx(1, 0);
        PRINT_TEXT1(0, 1, "MochaEX has now the SwapDRC function activated ...");
        PRINT_TEXT2(0, 3, "Tip: Swap screens with the TV button.");
        OSScreenFlipBuffersEx(0);
        OSScreenFlipBuffersEx(1);
        os_sleep(2);
        MEM1_free(screenBuffer);
        screenBuffer = NULL;
        memoryRelease();

        isFirstBoot = 0;
#endif
    }
    return EXIT_SUCCESS;
}

void ApplyPatches(){
    PatchInvidualMethodHooks(method_hooks_voice_swapping,   method_hooks_size_voice_swapping,   method_calls_voice_swapping);
    PatchInvidualMethodHooks(method_hooks_video_swapping,   method_hooks_size_video_swapping,   method_calls_video_swapping);
    PatchInvidualMethodHooks(method_hooks_vpad,             method_hooks_size_vpad,             method_calls_vpad);
    PatchInvidualMethodHooks(method_hooks_proc_ui,          method_hooks_size_proc_ui,          method_calls_proc_ui);
}

void RestorePatches(){
    RestoreInvidualInstructions(method_hooks_voice_swapping,    method_hooks_size_voice_swapping);
    RestoreInvidualInstructions(method_hooks_video_swapping,    method_hooks_size_video_swapping);
    RestoreInvidualInstructions(method_hooks_vpad,              method_hooks_size_vpad);
    RestoreInvidualInstructions(method_hooks_proc_ui,           method_hooks_size_proc_ui);
    KernelRestoreInstructions();
}

void deInit(){
    RestorePatches();
    log_deinit();
}

int isInMiiMakerHBL(){
    if (OSGetTitleID != 0 && (
                              OSGetTitleID() == 0x000500101004A200 || // mii maker eur
                              OSGetTitleID() == 0x000500101004A100 || // mii maker usa
                              OSGetTitleID() == 0x000500101004A000 || // mii maker jpn
                              OSGetTitleID() == 0x0005000013374842))
        {
        return 1;
        }
    return 0;
}

int Menu_Main(void)
{
        //!---------INIT---------
    InitOSFunctionPointers();
    InitSysFunctionPointers();
    InitGX2FunctionPointers();
    InitFSFunctionPointers();
    InitSocketFunctionPointers();
    InitVPadFunctionPointers();
    InitAXFunctionPointers();

    u64 currenTitleId = OSGetTitleID();

        // in case we are not in mii maker or HBL channel but in system menu or another channel we need to exit here
    if (currenTitleId != 0x000500101004A200 && // mii maker eur
        currenTitleId != 0x000500101004A100 && // mii maker usa
        currenTitleId != 0x000500101004A000 && // mii maker jpn
        currenTitleId != 0x0005000013374842)    // HBL channel
        {
        return EXIT_RELAUNCH_ON_LOAD;
        }
    else if(exitToHBLOnLaunch)
        {
        return 0;
        }

    VPADInit();
    int forceMenu = 0;

    {
    VPADData vpad;
    int vpadError = -1;
    VPADRead(0, &vpad, 1, &vpadError);

    if(vpadError == 0)
        {
        forceMenu = (vpad.btns_d | vpad.btns_h) & VPAD_BUTTON_B;
        }
    }

    mount_sd_fat("sd");

    cfw_config_t config;
    default_config(&config);
    read_config(&config);

    int launch = 1;

    if(forceMenu || config.directLaunch == 0)
        {
        launch = ShowMenu(&config);
        }

    int returnCode = 0;

    if(launch)
        {
        int res = ExecuteIOSExploit(&config);
        if(res == 0)
            {
            // The SwapDRC functionality has been introduced here.
            if(gAppStatus == 0){
                run_SwapDRC();
            }
            if(config.noIosReload == 0)
                {
                OSForceFullRelaunch();
                SYSLaunchMenu();
                returnCode = EXIT_RELAUNCH_ON_LOAD;
                }
            else if(config.launchSysMenu)
                {
                SYSLaunchMenu();
                exitToHBLOnLaunch = 1;
                returnCode = EXIT_RELAUNCH_ON_LOAD;
                }
            }
        }

    unmount_sd_fat("sd");

    return returnCode;
}

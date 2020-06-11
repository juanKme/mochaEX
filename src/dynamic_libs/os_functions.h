/****************************************************************************
 * Copyright (C) 2015
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
#ifndef __OS_FUNCTIONS_H_
#define __OS_FUNCTIONS_H_

#include <gctypes.h>
#include "common/os_defs.h"
#include "os_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BUS_SPEED                       248625000
#define SECS_TO_TICKS(sec)              (((unsigned long long)(sec)) * (BUS_SPEED/4))
#define MILLISECS_TO_TICKS(msec)        (SECS_TO_TICKS(msec) / 1000)
#define MICROSECS_TO_TICKS(usec)        (SECS_TO_TICKS(usec) / 1000000)

#define usleep(usecs)                   OSSleepTicks(MICROSECS_TO_TICKS(usecs))
#define sleep(secs)                     OSSleepTicks(SECS_TO_TICKS(secs))

#define FLUSH_DATA_BLOCK(addr)          asm volatile("dcbf 0, %0; sync" : : "r"(((addr) & ~31)))
#define INVAL_DATA_BLOCK(addr)          asm volatile("dcbi 0, %0; sync" : : "r"(((addr) & ~31)))

#define EXPORT_DECL(res, func, ...)     res (* func)(__VA_ARGS__) __attribute__((section(".data"))) = 0;
#define EXPORT_VAR(type, var)           type var __attribute__((section(".data")));


#define EXPORT_FUNC_WRITE(func, val)    *(u32*)(((u32)&func) + 0) = (u32)val

#define OS_FIND_EXPORT(handle, func)    funcPointer = 0;                                                                \
OSDynLoad_FindExport(handle, 0, # func, &funcPointer);                          \
if(!funcPointer)                                                                \
OSFatal("Function " # func " is NULL");                                     \
EXPORT_FUNC_WRITE(func, funcPointer);

#define OS_FIND_EXPORT_EX(handle, func, func_p)                                                                         \
funcPointer = 0;                                                                \
OSDynLoad_FindExport(handle, 0, # func, &funcPointer);                          \
if(!funcPointer)                                                                \
_os_find_export(handle, # func, &funcPointer);                                  \
OSFatal("Function " # func " is NULL");                                     \
EXPORT_FUNC_WRITE(func_p, funcPointer);

#define OS_MUTEX_SIZE                   44

/* Handle for coreinit */
extern unsigned int coreinit_handle;
extern void _os_find_export(u32 handle, const char *funcName, void *funcPointer);
void InitOSFunctionPointers(void);
void InitAcquireOS(void);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Lib handle functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern int (* OSDynLoad_Acquire)(const char* rpl, u32 *handle);
extern int (* OSDynLoad_FindExport)(u32 handle, int isdata, const char *symbol, void *address);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Security functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern int (* OSGetSecurityLevel)(void);
extern int (* OSForceFullRelaunch)(void);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Thread functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern int (* OSCreateThread)(void *thread, s32 (*callback)(s32, void*), s32 argc, void *args, u32 stack, u32 stack_size, s32 priority, u32 attr);
extern int (* OSResumeThread)(void *thread);
extern int (* OSSuspendThread)(void *thread);
extern int (* OSIsThreadTerminated)(void *thread);
extern int (* OSIsThreadSuspended)(void *thread);
extern int (* OSJoinThread)(void * thread, int * ret_val);
extern int (* OSSetThreadPriority)(void * thread, int priority);
extern void (* OSDetachThread)(void * thread);
extern void * (* OSGetCurrentThread)(void);
extern const char * (* OSGetThreadName)(void * thread);

extern void (* OSSleepTicks)(u64 ticks);
extern u64 (* OSGetTick)(void);
extern u64 (* OSGetTime)(void);
extern void (*OSTicksToCalendarTime)(u64 time, OSCalendarTime *calendarTime);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Mutex functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void (* OSInitMutex)(void* mutex);
extern void (* OSLockMutex)(void* mutex);
extern void (* OSUnlockMutex)(void* mutex);
extern int (* OSTryLockMutex)(void* mutex);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! System functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern u64 (* OSGetTitleID)(void);
extern void (* OSGetArgcArgv)(int* argc, char*** argv);
extern void (* __Exit)(void);
extern void (* OSFatal)(const char* msg);
extern void (* DCFlushRange)(const void *addr, u32 length);
extern void (* DCStoreRange)(const void *addr, u32 length);
extern void (* DCInvalidateRange)(const void *addr, u32 length);
extern void (* ICInvalidateRange)(const void *addr, u32 length);
extern void* (* OSEffectiveToPhysical)(const void*);
extern int (* __os_snprintf)(char* s, int n, const char * format, ...);
extern int * (* __gh_errno_ptr)(void);

extern void (*OSScreenInit)(void);
extern void (*OSScreenShutdown)(void);
extern unsigned int (*OSScreenGetBufferSizeEx)(unsigned int bufferNum);
extern int (*OSScreenSetBufferEx)(unsigned int bufferNum, void * addr);
extern int (*OSScreenClearBufferEx)(unsigned int bufferNum, unsigned int temp);
extern int (*OSScreenFlipBuffersEx)(unsigned int bufferNum);
extern int (*OSScreenPutFontEx)(unsigned int bufferNum, unsigned int posX, unsigned int posY, const char * buffer);
extern int (*OSScreenEnableEx)(unsigned int bufferNum, int enable);
extern u32 (*OSScreenPutPixelEx)(u32 bufferNum, u32 posX, u32 posY, u32 color);

typedef unsigned char (*exception_callback)(void * interruptedContext);
extern void (* OSSetExceptionCallback)(u8 exceptionType, exception_callback newCallback);

extern int (*IOS_Ioctl)(int fd, unsigned int request, void *input_buffer,unsigned int input_buffer_len, void *output_buffer, unsigned int output_buffer_len);
extern int (*IOS_Open)(char *path, unsigned int mode);
extern int (*IOS_Close)(int fd);


//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Memory functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern u32 *pMEMAllocFromDefaultHeapEx;
extern u32 *pMEMAllocFromDefaultHeap;
extern u32 *pMEMFreeToDefaultHeap;

extern s32 (* MEMGetBaseHeapHandle)(s32 mem_arena);
extern u32 (* MEMGetAllocatableSizeForFrmHeapEx)(s32 heap, s32 align);
extern void* (* MEMAllocFromFrmHeapEx)(s32 heap, u32 size, s32 align);
extern void (* MEMFreeToFrmHeap)(s32 heap, s32 mode);
extern void *(* MEMAllocFromExpHeapEx)(s32 heap, u32 size, s32 align);
extern s32 (* MEMCreateExpHeapEx)(void* address, u32 size, unsigned short flags);
extern void *(* MEMDestroyExpHeap)(s32 heap);
extern void (* MEMFreeToExpHeap)(s32 heap, void* ptr);
extern void* (* OSAllocFromSystem)(int size, int alignment);
extern void (* OSFreeToSystem)(void *addr);
extern int (* OSIsAddressValid)(void *ptr);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! MCP functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern int (* MCP_Open)(void);
extern int (* MCP_Close)(int handle);
extern int (* MCP_GetOwnTitleInfo)(int handle, void * data);


//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! LOADER functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern s32 (* LiWaitIopComplete)(s32 unknown_syscall_arg_r3, s32 * remaining_bytes);
extern s32 (* LiWaitIopCompleteWithInterrupts)(s32 unknown_syscall_arg_r3, s32 * remaining_bytes);
extern void (* addr_LiWaitOneChunk)(void);
extern void (* addr_sgIsLoadingBuffer)(void);
extern void (* addr_gDynloadInitialized)(void);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Kernel function addresses
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void (* addr_PrepareTitle_hook)(void);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Other function addresses
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void (*DCInvalidateRange)(void *buffer, u32 length);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Energy Saver functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
////Burn-in Reduction
extern s32 (*IMEnableDim)(void);
extern s32 (*IMDisableDim)(void);
extern s32 (*IMIsDimEnabled)(s32 * result);
//Auto power down
extern s32 (*IMEnableAPD)(void);
extern s32 (*IMDisableAPD)(void);
extern s32 (*IMIsAPDEnabled)(s32 * result);
extern s32 (*IMIsAPDEnabledBySysSettings)(s32 * result);

extern s32 (*OSSendAppSwitchRequest)(s32 param,void* unknown1,void* unknown2);

#ifdef __cplusplus
}
#endif

#endif // __OS_FUNCTIONS_H_

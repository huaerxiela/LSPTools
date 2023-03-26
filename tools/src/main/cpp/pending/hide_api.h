//
// Created by swift on 2019/1/21.
//

#ifndef SANDHOOK_HIDE_API_H
#define SANDHOOK_HIDE_API_H

#include <jni.h>
#include "jnitrace/JnitraceForC.h"
#include "dlfcn.h"
#include <memory>
#include "arch.h"
#include "dlfcn_nougat.h"

#if defined(__aarch64__)
# define __get_tls() ({ void** __val; __asm__("mrs %0, tpidr_el0" : "=r"(__val)); __val; })
#elif defined(__arm__)
# define __get_tls() ({ void** __val; __asm__("mrc p15, 0, %0, c13, c0, 3" : "=r"(__val)); __val; })
#endif
#define TLS_SLOT_ART_THREAD 7

extern "C" {
    bool canGetObject();
    jobject getJavaObject(JNIEnv* env, void* thread, void* address);
    void *getCurrentThread();

    void initHideApi(JNIEnv *env, HookFunType hook_func);


    bool hookClassInit(void(*callback)(void*));

    JNIEnv *attachAndGetEvn();

}

#endif //SANDHOOK_HIDE_API_H

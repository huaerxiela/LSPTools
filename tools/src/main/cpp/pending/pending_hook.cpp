#include <jni.h>
#include <logging.h>
#include <lsp.h>
#include "dobby.h"
#include "pending/hide_api.h"

extern "C" int SDK_INT = 0;
static HookFunType hook_func = nullptr;



extern "C" [[gnu::visibility("default")]] [[gnu::used]]
jint JNI_OnLoad(JavaVM *jvm, void*) {
    JNIEnv *env = nullptr;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
//    if (hook_func == nullptr){
        hook_func = DobbyHook;
//    }
//    HOOK_JNI(env, FindClass);
//    HOOK_JNI(env, CallObjectMethodV);
    return JNI_VERSION_1_6;
}

void on_library_loaded(const char *name, void *handle) {
    LOG(INFO) << "[DEBUG] " << name;
}

extern "C" [[gnu::visibility("default")]] [[gnu::used]]
NativeOnModuleLoaded native_init(const NativeAPIEntries *entries) {
    LOG(INFO) << "[DEBUG] " << "pengding_hook   native_init";
    hook_func = entries->hook_func;
    return on_library_loaded;
}






static jclass class_pending_hook = nullptr;
static jmethodID method_class_init = nullptr;


extern "C"
JNIEXPORT jobject JNICALL
Java_com_tool_classmonitor_PendingHookHandler_getObjectNative(JNIEnv *env, jclass clazz, jlong thread,
                                                          jlong address) {
    return getJavaObject(env, thread ? reinterpret_cast<void *>(thread) : getCurrentThread(), reinterpret_cast<void *>(address));

}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_tool_classmonitor_PendingHookHandler_canGetObject(JNIEnv *env, jclass clazz) {
    return static_cast<jboolean>(canGetObject());
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_tool_classmonitor_PendingHookHandler_initForPendingHook(JNIEnv *env, jclass clazz,
                                                             jint sdkint) {
    SDK_INT = sdkint;
    initHideApi(env, hook_func);
    LOG(INFO) << "[DEBUG] " << "hook_func: " << hook_func;


    class_pending_hook = static_cast<jclass>(env->NewGlobalRef(
            env->FindClass("com/tool/classmonitor/PendingHookHandler")));
    method_class_init = env->GetStaticMethodID(class_pending_hook, "onClassInit", "(J)V");
    auto class_init_handler = [](void *clazz_ptr) {
        attachAndGetEvn()->CallStaticVoidMethod(class_pending_hook, method_class_init, (jlong) clazz_ptr);
        attachAndGetEvn()->ExceptionClear();
    };
    return static_cast<jboolean>(hookClassInit(class_init_handler));
}
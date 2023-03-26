#include <jni.h>
#include <logging.h>
#include <jnitrace/JnitraceForC.h>
#include <parse.h>
#include "dobby.h"



static HookFunType hook_func = nullptr;

extern "C"
JNIEXPORT void JNICALL
Java_com_tool_jnitrace_JniTrace_startJnitrace(JNIEnv *env, jclass clazz, jobject soname,
                                              jstring filepath) {
    const list<std::string> &clist = parse::jlist2clist(env, soname);
    if (hook_func == nullptr){
        hook_func = DobbyHook;
        LOG(INFO) << "hook is use Dobby";
    }else{
        LOG(INFO) << "hook is use LSPant";
    }
    if (filepath != nullptr) {
        string path = parse::jstring2str(env, filepath);
        Jnitrace::startjnitrace(env, clist,const_cast<char *>(path.c_str()), hook_func);
    } else {
        Jnitrace::startjnitrace(env, clist, nullptr, hook_func);
    }
}








extern "C" [[gnu::visibility("default")]] [[gnu::used]]
jint JNI_OnLoad(JavaVM *jvm, void*) {
    JNIEnv *env = nullptr;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
//    HOOK_JNI(env, FindClass);
//    HOOK_JNI(env, CallObjectMethodV);
    return JNI_VERSION_1_6;
}

void on_library_loaded(const char *name, void *handle) {
    LOG(INFO) << "[DEBUG] " << name;
}

extern "C" [[gnu::visibility("default")]] [[gnu::used]]
NativeOnModuleLoaded native_init(const NativeAPIEntries *entries) {
    LOG(INFO) << "[DEBUG] " << "native_init";
    hook_func = entries->hook_func;
    return on_library_loaded;
}
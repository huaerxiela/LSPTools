//
// Created by swift on 2019/1/21.
//
#include "hide_api.h"

extern int SDK_INT;
extern "C" {
    jobject (*addWeakGlobalRef)(JavaVM *, void *, void *) = nullptr;

    const char* art_lib_path;
    JavaVM* jvm;

    HookFunType hook_native = nullptr;

    void (*class_init_callback)(void*) = nullptr;

    void initHideApi(JNIEnv* env, HookFunType hook_func) {
        env->GetJavaVM(&jvm);
        hook_native = hook_func;

        if (BYTE_POINT == 8) {
            if (SDK_INT >= ANDROID_Q) {
                art_lib_path = "/lib64/libart.so";
            } else {
                art_lib_path = "/system/lib64/libart.so";
            }
        } else {
            if (SDK_INT >= ANDROID_Q) {
                art_lib_path = "/lib/libart.so";
            } else {
                art_lib_path = "/system/lib/libart.so";
            }
        }
        const char* add_weak_ref_sym;
        if (SDK_INT < ANDROID_M) {
            add_weak_ref_sym = "_ZN3art9JavaVMExt22AddWeakGlobalReferenceEPNS_6ThreadEPNS_6mirror6ObjectE";
        } else if (SDK_INT < ANDROID_N) {
            add_weak_ref_sym = "_ZN3art9JavaVMExt16AddWeakGlobalRefEPNS_6ThreadEPNS_6mirror6ObjectE";
        } else  {
            add_weak_ref_sym = SDK_INT <= ANDROID_N2
                               ? "_ZN3art9JavaVMExt16AddWeakGlobalRefEPNS_6ThreadEPNS_6mirror6ObjectE"
                               : "_ZN3art9JavaVMExt16AddWeakGlobalRefEPNS_6ThreadENS_6ObjPtrINS_6mirror6ObjectEEE";
        }

        addWeakGlobalRef = reinterpret_cast<jobject (*)(JavaVM *, void *,
                                                        void *)>(getSymCompat(art_lib_path, add_weak_ref_sym));
    }

    void (*backup_fixup_static_trampolines)(void *, void *) = nullptr;
    void replaceFixupStaticTrampolines(void *thiz, void *clazz_ptr) {
        backup_fixup_static_trampolines(thiz, clazz_ptr);
        if (class_init_callback) {
            class_init_callback(clazz_ptr);
        }
    }

    void *(*backup_mark_class_initialized)(void *, void *, uint32_t *) = nullptr;
    void *replaceMarkClassInitialized(void *thiz, void *self, uint32_t* clazz_ptr) {
        void * result = backup_mark_class_initialized(thiz, self, clazz_ptr);
        if (class_init_callback) {
            class_init_callback(reinterpret_cast<void*>(*clazz_ptr));
        }
        return result;
    }

    bool hookClassInit(void(*callback)(void*)) {
        if (SDK_INT >= ANDROID_R) {
            void *symMarkClassInitialized = getSymCompat(art_lib_path,
                                                           "_ZN3art11ClassLinker20MarkClassInitializedEPNS_6ThreadENS_6HandleINS_6mirror5ClassEEE");
            if (symMarkClassInitialized == nullptr || hook_native == nullptr)
                return false;

            hook_native(symMarkClassInitialized, reinterpret_cast<void *>(replaceMarkClassInitialized),
                        reinterpret_cast<void **>(&backup_mark_class_initialized));

            if (backup_mark_class_initialized) {
                class_init_callback = callback;
                return true;
            } else {
                LOG(INFO) << "[DEBUG] " << "backup_mark_class_initialized: " << backup_mark_class_initialized;
                return false;
            }
        } else {
            void *symFixupStaticTrampolines = getSymCompat(art_lib_path,
                                                           "_ZN3art11ClassLinker22FixupStaticTrampolinesENS_6ObjPtrINS_6mirror5ClassEEE");

            if (symFixupStaticTrampolines == nullptr) {
                //huawei lon-al00 android 7.0 api level 24
                symFixupStaticTrampolines = getSymCompat(art_lib_path,
                                                         "_ZN3art11ClassLinker22FixupStaticTrampolinesEPNS_6mirror5ClassE");
            }
            if (symFixupStaticTrampolines == nullptr || hook_native == nullptr)
                return false;

            hook_native(symFixupStaticTrampolines, reinterpret_cast<void *>(replaceFixupStaticTrampolines),
                        reinterpret_cast<void **>(&backup_fixup_static_trampolines));

            if (backup_fixup_static_trampolines) {
                class_init_callback = callback;
                return true;
            } else {
                LOG(INFO) << "[DEBUG] " << "backup_fixup_static_trampolines: " << backup_fixup_static_trampolines;
                return false;
            }
        }
    }

    JNIEnv *getEnv() {
        JNIEnv *env;
        jvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        return env;
    }

    JNIEnv *attachAndGetEvn() {
        JNIEnv *env = getEnv();
        if (env == nullptr) {
            jvm->AttachCurrentThread(&env, nullptr);
        }
        return env;
    }



    bool canGetObject() {
        return addWeakGlobalRef != nullptr;
    }

    void *getCurrentThread() {
        return __get_tls()[TLS_SLOT_ART_THREAD];
    }

    jobject getJavaObject(JNIEnv* env, void* thread, void* address) {
        if (addWeakGlobalRef == nullptr)
            return nullptr;

        jobject object = addWeakGlobalRef(jvm, thread, address);
        if (object == nullptr)
            return nullptr;

        jobject result = env->NewLocalRef(object);
        env->DeleteWeakGlobalRef(object);

        return result;
    }

}


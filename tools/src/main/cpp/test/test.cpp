//
// Created by Zhenxi on 2022/1/15.
//

#include <jni.h>
#include <logging.h>

static jobject getApplication(JNIEnv *env) {

    jobject application = NULL;

    jclass activity_thread_clz = env->FindClass("android/app/ActivityThread");

    if (activity_thread_clz != NULL) {

        jmethodID currentApplication = env->GetStaticMethodID(

                activity_thread_clz, "currentApplication", "()Landroid/app/Application;");

        if (currentApplication != NULL) {

            application = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);

        } else {

            LOGE("Cannot find method: currentApplication() in ActivityThread.");

        }

        env->DeleteLocalRef(activity_thread_clz);

    } else {

        LOGE("Cannot find class: android.app.ActivityThread");

    }

    return application;

}




JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *_vm, void *) {

    LOG(INFO) << "start test  JNI_OnLoad ";

    JNIEnv *env;
    _vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);

    jclass tracetest = env->FindClass("com/tool/jnitrace/TraceTest");

    jmethodID md1 = env->GetStaticMethodID(tracetest, "test4", "(Ljava/lang/String;)I");
    
    env->CallStaticIntMethod(tracetest,md1,env->NewStringUTF("666"));



    jobject application = getApplication(env);

    // Context(ContextWrapper) class
    jclass context_clz = env->GetObjectClass(application);
    // getPackageManager()方法
    jmethodID getPackageManager = env->GetMethodID(context_clz, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jobject package_manager = env->CallObjectMethod(application, getPackageManager);

    jclass PMCLzz = env->FindClass("android/content/pm/PackageManager");
    jmethodID getPackageInfoMID = env->GetMethodID(PMCLzz, "getPackageInfo",
                                               "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jobject pkgInfo = env->CallObjectMethod(package_manager, getPackageInfoMID,  env->NewStringUTF("com.topjohnwu.magisk"), 0x00000000);
    if (env->ExceptionCheck()) { env->ExceptionClear(); }
    if (pkgInfo == nullptr){
        LOG(INFO) << "no com.topjohnwu.magisk";
    }
//    Build.BOARD
    jclass Build = env->FindClass("android/os/Build");
    jfieldID SERIAL = env->GetStaticFieldID(Build, "SERIAL", "Ljava/lang/String;");
    jstring serial = static_cast<jstring>(env->GetStaticObjectField(Build, SERIAL));
    const char* serialString = env->GetStringUTFChars(serial, 0);
    LOG(INFO) << "test  invoke finsh  :" << serialString;

    return JNI_VERSION_1_6;
}
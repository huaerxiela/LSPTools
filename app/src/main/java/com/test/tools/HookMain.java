package com.test.tools;

import android.annotation.SuppressLint;
import android.content.pm.ApplicationInfo;
import android.util.Log;

import com.tool.classmonitor.PendingHookHandler;
import com.tool.jnitrace.JniTrace;

import java.util.ArrayList;
import java.util.function.Consumer;

import de.robv.android.xposed.IXposedHookLoadPackage;
import de.robv.android.xposed.XposedHelpers;
import de.robv.android.xposed.callbacks.XC_LoadPackage;

public class HookMain implements IXposedHookLoadPackage {
    private static final String TAG = "HookMain";
    @SuppressLint("NewApi")
    @Override
    public void handleLoadPackage(XC_LoadPackage.LoadPackageParam lpparam) throws Throwable {
        if (BuildConfig.APPLICATION_ID.equals(lpparam.packageName)){
            return;
        }


        Log.i(TAG, "handleLoadPackage: PendingHookHandler = " + PendingHookHandler.canWork());
        PendingHookHandler.addClassInitCallBack("com.test.tools.AAA", new Consumer<Class<?>>() {
            @Override
            public void accept(Class<?> aClass) {
                Log.e(TAG, "addClassInitCallBack: " + aClass + " , classloader: " + aClass.getClassLoader());
            }
        });
        new AAA();


        Log.i(TAG, "handleLoadPackage: " + lpparam.packageName + " coming");
//        ArrayList<String> arrayList = new ArrayList<>();
//        arrayList.add("libsgmain.so");
//        JniTrace.startJnitrace(arrayList, null);
        JniTrace.startJnitrace(lpparam.appInfo, null);




    }
}

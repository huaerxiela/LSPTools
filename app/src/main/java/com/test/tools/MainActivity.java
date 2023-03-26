package com.test.tools;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;

import java.util.function.Consumer;

import com.tool.classmonitor.PendingHookHandler;
import com.tool.jnitrace.JniTrace;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "ActivityMain";

    @SuppressLint("NewApi")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

//        ArrayList<String> arrayList = new ArrayList<>();
//        arrayList.add("libtracetest.so");
//        JniTrace.startJnitrace(arrayList, null);
        JniTrace.startJnitrace(this.getApplicationInfo(), null);

        Log.i(TAG, "onCreate: " + Build.SERIAL);
        Log.i(TAG, "onCreate: PendingHookHandler.canWork() = " + PendingHookHandler.canWork());
        PendingHookHandler.addClassInitCallBack("com.test.tools.AAA", new Consumer<Class<?>>() {
            @Override
            public void accept(Class<?> aClass) {
                Log.e(TAG, "addClassInitCallBack: " + aClass + " , classloader: " + aClass.getClassLoader());
            }
        });
        new AAA();


        System.loadLibrary("tracetest");
    }
}
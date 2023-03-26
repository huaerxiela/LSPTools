package com.tool.jnitrace;

import android.content.pm.ApplicationInfo;
import android.util.Log;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class JniTrace {
    private static final String TAG = "JniTrace";

    /**
     * @param soname 需要过滤的soname,支持多个so同时过滤
     * @param savepath 保存的路径,当为null时候则通过日志进行打印
     */
    public static native void startJnitrace(List<String> soname, String savepath);

    public static List<String> getAllSo(ApplicationInfo applicationInfo){
        String nativeLibraryDir = applicationInfo.nativeLibraryDir;
        File f = new File(nativeLibraryDir);
        File[] fa = f.listFiles();
        List<String> allSo = new ArrayList<>();
        for (int i = 0; i < (fa != null ? fa.length : 0); i++) {//循环遍历
            File fs = fa[i];//获取数组中的第i个
            if (!fs.isDirectory()) {
                String soName = fs.getName();
                if (soName.contains("jnitrace") || soName.contains("pending")){
                    continue;
                }
                if (!allSo.contains(soName)){
                    allSo.add(soName);
                }
            }
        }
        return allSo;
    }

    public static void startJnitrace(ApplicationInfo applicationInfo, String savepath){
        List<String> allSo = getAllSo(applicationInfo);
        Log.i(TAG, "startJnitrace: " + allSo);
        startJnitrace(allSo, savepath);
    };

    static {
        System.loadLibrary("jnitrace");
    }
}
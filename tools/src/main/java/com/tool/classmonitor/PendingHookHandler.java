package com.tool.classmonitor;

import android.os.Build;
import android.util.Log;



import java.lang.reflect.Field;
import java.util.Map;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;
import java.util.function.Consumer;

// Pending for hook static method
// When Init class error!
public class PendingHookHandler {
    private static final String TAG = "PendingHookHandler";

    private static final Map<String, Vector<Consumer<Class<?>>>> classInitCallBacks = new ConcurrentHashMap<>();


    private static boolean canUsePendingHook;

    static {
        System.loadLibrary("pending");
        initThreadPeer();
        canUsePendingHook = initForPendingHook(Build.VERSION.SDK_INT);
    }


    public static boolean canWork() {
        boolean result = canGetObject();
        Log.i(TAG, "canUsePendingHook: " + canUsePendingHook + "  , canGetObject: " + result);
        return canUsePendingHook && result;
    }


    public static synchronized void addClassInitCallBack(String className, Consumer<Class<?>> callback) {
        Vector<Consumer<Class<?>>> callbacks = classInitCallBacks.get(className);
        if (callbacks == null) {
            callbacks = new Vector<>();
            classInitCallBacks.put(className, callbacks);
        }
        callbacks.add(callback);
    }

    public static void onClassInit(long clazz_ptr) {
        if (clazz_ptr == 0)
            return;
        Class<?> clazz = (Class<?>) getObject(clazz_ptr);
        if (clazz == null)
            return;

        String name = clazz.getName();
        Vector<Consumer<Class<?>>> callbacks = classInitCallBacks.get(name);
        if(callbacks != null){
            Log.w(TAG, "call callbacks for class " + name + "'s init");
            for (Consumer<Class<?>> callback : callbacks) {
                try {
                    callback.accept(clazz);
                } catch (Throwable ignored) {
                }
            }
        }
    }

    public static Object getObject(long address) {
        if (address == 0) {
            return null;
        }
        long threadSelf = getThreadId();
        return getObjectNative(threadSelf, address);
    }

    public static Field nativePeerField;

    public static Field getField(Class<?> topClass, String fieldName) throws NoSuchFieldException {
        while (topClass != null && topClass != Object.class) {
            try {
                Field field = topClass.getDeclaredField(fieldName);
                field.setAccessible(true);
                return field;
            } catch (Exception e) {
            }
            topClass = topClass.getSuperclass();
        }
        throw new NoSuchFieldException(fieldName);
    }

    public static long getThreadId() {
        if (nativePeerField == null)
            return 0;
        try {
            if (nativePeerField.getType() == int.class) {
                return nativePeerField.getInt(Thread.currentThread());
            } else {
                return nativePeerField.getLong(Thread.currentThread());
            }
        } catch (IllegalAccessException e) {
            return 0;
        }
    }

    private static void initThreadPeer() {
        try {
            nativePeerField = getField(Thread.class, "nativePeer");
        } catch (NoSuchFieldException e) {

        }
    }

    public static native boolean canGetObject();
    public static native boolean initForPendingHook(int SDKInt);
    public static native Object getObjectNative(long thread, long address);



}

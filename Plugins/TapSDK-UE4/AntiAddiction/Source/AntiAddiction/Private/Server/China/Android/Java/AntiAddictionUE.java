package com.tds;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;

import com.tapsdk.antiaddictionui.AntiAddictionUIKit;

public class AntiAddictionUE {

    public static void startup(Activity activity, String userIdentifier, boolean isTapUser) {
        Handler mainHandler = new Handler(Looper.getMainLooper());
        mainHandler.post(new Runnable() {
            @Override
            public void run() {
                AntiAddictionUIKit.startup(activity, userIdentifier, isTapUser);
            }
        });
    }
}

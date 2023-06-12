package com.tds;

import android.app.Activity;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;

import com.taptap.pay.sdk.library.DLCManager;
import com.taptap.pay.sdk.library.TapLicenseCallback;
import com.taptap.pay.sdk.library.TapLicenseHelper;

import java.util.HashMap;
import java.util.List;

public class TapLicenseUE {
    public static void setLicenseCallback(int callBackID) {
        TapLicenseHelper.setLicenseCallback(new TapLicenseCallback() {
            @Override
            public void onLicenseSuccess() {
                TapLicenseUE.onLicenseSuccess(callBackID);
            }
        });
    }

    public static void setDLCCallback(boolean checkOnce, String publicKey,
                                      int callBackID) {
        if (TextUtils.isEmpty(publicKey)) {
            publicKey = null;
        }
        TapLicenseHelper.setDLCCallback(checkOnce, publicKey, new DLCManager.InventoryCallback() {
            @Override
            public boolean onQueryCallBack(int code, HashMap<String, Integer> queryList) {
                TapLicenseUE.onQueryCallBack(code, queryList, callBackID);
                return false;
            }

            @Override
            public void onOrderCallBack(String dlc, int status) {
                TapLicenseUE.onOrderCallBack(dlc, status, callBackID);
            }
        });
    }

    public static void check(Activity activity, boolean forceCheck) {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                TapLicenseHelper.check(activity, forceCheck);
            }
        });
    }

    public static void queryDLC(Activity activity, List<String> dlcList) {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                String[] array = dlcList.toArray(new String[0]);
                TapLicenseHelper.queryDLC(activity, array);
            }
        });
    }

    public static void purchaseDLC(Activity activity, String dlc) {
        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                TapLicenseHelper.purchaseDLC(activity, dlc);
            }
        });
    }

    public native static void onLicenseSuccess(int callBackID);

    public native static void onQueryCallBack(int code, HashMap<String, Integer> queryList, int callBackID);

    public native static void onOrderCallBack(String sku, int status, int callBackID);
}

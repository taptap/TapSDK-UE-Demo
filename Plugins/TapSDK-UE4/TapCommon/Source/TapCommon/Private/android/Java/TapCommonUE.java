package com.tds;

import java.util.HashMap;
import java.util.Map;

import cn.leancloud.json.JSON;
import cn.leancloud.json.TypeReference;

import com.epicgames.ue4.GameActivity;

import android.content.res.Configuration;
import android.content.Context;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.WindowManager;

public class TapCommonUE {
    public static String mapToJson(Map jsonMap) {
        return JSON.toJSONString(jsonMap);
    }

    public static Map jsonToMap(String jsonStr) {
        return JSON.parseObject(jsonStr, new TypeReference<HashMap>(){});
    }
    
    public static boolean isPad() {
        boolean isPad = (GameActivity.Get().getResources().getConfiguration().screenLayout
                    & Configuration.SCREENLAYOUT_SIZE_MASK) >= Configuration.SCREENLAYOUT_SIZE_LARGE;
    
        WindowManager wm = (WindowManager) GameActivity.Get().getSystemService(GameActivity.Get().WINDOW_SERVICE);
        Display display = wm.getDefaultDisplay();
        DisplayMetrics dm = new DisplayMetrics();
        display.getMetrics(dm);
        double x = Math.pow(dm.widthPixels / dm.xdpi, 2);
        double y = Math.pow(dm.heightPixels / dm.ydpi, 2);
        double screenInches = Math.sqrt(x + y);
        return isPad || screenInches >= 7.0;
    }
}

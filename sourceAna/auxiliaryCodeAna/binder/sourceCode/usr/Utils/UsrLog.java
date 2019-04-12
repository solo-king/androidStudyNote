package com.android.server.usr.Utils;


import android.util.Log;

public class UsrLog {

    private static final boolean DEBUG = true;

    public static void LOGD(String tag, String str){

        if(DEBUG){

            Log.d(tag, str);

        }
    }

    public static void LOGE(String tag, String str){

        if(DEBUG){

            Log.e(tag, str);

        }
    }

}

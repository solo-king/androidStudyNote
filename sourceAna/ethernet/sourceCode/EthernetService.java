/*
 * Copyright (C) 2009 The Android-x86 Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Yi Sun <beyounn@gmail.com>
 */

package com.android.server;

import java.net.UnknownHostException;

import android.net.NetworkUtils;
import android.net.ethernet.EthernetNative;
import android.net.ethernet.IEthernetManager;
import android.net.ethernet.EthernetManager;
import android.net.ethernet.EthernetStateTracker;
import android.net.ethernet.EthernetDevInfo;
import android.os.SystemProperties;
import android.provider.Settings;
import android.util.Slog;
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import java.io.IOException;
import java.io.FileReader;
import java.io.BufferedReader;
import android.net.DhcpInfo;

public class EthernetService<syncronized> extends IEthernetManager.Stub{
    private Context mContext;
    private EthernetStateTracker mTracker;
    private String[] DevName;
    private static final String TAG = "EthernetService";
    private final boolean DEBUG = true;
    private int isEthEnabled ;
    private int mEthState= EthernetManager.ETH_STATE_UNKNOWN;

    /* add by chenqigan */
    private String mEthDevMode = EthernetDevInfo.ETH_DEV_MODE_WAN;
    
    private Handler mDelayedHandler;    
    private boolean isEthernetServiceInited = false;    


    public EthernetService(Context context, EthernetStateTracker Tracker){
        mTracker = Tracker;
        mContext = context;
        isEthEnabled = getPersistedState();
        mEthDevMode = getPersistedDevMode();//获取网卡模式
        Slog.v(TAG,"EthernetService构造函数:Ethernet dev enabled " + isEthEnabled +" mEthDevMode = "+mEthDevMode);
        getDeviceNameList();
        setEthState(isEthEnabled);//根据状态以及模式网卡设置
        registerForBroadcasts();
        Slog.v(TAG, "Trigger the ethernet monitor");
        mTracker.StartPolling();
        mDelayedHandler = new Handler();  
        isEthernetServiceInited = true;
    }

    public boolean isEthConfigured() {
        final ContentResolver cr = mContext.getContentResolver();
        int x = Settings.Global.getInt(cr, Settings.Global.ETH_CONF,0);

        if (x == 1)
            return true;
        return false;
    }
    /*  从数据库中获取 */
    public synchronized EthernetDevInfo getSavedEthConfig() {
        if (isEthConfigured()) {
            final ContentResolver cr = mContext.getContentResolver();
            EthernetDevInfo info = new EthernetDevInfo();
            info.setConnectMode(Settings.Global.getString(cr, Settings.Global.ETH_MODE));
            info.setIfName(Settings.Global.getString(cr, Settings.Global.ETH_IFNAME));
            info.setIpAddress(Settings.Global.getString(cr, Settings.Global.ETH_IP));
            info.setDnsAddr(Settings.Global.getString(cr, Settings.Global.ETH_DNS));
            info.setNetMask(Settings.Global.getString(cr, Settings.Global.ETH_MASK));
            info.setRouteAddr(Settings.Global.getString(cr, Settings.Global.ETH_ROUTE));
            info.setDevMode(Settings.Global.getString(cr, Settings.Global.ETH_DEV_MODE));

            return info;
        }
        return null;
    }

    /* dhcp manual */
    public synchronized void setEthMode(String mode) {
        final ContentResolver cr = mContext.getContentResolver();
        Slog.v(TAG,"Set ethernet mode " + DevName + " -> " + mode);
        if (DevName != null) {
            Settings.Global.putString(cr, Settings.Global.ETH_IFNAME, DevName[0]);
            Settings.Global.putInt(cr, Settings.Global.ETH_CONF,1);
            Settings.Global.putString(cr, Settings.Global.ETH_MODE, mode);
        }
    }

    /* wan/lan */
    public synchronized void setEthDevMode(String devMode) {
        final ContentResolver cr = mContext.getContentResolver();
        Slog.v(TAG,"Set ethernet dev mode " + DevName + " -> " + devMode);
        if (DevName != null) {
            Settings.Global.putString(cr, Settings.Global.ETH_IFNAME, DevName[0]);
            Settings.Global.putString(cr, Settings.Global.ETH_DEV_MODE, devMode);
        }
    }

    public synchronized void UpdateEthDevInfo(EthernetDevInfo info) {
        final ContentResolver cr = mContext.getContentResolver();
        Settings.Global.putInt(cr, Settings.Global.ETH_CONF,1);
        Settings.Global.putString(cr, Settings.Global.ETH_IFNAME, info.getIfName());
        Settings.Global.putString(cr, Settings.Global.ETH_IP, info.getIpAddress());
        Settings.Global.putString(cr, Settings.Global.ETH_MODE, info.getConnectMode());
        Settings.Global.putString(cr, Settings.Global.ETH_DNS, info.getDnsAddr());
        Settings.Global.putString(cr, Settings.Global.ETH_ROUTE, info.getRouteAddr());
        Settings.Global.putString(cr, Settings.Global.ETH_MASK,info.getNetMask());

        /* add by chenqigan */
        Slog.d(TAG, "info.getDevMode()= "+info.getDevMode());
        Settings.Global.putString(cr, Settings.Global.ETH_DEV_MODE,info.getDevMode());
        if (mEthState == EthernetManager.ETH_STATE_ENABLED) {
            try {
                mTracker.resetInterface();
                Slog.i(TAG, "$$UpdateEthDevInfo() call resetInterface()");
            } catch (UnknownHostException e) {
                Slog.e(TAG, "Wrong ethernet configuration");
            }
        }
    }

    /* 注册广播  */
    private void registerForBroadcasts() {
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(Intent.ACTION_SCREEN_ON);
        intentFilter.addAction(Intent.ACTION_SCREEN_OFF);
        mContext.registerReceiver(mReceiver, intentFilter);
    }

    public int getTotalInterface() {
        return EthernetNative.getInterfaceCnt();
    }

    public String getmEthDevMode() {
        return mEthDevMode;
    }

    public void setmEthDevMode(String mEthDevMode) {
        this.mEthDevMode = mEthDevMode;
    }

    /*  eth列表只选择eth开头的设备 *//*
      private int scanEthDevice() {
      int i = 1, j;
      DevName = new String[i];
      DevName[i]  = "eth0";
      for (j = 0; j < i; j++) {
          Slog.i(TAG," device " + j + " name " + DevName[j]);
      }
        return i;
    }*/

    private int scanEthDevice() {
        int i = 0,j;
        if ((i = EthernetNative.getInterfaceCnt()) != 0) {
            Slog.i(TAG, "total found " + i + " net devices");
            if (DevName == null || DevName.length != i)
                DevName = new String[i];
        }
        else
            return i;

        for (j = 0; j < i; j++) {
            DevName[j] = EthernetNative.getInterfaceName(j);
            if (DevName[j] == null)
                break;
            Slog.i(TAG," device " + j + " name " + DevName[j]);
        }

        return i;
    }

    public String[] getDeviceNameList() {
        return (scanEthDevice() > 0 ) ? DevName : null;
    }

    private int getPersistedState() {
        final ContentResolver cr = mContext.getContentResolver();
        try {
            return Settings.Global.getInt(cr, Settings.Global.ETH_ON);
        } catch (Settings.SettingNotFoundException e) {
            //return EthernetManager.ETH_STATE_UNKNOWN;
            return EthernetManager.ETH_STATE_DISABLED;
        }
    }

    /* 获取数据库中的网卡模式:wan/lan */
    private String getPersistedDevMode() {
        final ContentResolver cr = mContext.getContentResolver();

        try {
            return Settings.Global.getString(cr, Settings.Global.ETH_DEV_MODE);
        } catch (Exception e) {
            e.printStackTrace();
            return EthernetDevInfo.ETH_DEV_MODE_WAN;
        }
    }

    private synchronized void persistEthEnabled(boolean enabled) {

        final ContentResolver cr = mContext.getContentResolver();

        Settings.Global.putInt(cr, Settings.Global.ETH_ON,
        enabled ? EthernetManager.ETH_STATE_ENABLED : EthernetManager.ETH_STATE_DISABLED);

    }
    /*add by chenqigan
    * EthernetDevInfo.ETH_DEV_MODE_WAN
    * EthernetDevInfo.ETH_DEV_MODE_LAN*/
    public synchronized void persistEthDevMode(String ethDevMode) {
        final ContentResolver cr = mContext.getContentResolver();
        Settings.Global.putString(cr, Settings.Global.ETH_DEV_MODE, ethDevMode);
        setmEthDevMode(ethDevMode);
    }

    private final Runnable mResetInterface = new Runnable() {
        public void run() {
            if(mEthDevMode.equals(EthernetDevInfo.ETH_DEV_MODE_LAN)){
                Slog.d(TAG, "BroadcastReceiver:mEthDevMode = "+mEthDevMode);
            }else{
                try {
                    mTracker.resetInterface();
                    Slog.i(TAG, "$$ mResetInterface call resetInterface()");
                } catch (UnknownHostException e) {
                    Slog.e(TAG, "Wrong ethernet configuration");
                }
            }
        }
    };

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {

            if(mEthDevMode.equals(EthernetDevInfo.ETH_DEV_MODE_LAN)){
                Slog.d(TAG, "BroadcastReceiver:mEthDevMode = "+mEthDevMode);
            }else{
                String action = intent.getAction();
                if (action.equals(Intent.ACTION_SCREEN_ON)) {
                    Slog.d(TAG, "ACTION_SCREEN_ON");
                    if(getPersistedState() == 2){
                        mDelayedHandler.postDelayed(mResetInterface, 5*1000); // wait 5s for device ready
                    }
                } else if (action.equals(Intent.ACTION_SCREEN_OFF)) {
                    Slog.d(TAG, "ACTION_SCREEN_OFF");
                    mDelayedHandler.removeCallbacks(mResetInterface);
                    mTracker.stopInterface(false);
                }
            }

        }
    };

    public synchronized void setmEthStateOnlyWan(int state)
    {
        new Thread("stopInterface") {
            @Override
            public void run() {
                Slog.d(TAG, "2");
                mTracker.stopInterface(false);
            }
        }.start();
    }

    public synchronized void setEthState(int state) {

        /*时刻从数据库中获取*/
        Slog.d(TAG, "setEthState:getPersistedDevMode() = "+getPersistedDevMode());
        if(getPersistedDevMode().equals(EthernetDevInfo.ETH_DEV_MODE_LAN)){//dev_mode == lan
            Slog.i(TAG, "dev_mode == lan: setEthState from " + mEthState + " to "+ state);
            if (mEthState != state) {//只有状态变化的时候才执ctrlLanFunc
                mEthState = state;
                if(state == EthernetManager.ETH_STATE_ENABLED){
                    ctrlLanFunc(true);
                }else{
                    ctrlLanFunc(false);
                }
            }
        }else{
            Slog.i(TAG, "setEthState from " + mEthState + " to "+ state);
            if (mEthState != state) {//只有状态变化的时候才执行
                mEthState = state;
                if (state == EthernetManager.ETH_STATE_DISABLED) {
                    persistEthEnabled(false);
                    Slog.d(TAG, "1");
//              mTracker.stopInterface(false);
                    new Thread("stopInterface") {
                        @Override
                        public void run() {
                            Slog.d(TAG, "2");
                            mTracker.stopInterface(false);
                        }
                    }.start();
                    Slog.d(TAG, "3");
                } else {
                    persistEthEnabled(true);
                    Slog.d(TAG, "4");
                    if (!isEthConfigured()) {
                        // If user did not configure any interfaces yet, pick the first one
                        // and enable it.
                        setEthMode(EthernetDevInfo.ETH_CONN_MODE_DHCP);
                        Slog.d(TAG, "5");
                    }
                    if (!isEthernetServiceInited){
                        Slog.i(TAG, "$$ EthernetService uninited,disable setEthState() call resetInterface()");
                        Slog.i(TAG, "$$ resetInterface() will be called in reconnect()");
                    }
                    else {
                        Slog.i(TAG, "$$ setEthState() start thread to resetInterface()");
                        new Thread("resetInterface") {
                            @Override
                            public void run() {
                                try {
                                    Slog.d(TAG, "6");
                                    mTracker.resetInterface();
                                } catch (UnknownHostException e) {
                                    Slog.e(TAG, "Wrong ethernet configuration");
                                }
                            }
                        }.start();
                    }
                }
            }
        }
    }

    public int getEthState() {
        return mEthState;
    }

    public boolean isEthDeviceUp() {
        try {
            boolean retval = false;
            FileReader fr = new FileReader("/sys/class/net/" + DevName[0] +"/operstate");
            BufferedReader br = new BufferedReader(fr, 32);
            String status = br.readLine();
            if (status != null && status.equals("up")) {
                Slog.d(TAG, "EthDevice status:" + status);
                retval = true;
            }
            else if (status != null && status.equals("down")) {
                Slog.d(TAG, "EthDevice status:" + status);
                retval = false;
            }
            else {
                retval =  false;
            }
            br.close();
            fr.close();
            return retval;
        } catch (IOException e) {
            Slog.d(TAG, "get EthDevice status error");
            return false;
        }
    }

    public boolean isEthDeviceAdded() {
        /*获取数据库dev mode*/
        String devMode = getPersistedDevMode();
        Slog.d(TAG, "isEthDeviceAdded : devMode = "+devMode);
        if(devMode.equals(EthernetDevInfo.ETH_DEV_MODE_LAN)){//add by chenqigan
            return false;
        }else {
            if (null == DevName || null == DevName[0]) {
                Slog.d(TAG, "isEthDeviceAdded: trigger scanEthDevice");
                scanEthDevice();
            }

            if (null == DevName || null == DevName[0]) {
                Slog.d(TAG, "EthernetNative.isEthDeviceAdded: No Device Found");
                return false;
            }

            int retval = EthernetNative.isInterfaceAdded(DevName[0]);
            Slog.d(TAG, "EthernetNative.isEthDeviceAdded(" + DevName[0] +") return " + (0 == retval));
            if (retval == 0)
                return true;
            else
                return false;
        }
    }

    public DhcpInfo getDhcpInfo() {
        return mTracker.getDhcpInfo();
    }


    /*add by chenqigan */
    public int ctrlLanFunc(boolean isChecked){
        if(isChecked){
            Slog.d(TAG, "openLanFunc");
            /*关闭eth0，如果当前使用的为eth0，则触发切换网卡事件*/
           //setmEthStateOnlyWan(EthernetManager.ETH_STATE_DISABLED);
            /*设置字段dev_mode = lan*/
            persistEthDevMode(EthernetDevInfo.ETH_DEV_MODE_LAN);
            persistEthEnabled(isChecked);//单独设置网卡开
            /*设置属性关键字，专制root用户执行脚本*/
            /*  on property:sys.lan.open=1
                    start open_lan_service
                on property:sys.lan.open=2
                    start stop_lan_service
             */
            SystemProperties.set("sys.lan.open", "1");
        }else{
            Slog.d(TAG, "closeLanFunc");
        /*设置属性关键字，专制root用户执行脚本*/
            SystemProperties.set("sys.lan.open", "2");
        /* 设置网卡字段dev_mode = wan */
           // persistEthDevMode(EthernetDevInfo.ETH_DEV_MODE_WAN);
           // persistEthEnabled(EthernetManager.ETH_STATE_DISABLED);//设置下数据库中的eth_on字段
        }
        persistEthEnabled(isChecked);//设置下数据库中的eth_on字段
        return 0;
    }
}

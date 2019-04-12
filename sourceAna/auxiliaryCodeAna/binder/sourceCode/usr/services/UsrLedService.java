package com.android.server.usr.services;

import android.content.Context;
import android.os.IUsrLedService;
import android.service.usr.leds.IUsrLedManagerData;

import com.android.server.usr.Utils.UsrLog;
import com.android.server.usr.Utils.UsrGeneralUtils;
import android.telephony.SignalStrength;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;
import android.os.ServiceManager;

public class UsrLedService extends IUsrLedService.Stub implements IUsrLedManagerData{
    
    private native static int open_led_devices();
    private native static int led_ctrl(int cmd);

    private final String TAG = "UsrLedService";
    private final boolean DEBUG = false;//true:打印Log信息
    private Context mContext;
    private final int LED_BING_INTERVAL_TIME = 150;//ms
    private final int WORK_LED_BLING_INTERVAL_TIME = 1000;//ms

	private final int BLINK_COUNT = 3;

	private static int lastSignalLevel = -1;
    private static volatile boolean  serviceCanCalledFlags = false;
    //网络链接服务代理类
    private ConnectivityManager mConnectivityManager;
    /** ture:跑马灯闪烁前有网络状态变化通知,执行完需要进行网络恢复
     *  false:无通知
     *  解决在跑马灯前就已有网络通知
    */
    private volatile boolean networkStatusChangeBeforeBlink = false;
    /**true:信号指示灯不可设置，并且应该全部关闭
     * false:无限制
     */
    private volatile boolean holdSignalAndCloseAll = false;
    //当前信号灯的强度
    private volatile int currentlySignalLevel = -1;
	
    private Runnable bingRunnable = new Runnable() {
        @Override
        public void run() {
        
        	ledBlinkCount(BLINK_COUNT);
            new Thread(workLedBlink).start();
            serviceCanCalledFlags = true;
            //在跑马灯过程中有网络通知来过那么需要马上更新下网络状态
            if(networkStatusChangeBeforeBlink){
                updateNetLed();
            }
        }
    };

	
    private Runnable workLedBlink = new Runnable() {
        @Override
        public void run() {
            while(true){
                ledCtrl(WORK_LED_ON);
                UsrGeneralUtils.sleepMs(WORK_LED_BLING_INTERVAL_TIME);
                ledCtrl(WORK_LED_OFF);
                UsrGeneralUtils.sleepMs(WORK_LED_BLING_INTERVAL_TIME);
            }
        }
    };

    //放置一些系统服务的获取
    private void getImportComponents(){
        mConnectivityManager = (ConnectivityManager)mContext.getSystemService(Context.CONNECTIVITY_SERVICE);
        if(mConnectivityManager == null){
            Log.e(TAG, "#########################getImportComponents:[mConnectivityManager] is null!!!#########################");
        }
    }
    public UsrLedService(Context context){

        mContext = context;
        getImportComponents();
        openLedDevices();
        new Thread(bingRunnable).start();
        addNetWorkChanageListener();
        addSignalChanageListener();
        
    }

    //辅助函数，用于单独控制NET指示灯。依赖于ledCtrl()接口
    private void netLedCtrl(boolean netLedCtrlCmd){

        if(netLedCtrlCmd){
            ledCtrl(NET_LED_ON);
        }else{
            ledCtrl(NET_LED_OFF);
        }
    }
    
    private void updateNetLed(){
        //跑马灯ok后才可以进行操作
        if(!serviceCanCalledFlags){
            networkStatusChangeBeforeBlink = true;
            Log.w(TAG, "############################led is not ready!!!############################[networkStatusChangeBeforeBlink]"+networkStatusChangeBeforeBlink);
            return;
        }
        NetworkInfo networkInfo = null;
        try {
            networkInfo = mConnectivityManager.getActiveNetworkInfo();
            //如果为空代表无可用网络
            if(networkInfo == null){
                netLedCtrl(false);
                Log.e(TAG, "updateNetLed:no network can be used!!");
                return;
            }
            boolean isAvailable  = networkInfo.isAvailable();
            Log.w(TAG, "updateNetLed:[isAvailable]"+isAvailable);
            netLedCtrl(isAvailable);
        } catch (Exception e) {
            e.printStackTrace();
            netLedCtrl(false);
            Log.e(TAG, "############################updateNetLed:call updateNetLed error!!!############################");
        }

    }
    private boolean isNetworkChanged(String action){
        
        if(action == null)return false;
        return action.equals(ConnectivityManager.CONNECTIVITY_ACTION);
    }
    //信号指示灯广播接收
    class SignalStrengthReceiver extends BroadcastReceiver{

        @Override
        public void onReceive(Context context, Intent intent){
            String action = intent.getAction();
            if(DEBUG)Log.d(TAG, "onReceive:[action]"+action);
            if(action.equals("cn.usr.www.LED_SIGNAL_NOT_SHOW")){//需要对信号指示灯状态更新
                //更新信号指示灯
                boolean isHold = (boolean)intent.getBooleanExtra("isHold", false);
                if(DEBUG)Log.d(TAG, "SignalStrengthReceiver:onReceive[isHold]"+isHold+"[holdSignalAndCloseAll]"
                            +holdSignalAndCloseAll+"[currentlySignalLevel]"+currentlySignalLevel);
                /**1.只有变量状态发生变化才更新 */
                if((holdSignalAndCloseAll!= isHold)){
                    synchronized(this){
                        holdSignalAndCloseAll = isHold;    
                        if(holdSignalAndCloseAll){//只有LED信号灯需要全部关闭才执行，若为false则不执行
                            closeAllSignalLed();
                        }else{//不关闭并占有信号灯，那么则更新其值
                            signalCtrl(currentlySignalLevel);
                        }
                    }
                }
                
            }else if(action.equals("cn.usr.www.LED_SIGNAL_SHOW_LEVEL")){//需要对信号指示灯的数量进行更新
                if(holdSignalAndCloseAll){//如果信号塔图标现在为空或者不显示，那么更新level没有意义
                    return ;
                }
            }
        }
    }
    //注册网络信号灯变化广播
    private void addSignalChanageListener(){
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction("cn.usr.www.LED_SIGNAL_NOT_SHOW");
        mContext.registerReceiver(new SignalStrengthReceiver(), intentFilter);
    }
    class NetworkChanagerReceiver extends BroadcastReceiver{

        @Override
        public void onReceive(Context context, Intent intent){
            String action = intent.getAction();
            if(DEBUG)Log.d(TAG, "onReceive:[action]"+action);
            if(isNetworkChanged(action)){
                //更新NET指示灯状态
                updateNetLed();
            }
        }
    }
     
    //注册网络变化广播
    private void addNetWorkChanageListener(){
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
        mContext.registerReceiver(new NetworkChanagerReceiver(), intentFilter);
    }

    @Override
    public int openLedDevices()  {
        try{
            UsrLog.LOGD(TAG, "openLedDevices");
            return open_led_devices();
        }catch (Exception e){
            e.printStackTrace();
            return -1;
        }
    }

    @Override
    public synchronized int  ledCtrl(int cmd)  {
        //记录最新level值

        try {
            //UsrLog.LOGD(TAG, "ledCtrl, cmd = "+cmd);
            return led_ctrl(cmd);
        }catch (Exception e){
            e.printStackTrace();
        }
        return -1;

    }
	
	private void led_bing(){
		   ledCtrl(CUSTOM_LED_ON);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
		   ledCtrl(NET_LED_ON);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
   		   ledCtrl(WORK_LED_ON);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
		   ledCtrl(SIGNAL1_LED_ON);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
		   ledCtrl(SIGNAL2_LED_ON);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
		   ledCtrl(SIGNAL3_LED_ON);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
		   ledCtrl(SIGNAL4_LED_ON);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
	
	
		   ledCtrl(SIGNAL4_LED_OFF);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
		   ledCtrl(SIGNAL3_LED_OFF);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
		   ledCtrl(SIGNAL2_LED_OFF);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
		   ledCtrl(SIGNAL1_LED_OFF);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
		   ledCtrl(WORK_LED_OFF);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
   		   ledCtrl(NET_LED_OFF);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
		   ledCtrl(CUSTOM_LED_OFF);
		   UsrGeneralUtils.sleepMs(LED_BING_INTERVAL_TIME);
	   }

	private void ledBlinkCount(int blinkCount){

		while(((blinkCount--) == 0 ? false:true))
	           led_bing();
	}

	private synchronized void closeAllSignalLed(){

		ledCtrl(SIGNAL1_LED_OFF);
		ledCtrl(SIGNAL2_LED_OFF);
	    ledCtrl(SIGNAL3_LED_OFF);
		ledCtrl(SIGNAL4_LED_OFF);
	}
    
    /**1.保持要显示的level为最新状态
     * 2.存在更新操作则返回true,反之则返回false
     */
    private synchronized boolean updateCurrentlySignalLevel(int level){

        if(currentlySignalLevel != level){

            currentlySignalLevel = level;
            //return true;
        }
        return true;
    }
    private void updateSignalLevelLed(int level){
        
        switch(level){
			
			case SignalStrength.SIGNAL_STRENGTH_POOR:{
				closeAllSignalLed();
				ledCtrl(SIGNAL1_LED_ON);
				break;
			}
			case SignalStrength.SIGNAL_STRENGTH_MODERATE:{
				closeAllSignalLed();
				ledCtrl(SIGNAL1_LED_ON);
				ledCtrl(SIGNAL2_LED_ON);
				break;
			}
			case SignalStrength.SIGNAL_STRENGTH_GOOD:{
				closeAllSignalLed();
				ledCtrl(SIGNAL1_LED_ON);
				ledCtrl(SIGNAL2_LED_ON);
				ledCtrl(SIGNAL3_LED_ON);
				break;
			}
			case SignalStrength.SIGNAL_STRENGTH_GREAT:
			case SignalStrength.NUM_SIGNAL_STRENGTH_BINS:{
				closeAllSignalLed();
				ledCtrl(SIGNAL1_LED_ON);
				ledCtrl(SIGNAL2_LED_ON);
				ledCtrl(SIGNAL3_LED_ON);
				ledCtrl(SIGNAL4_LED_ON);
				break;
			}

			default:{
				UsrLog.LOGE(TAG, "UNKOWN [level]:"+level);
				closeAllSignalLed();
				break;
			}
		}
    }
	public void signalCtrl(int level){
        
        if(!updateCurrentlySignalLevel(level)){
            //存在更新时才需要往下执行
            return ;
        }

		if(!serviceCanCalledFlags) {//等待跑马灯结束
			UsrLog.LOGD(TAG, "wait ...,serviceCanCalledFlags = "+serviceCanCalledFlags);
			return;
        }

        if(holdSignalAndCloseAll){//等待信号灯可以显示
            if(DEBUG)Log.e(TAG, "signalCtrl:[holdSignalAndCloseAll]"+holdSignalAndCloseAll+"can't opr led signal!!!");
            return ;
        }
		updateSignalLevelLed(currentlySignalLevel);
	}
}

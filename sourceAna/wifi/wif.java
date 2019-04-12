Setting点击More-->Tethering&portable hostspot->Portable Wi-Fi hotspot调用分析:
setSoftapEnabled	//WifiApEnabler.java
	/* 先关闭sta模式wifi */
	mWifiManager.setWifiEnabled(false);
	mWifiManager.setWifiApEnabled(null, enable)	//WifiApEnabler.java
	/* IWifiManager mService */
	mService.setWifiApEnabled(wifiConfig, enabled);	//WifiManager.java
		//com.android.server.wifi;WifiService.java
		enforceChangePermission();
		//private static final int BASE = Protocol.BASE_WIFI_CONTROLLER;
		//static final int CMD_SET_AP                     = BASE + 10;
		//Message obtainMessage(int what, int arg1, int arg2, Object obj),what=CMD_SET_AP,arg1=1,arg2=0,obj=null
		/*  handler 处理  */
		mWifiController.obtainMessage(CMD_SET_AP, 1, 0, null).sendToTarget();//WifiService.java;
			/* 根据参数构造一个新的Message  */
			Message.obtain(mSmHandler, CMD_SET_AP, 1, 0, null);//StateMachine.java//Message obtainMessage(int what, int arg1, int arg2, Object obj)
		//		||发送消息后与WifiContriller中的processMessage处理
		//		\/
		mWifiStateMachine.setHostApRunning(null,true);//WifiController.java //(WifiConfiguration) msg.obj == null
		transitionTo(mApEnabledState);
		setHostApRunning //WifiStateMachine.java
		sendMessage(CMD_START_AP, null);	//



//mWifiController.obtainMessage(CMD_SET_AP, 1, 0, null).sendToTarget();代码处理结构
private WifiController mWifiController;

WifiService(Context context)//WifiService构造函数
	/*
	StateMachine(String name, Looper looper)
	HandlerThread wifiThread = new HandlerThread("WifiService");
	wifiThread.start();//HandlerThread继承自Handler,故触发run函数
		run()
			Looper.prepare();
			mLooper = Looper.myLooper();
			Looper.loop();
	*/
	mWifiController = new WifiController(mContext, this, wifiThread.getLooper());//WifiController(Context context, WifiService service, Looper looper)
		//实例化mSmHandler
		super(TAG, wifiThread.getLooper());//TAG = WifiController，StateMachine.java
			initStateMachine(name, looper);
				 mName = name;
				 mSmHandler = new SmHandler(looper, this);
	mWifiController.start();//调用其父类StateMachine中的start
		/*
			mSmHandler获得:
		*/
		SmHandler smh = mSmHandler;
		smh.completeConstruction();//通报mSmHandler已构建完成
	mWifiController.obtainMessage(CMD_SET_AP, 1, 0, null).sendToTarget();




//wifiThread.getLooper()获取
HandlerThread wifiThread = new HandlerThread("WifiService");
	wifiThread.start();

WifiService(Context context)
	mWifiController = new WifiController(mContext, this, wifiThread.getLooper());
	mWifiController.start();
 mEnableWifiAp =    (CheckBoxPreference) findPreference(ENABLE_WIFI_AP);//ENABLE_WIFI_AP == enable_wifi_ap

 tether_prefs.xml
 <CheckBoxPreference
        android:key="enable_wifi_ap"
        android:title="@string/wifi_tether_checkbox_text"
        android:persistent="false" />


public boolean onPreferenceChange(Preference preference, Object value) {


D/TetherSettings( 8150): startProvisioningIfNecessary: isProvisioningNeeded() = false
D/TetherSettings( 8150): onPreferenceChange:enable = true
D/TetherSettings( 8150): startProvisioningIfNecessary: isProvisioningNeeded() = false
D/WifiApEnabler( 8150): setSoftapEnabled =true

//点击onPreferenceChange触发条件
D/TetherSettings(10967): onPreferenceChange:enable = true
	startProvisioningIfNecessary(WIFI_TETHERING);//WIFI_TETHERING = -1
		mTetherChoice = choice;
			startProvisioningIfNecessary
				mProvisionApp.length == 2;
		if isProvisioningNeeded(){
			//...暂时不考虑
		}
		else{
			startTethering();
				mWifiApEnabler.setSoftapEnabled(true);
					 int wifiState = mWifiManager.getWifiState();
					 mWifiManager.setWifiApEnabled(null, enable)
		}

Settings配置文件:
	wifi_ap_dialog.xml  <-->WifiApDialog.java


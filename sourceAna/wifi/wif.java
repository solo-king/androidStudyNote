Setting���More-->Tethering&portable hostspot->Portable Wi-Fi hotspot���÷���:
setSoftapEnabled	//WifiApEnabler.java
	/* �ȹر�staģʽwifi */
	mWifiManager.setWifiEnabled(false);
	mWifiManager.setWifiApEnabled(null, enable)	//WifiApEnabler.java
	/* IWifiManager mService */
	mService.setWifiApEnabled(wifiConfig, enabled);	//WifiManager.java
		//com.android.server.wifi;WifiService.java
		enforceChangePermission();
		//private static final int BASE = Protocol.BASE_WIFI_CONTROLLER;
		//static final int CMD_SET_AP                     = BASE + 10;
		//Message obtainMessage(int what, int arg1, int arg2, Object obj),what=CMD_SET_AP,arg1=1,arg2=0,obj=null
		/*  handler ����  */
		mWifiController.obtainMessage(CMD_SET_AP, 1, 0, null).sendToTarget();//WifiService.java;
			/* ���ݲ�������һ���µ�Message  */
			Message.obtain(mSmHandler, CMD_SET_AP, 1, 0, null);//StateMachine.java//Message obtainMessage(int what, int arg1, int arg2, Object obj)
		//		||������Ϣ����WifiContriller�е�processMessage����
		//		\/
		mWifiStateMachine.setHostApRunning(null,true);//WifiController.java //(WifiConfiguration) msg.obj == null
		transitionTo(mApEnabledState);
		setHostApRunning //WifiStateMachine.java
		sendMessage(CMD_START_AP, null);	//



//mWifiController.obtainMessage(CMD_SET_AP, 1, 0, null).sendToTarget();���봦��ṹ
private WifiController mWifiController;

WifiService(Context context)//WifiService���캯��
	/*
	StateMachine(String name, Looper looper)
	HandlerThread wifiThread = new HandlerThread("WifiService");
	wifiThread.start();//HandlerThread�̳���Handler,�ʴ���run����
		run()
			Looper.prepare();
			mLooper = Looper.myLooper();
			Looper.loop();
	*/
	mWifiController = new WifiController(mContext, this, wifiThread.getLooper());//WifiController(Context context, WifiService service, Looper looper)
		//ʵ����mSmHandler
		super(TAG, wifiThread.getLooper());//TAG = WifiController��StateMachine.java
			initStateMachine(name, looper);
				 mName = name;
				 mSmHandler = new SmHandler(looper, this);
	mWifiController.start();//�����丸��StateMachine�е�start
		/*
			mSmHandler���:
		*/
		SmHandler smh = mSmHandler;
		smh.completeConstruction();//ͨ��mSmHandler�ѹ������
	mWifiController.obtainMessage(CMD_SET_AP, 1, 0, null).sendToTarget();




//wifiThread.getLooper()��ȡ
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

//���onPreferenceChange��������
D/TetherSettings(10967): onPreferenceChange:enable = true
	startProvisioningIfNecessary(WIFI_TETHERING);//WIFI_TETHERING = -1
		mTetherChoice = choice;
			startProvisioningIfNecessary
				mProvisionApp.length == 2;
		if isProvisioningNeeded(){
			//...��ʱ������
		}
		else{
			startTethering();
				mWifiApEnabler.setSoftapEnabled(true);
					 int wifiState = mWifiManager.getWifiState();
					 mWifiManager.setWifiApEnabled(null, enable)
		}

Settings�����ļ�:
	wifi_ap_dialog.xml  <-->WifiApDialog.java


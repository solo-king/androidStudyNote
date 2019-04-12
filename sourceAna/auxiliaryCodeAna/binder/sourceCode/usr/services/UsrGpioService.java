package com.android.server.usr.services;

import android.content.Context;
import android.os.IUsrGpioService;
import com.android.server.usr.Utils.UsrLog;
import com.android.server.usr.Utils.UsrGeneralUtils;
import android.util.Log;


public class UsrGpioService extends IUsrGpioService.Stub {
	private final String TAG = "UsrGpioService";
	private Context mContext;

	/*  STATIC MEMBER STATEMENT  */	
    native static int open_gpio_devices();
	native static int set_gpio_mode(int gpioName, int gpioMode, int defaultStatus);
	native static int get_gpio_mode(int gpioName);
	native static int set_gpio_output_status(int gpioName, int gpioStatus);
	native static int get_gpio_output_status(int gpioName);
	native static int get_gpio_input_status(int gpioName);
	native static int ctrl_adcx_status(int adcName, boolean status);
	native static boolean get_adcx_status(int adcName);
	native static int get_adcx_vaules(int adcName);
	native static int watchdog_ctrl(boolean open);
	native static int feet_dog(int time);
	native static int unsafe_reboot();
	native static int get_watchdog_status();
	native static java.lang.String get_version();
		
	
	public UsrGpioService(Context context){

        mContext = context;
        open_gpio_devices();
    }

	
    @Override
	public int setGpioMode(int gpioName, int gpioMode, int defaultStatus){

		return 	set_gpio_mode(gpioName, gpioMode, defaultStatus);
	}
	
    @Override
	public int getGpioMode(int gpioName){
	
		return 	get_gpio_mode(gpioName);
		
	}
	
    @Override
	public int setGpioOutputStatus(int gpioName, int gpioStatus){
		
		return  set_gpio_output_status(gpioName, gpioStatus);
		
	}
	
    @Override
	public int getGpioOutputStatus(int gpioName){
	
		return 	get_gpio_output_status(gpioName);
		
	}
	
	@Override
	public int getGpioInputStatus(int gpioName){
	
		return get_gpio_input_status(gpioName);
		
	}
	
	@Override
	public int ctrlAdcxStatus(int adcName, boolean status){
	
		return ctrl_adcx_status(adcName, status);
		
	}
	
	@Override
	public boolean getAdcxStatus(int adcName){

		return 	get_adcx_status(adcName);
	}
	
	@Override
	public int getAdcxVaules(int adcName){

		return get_adcx_vaules(adcName);
		
	}
	
	@Override
	public int watchdogCtrl(boolean open){
	
		Log.e(TAG, "watchdogCtrl,[open]"+open);
		return watchdog_ctrl(open);
		
	}
	
	@Override
	public int feetDog(int time){
	
		Log.e(TAG, "feetDog,[time]"+time);
		return 	feet_dog(time);
		
	}
	
	@Override
	public int unsafeReboot(){
	
		return	unsafe_reboot();
			
	}

	@Override
	public int getWatchdogStatus(){
	
		Log.e(TAG, "getWatchdogStatus");
		return get_watchdog_status();
		
	}

	@Override
	public java.lang.String getVersion(){
	
		Log.e(TAG, "getVersion");
		return get_version();
		
	}

}

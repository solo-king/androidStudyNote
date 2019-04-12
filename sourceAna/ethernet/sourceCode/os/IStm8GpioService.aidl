package android.os;

/** {@hide} */
interface IStm8GpioService{

	int setGpioMode(String gpioName, String gpioMode, String defaultStatus);
	String getGpioMode(String gpioName); 
	boolean setGpioOutputStatus(String gpioName, String status);
	String getGpioOutputStatus(String gpioName);
	int ctrlAdcxStatus(String adcName, boolean status);
	int getAdcxVaules(String adcName);
	String getGpioInputStatus(String gpioName);
	boolean getAdcxStatus(String adcName);
}

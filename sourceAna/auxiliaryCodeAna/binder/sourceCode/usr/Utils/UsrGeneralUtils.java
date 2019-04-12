package	com.android.server.usr.Utils;



public class UsrGeneralUtils{

	public static void sleepMs(long ms){

        try {
            Thread.sleep(ms);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }



}

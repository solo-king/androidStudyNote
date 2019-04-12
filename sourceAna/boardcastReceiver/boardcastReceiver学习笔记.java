涉及指令：
	logcat BootBroadCastReceiver:V  ShutDownBroadReceiver:V *:S&
基础知识:
	接收开机广播：
		//StartupReceiver.java
		public class StartupReceiver extends BroadcastReceiver{
			private static final String TAG = "StartupReceiver";
			@Override
			public void onReceive(Context context, Intent intent) {
				Log.i(TAG, "Received broadcast intent: " + intent.getAction());
			}
		}
		//manifest.xml
		<uses-permission android:name="android.permission.RECEIVE_BOOT_COMPLETED" />
		<application>
			...
			<receiver android:name=".StartupReceiver">
				<intent-filter>
					<action android:name="android.intent.action.BOOT_COMPLETED"/>
				</intent-filter>
			</receiver>
			...
		</application>
	接收关机广播：
		<receiver android:name=".BoardCasts.ShutDownBroadReceiver">
            <intent-filter>
                <action android:name="android.intent.action.ACTION_SHUTDOWN"></action>
                <category android:name="android.intent.category.HOME" />
            </intent-filter>
        </receiver>
		
		
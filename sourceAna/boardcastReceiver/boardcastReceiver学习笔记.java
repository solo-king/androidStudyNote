�漰ָ�
	logcat BootBroadCastReceiver:V  ShutDownBroadReceiver:V *:S&
����֪ʶ:
	���տ����㲥��
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
	���չػ��㲥��
		<receiver android:name=".BoardCasts.ShutDownBroadReceiver">
            <intent-filter>
                <action android:name="android.intent.action.ACTION_SHUTDOWN"></action>
                <category android:name="android.intent.category.HOME" />
            </intent-filter>
        </receiver>
		
		
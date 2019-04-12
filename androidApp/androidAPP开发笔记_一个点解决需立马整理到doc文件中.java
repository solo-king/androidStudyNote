android APP 广播：
	额外说明：
		A APP：广播接受APP
		B APP：广播发送APP
	问题点：
		1.B发送至A，且A能打印出接收信息
		2.A返回消息至B，且B能立马打印回复的信息
		3.参数传递
	基础知识：
	
			private IntentFilter intentFilter;
			private NetworkChangeReceiver networkChangeReceiver;
			intentFilter = new IntentFilter();
			intentFilter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
			networkChangeReceiver = new NetworkChangeReceiver();
			registerReceiver(networkChangeReceiver, intentFilter);
			有序广播的发送：
				sendOrderedBroadcast(intent, null);
				在接收器中截断广播：
					abortBroadcast()
				设置有序广播的优先级：
					<receiver android:name=".MyBroadcastReceiver">
						<intent-filter android:priority="100" >
							<action android:name="com.example.broadcasttest.MY_BROADCAST"/>
						</intent-filter>
					</receiver>
					存在疑问：
						１．如果存在多个相同优先级的广播接收器将如何处理
		本地广播：
			只能在同一个应用中传播
					
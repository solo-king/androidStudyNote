必备知识:
    Context的由来:
           public Context getApplicationContext() {//ContextWrapper.java
                return mBase.getApplicationContext();
            }
             /**
             * @return the base context as set by the constructor or setBaseContext
             */
            public Context getBaseContext() {
                return mBase;
            }
应用中如何使用:
    发送端：
        Intent intent = new Intent("cn.usr.www.LED_SIGNAL_NOT_SHOW");
        intent.putExtra("isHold", isHold);
        context.sendBroadcast(intent);
    接收端(注册广播):
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction("cn.usr.www.LED_SIGNAL_NOT_SHOW");
        mContext.registerReceiver(new SignalStrengthReceiver(), intentFilter);

        registerReceiver(BroadcastReceiver receiver, IntentFilter filter)
            registerReceiver(receiver, filter, null, null);
                registerReceiverInternal(receiver, getUserId(),filter, null, null, this);
                    //主线程中的Handler,这个又是什么
                    scheduler = mMainThread.getHandler();
                    //
                    rd = mPackageInfo.getReceiverDispatcher(receiver, context, scheduler, mMainThread.getInstrumentation(), true);
                    final Intent intent = ActivityManagerNative.getDefault().registerReceiver(
                        mMainThread.getApplicationThread(), mBasePackageName,
                        rd, filter, broadcastPermission, userId);



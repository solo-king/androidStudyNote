//应用启动时ActivityThread.java
main
	Looper.prepareMainLooper();
			prepare(false);//Looper.java
				//new Looper()-->mQueue = new MessageQueue(quitAllowed); mThread = Thread.currentThread();
				sThreadLocal.set(new Looper(quitAllowed));////Looper.java
---------------------------------------------------------------------------------------------------------------						
//初始化分析
Handler()
	Handler(NULL, false)
		//mLooper == new Looper(quitAllowed)即ActivityThread启动时创建的Looper，即主线程
		mLooper = Looper.myLooper();
				sThreadLocal.get()
		mQueue = mLooper.mQueue;//即主线程中实例化的new MessageQueue(quitAllowed)
		mCallback = null;
    mAsynchronous = false;
//myHandler.obtainMessage(1).sendToTarget();分析
	Message.obtain(this, 1); //this == MyHandler myHandler;
			Message m = obtain();//Message.java
      m.target = h;//MyHandler myHandler;
      m.what = what;  
  target.sendMessage(this);//Message.java myHandler.sendMessage();
  		sendMessageDelayed(msg, 0);//Handler.java
  				MessageQueue queue = mQueue;//即主线程中实例化的new MessageQueue(quitAllowed)
  				enqueueMessage(queue, msg, uptimeMillis);
  					msg.target = this;
  					queue.enqueueMessage(msg, uptimeMillis);//将msg压入栈
//事件处理
	Looper.loop();//ActivityThread.java
			Looper me = myLooper();
			MessageQueue queue = me.mQueue;
			for	
				Message msg = queue.next(); // might block
				msg.target.dispatchMessage(msg);
						//如果msg.callback != null
						handleCallback(msg);//Handler.java
								 message.callback.run();
						//mCallback != null 即提供的Handler提供了mCallback函数,调用完直接返回
						mCallback.handleMessage(msg)
						//如果Handler中没有mCallback被提供则执行如下
						handleMessage(msg);//为最常用的方法，常常重写该方法
						
总结如下:
		Handler机制调用顺序如下
			1.msg中的callback
			2.Handler中的mCallback
			3.Handler中的handleMessage
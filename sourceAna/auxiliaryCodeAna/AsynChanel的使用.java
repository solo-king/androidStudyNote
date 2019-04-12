AsyncChanel:
    Handle A
    mMessenger = new Messenger(this);
    asyncChannel.connect(mContext, mTrackerHandler, nfi.messenger)
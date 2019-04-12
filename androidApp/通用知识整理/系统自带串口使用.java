使用:
    private ByteBuffer mInputBuffer;
    private ByteBuffer mOutputBuffer;
    mSerialManager = (SerialManager)mContext.getSystemService(Context.SERIAL_SERVICE);
    if(openDirectBuf){
        mInputBuffer = ByteBuffer.allocateDirect(BUFF_SIZE);
        mOutputBuffer = ByteBuffer.allocateDirect(BUFF_SIZE);
    }else{
        mInputBuffer = ByteBuffer.allocate(BUFF_SIZE);
        mOutputBuffer = ByteBuffer.allocate(BUFF_SIZE);
    }
    //打开对应的串口
    mSerialManager.openSerialPort(getUartPath(uartName),speed)
    //关闭
    serialPortTmp.close();
    //读取数据
    mInputBuffer.clear();
    int ret = serialPortTmp.read(mInputBuffer);
    mInputBuffer.get(readBuf, READBUF_OFFSET, ret);
    //写数据
    mOutputBuffer.clear();
    mOutputBuffer.put(writeBuffer);
    serialPortTmp.write(mOutputBuffer, writeLength);
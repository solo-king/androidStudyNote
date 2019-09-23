初始化过程:
    1. rest chip
    2. 对下chip id
    3. ps的精度设置(猜测)
    4. 中断触发设置
        中断无效时该传感器为高阻态
    5. 设置 sensor->initComplete = true

proxymity数据的获取:
    proxIsr//中断
        对于初始状态处理
            osEnqueuePrivateEvt(EVT_SENSOR_PROX_INTERRUPT, NULL, NULL, mTask.tid);
                handle_event    //EVT_SENSOR_PROX_INTERRUPT
                    xfer = allocXfer(SENSOR_STATE_PROX_SAMPLING);
                    xfer->txrxBuf[0] = ROHM_RPR0521_REG_PS_DATA_LSB;
                    i2cMasterTxRx(I2C_BUS_ID, I2C_ADDR, xfer->txrxBuf, 1, xfer->txrxBuf, 7, i2cCallback, xfer);
                        handle_i2c_event

        初始化之后事件处理
            sample.fdata = (pinState) ? ROHM_RPR0521_REPORT_FAR_VALUE : ROHM_RPR0521_REPORT_NEAR_VALUE;
            data->proxState = (pinState) ? PROX_STATE_FAR : PROX_STATE_NEAR;
            osEnqueueEvt(sensorGetMyEventType(SENS_TYPE_PROX), sample.vptr, NULL);


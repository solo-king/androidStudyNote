解决问题:
    1. 初始化流程 
        lsm6dsm_startTask
            1 初始化中断引脚及中断
            2 配置自身spi接口参数，并获取spi master的ops
            3 分配三元数据存放
                类型为struct TripleAxisDataEvent
                放于 T(mDataSlabThreeAxis) = slabAllocatorNew(slabSize, 4, 20);中
            4 注册sensor
                T(sensors[i]).handle = sensorRegister(&LSM6DSMSensorInfo[i], &LSM6DSMSensorOps[i], NULL, false);
            5 配置调整 fifo
                T(fifoCntl).decimatorsIdx[FIFO_GYRO] = GYRO;
            6 初始化spi数据
                T_SLAVE_INTERFACE(mWbufCnt) = 0;//SPI中含有的buffer
                T_SLAVE_INTERFACE(mRegCnt) = 0;//spi packet num counter
            7 注册app启动事件，在其中进行二次初始化
                osEventSubscribe(T(tid), EVT_APP_START);
            8 开启中断
                extiClearPendingGpio(T(eint1));
                lsm6dsm_enableInterrupt(T(eint1), &T(isr1));
        lsm6dsm_handleEvent:
            1. 开定时器延迟100ms等待sensor硬件ok==>处理事件 EVT_APP_START
            2. 读取id==>处理事件SENSOR_BOOT
            3. 对比硬件id与driver id 是否一致 ==>deal SENSOR_VERIFY_WAI
            4. 变换task initState,该值代表当前task的状态
                T(initState) = RESET_LSM6DSM
            5. 真正开始sensor的初始化, 在lsm6dsm_sensorInit中进行
            6. 软重启 sensor    ==>deal initState is RESET_LSM6DSM,且当前task->state == SENSOR_VERIFY_WAI
            7.  
                

    2. 初始化过程做了哪些事情
    3. sensortest -e 1 50000最终调用sensor的那个函数
    4. acc 数据是如何上报的
基础知识:
    1. spi的读写方式
        SPI_WRITE(LSM6DSM_FUNC_CFG_ACCESS_ADDR, LSM6DSM_FUNC_CFG_ACCESS_ADDR, 50);
            lsm6dsm_spiQueueWrite(LSM6DSM_FUNC_CFG_ACCESS_ADDR, LSM6DSM_FUNC_CFG_ACCESS_ADDR, 50)   
                1. 填充一个 SpiPacket 结构体, 其来自于 LSM6DSMTask.slaveConn.mpackets[mRegCnt]
                2. mRegCnt则为当前spiPacket下标,每次调用完后则+1.
                3. 并未发起真正的spi写操作，写操作则位于 lsm6dsm_spiBatchTxRx 中
        lsm6dsm_spiBatchTxRx(&T_SLAVE_INTERFACE(mode), lsm6dsm_spiCallback, &mTask, __FUNCTION__);
            //最终调用如下方法发送数据
            spiMasterRxTx(T_SLAVE_INTERFACE(spiDev), T_SLAVE_INTERFACE(cs), T_SLAVE_INTERFACE(packets), regCount, mode, callback, cookie)
    2. 数据成员含义
        totalSip
            total number of samples in one pattern.
        minDecimator
            min value of decimators.
        maxDecimator
            max value of decimators.
        maxMinDecimator
            maxDecimator devided by minDecimator.

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
                5.1对于LSM6DSM
                    case INIT_LSM6DSM:
                        /**
                         * LSM6DSM_FUNC_CFG_ACCESS_ADDR(0x01)
                         *      FUNC_CFG_EN
                         *      FUNC_CFG_EN_B
                         *          00==> Bank A and B disabled (default)
                        */
                        SPI_WRITE(LSM6DSM_FUNC_CFG_ACCESS_ADDR, LSM6DSM_FUNC_CFG_ACCESS_BASE, 50);
                        /**
                         * LSM6DSM_DRDY_PULSE_CFG_ADDR(0x0b)
                         *      (DRDY_PULSED = 0)   DataReady latched mode. Returns to 0 only after output data has been read
                         *      (INT2_WRIST_TILT = 0) ==> INT2_WRIST_TILT
                        */
                        SPI_WRITE(LSM6DSM_DRDY_PULSE_CFG_ADDR, LSM6DSM_DRDY_PULSE_CFG_BASE);
                        /**
                         * LSM6DSM_CTRL1_XL_ADDR(0x10)
                         *       ((0 << 7)     ODR_XL3  先关闭加速度传感器的采样输出
                         *       (0 << 6)      ODR_XL2 
                         *       (0 << 5)      ODR_XL1 
                         *       (0 << 4)      ODR_XL0 
                         *       (1 << 3)      FS_XL1  Accelerometer full-scale selection==>11 +- 8g
                         *       (1 << 2)      FS_XL0
                         *       (0 << 1)      LPF1_BW_SEL
                         *              LPF: low-pass filter
                         *       (0 << 0))     (0)
                        */
                        buffer[0] = LSM6DSM_CTRL1_XL_BASE;                           /* LSM6DSM_CTRL1_XL */
                        /**
                         * CTRL2_G (0x11)
                         *       FS_125(1)  
                         *       ((0 << 7) ODR_G3  ==> 先关闭角速度采样输出
                         *       (0 << 6)  ODR_G2 
                         *       (0 << 5)  ODR_G1 
                         *       (0 << 4)  ODR_G0 
                         *       (1 << 3)  FS_G1 ==> 11: 2000 dps
                         *       (1 << 2)  FS_G0 
                         *       (0 << 1)  FS_125 Gyroscope full-scale at 125 dps --> 0
                         *       (0 << 0)) (0)
                        */
                        buffer[1] = LSM6DSM_CTRL2_G_BASE;                            /* LSM6DSM_CTRL2_G */
                        /**
                         * CTRL3_C (0x12)
                         *       ((0 << 7) BOOT ==>Reboots memory content,Reboots memory content.
                         *       (1 << 6) BDU  ==> Block Data Update: continuous update
                         *       (0 << 5) H_LACTIVE ==>push-pull mode
                         *       (0 << 4) PP_OD ==> push-pull mode
                         *       (0 << 3) SIM   ==> SPI MODE 4线模式
                         *       (1 << 2) IF_INC->Register address automatically incremented during a multiple byte access with a serial interface
                         *       (0 << 1) BLE  ==>选用小端模式 0: data LSB @ lower address
                         *       (0 << 0)) SW_RESET
                        */
                        buffer[2] = LSM6DSM_CTRL3_C_BASE;                            /* LSM6DSM_CTRL3_C */
                        /**
                         *  CTRL4_C (0x13)
                         *       ((0 << 7)   DEN_XL_EN==>Extend DEN functionality to accelerometer sensor:disabled
                         *       (0 << 6)   SLEEP :Gyroscope sleep mode enable-->disable
                         *       (1 << 5)   INT2_on_INT1:all interrupt signals in logic or on INT1 pad
                         *       (0 << 4)   DEN_DRDY_MASK=>DEN DRDY signal on INT1 pad:0: disabled
                         *       (0 << 3)   DRDY_MASK   Configuration 1 data available enable bit. 0: DA timer disabled
                         *       (1 << 2)   I2C_disable==>Disable I2C interface both
                         *       (0 << 1)   LPF1_SEL_G 
                         *       (0 << 0))  (0)
                        */
                        buffer[3] = LSM6DSM_CTRL4_C_BASE;                            /* LSM6DSM_CTRL4_C */
                        /**
                         * CTRL5_C (0x14)
                         *  000: no rounding
                         *  DEN active level configuration
                         *  Angular rate sensor self-test enable
                         *      00--disabled
                         *  Linear acceleration sensor self-test enable
                         *      00--disabled
                        */
                        buffer[4] = LSM6DSM_CTRL5_C_BASE;                            /* LSM6DSM_CTRL4_C */
                        SPI_MULTIWRITE(LSM6DSM_CTRL1_XL_ADDR, buffer, 5);
                        /**
                         * LSM6DSM_INT1_CTRL_ADDR(0x19)
                         *     ((0 << 7)   (WRIST_TILT_EN)   wrist tilt algorithm disabled
                         *     (0 << 6)   (0) 
                         *     (1 << 5)   TIMER_EN  开启时间搓功能:Enable timestamp count
                         *     (0 << 4)   PEDO_EN 计步器相关
                         *     (0 << 3)   TILT_EN 
                         *     (1 << 2)   FUNC_EN 
                         *     (1 << 1)   PEDO_RST_STEP  =>Reset pedometer step counte:ENABLE
                         *     (0 << 0))  SIGN_MOTION_EN =>disable significant motion detection function.
                        */
                        buffer[0] = LSM6DSM_CTRL10_C_BASE | LSM6DSM_RESET_PEDOMETER; /* LSM6DSM_CTRL10_C */
                        /**
                         * MASTER_CONFIG (1Ah)
                         *  0x00
                        */
                        buffer[1] = LSM6DSM_MASTER_CONFIG_BASE;                      /* LSM6DSM_MASTER_CONFIG */
                        SPI_MULTIWRITE(LSM6DSM_CTRL10_C_ADDR, buffer, 2);
                        /**
                         * LSM6DSM_INT1_CTRL_ADDR(0x0d)
                         *  INT1上的中断类型
                        */
                        SPI_WRITE(LSM6DSM_INT1_CTRL_ADDR, LSM6DSM_INT1_CTRL_BASE);
                        /**
                         * LSM6DSM_WAKE_UP_DUR_ADDR(0x5c)
                         *        (0<<7)  FF_DUR5
                         *        (0<<6)  WAKE_DUR1
                         *        (0<<5)  WAKE_DUR0
                         *        (1<<4)  TIMER_HR ==>Timestamp register resolution setting:1LSB = 25 μs
                         *        (0<<3)  SLEEP_DUR3
                         *        (0<<2)  SLEEP_DUR2
                         *        (0<<1)  SLEEP_DUR1
                         *        (0<<0)  SLEEP_DUR0
                         *  0x10 ==> Timestamp register resolution setting
                         *       1: 1LSB = 25 μs
                        */
                        SPI_WRITE(LSM6DSM_WAKE_UP_DUR_ADDR, LSM6DSM_WAKE_UP_DUR_BASE);
                5.1对于LSM6DS3--还未修改回来
                    case INIT_LSM6DSM:
                        /**
                         * LSM6DSM_FUNC_CFG_ACCESS_ADDR(0x01)
                         *      FUNC_CFG_EN
                         *      FUNC_CFG_EN_B
                         *          00==> Bank A and B disabled (default)
                        */
                        SPI_WRITE(LSM6DSM_FUNC_CFG_ACCESS_ADDR, LSM6DSM_FUNC_CFG_ACCESS_BASE, 50);
                        /**
                         * LSM6DSM_DRDY_PULSE_CFG_ADDR(0x0b)
                         *      (DRDY_PULSED = 0)   DataReady latched mode. Returns to 0 only after output data has been read
                         *      (INT2_WRIST_TILT = 0) ==> INT2_WRIST_TILT
                        */
                        SPI_WRITE(LSM6DSM_DRDY_PULSE_CFG_ADDR, LSM6DSM_DRDY_PULSE_CFG_BASE);
                        /**
                         * LSM6DSM_CTRL1_XL_ADDR(0x10)
                         *       ((0 << 7)     ODR_XL3  先关闭加速度传感器的采样输出
                         *       (0 << 6)      ODR_XL2 
                         *       (0 << 5)      ODR_XL1 
                         *       (0 << 4)      ODR_XL0 
                         *       (1 << 3)      FS_XL1  Accelerometer full-scale selection==>11 +- 8g
                         *       (1 << 2)      FS_XL0
                         *       (0 << 1)      LPF1_BW_SEL
                         *              LPF: low-pass filter
                         *       (0 << 0))     (0)
                        */
                        buffer[0] = LSM6DSM_CTRL1_XL_BASE;                           /* LSM6DSM_CTRL1_XL */
                        /**
                         * CTRL2_G (0x11)
                         *       FS_125(1)  
                         *       ((0 << 7) ODR_G3  ==> 先关闭角速度采样输出
                         *       (0 << 6)  ODR_G2 
                         *       (0 << 5)  ODR_G1 
                         *       (0 << 4)  ODR_G0 
                         *       (1 << 3)  FS_G1 ==> 11: 2000 dps
                         *       (1 << 2)  FS_G0 
                         *       (0 << 1)  FS_125 Gyroscope full-scale at 125 dps --> 0
                         *       (0 << 0)) (0)
                        */
                        buffer[1] = LSM6DSM_CTRL2_G_BASE;                            /* LSM6DSM_CTRL2_G */
                        /**
                         * CTRL3_C (0x12)
                         *       ((0 << 7) BOOT ==>Reboots memory content,Reboots memory content.
                         *       (1 << 6) BDU  ==> Block Data Update: continuous update
                         *       (0 << 5) H_LACTIVE ==>push-pull mode
                         *       (0 << 4) PP_OD ==> push-pull mode
                         *       (0 << 3) SIM   ==> SPI MODE 4线模式
                         *       (1 << 2) IF_INC->Register address automatically incremented during a multiple byte access with a serial interface
                         *       (0 << 1) BLE  ==>选用小端模式 0: data LSB @ lower address
                         *       (0 << 0)) SW_RESET
                        */
                        buffer[2] = LSM6DSM_CTRL3_C_BASE;                            /* LSM6DSM_CTRL3_C */
                        /**
                         *  CTRL4_C (0x13)
                         *       ((0 << 7)   DEN_XL_EN==>Extend DEN functionality to accelerometer sensor:disabled
                         *       (0 << 6)   SLEEP :Gyroscope sleep mode enable-->disable
                         *       (1 << 5)   INT2_on_INT1:all interrupt signals in logic or on INT1 pad
                         *       (0 << 4)   DEN_DRDY_MASK=>DEN DRDY signal on INT1 pad:0: disabled
                         *       (0 << 3)   DRDY_MASK   Configuration 1 data available enable bit. 0: DA timer disabled
                         *       (1 << 2)   I2C_disable==>Disable I2C interface both
                         *       (0 << 1)   LPF1_SEL_G 
                         *       (0 << 0))  (0)
                        */
                        buffer[3] = LSM6DSM_CTRL4_C_BASE;                            /* LSM6DSM_CTRL4_C */
                        /**
                         * CTRL5_C (0x14)
                         *  000: no rounding
                         *  DEN active level configuration
                         *  Angular rate sensor self-test enable
                         *      00--disabled
                         *  Linear acceleration sensor self-test enable
                         *      00--disabled
                        */
                        buffer[4] = LSM6DSM_CTRL5_C_BASE;                            /* LSM6DSM_CTRL4_C */
                        SPI_MULTIWRITE(LSM6DSM_CTRL1_XL_ADDR, buffer, 5);
                        /**
                         * LSM6DSM_INT1_CTRL_ADDR(0x19)
                         *     ((0 << 7)   (WRIST_TILT_EN)   wrist tilt algorithm disabled
                         *     (0 << 6)   (0) 
                         *     (1 << 5)   TIMER_EN  开启时间搓功能:Enable timestamp count
                         *     (0 << 4)   PEDO_EN 计步器相关
                         *     (0 << 3)   TILT_EN 
                         *     (1 << 2)   FUNC_EN 
                         *     (1 << 1)   PEDO_RST_STEP  =>Reset pedometer step counte:ENABLE
                         *     (0 << 0))  SIGN_MOTION_EN =>disable significant motion detection function.
                        */
                        buffer[0] = LSM6DSM_CTRL10_C_BASE | LSM6DSM_RESET_PEDOMETER; /* LSM6DSM_CTRL10_C */
                        /**
                         * MASTER_CONFIG (1Ah)
                         *  0x00
                        */
                        buffer[1] = LSM6DSM_MASTER_CONFIG_BASE;                      /* LSM6DSM_MASTER_CONFIG */
                        SPI_MULTIWRITE(LSM6DSM_CTRL10_C_ADDR, buffer, 2);
                        /**
                         * LSM6DSM_INT1_CTRL_ADDR(0x0d)
                         *  INT1上的中断类型
                        */
                        SPI_WRITE(LSM6DSM_INT1_CTRL_ADDR, LSM6DSM_INT1_CTRL_BASE);
                        /**
                         * LSM6DSM_WAKE_UP_DUR_ADDR(0x5c)
                         *        (0<<7)  FF_DUR5
                         *        (0<<6)  WAKE_DUR1
                         *        (0<<5)  WAKE_DUR0
                         *        (1<<4)  TIMER_HR ==>Timestamp register resolution setting:1LSB = 25 μs
                         *        (0<<3)  SLEEP_DUR3
                         *        (0<<2)  SLEEP_DUR2
                         *        (0<<1)  SLEEP_DUR1
                         *        (0<<0)  SLEEP_DUR0
                         *  0x10 ==> Timestamp register resolution setting
                         *       1: 1LSB = 25 μs
                        */
                        SPI_WRITE(LSM6DSM_WAKE_UP_DUR_ADDR, LSM6DSM_WAKE_UP_DUR_BASE);


            6. 软重启 sensor    ==>deal initState is RESET_LSM6DSM,且当前task->state == SENSOR_VERIFY_WAI
    2. sensortest -e 1 50000最终调用sensor的那个函数

    3. acc 数据是如何上报的
        
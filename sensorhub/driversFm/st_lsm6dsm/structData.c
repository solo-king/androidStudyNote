
/*
 * struct LSM6DSMTimeCalibrationWithoutTimer: data used when time calibration is performed during FIFO read.
 *      If latency is smaller than LSM6DSM_SYNC_DELTA_INTERVAL no need to use a timer but we can read timestamp before read FIFO data.
 * @lastTimestampDataAvlRtcTime: last time we perform a timestamp read from LSM6DSM based on RTC time.
 * @newTimestampDataAvl: when deltatime is enough we can read again timestamp from LSM6DSM.
 */
struct LSM6DSMTimeCalibrationWithoutTimer {
    uint64_t lastTimestampDataAvlRtcTime;
    bool newTimestampDataAvl;
};

/*
 * struct LSM6DSMTimeCalibration: time calibration task data
 * @sensorTimeToRtcData: timeSync algo data.
 * @noTimer: if timer is not used to perform time sync, those data will be used.
 * @lastSampleTimestamp: last sample timestamp from FIFO. Already coverted to RTC time.
 * @timeSyncRtcTime: Rtc time while performing timestamp read from LSM6DSM.
 * @sampleTimestampFromFifoLSB: current timestamp from FIFO in LSB. Needs to be stored becasue of overflow.
 * @timestampSyncTaskLSB: when timer is used to sync time, this is the last timestamp read from LSM6DSM in LSB. Needs to be stored becasue of overflow.
 * @deltaTimeMarginLSB: is it used to verify if timestamp from FIFO is valid, this is max jitter that timestamp can have from FIFO.
 * @timestampBaroLSB: if magn and baro are both enabled, barometer data are read with a timer because no slots are available in FIFO. This is the timestamp of baro data.
 * @theoreticalDeltaTimeLSB: theoretical value of timestamp based on sensor frequency.
 * @timestampIsValid: flag that indicate if current timestamp parsing FIFO is valid.
 */
struct LSM6DSMTimeCalibration {
    time_sync_t sensorTimeToRtcData;
    struct LSM6DSMTimeCalibrationWithoutTimer noTimer;
    uint64_t lastSampleTimestamp;
    uint64_t timeSyncRtcTime;
    enum LSM6DSMTimeCalibrationStatus status;
    uint32_t sampleTimestampFromFifoLSB;
    uint32_t timestampSyncTaskLSB;
    uint32_t deltaTimeMarginLSB;
    uint32_t timeId;
#if defined(LSM6DSM_I2C_MASTER_MAGNETOMETER_ENABLED) && defined(LSM6DSM_I2C_MASTER_BAROMETER_ENABLED)
    uint32_t timestampBaroLSB;
#endif /* LSM6DSM_I2C_MASTER_MAGNETOMETER_ENABLED, LSM6DSM_I2C_MASTER_BAROMETER_ENABLED */
    uint32_t theoreticalDeltaTimeLSB;
    bool timestampIsValid;
};

/*
 * struct LSM6DSMFifoCntl: fifo control data
 * @decimatorsIdx: give who is the sensor that store data in that FIFO slot.
 * @triggerRate: frequency of FIFO [Hz * 1024].
 * @watermark: watermark value in #num of samples.
 * @decimators: fifo decimators value.
 * @minDecimator: min value of decimators.
 * @maxDecimator: max value of decimators.
 * @maxMinDecimator: maxDecimator devided by minDecimator.
 * @totalSip: total number of samples in one pattern.
 * @timestampPosition: since timestamp in FIFO is the latest sensor, we need to know where is located during FIFO parsing.
 */
struct LSM6DSMFifoCntl {
    enum SensorIndex decimatorsIdx[FIFO_NUM];
    uint32_t triggerRate;
    uint16_t watermark;
    uint8_t fifoRateRegVal;
    uint8_t decimators[FIFO_NUM];
    uint8_t minDecimator;
    uint8_t maxDecimator;
    uint8_t maxMinDecimator;
    uint8_t totalSip;
    uint8_t timestampPosition[32];
};

struct SpiPacket {
    void *rxBuf;
    const void *txBuf;
    size_t size;
    uint32_t delay;
};
/*
 * struct LSM6DSMSPISlaveInterface: SPI slave data interface
 * @packets: spi packets needed to perform read/write operations.
 * @txrxBuffer: spi data buffer.
 * @spiDev: spi device info.
 * @mode: spi mode info (frequency, polarity, etc).
 * @mWbufCnt: counter of total data in spi buffer.
 * @cs: chip select used by SPI slave.
 * @funcSrcBuffer: pointer of txrxBuffer to access func source register data.
 * @tmpDataBuffer: pointer of txrxBuffer to access sporadic temp read.
 * @fifoDataBuffer: pointer of txrxBuffer to access fifo data.
 * @fifoStatusRegBuffer: pointer of txrxBuffer to access fifo status registers.
 * @stepCounterDataBuffer: pointer of txrxBuffer to access step counter data.
 * @tempDataBuffer: pointer of txrxBuffer to access sensor temperature data needed by calibration algos.
 * @timestampDataBuffer: pointer of txrxBuffer to access sensor timestamp data in order to syncronize time.
 * @timestampDataBufferBaro: pointer of txrxBuffer to access sensor timestamp data for barometer when not in FIFO.
 * @baroDataBuffer: pointer of txrx to access barometer data from DSM when not in FIFO.
 * @mRegCnt: spi packet num counter.
 * @spiInUse: flag used to check if SPI is currently busy.
 */
struct LSM6DSMSPISlaveInterface {
    struct SpiPacket packets[LSM6DSM_SPI_PACKET_SIZE];
    uint8_t txrxBuffer[SPI_BUF_SIZE];
    struct SpiDevice *spiDev;
    struct SpiMode mode;
    uint16_t mWbufCnt;
    spi_cs_t cs;
    uint8_t *funcSrcBuffer;
    uint8_t *tmpDataBuffer;
    uint8_t *fifoDataBuffer;
    uint8_t *fifoStatusRegBuffer;
    uint8_t *stepCounterDataBuffer;
    uint8_t *timestampDataBuffer;
    uint8_t mRegCnt;
    bool spiInUse;
};

/*
 * struct LSM6DSMTask: driver task data
 * @sensors: sensor status data list.
 * @slaveConn: slave interface / communication data.
 * @accelCal: accelerometer calibration algo data.
 * @gyroCal: gyroscope calibration algo data.
 * @overTempCal: gyroscope over temperature calibration algo data.
 * @magnCal: magnetometer calibration algo data.
 * @int1: int1 gpio data.
 * @isr1: isr1 data.
 * @mDataSlabThreeAxis: memory used to store three axis sensors data.
 * @mDataSlabOneAxis: memory used to store one axis sensors data.
 * @fifoCntl: fifo control data.
 * @time: time calibration data.
 * @currentTemperature: sensor temperature data value used by gyroscope/accelerometer bias calibration libs.
 * @lastFifoReadTimestamp: store when last time FIFO was read.
 * @initState: initialization is done in several steps (enum InitState).
 * @selftestState: self-test is performed in several steps (enum SelfTestState).
 * @calibrationState: sensor calibration is done in several steps (enum CalibrationState).
 * @tid: task id.
 * @totalNumSteps: total number of steps of step counter sensor.
 * @fifoDataToRead: number of byte to read in current FIFO read.
 * @fifoDataToReadPending: in order to reduce txrxBuffer, FIFO read is performed in several read. This value tell how many data still need to read from FIFO.
 * @baroTimerId: barometer task timer id.
 * @dataSelftestEnabled: sensor data read during GapSelfTestProgram while self-test bit is set.
 * @dataSelftestNotEnabled: sensor data read during GapSelfTestProgram while self-test bit is not set.
 * @dataCalibration: sensor data read during calibration program.
 * @accelCalibrationData: accelerometer offset value (hw) to store into sensor.
 * @gyroCalibrationData: gyroscope offset value (hw) applied to each sample (by software).
 * @state: task state, driver manage operations using a state machine (enum SensorState).
 * @numSamplesSelftest: temp variable storing number of samples read by self-test program.
 * @numSamplesCalibration: temp variable storing number of samples read by calibration program.
 * @mRetryLeft: counter used to retry operations #n times before return a failure.
 * @pedometerDependencies: dependencies mask of sensors that are using embedded functions.
 * @masterConfigDependencies: dependencies mask of sensors that are using I2C master.
 * @int1Register: interrupt 1 register content (addr: 0x0d).
 * @int2Register: interrupt 2 register content (addr: 0x0e).
 * @embeddedFunctionsRegister: embedded register content (addr: 0x19).
 * @pendingFlush: number of flush requested for each sensor.
 * @masterConfigRegister: i2c master register content (addr: 0x1a).
 * @readSteps: flag used to indicate if interrupt task need to read number of steps.
 * @sendFlushEvt: if flush is requested, send it out after FIFO read is completed.
 * @pendingEnableConfig: pending setEnable operations to be executed.
 * @pendingRateConfig: pending setRate operations to be executed.
 * @pendingInt: pending interrupt task to be executed.
 * @pendingTimeSyncTask: pending time sync task to be executed.
 * @pendingBaroTimerTask: pending baro read data task to be executed.
 * @pendingStoreAccelCalibData: pending calibration data store task to be executed.
 */
typedef struct LSM6DSMTask {
    struct LSM6DSMSensor sensors[NUM_SENSORS];
    struct LSM6DSMSPISlaveInterface slaveConn;
    struct Gpio *int1;
    struct Gpio *eint1;
    struct ChainedIsr isr1;
    struct SlabAllocator *mDataSlabThreeAxis;
    struct LSM6DSMFifoCntl fifoCntl;
    struct LSM6DSMTimeCalibration time;
    uint64_t lastFifoReadTimestamp;
    enum InitState initState;
    enum SelfTestState selftestState;
    enum CalibrationState calibrationState;
    uint32_t tid;
    uint32_t totalNumSteps;
    uint32_t fifoDataToRead;  
    uint32_t fifoDataToReadPending;
    int32_t dataSelftestEnabled[LSM6DSM_TRIAXIAL_NUM_AXIS];
    int32_t dataSelftestNotEnabled[LSM6DSM_TRIAXIAL_NUM_AXIS];
    int32_t dataCalibration[LSM6DSM_TRIAXIAL_NUM_AXIS];
    int32_t accelCalibrationData[LSM6DSM_TRIAXIAL_NUM_AXIS];
    int32_t gyroCalibrationData[LSM6DSM_TRIAXIAL_NUM_AXIS];
    volatile uint8_t state;
    uint8_t numSamplesSelftest;
    uint8_t numSamplesCalibration;
    uint8_t mRetryLeft;
    uint8_t pedometerDependencies;
    uint8_t masterConfigDependencies;
    uint8_t int1Register;
    uint8_t int2Register;
    uint8_t embeddedFunctionsRegister;
    uint8_t pendingFlush[NUM_SENSORS];
    bool readSteps;
    bool sendFlushEvt[NUM_SENSORS];
    bool pendingEnableConfig[NUM_SENSORS];
    bool pendingRateConfig[NUM_SENSORS];
    bool pendingInt;
    bool pendingTimeSyncTask;
    bool pendingStoreAccelCalibData;
} LSM6DSMTask;
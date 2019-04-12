基础知识:
		1.强引用调用会导致onFirstRef()被调用
//Android5.1版本
AudioTrack:
		应用将音频数据封装至AudioTrack中
		cpp应用程序
		java应用程序:
				AudioTrack.java-->(android_media_AudioTrack.cpp)jni-->AudioTrack.cpp
IAudioTrack:
		AudioTrack的Binder的接口，应用程序使用的AudioTrack通过IAudioTrak调用AudioFlinger
AudioFlinger:
		策略的执行者
		AudioFlinger.cpp
		audio_hw_hal.cpp
			规范音频厂家库的实现功能，一个接口文件
AudioHal:
AudioPolicyService:
		音频策略的制定者,例如当前声音从喇叭播放还是从耳机
		AudioPolicyService.cpp
			内部使用的方法
		AudioPolicyClientImpl.cpp
			用于和AudioFlinger进行binder通讯即porxy
		AudioPolicyInterfaceImp.cpp
			向别人提供服务，本身也是一个Native Binder Service
		AudioPolicyManagerBase.cpp//老方法
			用于厂家想定制策略时使用
track:音轨
音频设备:
	N:								1:						1
Track(声音来源)		thread <--> 音频设备绑定
device:
		支持哪些采样率
output:
		支持哪些device,即当前可以支持哪些设备。此处需要与profil做区别
		例如支持耳机和喇叭的codec，但是当前耳机未接入只有喇叭，那么当前output只能支持喇叭
		
hardware module:
	对应一个so文件
	支持哪些output
	用于访问声卡
profile:
		用来描述output能够支持的功能。此处与output做区分。
		例如支持耳机和喇叭的codec，但是当前耳机未接入只有喇叭，那么当前output只能支持喇叭，而profile还是描述其支持耳机和喇叭
stream type:
		声音的类型,音乐类型、电话语音
strategy:
		对声音类型分组，将相同行为的声音类型归为一组

罗升阳版本阅读:
    AudioTrack:
        AT
    AudioFlinger:
        AF
    AudioPolicyService:
        AP
    AudioHardwareInterface:
        在hal中代表一个Audio硬件
        管理着所有的音频设备
    AudioStreamOut:
        音频输出设备
    AudioStreamIn:
        音频输入设备
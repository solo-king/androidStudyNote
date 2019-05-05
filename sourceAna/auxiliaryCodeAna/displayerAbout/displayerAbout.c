rk官方开源网站：
	http://opensource.rock-chips.com/wiki_Main_Page
       //瑞芯微kernel官网
       https://github.com/rockchip-linux
硬件合成:
		1.opengl
			软件:
			硬件
		2.Hardware Composition
			硬件合成多个应用窗口界面
匿名共享驱动:
drm和framebuffer:
       //rk官方资料
       https://markyzq.gitbooks.io/rockchip_drm_integration_helper/content/zh/

drm( Direct Rendering Manager):
	参考资料:
		//DRM介绍
		https://blog.csdn.net/hexiaolong2009/article/details/83720940
		//ubuntu官方drm介绍文档，翻墙较快，国内网慢,已集成到ubunt18.04手册中,man drm-kms可以查看
		http://manpages.ubuntu.com/manpages/cosmic/en/man7/drm-kms.7.html
		//kernel官方资料
		https://www.kernel.org/doc/html/latest/gpu/drm-internals.html
基本概念:
	how KMS works, we need to introduce 5 objects: CRTCs, Planes, Encoders,
       Connectors and Framebuffers.
	CRTCs(将fremebuffer的数据硬件化，输出到Encoders，包含行列同步信号等,该组件应该是一个较标准的组件)
           A CRTC short for CRT Controller is an abstraction representing a part of the chip that
           contains a pointer to a scanout buffer. Therefore, the number of CRTCs available
           determines how many independent scanout buffers can be active at any given time. The
           CRTC structure contains several fields to support this: a pointer to some video memory
           (abstracted as a frame-buffer object), a list of driven connectors, a display mode and
           an (x, y) offset into the video memory to support panning or configurations where one
           piece of video memory spans multiple CRTCs. A CRTC is the central point where
           configuration of displays happens. You select which objects to use, which modes and
           which parameters and then configure each CRTC via drmModeCrtcSet(3) to drive the
           display devices
	Planes(应该是图层的概念,从而能够实现动态刷定画面局部细节的功能)
           A plane respresents an image source that can be blended with or overlayed on top of a
           CRTC during the scanout process. Planes are associated with a frame-buffer to crop a
           portion of the image memory (source) and optionally scale it to a destination size.
           The result is then blended with or overlayed on top of a CRTC. Planes are not provided
           by all hardware and the number of available planes is limited. If planes are not
           available or if not enough planes are available, the user should fall back to normal
           software blending (via GPU or CPU).
	Encoders(将从CRTC中接受到的数据转化为对应Connector的数据格式(例如转化为HDMI连接器所需要的格式(电平信号)),猜测一个Encoder可以输出多种不同格式的电信信号(例如mipi dsi、VGA、DP等))
           An encoder takes pixel data from a CRTC and converts it to a format suitable for any
           attached connectors. On some devices, it may be possible to have a CRTC send data to
           more than one encoder. In that case, both encoders would receive data from the same
           scanout buffer, resulting in a cloned display configuration across the connectors
           attached to each encoder.
	Connectors(直接和显示器链接，输出对应显示接口的电平信号(如HDMI电平))
           A connector is the final destination of pixel-data on a device, and usually connects
           directly to an external display device like a monitor or laptop panel. A connector can
           only be attached to one encoder at a time. The connector is also the structure where
           information about the attached display is kept, so it contains fields for display
           data, EDID data, DPMS and connection status, and information about modes supported on
           the attached displays.
	Framebuffers
           Framebuffers are abstract memory objects that provide a source of pixel data to
           scanout to a CRTC. Applications explicitly request the creation of framebuffers and
           can control their behavior. Framebuffers rely on the underneath memory manager for
           low-level memory operations. When creating a framebuffer, applications pass a memory
           handle through the API which is used as backing storage. The framebuffer itself is
           only an abstract object with no data. It just refers to memory buffers that must be
           created with the drm-memory(7) API.
基本概念:
       SurfaceControl:
       SurfaceComposerClient:
              应用与Surfaceflinger
涉及指令:
       logcat -s StrongPointMain&
基础知识:
       frameworks/rs/server/StrongPointer.h
涉及源码:
       frameworks/base/cmds/bootanimation/bootanimation_main.cpp
       frameworks/base/cmds/bootanimation/BootAnimation.cpp
       frameworks/native/libs/gui/SurfaceComposerClient.cpp
       frameworks/base/cmds/APP_0010_SurfaceTest/SurfaceTest.cpp
       /*
              1.与surfaceflinger建立联系
                    sp<SurfaceComposerClient>  mSession = new SurfaceComposerClient();
              2.确认当前系统是否已经启动完成
                     mSystemBoot = !property_get_bool(BOOT_COMPLETED_PROP_NAME, 0);
       */
       boot = new BootAnimation(false);

***************************************************************谷歌官方资料**********************************************
官方资料:
       //开启多屏支持
       https://source.android.com/devices/tech/display/multi-window
       //官方显示框架,很重要
       https://source.android.com/devices/graphics/architecture
开启free mode:
       开发者模式开启
       勾上force activities to be resizable 选项
       adb shell settings get global enable_freeform_support
       reboot
使用free mode 
       1.先开启要多窗口的应用
       2.点击右上角的多窗口选项
Surface
       java层的画布
Canvas:
       Canvas APIs provide a software implementation (with hardware-acceleration support) for drawing directly on a Surface (low-level alternative to OpenGL ES)
       使用该 Canvas接口可以直接在Surface上作画
SurfaceHolder
       Anything having to do with a View involves a SurfaceHolder, whose APIs enable getting and setting Surface parameters such as size and format.
       貌似作用就是配置Surface
OpenGL ES：
        1.OpenGL ES (GLES) defines a graphics-rendering API designed to be combined with EGL
        2.a library that knows how to create and access windows through the operating system
EGLSurface
       This page also covers ANativeWindow, the C/C++ equivalent of the Java Surface class used to create an EGL window surface from native code.
       c/c++层面的Surface,与java层的Surface等价，一般使用ANativeWindow封装 EGLSurface
SurfaceView
       SurfaceView combines a Surface and a View. SurfaceView s View components are composited by SurfaceFlinger (and not the app), 
       enabling rendering from a separate thread/process and isolation from app UI rendering. 
       GLSurfaceView provides helper classes to manage EGL contexts, inter-thread communication, 
       and interaction with the Activity lifecycle (but is not required to use GLES).
GLSurfaceView
SurfaceTexture
       SurfaceTexture combines a Surface and GLES texture to create a BufferQueue for which your app is the consumer.
       When a producer queues a new buffer, it notifies your app, which in turn releases the previously-held buffer, 
       acquires the new buffer from the queue, and makes EGL calls to make the buffer available to GLES as an external texture. 
       Android 7.0 adds support for secure texture video playback enabling GPU post-processing of protected video content.
TextureView
       TextureView combines a View with a SurfaceTexture. 
       TextureView wraps a SurfaceTexture and takes responsibility for responding to callbacks and acquiring new buffers. 
       When drawing, TextureView uses the contents of the most recently received buffer as its data source, 
       rendering wherever and however the View state indicates it should. View composition is always performed with GLES, 
       meaning updates to contents may cause other View elements to redraw as well.
SurfaceFlinger
Vulkan
       1.low-overhead
       2.cross-platform API for high-performance 3D graphics. 
       3.类似于OpenGL ES, Vulkan provides tools for creating high-quality, real-time graphics in applications. 
       4.Vulkan advantages include reductions in CPU overhead and support for the SPIR-V Binary Intermediate language.
BufferQueue：      
       1.连接生产者和消费者的图型数据




rk官方开源网站：
	http://opensource.rock-chips.com/wiki_Main_Page
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

AudioFlinger:
重要数据结构：

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
	
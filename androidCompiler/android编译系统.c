Makefile基础
    见<GUNMake.c>
环境变量
    编译系统涉及的前缀变量:
        LOCAL_ - These variables are set per-module. They are cleared by the include $(CLEAR_VARS) line, 
            so you can rely on them being empty after including that file. Most of the variables you well use in most modules are LOCAL_ variables.
        PRIVATE_ - These variables are make-target-specific variables. That means they are only usable within the commands for that module. 
            It also means that they are unlikely to change behind your back from modules that are included after yours. 
            This link to the make documentation describes more about target-specific variables.
        HOST_ and TARGET_ - These contain the directories and definitions that are specific to either the host or the target builds. 
            Do not set variables that start with HOST_ or TARGET_ in your makefiles.
        BUILD_ and CLEAR_VARS - These contain the names of well-defined template makefiles to include. 
            Some examples are CLEAR_VARS and BUILD_HOST_PACKAGE.
            Any other name is fair-game for you to use in your Android.mk. However, remember that this is a non-recursive build system, 
            so it is possible that your variable will be changed by another Android.mk included later, 
            and be different when the commands for your rule / module are executed
    所有的LOCAL环境变量位置:
        <androidSource>/build/core/clear_vars.mk
    include $(BUILD_PACKAGE)
        build/core/package.mk
    
Android.mk的调试
    基本描述:
        1.是 GNU makefile的一个片段，并且其会被编译系统使用一次或多次,所以在编译期间不要有以下操作
          The file is really a tiny GNU Makefile fragment that will be parsed one or more times by the build system        
            1.1 不要在编译系统解析Android.mk期间改变Android.mk
            do not assume that anything is not defined during parsing
        2. The file syntax is designed to allow you to group your sources into 'modules'
            - a static library 
            - a shared library
            NOTE:
                Only shared libraries will be installed/copied to your application package. Static libraries can be used to generate shared libraries though.
        3. You can define one or more modules in each Android.mk file, and you can use the same source file in several modules.
        4. 编译系统会自动处理一些细节，例如源文件的头文件及其依赖关系
    能够打印编译过程中的环境变量
        $(error PATH$(LOCAL_PATH))
        $(warning $(parameter))
    跟踪编译过程的详细log输出
编译c++项目
    可编译出动态/静态执行文件
    可编译出动态库
将现有Makefile版本的程序移植使用Makefile编译

参考资料:
        https://blog.csdn.net/getnextwindow/article/details/48158873
    //关于kati
        http://note.qidong.name/2017/08/android-kati/
    //好文
        https://www.ibm.com/developerworks/cn/
    官方参考资料:
        http://android.mk/
    将APP集成到固件中参考资料:
        https://blog.csdn.net/u010218230/article/details/79153986
    使用三方库或jar:
        https://blog.csdn.net/thl789/article/details/7918093

变量描述:
    LOCAL_PATH := $(call my-dir)
        is used to return the path of the current directory (i.e. the directory containing the Android.mk file itself).
    include $(CLEAR_VARS)
        will clear many LOCAL_XXX variables for you (e.g. LOCAL_MODULE, LOCAL_SRC_FILES, LOCAL_STATIC_LIBRARIES, etc...),
        with the exception of LOCAL_PATH. This is needed because all build control files are parsed in a single GNU Make execution context where all variables are global.
    LOCAL_MODULE := hello-jni
        Note:
            1.此处如果编译的是include $(BUILD_SHARED_LIBRARY),即动态库的话,
                1.LOCAL_MODULE := hello-jni，最终名字为libhello-jni.so
                2.LOCAL_MODULE := llibhello-jni,最终名字还是为libhello-jni.so，为了和以前的项目Android.mk兼容
                If you name your module 'libfoo', the build system will not add another 'lib' prefix and will generate libfoo.so as well. This is to support Android.mk files that originate from the Android platform sources, would you need to use these.
    LOCAL_CPP_EXTENSION:
        用于自定义c++源文件后缀，默认为cpp
    TARGET_ARCH
        Name of the target CPU architecture as it is specified by the full Android open-source build. This is 'arm' for any ARM-compatible build, independent of the CPU architecture revision.
Android编译系统提供函数:
    使用方法
        与GNU makefile一致,$(call xxx)//xxx为函数名称
    my-dir
        返回Android.mk所在的路径
            LOCAL_PATH := $(call my-dir)
        注意:
            1.LOCAL_PATH := $(call my-dir)申明要放在所有模块的前面
            2.对于1中所描述的问题也可以定义新的变量
                MY_LOCAL_PATH := $(call my-dir)
                LOCAL_PATH := $(MY_LOCAL_PATH)
                ... declare one module
                include $(LOCAL_PATH)/foo/Android.mk
                LOCAL_PATH := $(MY_LOCAL_PATH)
                ... declare another module
    all-subdir-makefiles:
        Returns a list of Android.mk located in all sub-directories of the current 'my-dir' path.
        For example, consider the following hierarchy:
            sources/foo/Android.mk
            sources/foo/lib1/Android.mk
            sources/foo/lib2/Android.mk
        If sources/foo/Android.mk contains the single line:
            include $(call all-subdir-makefiles)
        Then it will include automatically sources/foo/lib1/Android.mk and sources/foo/lib2/Android.mk 
        This function can be used to provide deep-nested source directory hierarchies to the build system. 
        Note that by default, the NDK will only look for files in sources/xx/Android.mk*
    this-makefile
        Returns the path of the current Makefile (i.e. where the function is called).
    parent-makefile
         Returns the path of the parent Makefile in the inclusion tree, i.e. the path of the Makefile that included the current one.
    import-module
        A function that allows you to find and include the Android.mk of another module by name. 
        A typical example is:
            $(call import-module,<name>)
            And this will look for the module tagged <name> in the list of directories referenced by your NDK_MODULE_PATH environment variable, 
            and include its Android.mk automatically for you. Read docs/IMPORT-MODULE.html for more details
模块描述变量:
    LOCAL_PATH:
        This variable is used to give the path of the current file. You MUST define it at the start of your Android.mk, 
        which can be done with:
        LOCAL_PATH := $(call my-dir)
        注意:
            1.在 include $(CLEAR_VARS) 后并不会清除LOCAL_PATH变量
    LOCAL_MODULE:
        1.This is the name of your module. 
        2.It must be unique among all module names
        3.shall not contain any space. 
        4.You MUST define it before including any $(BUILD_XXXX) script. 
        5.By default, the module name determines the name of generated files, e.g. lib<foo>.so for a shared library module named <foo>. 
        6.You should only refer to other modules with their 'normal' name (e.g. <foo>) in your NDK build files (either Android.mk or Application.mk)
    LOCAL_MODULE_FILENAME:
        1.This variable is optional, and allows you to redefine the name of generated files. 
        2.By default, module <foo> will always generate a static library named lib<foo>.a or a shared library named lib<foo>.so,
        which are standard Unix conventions. You can override this by defining LOCAL_MODULE_FILENAME, For example:
            LOCAL_MODULE := foo-version-1
            LOCAL_MODULE_FILENAME := libfoo
        NOTE:
            1.你不需要规定路径和拓展名，这些在编译系统里被自动处理。
    LOCAL_SRC_FILES：
        This is a list of source files that will be built for your module. 
        Only list the files that will be passed to a compiler, since the build system automatically computes dependencies for you.
        Note that source files names are all relative to LOCAL_PATH and you can use path components, e.g.:
            LOCAL_SRC_FILES := foo.c \
                                toto/bar.c
        NOTE: Always use Unix-style forward slashes (/) in build files. Windows-style back-slashes will not be handled properly.
    LOCAL_CPP_EXTENSION：
        This is an optional variable that can be defined to indicate the file extension of C++ source files. 
        The default is '.cpp' but you can change it. For example:
            LOCAL_CPP_EXTENSION := .cxx
        Since NDK r7, you can list several extensions in this variable, as in:
            LOCAL_CPP_EXTENSION := .cxx .cpp .cc
    LOCAL_CPP_FEATURES：
        This is an optional variable that can be defined to indicate that your code relies on specific C++ features. 
        To indicate that your code uses RTTI (RunTime Type Information), use the following:
            LOCAL_CPP_FEATURES := rtti
        To indicate that your code uses C++ exceptions, use:
            LOCAL_CPP_FEATURES := exceptions
        You can also use both of them with (order is not important):
            LOCAL_CPP_FEATURES := rtti exceptions
        The effect of this variable is to enable the right compiler/linker flags when building your modules from sources. 
        For prebuilt binaries, this also helps declare which features the binary relies on to ensure the final link works correctly. 
        It is recommended to use this variable instead of enabling -frtti and -fexceptions directly in your LOCAL_CPPFLAGS definition.    
    LOCAL_C_INCLUDES：
        An optional list of paths, relative to the NDK *root* directory, which will be appended to the include search path when compiling all sources (C, C++ and Assembly). For example:
            LOCAL_C_INCLUDES := sources/foo
        Or even:
            LOCAL_C_INCLUDES := $(LOCAL_PATH)/../foo
        These are placed before any corresponding inclusion flag in LOCAL_CFLAGS / LOCAL_CPPFLAGS The LOCAL_C_INCLUDES path are also used automatically when launching native debugging with ndk-gdb.
    LOCAL_CFLAGS:
        An optional set of compiler flags that will be passed when building C *and* C++ source files. 
        This can be useful to specify additional macro definitions or compile options. 
        IMPORTANT: 
            Try not to change the optimization/debugging level in your Android.mk, 
            this can be handled automatically for you by specifying the appropriate information in your Application.mk, 
            and will let the NDK generate useful data files used during debugging. 
            NOTE: 
                1.在android-ndk-1.5_r1开始该变量只作用于c文件，而不对c++文件生效,如果需要c++也生效请用LOCAL_CPPFLAGS变量
    LOCAL_CXXFLAGS:
        An alias for LOCAL_CPPFLAGS. Note that use of this flag is obsolete as it may disappear in future releases of the NDK.      
    LOCAL_STATIC_LIBRARIES:
        The list of static libraries modules (built with BUILD_STATIC_LIBRARY) that should be linked to this module.
        注意:
            只在编译动态/静态共享库时生效
    LOCAL_SHARED_LIBRARIES:
        The list of shared libraries *modules* this module depends on at runtime. 
        This is necessary at link time and to embed the corresponding information in the generated file.
    
    
APK编译:
    http://android.mk/#simpleAPK
    LOCAL_PRIVILEGED_MODULE := true
        决定apk安装位置,false:/system/app
                      true:/system/priv-app
    包含AIDL文件:
    LOCAL_SRC_FILES := $(call all-java-files-under, src) $(call all-Iaidl-files-under, src)
    #指定AndroidManifest.xml
    LOCAL_MANIFEST_FILE := app/src/main/AndroidManifest.xml
编译应用参考模板
    LOCAL_PATH := $(call my-dir)
        函数定义:
            build/core/definitions.mk:133:define my-dir

    include $(CLEAR_VARS)
        build/core/main.mk:58:BUILD_SYSTEM := $(TOPDIR)build/core
        build/core/config.mk:79:CLEAR_VARS:= $(BUILD_SYSTEM)/clear_vars.mk
        实际目录:
            build/core/clear_vars.mk
            #从而推出
            BUILD_SYSTEM == $(TOPDIR)build/core == build/core/
        实际上就是调用:=对各个LOCAL_XXX进行清零
    res_dirs = app/src/main/res
    LOCAL_MANIFEST_FILE := app/src/main/AndroidManifest.xml
    LOCAL_SRC_FILES := $(call all-java-files-under,src) $(call all-Iaidl-files-under, src)\
                            app/src/main/aidl/cn/usr/www/usrbroadcastcentor/BookManager.aidl
    LOCAL_RESOURCE_DIR := $(addprefix $(LOCAL_PATH)/, $(res_dirs))
    LOCAL_PACKAGE_NAME := UsrBroadcastCentor
    LOCAL_MODULE_TAGS := eng
    LOCAL_PROGUARD_ENABLED := disabled
    LOCAL_CERTIFICATE := platform
    LOCAL_PRIVILEGED_MODULE := true
    LOCAL_DEX_PREOPT := false
    include $(BUILD_PACKAGE)
DSL
    所谓DSL领域专用语言(domain specific language / DSL)，其基本思想是“求专不求全”，不像通用目的语言那样目标范围涵盖一切软件问题，而是专门针对某一特定问题的计算机语言。
Ninjia编译工具链大致关系:
    Makefile/Android.mk -> Kati -> Ninja
    Android.bp -> Blueprint -> Soong -> Ninja
Android.bp:
    是用来替换Android.mk的配置文件。 它使用Blueprint框架来解析，最终转换成Ninja文件。

编译新产品大致步骤:
source build/envsetup.sh
lunch中的配置从哪来:
    devices/xxx/vendorsetup.sh
定制设备需添加如下几个脚本:
    vendorsetup.sh
    AndroidProducts.mk
    BoardConfig.mk
    Android.mk
Android6.0编译系统:
    1.最后的一个版本使用gnu make 进行对Android源码进行编译

Android.mk 编译模板:
//Building a simple APK
  LOCAL_PATH := $(call my-dir)
  include $(CLEAR_VARS)
   
  # Build all java files in the java subdirectory
  LOCAL_SRC_FILES := $(call all-subdir-java-files)
   
  # Name of the APK to build
  LOCAL_PACKAGE_NAME := LocalPackage
   
  # Tell it to build an APK
  include $(BUILD_PACKAGE)

//Building a APK that depends on a static .jar file
  LOCAL_PATH := $(call my-dir)
  include $(CLEAR_VARS)
  # List of static libraries to include in the package
  LOCAL_STATIC_JAVA_LIBRARIES := static-library
   
  # Build all java files in the java subdirectory
  LOCAL_SRC_FILES := $(call all-subdir-java-files)
   
  # Name of the APK to build
  LOCAL_PACKAGE_NAME := LocalPackage
   
  # Tell it to build an APK
  include $(BUILD_PACKAGE)

//Building a APK that should be signed with the platform key
  LOCAL_PATH := $(call my-dir)
  include $(CLEAR_VARS)
   
  # Build all java files in the java subdirectory
  LOCAL_SRC_FILES := $(call all-subdir-java-files)
   
  # Name of the APK to build
  LOCAL_PACKAGE_NAME := LocalPackage
   
  LOCAL_CERTIFICATE := platform
   
  # Tell it to build an APK
  include $(BUILD_PACKAGE)

//Building a APK that should be signed with a specific vendor key
  LOCAL_PATH := $(call my-dir)
  include $(CLEAR_VARS)
   
  # Build all java files in the java subdirectory
  LOCAL_SRC_FILES := $(call all-subdir-java-files)
   
  # Name of the APK to build
  LOCAL_PACKAGE_NAME := LocalPackage
   
  LOCAL_CERTIFICATE := vendor/example/certs/app
   
  # Tell it to build an APK
  include $(BUILD_PACKAGE)

//Adding a prebuilt APK
  LOCAL_PATH := $(call my-dir)
  include $(CLEAR_VARS)
   
  # Module name should match apk name to be installed.
  LOCAL_MODULE := LocalModuleName
  LOCAL_SRC_FILES := $(LOCAL_MODULE).apk
  LOCAL_MODULE_CLASS := APPS
  LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
  include $(BUILD_PREBUILT)

//Adding a Static Java Library
  LOCAL_PATH := $(call my-dir)
  include $(CLEAR_VARS)
   
  # Build all java files in the java subdirectory
  LOCAL_SRC_FILES := $(call all-subdir-java-files)
   
  # Any libraries that this library depends on
  LOCAL_JAVA_LIBRARIES := android.test.runner
   
  # The name of the jar file to create
  LOCAL_MODULE := sample
   
  # Build a static jar file.
  include $(BUILD_STATIC_JAVA_LIBRARY)




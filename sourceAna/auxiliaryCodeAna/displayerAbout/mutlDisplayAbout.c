官方资料:
       //开启多屏支持
       https://source.android.com/devices/tech/display/multi-window
       //官方显示框架,很重要
       https://source.android.com/devices/graphics/architecture
       //可以使系统默认开启 freeform
       https://github.com/farmerbb/Taskbar
       //导航栏加载
       https://juejin.im/post/5ad9a077f265da0b767d0669
       //近期任务，官方资料
       https://developer.android.com/guide/components/activities/recents
开启 Freeform:
       开发者模式开启
       勾上force activities to be resizable 选项
       adb shell settings get global enable_freeform_support
       settings get global enable_freeform_support
       settings put global enable_freeform_support 1

近期任务:
ActivityManager: START u0 {flg=0x10804000 cmp=com.android.systemui/.recents.RecentsActivity} from uid 10014
ActivityManager: getPackageFerformanceMode--ComponentInfo{com.android.systemui/com.android.systemui.recents.RecentsActivity}----com.android.systemui

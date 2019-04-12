使用repo欲解决问题:
    1.直接使用git分支切换过慢问题
    2.对于模块能建立独立的分支
repo init --repo-url ssh://git@www.rockchip.com.cn/repo/rk/tools/repo -u ssh://git@www.rockchip.com.cn/gerrit/rk/platform/manifest -b android-7.0 -m rk3288_tablet_nougat_release.xml
.repo/repo/repo sync -l
.repo/repo/repo sync -c --no-tags
参考资料:
    https://blog.csdn.net/u013171283/article/details/80895125
    http://duanqz.github.io/2015-06-25-Intro-to-Repo#32-sync
    //repo官方仓库
    https://gerrit.googlesource.com/git-repo/
    //官方文档
    https://source.android.com/source/using-repo.html
疑问点:
    1.repo克隆分支如何加快
        使用repo start <BRANCH> --all对所有git仓库穿创建git分支,若已有分支则未切换
    2.
对多个git仓库执行同一种命令
    repo forall [PROJECT_LIST] -c <COMMAND>
        1.PROJECT_LIST中以空格隔开
        2.如果没有指定PROJECT_LIST，那么，会对repo管理的所有git库都同时执行命令。
    例子:
        .repo/repo/repo forall external/rk-pcba-test/ RKTools/ bootable/recovery/ build/ device/rockchip/common/ device/rockchip/rk3288/ external/f2fs-tools/ external/libselinux/ external/wlan_loader/ external/wpa_supplicant_8/ frameworks/av/ frameworks/base/ frameworks/native/ frameworks/opt/net/pppoe/ frameworks/opt/net/wifi/ hardware/broadcom/libbt/ hardware/rockchip/audio/ hardware/rockchip/camera/ hardware/rockchip/hwcomposer/ hardware/rockchip/libgralloc/ hardware/rockchip/liblights/ hardware/rockchip/librga/ hardware/rockchip/librkvpu/  hardware/rockchip/omx_il/ hardware/rockchip/power/ hardware/rockchip/sensor/ kernel/ packages/apps/Bluetooth/ packages/apps/Camera2/ packages/apps/DeskClock/ packages/apps/Gallery2/ packages/apps/Launcher3/ packages/apps/Music/ packages/apps/QuickSearchBox/ packages/apps/Settings/ packages/apps/WallpaperPicker/ system/bt/ system/core/ system/displayd/ system/vold/ u-boot/ vendor/rockchip/common/  -c "git config core.filemode false"
        .repo/repo/repo forall external/rk-pcba-test/ RKTools/ bootable/recovery/ build/ device/rockchip/common/ device/rockchip/rk3288/ external/f2fs-tools/ external/libselinux/ external/wlan_loader/ external/wpa_supplicant_8/ frameworks/av/ frameworks/base/ frameworks/native/ frameworks/opt/net/pppoe/ frameworks/opt/net/wifi/ hardware/broadcom/libbt/ hardware/rockchip/audio/ hardware/rockchip/camera/ hardware/rockchip/hwcomposer/ hardware/rockchip/libgralloc/ hardware/rockchip/liblights/ hardware/rockchip/librga/ hardware/rockchip/librkvpu/  hardware/rockchip/omx_il/ hardware/rockchip/power/ hardware/rockchip/sensor/ kernel/ packages/apps/Bluetooth/ packages/apps/Camera2/ packages/apps/DeskClock/ packages/apps/Gallery2/ packages/apps/Launcher3/ packages/apps/Music/ packages/apps/QuickSearchBox/ packages/apps/Settings/ packages/apps/WallpaperPicker/ system/bt/ system/core/ system/displayd/ system/vold/ u-boot/ vendor/rockchip/common/  -c "git reset --hard HEAD "
避免在匿名分支工作
    在sync完代码后，所有git库默认都是在一个匿名分支上(no branch)，很容易会由于误操作导致丢失代码修改。可以使用如下命令将所有的git库切换到开发分支
        repo start <BRANCH> --all
修改提交:
    到project下使用git命令
撤销修改:
    到project下使用git命令
创建分支: 
    repo start <BRANCH> --all
合并分支:
    到project下使用git命令
切换分支:
    repo start <BRANCH> --all  
删除已经合并后的分支:
    repo prune [<PROJECT_LIST>]
    说明:
        删除指定PROJECT中，已经合并的分支。当在开发分支上代码已经合并到主干分支后，使用该命令就可以删除这个开发分支。
    例子:
        随着时间的演进，开发分支会越来越多，在多人开发同一个git库，多开发分支的情况会愈发明显，假设当前git库有如下分支：
        * master
        dev_feature1_201501   # 已经合并到master
        dev_feature2_201502   # 已经合并到master
        dev_feature3_201503   # 正在开发中，还有改动记录没有合并到master
        那么，针对该git库使用prune命令，会删除 dev_feature1_201501 和 dev_feature2_201502。
查看project list 项目状态
    repo status [<PROJECT_LIST>]
    status用于查看多个git库的状态。实际上，是对git status命令的封装。
对项目清单进行定制:
    通过local_manifest机制，能够避免了直接修改default.xml，不会造成下次同步远程清单文件的冲突。
添加一个path至repo清单中:
    将定制的XML放在新建的.repo/local_manifests子目录即可文件名可以自定义，譬如local_manifest.xml, another_local_manifest.xml等， 
    将定制的XML放在新建的.repo/local_manifests子目录即可。repo会遍历.repo/local_manifests目录下的所有*.xml文件，
    最终与default.xml合并成一个总的项目清单文件manifest.xml。
    例子:
        $ ls .repo/local_manifests
        local_manifest.xml
        another_local_manifest.xml

        $ cat .repo/local_manifests/local_manifest.xml
        <?xml version="1.0" encoding="UTF-8"?>
        <manifest>
            <project path="manifest" name="tools/manifest" />
            <project path="platform-manifest" name="platform/manifest" />
        </manifest>
仓库目录和工作目录:
    manifests： 
        仓库目录有两份拷贝，一份位于工作目录(.repo/manifests)的.git目录下，另一份独立存放于.repo/manifests.git
    repo：
        仓库目录位于工作目录(.repo/repo)的.git目录下
    project：
        所有被管理git库的仓库目录都是分开存放的，位于.repo/projects目录下。
        同时，也会保留工作目录的.git，但里面所有的文件都是到.repo的链接。
        这样，即做到了分开存放，也兼容了在工作目录下的所有git命令
    Note:
        既然.repo目录下保存了项目的所有信息，所有要拷贝一个项目时，只是需要拷贝这个目录就可以了。repo支持从本地已有的.repo中恢复原有的项目。
            


    

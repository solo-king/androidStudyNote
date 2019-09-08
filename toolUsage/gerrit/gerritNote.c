################################################################################################################################
##################################################  环境搭建 ####################################################################
##################################################  环境搭建 ####################################################################
################################################################################################################################
参考资料:
    //官方资料
    https://gerrit-documentation.storage.googleapis.com/Documentation/3.0.2/linux-quickstart.html
一、依赖工具
    sudo apt install git
    //gerit只能使用8版本
    sudo apt install openjdk-8-jdk
二、基本操作
    //开启初始化一个gerrit服务器
    //gerrit-2.15.13.war 则从官网下载即可
    java -jar gerrit-2.15.13.war init --batch --dev -d ~/gerrit_testsite
    //改变监听url
    git config --file ~/gerrit_testsite/etc/gerrit.config httpd.listenUrl 'http://localhost:8080'
    //改变完配置后重启下gerrit
    ~/gerrit_testsite/bin/gerrit.sh restart
    //登入gerrit
    http://localhost:8080

问题解决:
    1.  权限问题
        Starting Gerrit Code Review: WARNING: Could not adjust Gerrit's process for the kernel's out-of-memory killer.
         This may be caused by /home/qigan/gerrit_testsite/bin/gerrit.sh not being run as root.
         Consider changing the OOM score adjustment manually for Gerrit's PID= with e.g.:
         echo '-1000' | sudo tee /proc//oom_score_adj
        //获得pid为18985
        ps -aux|grep gerrit
        echo '-1000'|sudo tee  /proc/18985/oom_score_adj

########################################################################################################################
######################################## 通用操作 ######################################################################
######################################## 通用操作 ######################################################################
########################################################################################################################
一、参考资料:
    https://gerrit-documentation.storage.googleapis.com/Documentation/3.0.2/intro-gerrit-walkthrough.html

二、添加管理员

三、添加3个用户
    其一为 code developer
    其二、其三为 code reviewer
四、如何将项目托管到gerrit中去

五、code developer如何提交修改过的代码

六、code reviewer 如何审阅代码

七、代码管理员如何合并代码

The refs/for/[BRANCH_NAME] syntax描述:
    refs/changes/[CD]/[ABCD]/[EF]
    Where:
        [CD] is the last two digits of the change number
        [ABCD] is the change number
        [EF] is the patch set number
    For example:
        refs/changes/20/884120/1
You can use the change reference to fetch its corresponding commit:
    git fetch https://[GERRIT_SERVER_URL]/[PROJECT] refs/changes/[XX]/[YYYY]/[ZZ] && git checkout FETCH_HEAD

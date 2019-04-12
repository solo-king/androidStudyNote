基础知识:
    poll:
        struct pollfd {
               int   fd;         /* file descriptor */
               short events;     /* requested events */
               short revents;    /* returned events */
           };

    epoll

    inotify
        inotify_init():
            creates an inotify instance and returns a file descriptor referring to the inotify instance.
        inotify_add_watch(2):
              manipulates the "watch list" associated with an inotify instance.  Each item ("watch") in the watch list specifies the pathname of a file or directory, 
              along with some set of events that the kernel should monitor for the file referred to by that pathname.  inotify_add_watch(2) either creates a new watch item, 
              or modifies an existing watch.   Each  watch  has  a  unique "watch descriptor", an integer returned by inotify_add_watch(2) when the watch is created.
        可以使用read从fd中读取事件:
            When  events  occur  for monitored files and directories, those events are made available to the application as structured data that can be read from 
            the inotify file descriptor using read(2) (see below).
        inotify_rm_watch(2)
             removes an item from an inotify watch list
        With  careful  programming, an application can use inotify to efficiently monitor and cache the state of a set of filesystem objects.  However, 
        robust applications should allow for the fact that bugs in the monitoring logic or races of the kind described below may leave the cache 
        inconsistent with the filesystem state.  It is probably wise to do some consistency checking, and  rebuild  the  cache when inconsistencies are detected.
        返回数据的结构体:
            struct inotify_event {
               int      wd;       /* Watch descriptor */
               uint32_t mask;     /* Mask describing event */
               uint32_t cookie;   /* Unique cookie associating related
                                     events (for rename(2)) */
               uint32_t len;      /* Size of name field */
               char     name[];   /* Optional null-terminated name */
           };
        mask:
            IN_CREATE
                File/directory created in watched directory (e.g., open(2) O_CREAT, mkdir(2), link(2), symlink(2), bind(2) on a UNIX domain socket).
            IN_DELETE
                File/directory deleted from watched directory.
            IN_DELETE_SELF
                Watched  file/directory  was  itself  deleted.   (This event also occurs if an object is moved to another filesystem, since mv(1) in effect copies the file to the other filesystem and then
                  deletes it from the original filesystem.)  In addition, an IN_IGNORED event will subsequently be generated for the watch descriptor.
        编程必备接口:
            1. 创建一个inoty句柄
                fd = inotify_init1(IN_NONBLOCK);
                if (fd == -1) {
                    perror("inotify_init1");
                    exit(EXIT_FAILURE);
                }
            2. 添加一个文件路径到inoty句柄中，后面改路径文件发生变化将导致fd变化
                wd[i] = inotify_add_watch(fd, argv[i],
                                         IN_OPEN | IN_CLOSE);
            3. 读取事件
                const struct inotify_event *event;
                len = read(fd, buf, sizeof buf);
            4.对比fd，从而知道哪个文件出现变化
                for (i = 1; i < argc; ++i) {
                        if (wd[i] == event->wd) {
                            printf("%s/", argv[i]);
                            break;
                        }
                    }
            
问题：
    1. 实现监听一个文件夹下文件的删除、添加、文件内容的变化


*.idc
	对指定输入设备进行一些特定的配置,例如指定该设备是否为内嵌设备
*.kl	
	使用KeyLayoutMap类描述
*.kcm
	使用KeyCharacterMap类描述
EventHub.cpp
	openDeviceLocked()

在EventHub及InputReader中都各自维护一个mDevices的原因
		1. 做分层
		2. EventHub的主要用于读取数据，而InputReader中的主要用于处理

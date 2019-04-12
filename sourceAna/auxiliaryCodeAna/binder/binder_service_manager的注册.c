//驱动的注册binder.c
static const struct file_operations binder_fops = {
	.owner = THIS_MODULE,
	.poll = binder_poll,
	.unlocked_ioctl = binder_ioctl,
	.compat_ioctl = binder_ioctl,
	.mmap = binder_mmap,
	.open = binder_open,
	.flush = binder_flush,
	.release = binder_release,
};
binder_init(void)
    init_binder_device(const char *name)
        binder_device->miscdev.fops = &binder_fops;
        binder_device->miscdev.minor = MISC_DYNAMIC_MINOR;
        binder_device->miscdev.name = name;
        binder_device->context.binder_context_mgr_uid = INVALID_UID;
        binder_device->context.name = name;
        ret = misc_register(&binder_device->miscdev);
/*
    服务的注册:
        ①frameworks/native/cmds/servicemanager/service_manager.c
        ②frameworks/native/cmds/servicemanager/binder.c
        ③kernel/drivers/android/binder.c
*/
main()                                                                  //①
    bs = binder_open(128*1024);
        /*主要创建struct binder_proc *proc结构体*/
        bs->fd = open("/dev/binder", O_RDWR | O_CLOEXEC);
            struct binder_proc *proc;                                  //③处的代码
	        struct binder_device *binder_dev;
            /*疑问:
                filp->private_data是何时被赋值为miscdev的
                获取binder_device
            */
            binder_dev = container_of(filp->private_data, struct binder_device, miscdev);
            //binder_device->context.binder_context_mgr_uid = INVALID_UID;
            proc->context = &binder_dev->context;
            filp->private_data = proc;
        bs->mapsize = mapsize;
        /*
            PROT_READ  Pages may be read.
            MAP_PRIVATE
              Create a private copy-on-write mapping.
            mapsize :128*1024==128KB
        */
        bs->mapped = mmap(NULL, mapsize, PROT_READ, MAP_PRIVATE, bs->fd, 0);
    binder_become_context_manager(bs)                                //①
        ioctl(bs->fd, BINDER_SET_CONTEXT_MGR, 0);
            binder_ioctl_set_ctx_mgr(filp);                          //③
                //binder_context_mgr_node为不为NULL代表已被注册过
                context->binder_context_mgr_node
                //还没有大管家，那么将新建一个binder_node表示service_manager在kernel中的对象
                new_node = binder_new_node(proc, NULL);//做了一系列的初始化
                context->binder_context_mgr_node = new_node;
    binder_loop(bs, svcmgr_handler);                                //①
        struct binder_write_read bwr;
        bwr.write_size = 0;
        bwr.write_consumed = 0;
        bwr.write_buffer = 0;
        readbuf[0] = BC_ENTER_LOOPER;
        binder_write(bs, readbuf, sizeof(uint32_t));
            bwr.write_size = len;//4
            bwr.write_consumed = 0;
            bwr.write_buffer = (uintptr_t) data;//readbuf[0] = BC_ENTER_LOOPER
            bwr.read_size = 0;
            bwr.read_consumed = 0;
            bwr.read_buffer = 0;
            //最终通过ioctl将binder_write_read发送出去
            res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);           
                ret = binder_ioctl_write_read(filp, BINDER_WRITE_READ, arg, thread);
                        copy_from_user(&bwr, ubuf, sizeof(bwr))//复制用户空间的bwr      //③
                        // BC_ENTER_LOOPER 为BINDER_WRITE_READ中的子命令
                        ret = binder_thread_write(proc, thread, bwr.write_buffer, bwr.write_size, &bwr.write_consumed);
                        //设置该线程标志，代表其进入了循环
                        thread->looper |= BINDER_LOOPER_STATE_ENTERED;
            for:
                bwr.read_size = sizeof(readbuf);
                bwr.read_consumed = 0;
                bwr.read_buffer = (uintptr_t) readbuf;//uint32_t readbuf[32];
                res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
                    ret = binder_thread_read(proc, thread, bwr.read_buffer, bwr.read_size, &bwr.read_consumed,filp->f_flags & O_NONBLOCK);
                //解析从内核中读到的数据
                binder_parse(bs, 0, (uintptr_t) readbuf, bwr.read_consumed, func);//func==svcmgr_handler
遗留问题:
    1. client是如何通过handle与对应的Service通讯的
    2. 一个server中有多个服务时是如何区分的-->ThreadState
    3. client是通过哪个数据指定调用service中的函数(方法的)
    4. todo和 wait 链表作用

binder_init入口函数总结:
    1.所有的binder_proc都挂载binder_procs链表下
    2.binder驱动中binder_init(void)做了如下几件事
        2.1 binder_alloc_shrinker_init//初始化binder缓冲区
        2.2 初始化一个工作队列
            binder_deferred_workqueue = create_singlethread_workqueue("binder");
        2.3 根据配置的名字生成设备，暂且只分析binder节点,并将所有的binder_device挂到binder_devices设备链表中去
            CONFIG_ANDROID_BINDER_DEVICES="binder,hwbinder,vndbinder"
            init_binder_device("binder");
                binder_device->miscdev.fops = &binder_fops;
                binder_device->miscdev.minor = MISC_DYNAMIC_MINOR;
                binder_device->miscdev.name = name;//"binder"
                binder_device->context.binder_context_mgr_uid = INVALID_UID;
                binder_device->context.name = name;//"binder"
binder_open函数总结:
    1. 设置所属任务
        proc->tsk = current->group_leader;
    2. 初始化一个todo链表，struct list_head todo，只要包含struct list_head的结构体都能挂上来
        INIT_LIST_HEAD(&proc->todo);
    3. 对binder_proc数据结构进行初始化,并将binder_proc挂到 binder_procs 全局链表的头部,及文件指针的私有数据中以便其他函数使用
        //这里其实就是binder_init阶段中使用init_binder_device建立的binder device
        binder_dev = container_of(filp->private_data, struct binder_device, miscdev);
        /*
            proc->context.binder_context_mgr_uid = INVALID_UID
            proc->context.name = "binder"
        */
        proc->context = &binder_dev->context;
        filp->private_data = proc;
        hlist_add_head(&proc->proc_node, &binder_procs);
        filp->private_data = proc;

    

                










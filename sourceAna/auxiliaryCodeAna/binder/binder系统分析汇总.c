1.c语言层的调用
必要知识点:
    1.DEFINE_WAIT:
        #define DEFINE_WAIT(name) DEFINE_WAIT_FUNC(name, autoremove_wake_function)
            #define DEFINE_WAIT_FUNC(name, function)				\
                wait_queue_t name = {						\
                    .private	= current,				\
                    .func		= function,				\
                    .task_list	= LIST_HEAD_INIT((name).task_list),	\
                }
    2.autoremove_wake_function
        参考资料:
            https://blog.csdn.net/tiantao2012/article/details/78798646
        作用:
            用于唤醒等待队列wait中的thread，如果唤醒成功，则清空等待队列wait。
    3.kernel中的current全局变量
        static __always_inline struct task_struct *get_current(void)
        {
            return percpu_read_stable(current_task);
        }
        #define current get_current()
        作用:
            It is a pointer to the current process
    4.kernel中队列的使用
    5.LIST_HEAD的使用
    6.
涉及源码:
    frameworks/native/cmds/servicemanager/service_manager.c
    frameworks/native/cmds/servicemanager/binder.c
    kernel/drivers/android/binder.c
binder驱动的注册:
    device_initcall(binder_init);
        /*
            1.创建一个名为binder的工作队列
                binder_deferred_workqueue = create_singlethread_workqueue("binder");
        */
        binder_init
            /*
                1.初始化binder_device *binder_device,初始化的个数及名字由宏CONFIG_ANDROID_BINDER_DEVICES决定
                    kernel/.config/CONFIG_ANDROID_BINDER_DEVICES="binder,hwbinder,vndbinder"
                    1.1 初始化内容如下
                        binder_device->miscdev.fops = &binder_fops;
                        binder_device->miscdev.minor = MISC_DYNAMIC_MINOR;
                        binder_device->miscdev.name = name;
                        binder_device->context.binder_context_mgr_uid = INVALID_UID;
                        binder_device->context.name = name;
                    1.2注册砸向设备
                        misc_register(&binder_device->miscdev);
                    1.3将新建的binder_device添加到binder_devices队列中
                        hlist_add_head(&binder_device->hlist, &binder_devices);
            */
            init_binder_device
服大管家的注册:
    A1.bs = binder_open(128*1024);
        /*
            1.在kernel层创建一个binder_proc *proc;
                1.1设置当前任务为领导进程proc->tsk = current->group_leader;
                1.2设置proc->context = &binder_dev->context;改进程的上下文,属于哪个binder_device
                1.3初始化一个proc->alloc,其中alloc用于跟踪该proc的缓冲区.binder_alloc_init(&proc->alloc);
                1.4设置当前文件的私有数据指针指向该procfilp->private_data = proc;
                1.5将该proc添加到kernel中的binder_procs双向链表中,hlist_add_head(&proc->proc_node, &binder_procs);
        */
        B1.bs->fd = open("/dev/binder", O_RDWR | O_CLOEXEC);
    A2.binder_become_context_manager(bs)
        /*
            1.等待条件binder_stop_on_user_error < 2成立时,才往下执行,这里第一次故为0,条件成立
                ret = wait_event_interruptible(binder_user_error_wait, binder_stop_on_user_error < 2);
                源码位置:
        */
        B1.ioctl(bs->fd, BINDER_SET_CONTEXT_MGR, 0);//以下开始进入内核空间
            C1.binder_get_thread
                /*
                    1.创建一个struct binder_thread *thread;结构体
                    2.设置单前线程proc指向binder_proc
                        thread->proc = proc;
	                3.设置进程id,这里应该是第一个线程
                        thread->pid = current->pid;,thread->task = current;
                    4.初始化一个线程wait队列
                        init_waitqueue_head(&thread->wait);
                    5.初始化一个线程todo链表头
                        INIT_LIST_HEAD(&thread->todo);
                    6.一系列初始化,后期用到再解释
                        thread->looper_need_return = true;
                        thread->return_error.work.type = BINDER_WORK_RETURN_ERROR;
                        thread->return_error.cmd = BR_OK;
                        thread->reply_error.work.type = BINDER_WORK_RETURN_ERROR;
                        thread->reply_error.cmd = BR_OK;
                        INIT_LIST_HEAD(&new_thread->waiting_thread_node);
                */
                D1.thread = binder_get_thread_ilocked(proc, NULL);
            /*
                1.检查下是否第一次注册,不为NULL就直接返回,说明已经注册过if (context->binder_context_mgr_node) 
                2.因为第一次注册大管家,所以context->binder_context_mgr_uid==INVALID_UID==-1
                    context->binder_context_mgr_uid = current_euid();
            */
            C2.ret = binder_ioctl_set_ctx_mgr(filp);
                D1.new_node = binder_new_node(proc, NULL);
                    /*
                        1.ptr =0, cookie=0,flags=0
                        2.对新的binder_node进行一系列初始化
                            node->proc = proc;
                            node->ptr = ptr;==0
                            node->cookie = cookie;==0
                            node->work.type = BINDER_WORK_NODE;
                    */
                    E1.node = binder_init_node_ilocked(proc, new_node, NULL);
                D2.context->binder_context_mgr_node = new_node;
                D3.binder_put_node(new_node);//释放一个指针引用
    A3.binder_loop(bs, svcmgr_handler);
        /*
            设置kernel中的thread->looper标志
            thread->looper |= BINDER_LOOPE
            readbuf[0] = BC_ENTER_LOOPER;
        */
        B1.binder_write(bs, readbuf, sizeof(uint32_t));
        /*
            bwr.read_size = sizeof(readbuf);//128byte
            bwr.read_consumed = 0;
            bwr.read_buffer = (uintptr_t) readbuf;
        */
        B2.for res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
            /* 
                1.参数说明:
                    cmd==BINDER_WRITE_READ
                    arg==&bwr
                    thread当前线程,即在大管家注册的时候创建的
                2.
            */
            C1.binder_ioctl_write_read(filp, cmd, arg, thread);
                /*
                    1.首先放一个BR_NOOP到read_buf中 
                        put_user(BR_NOOP, (uint32_t __user *)ptr)
                */
                D1.ret = binder_thread_read(proc, thread, bwr.read_buffer, bwr.read_size, &bwr.read_consumed, filp->f_flags & O_NONBLOCK);
                    /*
                        1.满足如下三个条件,返回true
                            1.1 thread->transaction_stack为NULL
                            1.2 thread->todo链表是空的
                            1.3 thread->looper的状态为 BINDER_LOOPER_STATE_ENTERED 或者 BINDER_LOOPER_STATE_REGISTERED
                        对于大管家进入的looper,这里显然是满足的,故wait_for_proc_work==1
                    */
                    E1.wait_for_proc_work = binder_available_for_proc_work_ilocked(thread);
                    
                    E2.ret = binder_wait_for_work(thread, wait_for_proc_work);//wait_for_proc_work==1
                        /*
                            1.申明一个 wait 对待队列,并设置其func为autoremove_wake_function,用于唤醒其上的进程
                            wait_queue_t wait = {
                                .private	= current,
                                .func		= autoremove_wake_function,
                                .task_list	= LIST_HEAD_INIT((wait).task_list),
                            }
                        */
                        F1.DEFINE_WAIT(wait)
                        F2.freezer_do_not_count()//不允许kernel进入休眠,与freezer_cont()成对出现
                        F3.for 
                            /*将wait等待队列挂到thread->wait,及等待队列头中,并设置为可中断TASK_INTERRUPTIBLE*/
                            G1.prepare_to_wait(&thread->wait, &wait, TASK_INTERRUPTIBLE);//实现kernel/sched/wait.c
                            /*
                                1.满足如下要求跳出等待
                                    1.1 如果thread->todo非空
                                    1.2 do_proc_work为true
                                    1.3 thread所在proc中的todo链表非空,!binder_worklist_empty_ilocked(&thread->proc->todo)
                                对于此处,则继续在循环中等待
                            */
                            G2.binder_has_work_ilocked(thread, do_proc_work)
                            /*
                                1.如果do_proc_work为true,将proc->waiting_threads挂到thread->waiting_thread_node中作用????
                            */
                            G3.list_add(&thread->waiting_thread_node, &proc->waiting_threads);
                            /*
                                对于service_manager刚刚注册,todo链表为空故启动任务切换
                            */
                            G4.schedule();
                    /* 执行完binder_wait_for_work后就代表thread->todo或者proc->todo链表不为空了

                     */
                    E3.thread->looper |= BINDER_LOOPER_STATE_WAITING;
                    E4.先检查检查thread->todo链表再检查proc->todo链表,获得一个list,list = &thread->todo;
                    /*
                        1.从lis队列中取出一个binder_work 
                            w = list_first_entry_or_null(list, struct binder_work, entry);
                        2.在list中删除改binder_work
                    */
                    E5.w = binder_dequeue_work_head_ilocked(list);
                    E6.需要看看w的类型再进一步
service_manager对第三方服务注册请求的处理:
    /*
        binder_transaction t的数据如下:
        t->from = thread;//当前服务所在线程
        t->sender_euid = task_euid(proc->tsk);
        t->to_proc = target_proc;//目标进程,此处为service_manager所在进程
        t->to_thread = target_thread;//目标线程,此处为service_manager所在线程
        t->code = tr->code;//SVC_MGR_ADD_SERVICE
        t->flags = tr->flags;//0
        t->buffer->allow_user_free = 0;
        t->buffer->debug_id = t->debug_id;
        t->buffer->transaction = t;//指向本身
        t->buffer->target_node = target_node;//service_manager的binder_node
        t->buffer->data ==msg:指向usr层的msg,此处数据如下
            bio_init(&msg, iodata, sizeof(iodata), 4);
            bio_put_uint32(&msg, 0);  // strict mode header
            bio_put_string16_x(&msg, SVC_MGR_NAME);//SVC_MGR_NAME "android.os.IServiceManager"
            bio_put_string16_x(&msg, name);//name = USR_NATIVE_SERVICES_NAME
            bio_put_obj(&msg, ptr);//&token,此处为0
                struct flat_binder_object *obj;//obj实际上指向msg->data区域
                msg->date->flags = 0x7f | FLAT_BINDER_FLAG_ACCEPTS_FDS;
                msg->date->type = BINDER_TYPE_BINDER;
                msg->date->binder = (uintptr_t)ptr;
                msg->date->cookie = 0;
        t->work.type = BINDER_WORK_TRANSACTION;
        t->need_reply = 1;
        t->from_parent = thread->transaction_stack;//这里的thread为将要注册的服务所在线程,此处即为 "shell_cmd_deal" 服务
        thread->transaction_stack = t;
        //下面的数据结构在service_manager所在进程中处理
        t->buffer->allow_user_free = 1;
        t->to_parent = thread->transaction_stack;//此处的thread为service_manager的
        t->to_thread = thread;//此处的thread为service_manager的
        thread->transaction_stack = t;//此处的thread为service_manager的
    */
    接着上面E2步骤开始,此时service_manager的thread->todo链表非空了,也就不需要阻塞了
        D1.ret = binder_thread_read(proc, thread, bwr.read_buffer, bwr.read_size, &bwr.read_consumed, filp->f_flags & O_NONBLOCK);
            E2.binder_wait_for_work(thread, wait_for_proc_work);
            /* 改变下service_manager 的循环状态*/
            E3.thread->looper &= ~BINDER_LOOPER_STATE_WAITING;
            E4.获取todo队列,并进行参数检查
                list = &thread->todo;//这里先获取thread中的todo链表,再获取proc的todo链表,否则代表没数据过来需要继续休眠
                if (end - ptr < sizeof(tr) + 4)//如果用户空间的bwr.read_bufferf分配的大小,小于一个binder_transaction_data+4字节的大小则触发错误
                /*
                    1. 获得w其类型为binder_work类型,在binder_transaction()中被初始化t->work.type==BINDER_WORK_TRANSACTION
                */
            E5.w = binder_dequeue_work_head_ilocked(list);
            /* 获得根据w的地址从而获取其宿主数据,即从注册服务中传来的binder_transaction t */
            E6.t = container_of(w, struct binder_transaction, work);
            /*
                    1.binder_transaction_data tr;
                    2.设置此处ptr ==service_manger->binder_node->ptr == 0
                        tr.target.ptr = target_node->ptr;
                    3.此处tr.cookie =  target_node->cookie = 0
                    4.继续填充数据
                        tr.code = t->code == SVC_MGR_ADD_SERVICE
                        tr.flags = t->flags;//0
                        tr.sender_euid = from_kuid(current_user_ns(), t->sender_euid);
                        tr.sender_pid = task_tgid_nr_ns(sender,task_active_pid_ns(current));
                        tr.data_size = t->buffer->data_size;
                        tr.offsets_size = t->buffer->offsets_size;
                        tr.data.ptr.buffer = (binder_uintptr_t)((uintptr_t)t->buffer->data(注册夫服务的binder_io msg) +binder_alloc_get_user_buffer_offset(&proc->alloc));
                        tr.data.ptr.offsets = tr.data.ptr.buffer +ALIGN(t->buffer->data_size, sizeof(void *));
            */
            E7.填充 tr数据
            /*
                1.uint32_t cmd = BR_TRANSACTION;
            */
            E8.更改cmd值为BR_TRANSACTION
            /*
                    1.获取这个binder_transaction t的发送进程
                        t_from = binder_get_txn_from(t);
                    2.如果t_from存在,那就记录他所在的进程
                        struct task_struct *sender = t_from->proc->tsk;
                    3.设置tr的回复线程id
                        tr.sender_pid = task_tgid_nr_ns(sender,
                                        task_active_pid_ns(current));
            */
            E9.记录结果接收进程(kernel中用task_struct标志一个进程)
            /*
                    1.此处的prt为用户空间中的bwr.read_buffer
                        put_user(cmd, (uint32_t __user *)ptr)
                    2.将整个tr数据拷贝到用户空间
                        copy_to_user(ptr, &tr, sizeof(tr))
            */
            E10.拷贝kernel数据到用户空间
            /*
                1.将在kernel空间的bwr拷贝至用户空间及ubuf == B2中的bwr所在内存空间
                    for res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
            */
        D2.copy_to_user(ubuf, &bwr, sizeof(bwr))
    /*
        1.终于从内核空间返回到用户空间了,带回来的 binder_write_read bwr数据如下
            bwr->read_buffer = BR_TRANSACTION|tr,tr数据类型为binder_transaction_data
                    tr.target.ptr = target_node->ptr=0=BINDER_SERVICE_MANAGER
                    tr.code = t->code == SVC_MGR_ADD_SERVICE
                    tr.flags = t->flags;//0
                    tr.sender_euid = from_kuid(current_user_ns(), t->sender_euid);
                    tr.sender_pid = task_tgid_nr_ns(sender,task_active_pid_ns(current));
                    tr.data_size = t->buffer->data_size;
                    tr.offsets_size = t->buffer->offsets_size;
                    tr.data.ptr.buffer = (binder_uintptr_t)((uintptr_t)t->buffer->data(注册夫服务的binder_io msg) +binder_alloc_get_user_buffer_offset(&proc->alloc));
                    tr.data.ptr.offsets = tr.data.ptr.buffer +ALIGN(t->buffer->data_size, sizeof(void *));
        2.对bwr中的数据进行解析
            2.1从ptr中获得cmd==BR_TRANSACTION
            2.2获取tr,并使用txn指针指向它
                struct binder_transaction_data *txn = (struct binder_transaction_data *) ptr;
    */
    B3.res = binder_parse(bs, 0, (uintptr_t) readbuf, bwr.read_consumed, func);
        /* 
            1.还原出 binder_io msg,这里的msg等于第三方服务的msg,这里直接给出数据
                bio_init(&msg, iodata, sizeof(iodata), 4);
                bio_put_uint32(&msg, 0);  // strict mode header
                bio_put_string16_x(&msg, SVC_MGR_NAME);//SVC_MGR_NAME "android.os.IServiceManager"
                bio_put_string16_x(&msg, name);//name = USR_NATIVE_SERVICES_NAME
                bio_put_obj(&msg, ptr);//&token,此处为0
                msg->date->flags = 0x7f | FLAT_BINDER_FLAG_ACCEPTS_FDS;
                msg->date->type = BINDER_TYPE_BINDER;
                msg->date->binder = (uintptr_t)ptr;
                msg->date->cookie = 0;
        */
        C1.根据传递过来的tr将从第三方服务构造的binder_io数据还原出来
        /*
            1.此处即调用svcmgr_handler(struct binder_state *bs,struct binder_transaction_data *txn,struct binder_io *msg, struct binder_io *reply)
        */
        C2.res = func(bs, txn, &msg, &reply);
            /*
                1.
                    struct flat_binder_object *obj;
                2.kernel中对于handle的计算
                    2.1 handle从何而来
                        A1.ret = binder_inc_ref_for_node(target_proc, node,fp->hdr.type == BINDER_TYPE_BINDER//BINDER_TYPE_BINDER只能是服务自己持有
                                    ,&thread->todo, &rdata);
                                B1.ref = binder_get_ref_for_node_olocked(proc, node, NULL);
                                    //从现有的红黑树中取出一个binder_ref
                                    //如果存在binder_ref->binder_node ==当前要注册的服务binder_node说明已经注册过了,直接返回该binder_ref就行
                                    C1.ref = rb_entry(parent, struct binder_ref, rb_node_node);
                                    //对于第一次注册肯定需要生成一个binder_ref
                                B2.new_ref = kzalloc(sizeof(*ref), GFP_KERNEL);
                                //对于binder_ref->data.desc则是在每次新生成一个binder_ref的时候都会遍历refs_by_desc,最终为其最大值
                                B3.ref = binder_get_ref_for_node_olocked(proc, node, new_ref);
                    2.1 handle是如何增加的
                        //对于binder_ref->data.desc则是在每次新生成一个binder_ref的时候都会遍历refs_by_desc,最终为其最大值
                    2.2 handle作用
                        在kernel中可以通过handle值找到binder_ref再找到binder_node
                3.这里由于是第一个注册的服务,所以handle值为1
            */
            D1.handle = bio_get_ref(msg);
            /*
                3.service_manager中对于注册的服务也只是做个记录,真正的内容实际上在kernel中
                    记录结构体为:struct svcinfo *si
                        si->handle = handle;//1,随着服务的增加++1,第一个注册的值则为1
                        si->len = len;
                        memcpy(si->name, s, (len + 1) * sizeof(uint16_t));
                        si->name[len] = '\0';//字符串尾部添加结束符
                        si->death.func = (void*) svcinfo_death;
                        si->death.ptr = si;
                        si->allow_isolated = allow_isolated;//1
                        si->next = svclist;//双向链表添加到svclist
                        svclist = si;双向链表添加到svclist
            */
        /*
                uint32_t cmd[2];
                cmd[0] = BC_ACQUIRE;
                cmd[1] = target;
                binder_write(bs, cmd, sizeof(cmd));
        */
        D2.binder_acquire(bs, handle);
        /*
            将结果发送给,第三方服务
        */
        C3.binder_send_reply(bs, &reply, txn->data.ptr.buffer, res);
A3.结束一次循环即binder_looper(),返回再次执行循环等待新的binder_transaction 到来,从而源源不断地处理数据

服务的注册:
    涉及源码:
        frameworks/native/cmds/usr_native_services/cImpl/usr_native_services.c
    服务:
        include $(CLEAR_VARS)
        LOCAL_SHARED_LIBRARIES := liblog
        LOCAL_SRC_FILES :=      usr_native_services.c \
                                src/binder.c \
                                src/usr_string_translate.c\
                                src/usr_native_cmd_deal.c
        LOCAL_CFLAGS += $(svc_c_flags)
        LOCAL_MODULE := usr_native_services
服务注册源码分析:
/*
    1.在kernel层创建一个binder_proc *proc;
        1.1设置当前任务为领导进程proc->tsk = current->group_leader;
        1.2设置proc->context = &binder_dev->context;
        1.3初始化一个proc->alloc,其中alloc用于跟踪该proc的缓冲区.binder_alloc_init(&proc->alloc);
        1.4设置当前文件的私有数据指针指向该procfilp->private_data = proc;
        1.5将该proc添加到kernel中的binder_procs双向链表中,hlist_add_head(&proc->proc_node, &binder_procs);
*/
A1.bs = binder_open(USR_NATIVE_SERVICES_BINDER_SIZE);
/*
    1.创建并初始化一个binder_io msg
        bio_init(&msg, iodata, sizeof(iodata), 4);
        bio_put_uint32(&msg, 0);  // strict mode header
        bio_put_string16_x(&msg, SVC_MGR_NAME);//SVC_MGR_NAME "android.os.IServiceManager"
        bio_put_string16_x(&msg, name);//name = USR_NATIVE_SERVICES_NAME
        bio_put_obj(&msg, ptr);//&token,此处为0
            struct flat_binder_object *obj;//obj实际上指向msg->data区域
            msg->date->flags = 0x7f | FLAT_BINDER_FLAG_ACCEPTS_FDS;
            msg->date->type = BINDER_TYPE_BINDER;
            msg->date->binder = (uintptr_t)ptr;
            msg->date->cookie = 0;
*/
A2.svcmgr_publish(bs, svcmgr/*为目标进程handler此处为 BINDER_SERVICE_MANAGER=0*/, USR_NATIVE_SERVICES_NAME, &token);
    /*
        1.创建传输数据
            writebuf.txn.target.handle = target;//BINDER_SERVICE_MANAGER
            writebuf.txn.code = code;//SVC_MGR_ADD_SERVICE
            writebuf.txn.flags = 0;
            writebuf.txn.data_size = msg->data - msg->data0;
            writebuf.txn.offsets_size = ((char*) msg->offs) - ((char*) msg->offs0);
            writebuf.txn.data.ptr.buffer = (uintptr_t)msg->data0;
            writebuf.txn.data.ptr.offsets = (uintptr_t)msg->offs0;
            writebuf.cmd = BC_TRANSACTION;
        2.将writebuf封装进 bwr
            bwr.write_size = sizeof(writebuf);
            bwr.write_consumed = 0;
            bwr.write_buffer = (uintptr_t) &writebuf;
        3.进入for循环处理事件
            bwr.read_size = sizeof(readbuf);
            bwr.read_consumed = 0;
            bwr.read_buffer = (uintptr_t) readbuf;
    */
    B1.binder_call(bs, &msg, &reply, target, SVC_MGR_ADD_SERVICE)
        /*
            1.此处的bwr.read_size,bwr.write_size都不为0
            2.获取本服务的thread
                thread = binder_get_thread(proc);
            3.
        */
        C1.ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
            /* 
                1.在用户空间吧bwr拷贝下来
                    copy_from_user(&bwr, ubuf, sizeof(bwr)
                2.
            */
            D1.ret = binder_ioctl_write_read(filp, cmd/* BC_TRANSACTION */, arg/* &bwr */, thread/* 本进程 */);
                /*
                    1.对于bwr.write_size>0的情况调用
                    2.使用BC_TRANSACTION分支处理
                    3.从用户空间获取一个binder_transaction_data,即将writebuf.txn拷贝到kernel中的tr
                */
                E1.binder_thread_write(proc, thread,bwr.write_buffer/*见B1的writebuf创建传输数据*/,bwr.write_size/*writebuf的大小*/,&bwr.write_consumed);
                    /*
                        1.tr数据如下
                            tr.target.handle = target;//BINDER_SERVICE_MANAGER
                            tr.code = code;//SVC_MGR_ADD_SERVICE
                            tr.flags = 0;
                            tr.data_size = msg->data - msg->data0;
                            tr.offsets_size = ((char*) msg->offs) - ((char*) msg->offs0);
                            tr.data.ptr.buffer = (uintptr_t)msg->data0;
                            tr.data.ptr.offsets = (uintptr_t)msg->offs0;
                            cmd = BC_TRANSACTION;
                        2.此处走非reply分支,且tr->target.handle为0
                        3.从binder_proc中获取target_node,这里即为service_manager在注册时创建的
                            node->proc = proc;//这个proc指向service_manager所在proc
                            node->ptr = ptr;==0
                            node->cookie = cookie;==0
                            node->work.type = BINDER_WORK_NODE;
                        4.创建一个 binder_transaction,并初进行各种始化;t = kzalloc(sizeof(*t), GFP_KERNEL);
                            t->from = thread;//当前服务所在线程
                            t->sender_euid = task_euid(proc->tsk);
                            t->to_proc = target_proc;//目标进程,此处为service_manager所在进程
                            t->to_thread = target_thread;//目标线程,此处为service_manager所在线程
                            t->code = tr->code;//SVC_MGR_ADD_SERVICE
                            t->flags = tr->flags;//0
                            t->buffer->allow_user_free = 0;
                            t->buffer->debug_id = t->debug_id;
                            t->buffer->transaction = t;//指向本身
                            t->buffer->target_node = target_node;//service_manager的binder_node
                            t->buffer->data ==msg:指向usr层的msg,此处数据如下
                                bio_init(&msg, iodata, sizeof(iodata), 4);
                                bio_put_uint32(&msg, 0);  // strict mode header
                                bio_put_string16_x(&msg, SVC_MGR_NAME);//SVC_MGR_NAME "android.os.IServiceManager"
                                bio_put_string16_x(&msg, name);//name = USR_NATIVE_SERVICES_NAME
                                bio_put_obj(&msg, ptr);//&token,此处为0
                                    struct flat_binder_object *obj;//obj实际上指向msg->data区域
                                    msg->date->flags = 0x7f | FLAT_BINDER_FLAG_ACCEPTS_FDS;
                                    msg->date->type = BINDER_TYPE_BINDER;
                                    msg->date->binder = (uintptr_t)ptr;
                                    msg->date->cookie = 0;
                            t->work.type = BINDER_WORK_TRANSACTION;
                            t->need_reply = 1;
                            t->from_parent = thread->transaction_stack;//这里的thread为将要注册的服务所在线程,此处即为 "shell_cmd_deal" 服务
                            thread->transaction_stack = t;
                        5.创建一个 binder_work  tcomplete = kzalloc(sizeof(*tcomplete), GFP_KERNEL);
                        6.根据hdr获取到一个flat_binder_object,其中应用层的flat_binder_object初始化在bio_put_obj中进行
                            fp = to_flat_binder_object(hdr);//此处的fp为msg->date
                    */
                    F1.binder_transaction(proc/* 注册服务所在进程 */, thread/*将要注册的服务所在线程*/, &tr, cmd == BC_REPLY, 0);
                        /*
                            1.先根据fp->binder去获取binder_node如果没有则创建一个新的binder_node
                                node = binder_new_node(proc, fp);
                            2.添加一个指向该服务binder_node的binder_ref至target_proc->refs_by_node中
                            3.修改其type为BINDER_TYPE_HANDLE
                                fp->hdr.type = BINDER_TYPE_HANDLE;
                            4.设置fp
                            fp->binder = 0;
                            //这个值对应binder_ref个数,即可以用该值在service_manager的proc中找到binder_ref从而找到对应服务进程的binder_node
                            fp->handle = rdata.desc;
                            fp->cookie = 0;
                        */
                        G1.ret = binder_translate_binder(fp, t, thread);
                        G2.tcomplete->type = BINDER_WORK_TRANSACTION_COMPLETE;
                        /*将 binder_work tcomplete 添加到服务线程的todo链表中*/
                        G3.binder_enqueue_work(proc, tcomplete, &thread->todo);
                        G4.t->work.type = BINDER_WORK_TRANSACTION;
                        G5.t->need_reply = 1;
                        G6.t->from_parent = thread->transaction_stack;
                        G7.thread->transaction_stack = t;//此处的thread是?
                        /*
                            职责:将binder_transaction t到进程中,并唤醒他
                                sends a transaction to a process and wakes it up
                            1.将t挂到目标进程(这里即为service_manager所在的binder_proc->thread->tod链表中)的todo链表中
                                binder_enqueue_work_ilocked(&t->work, target_list);
                            2.唤醒目标进程.binder_wakeup_thread_ilocked(proc, thread, !oneway)
                            3.对于第三方服务则休眠在此
                        */
                        G8.binder_proc_transaction(t, target_proc, target_thread)
                /*
                    1.此处读写read_buf不为空故执行,binder_thread_read
                    2.唤醒本线程的条件本进程的todo链表非空则唤醒本线程
                        binder_worklist_empty_ilocked(&proc->todo)
                        binder_wakeup_proc_ilocked(proc);
                    3.拷贝结果回用户空间
                        copy_to_user(ubuf, &bwr, sizeof(bwr)
                */
                E2.ret = binder_thread_read(proc, thread, bwr.read_buffer,bwr.read_size,&bwr.read_consumed,filp->f_flags & O_NONBLOCK);
第三方client调用第三方service分析
    参考代码:
        client:
            include $(CLEAR_VARS)
            LOCAL_SHARED_LIBRARIES := liblog
            LOCAL_SRC_FILES :=      usr_native_services_test.c \
                                    src/binder.c \
                                    src/usr_string_translate.c\
                                    src/usr_native_cmd_deal.c
            LOCAL_CFLAGS += $(svc_c_flags)
            LOCAL_MODULE := usr_native_services_client
第三方cliet调用第三方service源码分析:
/*
1.在kernel层创建一个binder_proc *proc;
        1.1设置当前任务为领导进程proc->tsk = current->group_leader,猜测是因为一个进程下支持多个线程(虽然kernel中没有线程的概念)
        1.2设置proc->context = &binder_dev->context;
        1.3初始化一个proc->alloc,其中alloc用于跟踪该proc的缓冲区.binder_alloc_init(&proc->alloc);
        1.4设置当前文件的私有数据指针指向该procfilp->private_data = proc;
        1.5将该proc添加到kernel中的binder_procs双向链表中,hlist_add_head(&proc->proc_node, &binder_procs);
*/
A1.bs = binder_open(USR_NATIVE_SERVICES_BINDER_SIZE);
/*
    1.看描述则是根据名字获取到对应目标service的handle值,获取到handle值后就可已在kernel中通过binder_ref找到对应服务进程的binder_node
    2.最终调用service_manager.c中的
        do_find_service(const uint16_t *s, size_t len, uid_t uid, pid_t spid)函数返回一个handle
*/
A2.handle = svcmgr_lookup(bs, svcmgr, USR_NATIVE_SERVICES_NAME);
/*

*/
A3.binder_call(g_bs, &msg, &reply, g_handle, judge_call_code(cmd)
    

2.c++语言的使用
    调试涉及命令:
        logcat clientTest:V NiaoRenService:V *:S&

疑问点:
    1.BnHelloService::onTransact的函数最终在BBinder::transact中调用
        err = onTransact(code, data, reply, flags);
    2.DECLARE_META_INTERFACE(HelloService);用处
        /* 在代理侧的.h文件中使用即申明,对asInterface()  getInterfaceDescriptor()等方法进行声明*/
        DECLARE_META_INTERFACE(HelloService);
            #define DECLARE_META_INTERFACE(HelloService)                               \
            static const android::String16 descriptor;                          \
            static android::sp<IHelloService> asInterface(                       \
                    const android::sp<android::IBinder>& obj);                  \
            virtual const android::String16& getInterfaceDescriptor() const;    \
            IHelloService();                                                     \
            virtual ~IHelloService();
        /* 在代理侧的.cpp文件中使用即定义,对asInterface()  getInterfaceDescriptor()等方法进行定义*/
        IMPLEMENT_META_INTERFACE(HelloService, "android.media.IHelloService");
        const android::String16 IHelloService::descriptor("android.media.IHelloService");             \
        const android::String16&                                            \
                IHelloService::getInterfaceDescriptor() const {              \
            return IHelloService::descriptor;                                \
        }                                                                   \
        android::sp<IHelloService> IHelloService::asInterface(                \
                const android::sp<android::IBinder>& obj)                   \
        {                                                                   \
            android::sp<IHelloService> intr;                                 \
            if (obj != NULL) {                                              \
                intr = static_cast<IHelloService*>(                          \
                    obj->queryLocalInterface(                               \
                            IHelloService::descriptor).get());               \
                if (intr == NULL) {                                         \
                    intr = new BpHelloService(obj);                          \
                }                                                           \
            }                                                               \
            return intr;                                                    \
        }                                                                   \
        IHelloService::IHelloService() { }                                    \
        IHelloService::~IHelloService() { } 
涉及源码:
    frameworks/rs/server/RefBase.h
    frameworks/native/include/binder/IInterface.h
    frameworks/native/libs/binder/Binder.cpp
    /home/chenqigan/work/newest_rk3288_mid_android7.1/frameworks/native/include/binder/IPCThreadState.h
源码分析        
    1.服务的注册
        /* 单例模式,一个进程中至存在一个.主要作用如下
            1.打开binder驱动
                int mDriverFD = open_driver
            2.初始化一些参数,后续使用到再添加
        */
        A1.sp<ProcessState> proc(ProcessState::self());
        /*
            frameworks/native/include/binder/IServiceManager.h
            frameworks/native/libs/binder/IServiceManager.cpp
        */
        A2.sp<IServiceManager> sm = defaultServiceManager();//最终调用new BpServiceManager(new BpBinder(0))
            /* 1.如果 gDefaultServiceManager == NULL,那么则不断的循环去获取
               2.最终将BpBinder(0)的对象转化为 gDefaultServiceManager
               3.interface_cast<INTERFACE>最终调用INTERFACE内部的asInterface,最终返回new BpServiceManager(new BpBinder(0))
                    BpServiceManager对象则定义在 IServiceManager.cpp中
            */
            gDefaultServiceManager = interface_cast<IServiceManager>(
                ProcessState::self()->getContextObject(NULL));//getStrongProxyForHandle(0)
                    /* 此步骤只是确定SM是否已经存在 */
                    status_t status = IPCThreadState::self()->transact(0, IBinder::PING_TRANSACTION, data, NULL, 0);
                    /* 对于BpBinder:1.mHandle = 0 2.增加引用计数 */
                    b = new BpBinder(handle); //handle==0
        A3.sm->addService( String16("NiaoRenService"),  new BnNiaoRenService(24));
            /*
                data:
                    data.writeInterfaceToken(INiaoRenService::getInterfaceDescriptor());//"android.testing.INiaoRenService"
                    data.writeString16(name);//NiaoRenService
                    data.writeStrongBinder(service);//service ==new BnNiaoRenService(24)
                        flatten_binder(ProcessState::self(), service, this);
                            flat_binder_object obj;
                    data.writeInt32(allowIsolated ? 1 : 0);
                remote()在RefBase()中定义:
                    此处remote()->等价于new BpBinder(0),即remote()是对handle的一层封装
                    inline BpInterface<INTERFACE>::BpInterface(const sp<IBinder>& remote)
                        : BpRefBase(remote)
            */
        A4.remote()->transact(ADD_SERVICE_TRANSACTION, data, &reply);
                //mHandle=0, code=ADD_SERVICE_TRANSACTION, data, reply, flags=0
                //最终调用BpBinder(0)->transact()在调用IPCThreadState-->transact()
                status_t status = IPCThreadState::self()->transact(mHandle, code, data, reply, flags);
                    /*
                        1.使用 binder_transaction_data 对Parcel data进行封装
                            tr.target.ptr = 0; 
                            tr.target.handle = 0;
                            tr.code = ADD_SERVICE_TRANSACTION;
                            tr.flags = 0;
                            tr.cookie = 0;
                            tr.data = data//此处的 data来源A4步骤中的data
                        2.最终使用将cmd及tr写入mOut但并未发送至binder driver
                            mOut.writeInt32(cmd);//cmd==BC_TRANSACTION
                            mOut.write(&tr, sizeof(tr));

                    */
                    err = writeTransactionData(BC_TRANSACTION, 0, 0, ADD_SERVICE_TRANSACTION, data, NULL);
                        mOut.writeInt32(cmd);//cmd ==BC_TRANSACTION
                        mOut.write(&tr, sizeof(tr));
                    /*
                        1.真正与binder驱动打交道的地方
                    */
                    err = waitForResponse(reply);
                        while (1){
                            if ((err=talkWithDriver(true)) < NO_ERROR)
                                    /* 对于数据刚刚开始,肯定两者都为0,故为true */
                                    bool needRead = mIn.dataPosition() >= mIn.dataSize();
                                    outAvail = mOut.dataSize();
                                    /* 填充 binder_write_read bwr数据 */
                                    bwr.write_size = outAvail;
                                    bwr.write_buffer = (uintptr_t)mOut.data();
                                    /* 对于第一次循环,不需要从kernel中获取数据 */
                                    bwr.read_size = 0;
                                    bwr.read_buffer = 0;
                                    /* 驱动返回非负数代表调用成功,err = NO_ERROR */
                                    ioctl(mProcess->mDriverFD, BINDER_WRITE_READ, &bwr)
                                        //进入驱动:kernel/drivers/android/binder.c
                                        ret = wait_event_interruptible(binder_user_error_wait, binder_stop_on_user_error < 2);
                                        /*struct file *filp:当前进程打开/dev/binder的文件描述
                                            cmd =BINDER_WRITE_READ
                                            arg =&tr
                                            thread=当前调用线程
                                        */
                                        ret = binder_ioctl_write_read(filp, cmd, arg, thread);
                                            struct binder_write_read bwr;
                                            if (bwr.write_size > 0)//此处为向binder进行写操作,故走这条分支
                                                ret = binder_thread_write(proc/*调用进程*/, thread/*调用线程*/,
                                                                            bwr.write_buffer,
                                                                            bwr.write_size,
                                                                            &bwr.write_consumed);
                                                        while (ptr < end && thread->return_error.cmd == BR_OK)//循环处理
                                                            /* 这里能直接获取到cmd,原因在
                                                                于填充buf是先填充的mOut.writeInt32(cmd);//cmd ==BC_TRANSACTION */
                                                            get_user(cmd, (uint32_t __user *)ptr)
                                                            /* 对于BC_TRANSACTION和BC_REPLY都走下面的流程
                                                            1.从writebuf中取出一个 binder_transaction_data 进行处理*/
                                                            binder_transaction(proc, thread, &tr,cmd == BC_REPLY, 0);
                                                                /*1.增加note引用计数
                                                                  2.检查node所在的进程是否可用,若不可用将返回 BR_DEAD_REPLY*/
                                                                target_node = binder_get_node_refs_for_txn(target_node, 
                                                                                                            &target_proc,
					    	                                                                                &return_error);
                                                                /* 对于本进程之间的请求处理分支 */
                                                                if (target_node && target_proc == proc)
                                                                /*struct binder_transaction *t;*/
                                                                t = kzalloc(sizeof(*t), GFP_KERNEL);
                                                                tcomplete = kzalloc(sizeof(*tcomplete), GFP_KERNEL);
                                                                /* 记录调用者线程 */
                                                                if (!reply && !(tr->flags & TF_ONE_WAY))
		                                                            t->from = thread;
                                                                t->sender_euid = task_euid(proc->tsk);
                                                                t->to_proc = target_proc;
                                                                t->to_thread = target_thread;
                                                                t->code = tr->code;
                                                                t->flags = tr->flags;
                                                                t->buffer->transaction = t;
	                                                            t->buffer->target_node = target_node;//sm
                                                                /* 
                                                                t->buffer->data = A4步骤的data即用户传下来的信息 */
                                                                copy_from_user(t->buffer->data, (const void __user *)(uintptr_t)
			                                                                    tr->data.ptr.buffer, tr->data_size)
                                                                /*1.对于 BINDER_TYPE_BINDER 类型的struct binder_object_header 
                                                                  2.binder driver需要创建一个binder_node
                                                                  3.将 BINDER_TYPE_BINDER 改为 BINDER_TYPE_HANDLE;
                                                                */
                                                                hdr = (struct binder_object_header *)(t->buffer->data + *offp);
                                                                ret = binder_translate_binder(fp, t, thread);
                                                                    node = binder_new_node(proc, fp);
                                                                /* 在调用者的线程todo链表中加入一个 tcomplete,表示当前线程有一个事务待处理*/
                                                                binder_enqueue_work(proc, tcomplete, &thread->todo);
                                                                /* 将t挂到目标进程或线程的todo链表中,并唤醒目标线程 */
                                                                binder_proc_transaction(t, target_proc, target_thread)
                                    /* 如果驱动有数据返回那么将设置 read_consumed,即返回数据的大小*/
                                    mIn.setDataSize(bwr.read_consumed);
                                    mIn.setDataPosition(0);//设置数据指向为起始位置
                                    /* 
                                    1.如果mIn.dataAvail() == 0那么进入则继续调用talkWithDriver(true)直至发生错误位置
                                    2.从kernel处获取cmd */
                                cmd = (uint32_t)mIn.readInt32();
                        }
3.java语言的使用
    涉及源码:
        frameworks/base/core/java/android/app/ContextImpl.java
        frameworks/base/core/java/android/os/ServiceManager.java
        frameworks/base/core/java/android/os/ServiceManagerNative.java
        frameworks/base/core/java/com/android/internal/os/BinderInternal.java
        frameworks/base/core/java/android/os/Binder.java
        frameworks/base/core/jni/android_util_Binder.cpp
        frameworks/native/libs/binder/ProcessState.cpp
    基础知识:
        JNI基础:
            见</media/chenqigan/GANYE_SSD/androidStudyNote/devLanguage/JNI/JNI学习笔记.c>
    疑问点:
        1.BinderProxy 是什么时候生成的
        2.java中的onTransact是如何被调用到的
            涉及源码:
                frameworks/base/core/java/android/os/IInterface.java
                frameworks/base/core/java/android/os/Binder.java
                frameworks/base/core/java/android/os/IBinder.java
                frameworks/native/include/binder/Binder.h
                class BBinder : public IBinder//BBinder 在Binder.h中申明
                最终调用BBinder->transact()
    client获取服务的几种方式:
        /* 1.这种方式获取到的并非service的代理,只能获取到service代理的封装类,如LedManager(...)
           2.serivce代理的封装类,封装类中的service代理最终也是使用 ServiceManager.getService()获取的
         */
         1.mUsrLedManager = (UsrLedManager)mContext.getSystemService(Context.USR_LED_SERVICE);          
            SystemServiceRegistry.getSystemService(this, name)
                /*
                    // ServiceFetcher<?> fetcher = SYSTEM_SERVICE_FETCHERS.get(name);/SYSTEM_SERVICE_FETCHERS =new HashMap<String, ServiceFetcher<?>>();
                */
                return fetcher != null ? fetcher.getService(ctx) : null;
         2.ServiceManager.getService("mount")分析
            /* 等价于 ServiceManagerProxy().getService(name)*/
            getIServiceManager().getService(name);
                对于getIServiceManager():
                    /* 1.最终返回一个 ServiceManagerProxy(obj); 类型的对象,其中 obj为c++层的BpBinder(0),在JAVA层则为Binder.java@BinderProxy()
                       2.ServiceManagerProxy为ServiceManagerNative的内部类
                    */
                    sServiceManager = ServiceManagerNative.asInterface(BinderInternal.getContextObject());
                        /* 最终将c++层的IBinder转化为java层的IBinder */
                        对于 BinderInternal.getContextObject():
                                android_os_BinderInternal_getContextObject()//android_util_Binder.cpp
                                    sp<IBinder> b = ProcessState::self()->getContextObject(NULL);
                                        getStrongProxyForHandle(0);//ProcessState.cpp,最终返回一个 BpBinder(0) 的本地对象,接下去和c++层的一致
                                    /*将c++层的IBinder对象转化为java层的 BinderProxy 对象*/
                                    return javaObjectForIBinder(env, b);
            //根据以上分析,ServiceManager.getService("mount")等价于如下方法调用
            ServiceManagerProxy(BinderProxy()).getService("mount")
                /*
                    1.填充数据
                        data.writeInterfaceToken(IServiceManager.descriptor);
                        data.writeString(name);
                    2.mRemote.transact 等价于 Binder.java$BinderProxy@transact
                */
                mRemote.transact(GET_SERVICE_TRANSACTION, data, reply, 0);
                    transactNative(code, data, reply, flags);//Binder.java$BinderProxy@transact
                        /*
                            //进入JNI调用,android_util_Binder.cpp@android_os_BinderProxy_transact()
                        */
                        native boolean transactNative(int code, Parcel data, Parcel reply,int flags)
                            /*
                                1.从BinderProxy中获取Native层的 BpBinder(0);
                            */
                            IBinder* target = (IBinder*)env->GetLongField(obj, gBinderProxyOffsets.mObject);
                            /*
                                调用BpBinder(0)->transact(...)
                            */
                            target->transact(code, *data, reply, flags);
                            /* 此处则与c++层保持一致了 */
                            IPCThreadState::self()->transact(mHandle, code, data, reply, flags);
    Service注册服务:
    第三方client调用第三方service:
        1.第三方服务如何获取到数据
        2.aild中的Proxy.onTransaction()函数是如何被调用到的
        3.在Proxy.OnTransaction()中是如何将请求发送出去的



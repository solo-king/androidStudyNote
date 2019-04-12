分析对象:
    service   sw_to_host /system/bin/sh /system/bin/usr_opr_device.sh swtich_to_host
        user root
        group root
        oneshot
        disabled
最终被解析为:
    svc->name = "sw_to_host";
    svc->classname = "default";
    svc->args = "/system/bin/sh" "/system/bin/usr_opr_device.sh" "swtich_to_host" '0';
    svc->nargs = 4;
    svc->onrestart.name = "onrestart";
    state->parse_line = parse_line_service;
    svc->ioprio_class = IoSchedClass_NONE;
    svc->uid = decode_uid(args[1]);//对user root的解析,这里为root
    svc->gid = decode_uid(args[1]);//对group root的解析,这里为root
    svc->flags |= SVC_ONESHOT|SVC_DISABLED|SVC_RC_DISABLED;//对oneshot的解析处理
    
    on property:sys.device.mode=1
        start sw_to_host
    on property:sys.device.mode=2
        start sw_to_peri

需解决问题:
    1. 设置属性，是如何触发对应服务的
        1.1 on property:xxx 是如何被初始化的
            main
                queue_builtin_action(property_service_init_action, "property_service_init");//构造一个struct action
                ...
                execute_one_command();
                    /* 对于 "property_service_init"名字的action 使用 property_service_init_action 进行处理*/
                    property_service_init_action
                        start_property_service();
                            fd = create_socket(PROP_SERVICE_NAME, SOCK_STREAM, 0666, 0, 0);
                            listen(fd, 8);
                            property_set_fd = fd;
                /* 处理该socket的地方 */
                handle_property_set_fd();
                    property_set((char*) msg.name, (char*) msg.value);
                        property_changed(name, value);
                            queue_property_triggers(name, value);
    2. 服务的标志是如何生效的，例如没有设置oneshot、disabled时候可以自动启动
        2.1 自定义服务初始化时对标志位的设置
            state->parse_line = parse_line_service;进行处理
        2.2 class 类别为default的service如何启动

            


重要结构体:
    struct parse_state 
    {//system/core/init/parser.h
        char *ptr;  //指向文件
        char *text;
        int line;
        int nexttoken;
        void *context;
        void (*parse_line)(struct parse_state *state, int nargs, char **args);
        const char *filename;
        void *priv;
    };
    /*关键字处理结构体*/
    struct {
        const char *name;
        int (*func)(int nargs, char **args);service   sw_to_host /system/bin/sh /system/bin/usr_opr_device.sh swtich_to_host
        user root
        group root
        oneshot
        disabled
        unsigned char nargs;
        unsigned char flags;
    } keyword_info[KEYWORD_COUNT] = {
    [ K_UNKNOWN ] = { "unknown", 0, 0, 0 },

    /*用于描述服务*/
    struct service {
        /* list of all services */
        struct listnode slist;

        const char *name;
        const char *classname;
        unsigned flags;
        pid_t pid;
        time_t time_started;    /* time of last start */
        time_t time_crashed;    /* first crash within inspection window */
        int nr_crashed;         /* number of times crashed within window */
        
        uid_t uid;
        gid_t gid;
        gid_t supp_gids[NR_SVC_SUPP_GIDS];
        size_t nr_supp_gids;

        char *seclabel;

        struct socketinfo *sockets;
        struct svcenvinfo *envvars;

        struct action onrestart;  /* Actions to execute on restart. */
        
        /* keycodes for triggering this service via /dev/keychord */
        int *keycodes;
        int nkeycodes;
        int keychord_id;

        int ioprio_class;
        int ioprio_pri;

        int nargs;
        /* "MUST BE AT THE END OF THE STRUCT" */
        char *args[1];
    }; /*     ^-------'args' MUST be at the end of this struct! */

    /* 用于描述XXXX,带补充 */
    struct action {
            /* node in list of all actions */
        struct listnode alist;
            /* node in the queue of pending actions */
        struct listnode qlist;
            /* node in list of actions for a trigger */
        struct listnode tlist;

        unsigned hash;
        const char *name;
        
        struct listnode commands;
        struct command *current;
    };
涉及源码
    system/core/init/init.c
    android/system/core/init/init_parser.c
    system/core/init/ueventd_parser.c
init.c大致流程
main
    /*
        1. 最终打开/dev/__properties__/
        2..加载一些默认的属性值
        
    */
    property_init();
        /*
            ./bionic/libc/include/sys/_system_properties.h:81:#define PROP_PATH_RAMDISK_DEFAULT  "/default.prop"
        */
        load_properties_from_file(PROP_PATH_RAMDISK_DEFAULT);
    property_load_boot_defaults();
        load_properties_from_file(PROP_PATH_RAMDISK_DEFAULT);
    init_parse_config_file("/init.rc");
        /*  读取init.rc所有内容至内存 */
        data = read_file(fn, 0);//fn == /init.rc
        parse_config(fn, data);//data指向/init.rc的内容,解析函数位于init_parser.c
            /* 初始化一个 struct parse_state state*/
            nargs = 0;
            state.filename = fn;
            state.line = 0;
            state.ptr = s;//data指向/init.rc的内容
            state.nexttoken = 0;
            state.parse_line = parse_line_no_op;//空函数，什么也不做
            list_init(&import_list);//创建import链表
            state.priv = &import_list;
            
            /*  
                KEYWORD(service,     SECTION, 0, 0) ,展开如下
                [ K_service ] = { 
                    .name = "service", 
                    .func = 0,
                    .nargs =  1,
                    .flags = SECTION }
            */
            int kw = lookup_keyword(args[0]);//kw == K_service,if (!strcmp(s, "ervice")) return K_service;
            //kw_is(kw, SECTION)为真时进入如下阶段，此处service关键字属于SECTION
            state.parse_line(&state, 0, 0);//只是一个空函数不做处理
            /*char**args == service   sw_to_host /system/bin/sh /system/bin/usr_opr_device.sh swtich_to_host  nargs = 6,nargs最大支持到64
              这里只是对sevice所在的行进行解析，并没有对其下面的关键字进行解析
            */
            parse_new_section(&state, kw, nargs, args);//init_parser.c
                state->context = parse_service(state, nargs, args);//这里state->context == parse_line_service函数，位于init_parser.c
                    svc = service_find_by_name(args[1]);//检查该服务是否已被添加，若已被添加那么直接返回0.这里也说明了重复定义的service只会以第一次定义为主
                    nargs -= 2;//减去服务名称及service字段只留下真正的命令,此处nargs = 4 
                    /* 初始化svc */
                    svc->name = args[1];//sw_to_host
                    svc->classname = "default";
                    //初始化args参数,args == /system/bin/sh /system/bin/usr_opr_device.sh swtich_to_host
                    memcpy(svc->args, args + 2, sizeof(char*) * nargs);
                    svc->args[nargs] = 0;//设置字符串尾部为0，以标志终止
                    svc->nargs = nargs;//4
                    svc->onrestart.name = "onrestart";
                    /* 初始化onrestart的命令列表 */
                    list_init(&svc->onrestart.commands);
                    /* 将该服务挂到全局 service_list 服务链表的尾部*/
                    list_add_tail(&service_list, &svc->slist)
                    /* 设置行解析函数，用以替代最早的parse_line_no_op空函数 */
                    state->parse_line = parse_line_service;
            /* 对于service下面的命令则用 parse_line_service 函数进行解析
                这里的state还指向 service sw_to_host的服务，其内部的context成员变量指向service svc服务即sw_to_host
                service   sw_to_host /system/bin/sh /system/bin/usr_opr_device.sh swtich_to_host
                    user root
                    group root
                    oneshot
                    disabled
            */
            state.parse_line(&state, nargs, args);==parse_line_service(&state, nargs, args)
                svc->ioprio_class = IoSchedClass_NONE;
                svc->uid = decode_uid(args[1]);//对user root的解析,这里为root
                svc->gid = decode_uid(args[1]);//对group root的解析,这里为root
                svc->flags |= SVC_ONESHOT;//对oneshot的解析处理
                /*对 disabled 的解析处理
                   最终svc->flags = SVC_ONESHOT|SVC_DISABLED|SVC_RC_DISABLED=0x02|0x01|0x80==1000 0011                              
                */
                svc->flags |= SVC_DISABLED;
                svc->flags |= SVC_RC_DISABLED;


                
            
    action_for_each_trigger("early-init", action_add_queue_tail);//只启动 early-init 的触发器
    queue_builtin_action(wait_for_coldboot_done_action, "wait_for_coldboot_done");
    queue_builtin_action(mix_hwrng_into_linux_rng_action, "mix_hwrng_into_linux_rng");
    queue_builtin_action(keychord_init_action, "keychord_init");
        struct action *act;
        struct command *cmd;
        act->name = name;//"keychord_init"
        cmd->func = func;//keychord_init_action(...)//form init.c
        cmd->args[0] = name;//"keychord_init"
        /* 添加cmd至act的cmd队列尾部 */
        list_add_tail(&act->commands, &cmd->clist);
        /* 添加 act 至 action_queue 队列尾部 */
        action_add_queue_tail(act);
            list_add_tail(&action_queue, &act->qlist);

    queue_builtin_action(console_init_action, "console_init");
    /* execute all the boot actions to get us started */
    action_for_each_trigger("init", action_add_queue_tail);
    action_for_each_trigger("early-fs", action_add_queue_tail);
    action_for_each_trigger("fs", action_add_queue_tail);
    action_for_each_trigger("post-fs", action_add_queue_tail);
    action_for_each_trigger("post-fs-data", action_add_queue_tail);
    for(;;) {
        execute_one_command();
            cur_action = action_remove_queue_head();//假设获取到名为 keychord_init 的action
            cur_command = get_first_command(cur_action);
            //nargs == 0, args = "keychord_init"
            ret = cur_command->func(cur_command->nargs, cur_command->args); ==keychord_init_action()//from init.c
                keychord_init();//keychords.c
                    service_for_each(add_service_keycodes);
        restart_processes();
            process_needs_restart = 0;
            service_for_each_flags(SVC_RESTARTING, restart_service_if_needed)
                list_for_each(node, &service_list) {//从service_list服务链表中取出每一个svc
                    svc = node_to_item(node, struct service, slist);//根据svc的成员slist变量去除svc本身

        /*轮训句柄*/ 
        nr = poll(ufds, fd_count, timeout);
        for (i = 0; i < fd_count; i++) {
            if (ufds[i].revents == POLLIN) {
                if (ufds[i].fd == get_property_set_fd())
                    handle_property_set_fd();
                else if (ufds[i].fd == get_keychord_fd())
                    handle_keychord();
                else if (ufds[i].fd == get_signal_fd())
                    handle_signal();
            }
        }
    }

属性服务设置
    int property_set(const char *name, const char *value)



epc300 rc树状图:
init.rc
    /init.environ.rc
    /init.usb.rc
    /init.qcom.rc-->init.${ro.hardware}.rc
        init.qcom.usb.rc
            init.qcom.usb.rc
            init.target.rc    
        init.target.rc
            init.qti.carrier.rc
    /init.trace.rc



//真正能启动service的
void service_for_each(void (*func)(struct service *svc))
    keychord_init();//init.c:643:
        void keychord_init()//keychords.c
            service_for_each(add_service_keycodes);
void service_for_each_class(const char *classname,
                            void (*func)(struct service *svc))
void service_for_each_flags(unsigned matchflags,
                            void (*func)(struct service *svc))

目标:
    1. 弄清楚一个service是如何通过service_manager将自己注册的
涉及源码:
   frameworks/native/cmds/usr_native_services/cImpl/usr_native_services.c   ①
   frameworks/native/cmds/usr_native_services/cImpl/include/binder.h        ②
   frameworks/native/cmds/usr_native_services/cImpl/src/binder.c            ③
   kernel/drivers/android/binder.c                  ④
服务的注册: 
    main                                    //①
        bs = binder_open(USR_NATIVE_SERVICES_BINDER_SIZE);  
        ret = svcmgr_publish(bs, svcmgr, USR_NATIVE_SERVICES_NAME, &token);
            struct binder_io msg, reply;
            /*   构造 binder_io 类型数据 */
            bio_init(&msg, iodata, sizeof(iodata), 4);
            bio_put_uint32(&msg, 0);  // strict mode header
            bio_put_string16_x(&msg, SVC_MGR_NAME);//"android.os.IServiceManager"
            bio_put_string16_x(&msg, name);//USR_NATIVE_SERVICES_NAME	"shell_cmd_deal"
            bio_put_obj(&msg, ptr);//&token里面什么也没有
            /* */
            binder_call(bs, &msg, &reply, target, SVC_MGR_ADD_SERVICE)  //③
                struct binder_write_read bwr;
                struct {
                    uint32_t cmd;
                    struct binder_transaction_data txn;
                } __attribute__((packed)) writebuf;//构造并实例化了writebuf结构体
                /* 根据msg构造数据 */
                writebuf.cmd = BC_TRANSACTION;
                writebuf.txn.target.handle = target;//BINDER_SERVICE_MANAGER=0U
                writebuf.txn.code = code;//SVC_MGR_ADD_SERVICE
                writebuf.txn.flags = 0;
                /* 填充驱动需要的结构体 */
                bwr.write_size = sizeof(writebuf);
                bwr.write_consumed = 0;
                bwr.write_buffer = (uintptr_t) &writebuf;
            for: /* read_size不为零，在驱动中也会处理 */
                bwr.read_size = sizeof(readbuf);
                bwr.read_consumed = 0;
                bwr.read_buffer = (uintptr_t) readbuf;
                res = ioctl(bs->fd, BINDER_WRITE_READ, &bwr);
                    //cmd = BINDER_WRITE_READ, arg=从用户空间传进来的bwr
                   ret = binder_ioctl_write_read(filp, cmd, arg, thread);//④
                        struct binder_proc *proc = filp->private_data;
                        //拷贝用户空间的bwr到kernel中的bwr
                        copy_from_user(&bwr, ubuf, sizeof(bwr)
                        //由于是第一次循环,所以bwr.write_size>0
                        ret = binder_thread_write(proc, thread, bwr.write_buffer, bwr.write_size, &bwr.write_consumed);
                            struct binder_context *context = proc->context;
                        case BC_TRANSACTION:
                            struct binder_transaction_data tr;
                            //实际上就是获得user空间的writebuf.txn
                            copy_from_user(&tr, ptr, sizeof(tr))
                            //cmd == BC_REPLY-->false
                            binder_transaction(proc, thread, &tr, cmd == BC_REPLY, 0);
                                /* target_node为代表service的binder_node
                                */
                                target_node = context->binder_context_mgr_node;
                                target_node = binder_get_node_refs_for_txn(target_node, &target_proc,&return_error);

flat_binder_object结构体作用


                                

                    






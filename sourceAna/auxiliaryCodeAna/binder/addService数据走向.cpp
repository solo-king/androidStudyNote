data(Parcel.cpp):
    data.writeInterfaceToken(IServiceManager::getInterfaceDescriptor());//"android.testing.INiaoRenService"
    data.writeString16(name);//NiaoRenService
    data.writeStrongBinder(service);//service ==new BnNiaoRenService(24)
    data.writeInt32(allowIsolated ? 1 : 0);
remote()->transact(ADD_SERVICE_TRANSACTION, data, &reply);

binder_transaction_data tr:
    tr.target.ptr = 0; 
    tr.target.handle = 0;
    tr.code = ADD_SERVICE_TRANSACTION;
    tr.flags = 0;
    tr.cookie = 0;
    tr.data = data//此处的 data来源A4步骤中的data
    tr.data_size = data.ipcDataSize();
    tr.data.ptr.buffer = data.ipcData();
    tr.offsets_size = data.ipcObjectsCount()*sizeof(binder_size_t);
    tr.data.ptr.offsets = data.ipcObjects();
    tr->data.ptr.buffer =//指向?
Parcel mOut:
mOut.writeInt32(cmd);//cmd==BC_TRANSACTION
mOut.write(&tr, sizeof(tr));

struct binder_transaction *t;
    t->from = test_server线程;
    t->to_proc = sm所在进程;
	t->to_thread = sm所在线程;
	t->code = ADD_SERVICE_TRANSACTION;
	t->flags = 0;
    /*目标线程的buffer数据结构类型为:binder_buffer*/
    t->buffer = binder_alloc_new_buf(&target_proc->alloc, tr->data_size,
		tr->offsets_size, extra_buffers_size,
		!reply && (t->flags & TF_ONE_WAY));
    t->buffer->allow_user_free = 0;
	t->buffer->transaction = t;//指向自己
	t->buffer->target_node = target_node;//sm在kernel的记录节点
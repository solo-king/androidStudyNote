IFregService:public IInterface
BnFregService:public BnInterface<IFregService>
BpFregService:BpInterface<IFregService>

Binder本地对象：
	BBinder,在用户空间创建且运行与Server进程
Binder实体：
	struct binder_node,运行于内核空间
Binder引用对象:
	struct binder_ref,指向binder_node

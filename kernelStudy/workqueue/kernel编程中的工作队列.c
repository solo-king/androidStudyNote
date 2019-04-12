延迟工作方式,经常在kernel的下半部执行:
    1.tasklet
        基于软件中断完成
    2.工作队列
    3.软中断

1.工作队列的执行上下文是内核线程， 因此可以调度和睡眠

简单使用方法:
    1.使用自己的工作队列

        struct my_data
        {
            struct work_struct my_work;
            int value; 
        };
        /*工作队列函数*/
        static void work_func(struct work_struct *work)
        {
            struct my_data *md=container_of(work,struct my_data,my_work);
            printk("<2>""Thevalue of my data is:%d\n",md->value);
        }
        struct my_data* init_data(structmy_data *md)
        {
            md=(struct my_data*)kmalloc(sizeof(struct my_data),GFP_KERNEL);
            md->value = 1;
            md->my_work = work_queue;
            return md;
        }
        struct workqueue_struct *wq = NULL;
        struct work_struct work_queue;
        struct my_data *md2 = init_data(md2);
        wq = create_workqueue("test");
        INIT_WORK(&md2->my_work, work_func);
        queue_work(wq, &md2->my_work);
        //销毁工作队列
        destroy_workqueue(wq);
    2.使用共享工作队列
        2.1 只要使用 schedule_work 添加的工作节点都是添加到内核共享工作队列中
        2.2 

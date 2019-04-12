所需知识点:
    1.kernel clock
    2.struct irq_desc *desc;
    3.struct tasklet_struct remote_tasklet;
    4.struct wake_lock remotectl_wake_lock;
        //防止系统进入休眠
    5.struct cpumask cpumask;
    6.从r = platform_get_resource(pdev, IORESOURCE_MEM, 0);中能获取到什么
涉及源码:
    drivers/input/remotectl/rockchip_pwm_remotectl.c
问题:
    1.如何将设备树关键节点含义
        remote_pwm_id = <0>;
	    handle_cpu_id = <1>;
        ir_key1{
		rockchip,usercode = <0x4040>;
		rockchip,key_table =
			<0xf2	KEY_REPLY>,
源码分析:
    rk_pwm_probe
        //获取device资源
        r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        ddata->base = devm_ioremap_resource(&pdev->dev, r);
        //获取时钟,并使能
        count = of_property_count_strings(np, "clock-names");
        clk = devm_clk_get(&pdev->dev, NULL);
		p_clk = clk;
        ret = clk_prepare_enable(clk);
        //保存驱动私有数据
        platform_set_drvdata(pdev, ddata);
        //获取最大支持的遥控器数量
        num = rk_remotectl_get_irkeybd_count(pdev);        
        ddata->maxkeybdnum = num;
        //分配num个遥控器描述结构体
        //struct rkxx_remotectl_button *remotectl_button;
        remotectl_button = devm_kzalloc(&pdev->dev,num*sizeof(struct rkxx_remotectl_button),GFP_KERNEL);
        //分配一个输入设备，并做一些初始化
        input = devm_input_allocate_device(&pdev->dev);
        input->name = pdev->name;
        ...
        input->id.bustype = BUS_HOST;
        //将输入设备放入驱动私有数据中
        ddata->input = input;
        //获取中断引脚
        irq = platform_get_irq(pdev, 0);
        ddata->irq = irq;
	    ddata->wakeup = 1;//可唤醒
重要结构体:
    /* 用于描述，遥控上按键的数值与真正上报的key code */
    struct rkxx_remote_key_table {
        int scancode;
        int keycode;
    };
    /* 用于描述一个遥控器 */
    struct rkxx_remotectl_button {
        int usercode;//用于区别遥控的型号,每一个遥控都是唯一的
        int nbuttons;//遥控含有多少个按键
        struct rkxx_remote_key_table key_table[MAX_NUM_KEYS];//遥控按键与其按键码
    };
    /*  */
    struct rkxx_remotectl_drvdata {
	void __iomem *base;
	int state;
	int nbuttons;
	int result;
	int scandata;
	int count;
	int keynum;
	int maxkeybdnum;
	int keycode;
	int press;
	int pre_press;
	int irq;
	int remote_pwm_id;
	int handle_cpu_id;
	int wakeup;
	int clk_rate;
	int support_psci;
	unsigned long period;
	unsigned long temp_period;
	int pwm_freq_nstime;
	struct input_dev *input;
	struct timer_list timer;
	struct tasklet_struct remote_tasklet;
	struct wake_lock remotectl_wake_lock;
};
参考设备树:

&pwm0 {
	compatible = "rockchip,remotectl-pwm";
	remote_pwm_id = <0>;
	handle_cpu_id = <1>;
	status = "okay";

	ir_key1{
		rockchip,usercode = <0x4040>;
		rockchip,key_table =
			<0xf2	KEY_REPLY>,
	};
	ir_key2{
		rockchip,usercode = <0xff00>;
		rockchip,key_table =
			<0xf9	KEY_HOME>,
	};
	ir_key3{
		rockchip,usercode = <0x1dcc>;
		rockchip,key_table =
			<0xee	KEY_REPLY>,
	};
};
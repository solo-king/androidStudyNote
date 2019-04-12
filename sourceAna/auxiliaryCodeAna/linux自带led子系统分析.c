系统升级watchdog:
	涉及基础:
		1.count = device_get_child_node_count(dev);	
		2.struct fwnode_handle *child;
			1.1对于平台设备struct device	dev;中的dev成员成员节点挂载模型是怎么样的
			1.2为什么是直接获取下一个节点就是设备树节点的第一个
		3.工作队列的使用及概念分析
			INIT_WORK(&led_dat->work, gpio_led_work);
		4.kernel中的list
			LIST_HEAD(leds_list);
		5.kernel中的读写信号量
			static DECLARE_RWSEM(triggers_list_lock);
	//涉及driver文件
	kernel/drivers/leds/led-class.c
	kernel/drivers/leds/led-core.c
	kernel/drivers/leds/leds-gpio.c
	kernel/drivers/leds/led-triggers.c
	kernel/drivers/leds/trigger/ledtrig-timer.c
	//重要数据结构
	//from include/linux/leds.h
	struct gpio_led {
		const char *name;
		const char *default_trigger;//默认指定的触发器
		unsigned 	gpio;
		unsigned	active_low : 1;
		unsigned	retain_state_suspended : 1;
		unsigned	default_state : 2;
		/* default_state should be one of LEDS_GPIO_DEFSTATE_(ON|OFF|KEEP) */
		struct gpio_desc *gpiod;
	};
	struct led_classdev {
		const char		*name;
		enum led_brightness	 brightness;
		enum led_brightness	 max_brightness;
		int			 flags;
		...
		struct device		*dev;
		const struct attribute_group	**groups;
		...
	};
	//from leds-gpio.c
	struct gpio_led_data {
		struct led_classdev cdev;
		struct gpio_desc *gpiod;
		struct work_struct work;
		u8 new_level;
		u8 can_sleep;
		u8 blinking;
		int (*platform_gpio_blink_set)(struct gpio_desc *desc, int state,
				unsigned long *delay_on, unsigned long *delay_off);
	};
	struct gpio_leds_priv {
		int num_leds;
		struct gpio_led_data leds[];
	};
	//leds-gpio.c大致加载流程
	gpio_led_probe(struct platform_device *pdev)
		A1.struct gpio_leds_priv *gpio_leds_create(struct platform_device *pdev)
			device_for_each_child_node(dev, child){
				//中间对led各种初始化
				struct gpio_led led = {};
				ret = create_gpio_led(&led, &priv->leds[priv->num_leds],dev, NULL);
					//struct gpio_led_data *led_dat
					//对gpio_led_data结构体进行一系列初始化,主要包含gpiod(控制pin)、name、pin初始电平
					led_dat->gpiod = template->gpiod;
					led_dat->cdev.name = template->name;
					led_dat->cdev.default_trigger = template->default_trigger;
					led_dat->can_sleep = gpiod_cansleep(led_dat->gpiod);
					led_dat->blinking = 0;
					...
					//初始化一个工作队列work，后面唤醒后就进入gpio_led_work中执行	
					INIT_WORK(&led_dat->work, gpio_led_work);
					/*
				          总结:
						1.在sysfs的leds类下创建对应的device
						2.将该led_cdev加入到leds_list链表中，该链表头由led-core.c管理
						3.增加工作队列及定时器
						4.对触发器的一些初始化
					*/
					led_classdev_register(parent, &led_dat->cdev);//from led-class.c
						//在leds_class类下创建name的设备，并创建属性组
						led_cdev->dev = device_create_with_groups(leds_class, parent, 0,led_cdev, led_cdev->groups, "%s", name);
						//添加一个led_classdev至leds_list链表中
						list_add_tail(&led_cdev->node, &leds_list);	
						//此函数实际上并不执行，led_cdev->brightness_get并没有赋值
						led_update_brightness(led_cdev);
						//主要初始化一个工作队列及一路定时器
						led_init_core(led_cdev);//led-core.c
						//如果定义了的话则对触发器做初始化,这些放在触发器相关章节说明
						led_trigger_set_default(led_cdev);//led-triggers.c
			}
		//将struct gpio_leds_priv数据添加到平台设备的私有数据中
		A2.platform_set_drvdata(pdev, priv);

led子系统中的触发器:
		可参考代码:
				Ledtrig-backlight.c (drivers\leds\trigger):	return led_trigger_register(&bl_led_trigger);
				Ledtrig-default-on.c (drivers\leds\trigger):	return led_trigger_register(&defon_led_trigger);
				Ledtrig-gpio.c (drivers\leds\trigger):	return led_trigger_register(&gpio_led_trigger);
				Ledtrig-heartbeat.c (drivers\leds\trigger):	int rc = led_trigger_register(&heartbeat_led_trigger);
				Ledtrig-oneshot.c (drivers\leds\trigger):	return led_trigger_register(&oneshot_led_trigger);
				Ledtrig-timer.c (drivers\leds\trigger):	return led_trigger_register(&timer_led_trigger);
				Ledtrig-transient.c (drivers\leds\trigger):	return led_trigger_register(&transient_trigger);
		重要结构体:
				struct led_trigger {
					/* Trigger Properties */
					const char	 *name;
					void		(*activate)(struct led_classdev *led_cdev);
					void		(*deactivate)(struct led_classdev *led_cdev);
					/* LEDs under control by this trigger (for simple triggers) */
					rwlock_t	  leddev_list_lock;
					struct list_head  led_cdevs;
					/* Link to next registered trigger */
					struct list_head  next_trig;
				};
		int led_trigger_register(struct led_trigger *trig)

//时钟触发器注册分析
static struct led_trigger timer_led_trigger = {
	.name     = "timer",
	.activate = timer_trig_activate,//启用该触发器时调用
	.deactivate = timer_trig_deactivate,//关闭该触发器时调用
};
//触发器的注册
led_trigger_register(struct led_trigger *trig);//ledtrig-timer.c,trig= timer_led_trigger
		struct led_trigger *_trig;
		//初始化第一个触发器的led_cdevs作为链表头
		INIT_LIST_HEAD(&trig->led_cdevs);
		//添加新触发器至trigger_list中
		list_add_tail(&trig->next_trig, &trigger_list);
				list_for_each_entry(led_cdev, &leds_list, node) {
					/*
						1.从leds_list中取出每一个struct led_classdev led_cdev
						2.调用led_trigger_set有如下三个条件
							2.1 led_cdev未设置触发器
							2.2 指定默认触发器名
							2.3 指定的的默认触发器名与当前要注册的触发器名称一致时才走下面代码
					*/
					led_trigger_set(led_cdev, trig);
							//trig_list代表该led_classdev所支持的所有触发器
							list_add_tail(&led_cdev->trig_list, &trig->led_cdevs);
							led_cdev->trigger = trig;
							//启用该触发器,此处即调用了ledtrig-timer.c中timer_trig_activate
							trig->activate(led_cdev);
				}
//触发器的启用
timer_trig_activate(struct led_classdev *led_cdev)//ledtrig-timer.c
			device_create_file(led_cdev->dev, &dev_attr_delay_on);
			device_create_file(led_cdev->dev, &dev_attr_delay_off);
			led_blink_set(led_cdev, &led_cdev->blink_delay_on,&led_cdev->blink_delay_off);//led-core.c
					//del_timer_sync - deactivate a timer and wait for the handler to finish.
					del_timer_sync(&led_cdev->blink_timer);
					//设置LED标志
					led_cdev->flags &= ~LED_BLINK_ONESHOT;
					led_cdev->flags &= ~LED_BLINK_ONESHOT_STOP;
					led_blink_setup(led_cdev, delay_on, delay_off);
						//如果delay_on, delay_off是空的，那么LED闪烁频率为1HZ
						*delay_on = *delay_off = 500;
						led_set_software_blink(led_cdev, *delay_on, *delay_off);
						//最终调用leds-gpio.c中的brightness_set函数
						led_set_brightness_async(led_cdev, LED_OFF);//drivers/leds/leds.h
						led_set_brightness_async(led_cdev, led_cdev->blink_brightness);
						//对于定时器的初始化位于led-class.c 的led_classdev_register中调用led_delay_on_show进行
						mod_timer(&led_cdev->blink_timer, jiffies + 1);
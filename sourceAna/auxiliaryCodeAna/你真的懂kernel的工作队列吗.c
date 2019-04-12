//简单使用
1.初始化工作队列
	struct work_struct work;
	INIT_WORK(&led_dat->work, gpio_led_work);
2.工作任务
	static void gpio_led_work(struct work_struct *work)
	{
		struct gpio_led_data *led_dat =
			container_of(work, struct gpio_led_data, work);

		if (led_dat->blinking) {
			led_dat->platform_gpio_blink_set(led_dat->gpiod,
						led_dat->new_level, NULL, NULL);
			led_dat->blinking = 0;
		} else
			gpiod_set_value_cansleep(led_dat->gpiod, led_dat->new_level);
	}

3.唤醒工作任务
	//真正调度的地方
	schedule_work(&led_dat->work);


驱动支持：
	CONFIG_USB_SERIAL_CP210X=y
	/usb/serial/cp210x.c
基础分析:
		
		module_usb_serial_driver(serial_drivers, id_table);
				usb_serial_module_driver(KBUILD_MODNAME, serial_drivers, id_table)
					static int __init usb_serial_module_init(void)			
					{									
						return usb_serial_register_drivers(serial_drivers,KBUILD_MODNAME, id_table);
					}								
					module_init(usb_serial_module_init);				
					static void __exit usb_serial_module_exit(void)	
					{									
						usb_serial_deregister_drivers(serial_drivers);
					}									
					module_exit(usb_serial_module_exit);
	
	
	struct bus_type usb_serial_bus_type = {
		.name =		"usb-serial",
		.match =	usb_serial_device_match,
		.probe =	usb_serial_device_probe,
		.remove =	usb_serial_device_remove,
		.drv_attrs = 	drv_attrs,
	};	
	static struct usb_serial_driver cp210x_device = {
			.driver = {
				.owner =	THIS_MODULE,
				.name  =		"cp210x",
				.bus	 = &usb_serial_bus_type, //from bus.c
			},
			.id_table		= id_table,
			.num_ports		= 1,
			.bulk_in_size		= 256,
			.bulk_out_size		= 256,
			.open			= cp210x_open,
			.close			= cp210x_close,
			.break_ctl		= cp210x_break_ctl,
			.set_termios		= cp210x_set_termios,
			.tiocmget		= cp210x_tiocmget,
			.tiocmset		= cp210x_tiocmset,
			.attach			= cp210x_startup,
			.release		= cp210x_release,
			.dtr_rts		= cp210x_dtr_rts
			//动态添加
			.description = "cp210x",
			.write = usb_serial_generic_write; //from usb core
			.write_room = usb_serial_generic_write_room
			.chars_in_buffer = usb_serial_generic_chars_in_buffer;
			.usb_driver = udriver,
			
		};
		static struct usb_serial_driver * const serial_drivers[] = {
			&cp210x_device, NULL
		};				
		usb_serial_register_drivers(serial_drivers,KBUILD_MODNAME, id_table);
			/* usb driver register,usb设备的基础设施 */
			struct usb_driver *udriver;
			...
			/*usb_serial_driver - describes a usb serial driver */
			...//一系列的usb driver初始化
			rc = usb_register(udriver);
			struct usb_serial_driver * const *sd;
		for sd = serial_drivers:		
			(*sd)->usb_driver = udriver;
			usb_serial_register(*sd);
				driver->description = "cp210x";
				usb_serial_operations_init(sd)
						set_to_generic_if_null(device, write);
								sd->write = usb_serial_generic_write;
						set_to_generic_if_null(device, write_room);
								sd->write_room = usb_serial_generic_write_room;
						set_to_generic_if_null(device, chars_in_buffer);
								sd->chars_in_buffer = usb_serial_generic_chars_in_buffer;
				usb_serial_bus_register(sd);
						driver->driver.bus = &usb_serial_bus_type;
						driver_register(&sd->driver);
						|		struct device_driver *other;
						|		/* 保证驱动唯一性driver  */
						|		other = driver_find("cp210x", &usb_serial_bus_type);
						|		ret = bus_add_driver(&sd->driver);
						|		|		struct bus_type *bus;
						|		|		struct driver_private *priv;
						|		|		bus = usb_serial_bus_type;
						|		|		priv->driver = driver;
						|		|		driver->p = priv;
						|		|		priv->kobj.kset = bus->p->drivers_kset;
						|		|		if (drv->bus->p->drivers_autoprobe) {/*待定*/
												error = driver_attach(drv);
													bus_for_each_dev(&usb_serial_bus_type, NULL, drv, __driver_attach);
														struct klist_iter i;
														struct device *dev;
														while:(遍历usb_serial_bus_type总线上的所有设备)
																__driver_attach（ dev, drv）//dev为usb_serial_bus_type上挂着的一个设备
																			driver_probe_device(drv, dev);
																					really_probe(dev, drv);
																							dev->driver = drv;
																							/* probe调用顺序:bus的probe >driver->probe */
																							dev->bus->probe(dev);//usb_serial_bus_type->usb_serial_device_probe
																								struct usb_serial_driver *driver;
																								struct usb_serial_port *port;
																								port = to_usb_serial_port(dev);//实际调用container_of获得usb_serial_port原始结构体
																								device_create_file(dev, &dev_attr_port_number);
																								tty_register_device(usb_serial_tty_driver, minor, dev);
																											...	
										}		
						|		|		module_add_driver(THIS_MODULE, driver);
						|		|				struct module_kobject *mk = NULL;
						|		|				mk = &mod->mkobj;
						|		|				driver_name = make_driver_name(drv);//"usb-serial:cp210x"
						|		|				...
						|		|		error = driver_create_file(drv, &driver_attr_uevent);
						|		|		error = driver_add_attrs(bus, drv);
												
										
										
涉及指令：
	sudo cat /sys/kernel/debug/usb/devices			
问题点：
	1.USB设备指定匹配内容
		struct usb_device_id {
			/* which fields to match against? */
			__u16		match_flags;
			...
		}
			
	2.usb设备匹配机制
		1.1PID,VID匹配(.match_flags = USB_DEVICE_ID_MATCH_DEVICE,s)
			USB_DEVICE(0x045B, 0x0053)
		当USB核心检测到某个设备的属性和某个驱动程序的usb_device_id结构体所携带的信息一致时，这个驱动程序的probe()函数就被执行
	3.usb设备如何注册为/dev/ttyUSBx节点
				usb_serial_init	//usb-serial.c
					usb_serial_tty_driver->name = "ttyUSB";
	
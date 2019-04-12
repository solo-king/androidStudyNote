数据结构:
	struct pinctrl_state {
    struct list_head node;－－－挂入链表头的节点
    const char *name;－－－－－该state的名字
    struct list_head settings;－－－属于该状态的所有的settings
	}; 
					/\
					||
	struct pinctrl_setting {
    struct list_head node;
    enum pinctrl_map_type type;
    struct pinctrl_dev *pctldev;
    const char *dev_name;
    union {
        struct pinctrl_setting_mux mux;
        struct pinctrl_setting_configs configs;
    } data;
	}; 
	//
	 enum pinctrl_map_type {
	    PIN_MAP_TYPE_INVALID,
	    PIN_MAP_TYPE_DUMMY_STATE,
	    PIN_MAP_TYPE_MUX_GROUP,－－－功能复用的setting
	    PIN_MAP_TYPE_CONFIGS_PIN,－－－－设定单一一个pin的电气特性
	    PIN_MAP_TYPE_CONFIGS_GROUP,－－－－设定单pin group的电气特性
	 }; 
	 struct pinctrl_setting_mux {
	    unsigned group;－－－－－－－－该setting所对应的group selector
	    unsigned func;－－－－－－－－－该setting所对应的function selector
		}; 
		
		struct pinctrl_map_configs {
		    const char *group_or_pin;－－－－该pin或者pin group的名字
		    unsigned long *configs;－－－－要设定的值的列表。这个值被用来写入HW
		    unsigned num_configs;－－－－列表中值的个数
		}; 
		
		
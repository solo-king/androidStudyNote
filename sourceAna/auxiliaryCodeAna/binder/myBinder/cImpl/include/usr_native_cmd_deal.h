#ifndef		USR_NATIVE_CMD_DEAL_H
#define		USR_NATIVE_CMD_DEAL_H

#include<stdio.h>
#include"usr_native_cmd_base.h"

struct usr_native_cmd{
	int cmd_arg_count;
	/* cmd[1] is function flags*/
	char**cmd;
	char combined[CMD_MAX_LENGTH];
};


int usr_native_cmd_init(struct usr_native_cmd* cmd , int argc, char**argv);
int	usr_native_cmd_combination(struct usr_native_cmd* cmd);


#endif

#define		LOG_TAG		"UsrNativeCmd"
#include <cutils/log.h>
#include <string.h>
#include "../include/usr_native_cmd_deal.h"
#include "../include/usr_native_cmd_base.h"

int usr_native_cmd_init(struct usr_native_cmd* cmd , int argc, char**argv)
{


	int count = 0;
	
	ALOGD("%s,%d:argc=%d",__func__, __LINE__, argc);
	for( count = 0 ; count < argc; ++count )
		ALOGD("%s,%d:argv[%d]=%s",__func__, __LINE__, count, argv[count]);
	
	if(cmd ==  NULL)
	{
		ALOGE("%s, %d:cmd is null!!", __func__, __LINE__);
		return USR_NATIVE_CMD_ERROR;
	}

	cmd->cmd = argv;
	cmd->cmd_arg_count = argc;
	return USR_NATIVE_CMD_OK;
}


int	usr_native_cmd_combination(struct usr_native_cmd* cmd)
{

	int cmd_length = cmd->cmd_arg_count;
	char** tmp_cmd = cmd->cmd;
	int tmp_count ;
	
	memset(cmd->combined, '\0', CMD_MAX_LENGTH);
	for(tmp_count = 2; tmp_count < cmd_length; ++tmp_count){
		ALOGD("%s,%d:tmp_cmd[%d]=%s",__func__, __LINE__, tmp_count, tmp_cmd[tmp_count]);
		sprintf(cmd->combined, "%s %s ", cmd->combined, tmp_cmd[tmp_count]);
	}
	
		sprintf(cmd->combined, "%s 2>&1 ", cmd->combined);

	ALOGD("%s,%d:[cmd->combined]=%s", __func__, __LINE__, cmd->combined);
	
	return USR_NATIVE_CMD_OK;
}


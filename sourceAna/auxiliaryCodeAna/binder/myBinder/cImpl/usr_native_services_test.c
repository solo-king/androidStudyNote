#define		LOG_TAG		"UsrNativeServicesTest"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/types.h>
#include <stdbool.h>
#include <string.h>
#include <cutils/log.h>
#include <private/android_filesystem_config.h>

#include "include/binder.h"
#include "include/usr_native_services.h"
#include "include/usr_native_services_test.h"
#include "include/usr_string_translate.h"
#include "include/usr_native_cmd_deal.h"
#include "include/usr_native_cmd_base.h"



static uint32_t svcmgr_lookup(struct binder_state *bs, uint32_t target, const char *name);
static int usage_print(struct usr_native_cmd*);


static struct	binder_state	*g_bs;
static uint32_t g_handle;



int main(int argc, char **argv)
{
    struct binder_state *bs;
    uint32_t svcmgr = BINDER_SERVICE_MANAGER;
    uint32_t handle;
	int ret;
	char* shell_cmd;
	struct usr_native_cmd input_cmd; 
	/* usr_native_cmd init and combination */
	usr_native_cmd_init(&input_cmd, argc, argv);
	usr_native_cmd_combination(&input_cmd);
	
	if(usage_print(&input_cmd))	return USR_NATIVE_CMD_ERROR;

    bs = binder_open(USR_NATIVE_SERVICES_BINDER_SIZE);
    if (!bs) {
        ALOGE("failed to open binder driver");
        return -1;
    }
	
	g_bs = bs;
	/* get service */
	handle = svcmgr_lookup(bs, svcmgr, USR_NATIVE_SERVICES_NAME);
	if (!handle) {
        ALOGE("failed to get hello service");
        return -1;
	}
	
	g_handle = handle;
	
	shell_cmd_deal(&input_cmd);
	binder_release(bs, handle);

    return 0;
}

static void get_reply(struct binder_io* reply)
{

	uint32_t line_number = bio_get_uint32(reply);
	uint32_t tmp_count;
	size_t	line_length = 0;
	uint16_t* line_str16 = NULL;
	char* line_str8 = NULL;
	
	ALOGD("#######################%s,%d:[line_number]=%d#######################", __func__, __LINE__, line_number);
	for( tmp_count = 0; tmp_count < line_number; ++tmp_count){
		
		line_str16 = bio_get_string16(reply, &line_length);

		line_str8 = (char *)malloc( line_length );
		if(line_str8 ==  NULL){
			ALOGE("error, line_str8 is NULL");
			return ;
		}
		string16_to_string8( line_str16, line_str8 , line_length);
		//ALOGD("%s", line_str8);
		fprintf(stderr, "%s", line_str8);
		if(line_str8 != NULL){
			free(line_str8);
			line_str8 = NULL;
		}
		
	}
	ALOGD("#######################%s,%d:[line_number]=%d#######################", __func__, __LINE__, line_number);
}

static uint32_t judge_call_code(struct usr_native_cmd*cmd)
{
	uint32_t call_code = FUNC_SHELL_CMD_DEAL;
	
	if(!strncmp(cmd->cmd[1], "1", 1)){
		
		call_code = FUNC_SHELL_CMD_DEAL;
		
	}else if(!strncmp(cmd->cmd[1], "2", 1)){

		call_code = FUNC_SHELL_CMD_ONLY_DO;	
		
	}else{
	
		call_code = FUNC_SHELL_CMD_DEAL;

	}
	ALOGD("%s,%d:call_code=%u", __func__, __LINE__ , call_code);
	return call_code;

}

static uint32_t shell_cmd_deal(struct usr_native_cmd*cmd)
{

	unsigned iodata[4096/4];
    struct binder_io msg, reply;
	uint32_t	ret  = 0;
	uint32_t	get_length = 0;
	uint16_t* read16_buf;
	int tmp_count;
	uint16_t* reply_info16[2];	
	char* cmd_8 = NULL;
	uint32_t call_code = FUNC_SHELL_CMD_DEAL;
	
    bio_init(&msg, iodata, sizeof(iodata), 4);
    bio_put_uint32(&msg, 0);  // strict mode header
    bio_put_string16_x(&msg, cmd->combined);

    if (binder_call(g_bs, &msg, &reply, g_handle, judge_call_code(cmd))){
		ALOGE("binder call error!!!");
		return USR_NATIVE_BINDER_ERROR;
	}
	if(judge_call_code(cmd) == FUNC_SHELL_CMD_DEAL){
		
		get_reply(&reply);
		
	}else if(judge_call_code(cmd) == FUNC_SHELL_CMD_ONLY_DO){

		ALOGD("%s,%d:return=%u", __func__, __LINE__ , bio_get_uint32(&reply));
	}
	
	
    binder_done(g_bs, &msg, &reply);
	return ret;
}


static uint32_t svcmgr_lookup(struct binder_state *bs, uint32_t target, const char *name)
{
    uint32_t handle;
    unsigned iodata[512/4];
    struct binder_io msg, reply;

    bio_init(&msg, iodata, sizeof(iodata), 4);
    bio_put_uint32(&msg, 0);  // strict mode header
    bio_put_string16_x(&msg, SVC_MGR_NAME);
    bio_put_string16_x(&msg, name);

    if (binder_call(bs, &msg, &reply, target, SVC_MGR_CHECK_SERVICE))
        return 0;

    handle = bio_get_ref(&reply);

    if (handle)
        binder_acquire(bs, handle);

    binder_done(bs, &msg, &reply);

    return handle;
}

static int usage_print(struct usr_native_cmd* cmd)
{
	int tmp_count;
	
	#if		0
	/* print argc and argv */
	ALOGD("%s,%d:argc=%d,argv:",__func__, __LINE__, argc);
	for(tmp_count = 1; tmp_count < argc; ++tmp_count)
		ALOGD("argv[%d]=%s", tmp_count, argv[tmp_count]);
	ALOGD("############END############");
	#endif
	if(cmd->cmd_arg_count < OPR_CMD_COUNT){
		
		ALOGD("%s	<shell_cmd>",cmd->cmd[0]);
		
		return USR_NATIVE_CMD_ERROR;
	}
	
	return USR_NATIVE_CMD_OK;
}








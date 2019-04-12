
#define LOG_TAG "UsrNativeServices"

#include "include/usr_native_services.h"

unsigned int token;
uint32_t	g_reply_count;

int main(int argc, char **argv)
{
    struct binder_state *bs;
    uint32_t svcmgr = BINDER_SERVICE_MANAGER;
    uint32_t handle;
	int ret;

    bs = binder_open(USR_NATIVE_SERVICES_BINDER_SIZE);
    if (!bs) {
        fprintf(stderr, "failed to open binder driver\n");
        return -1;
    }

	/* add service */
	ret = svcmgr_publish(bs, svcmgr, USR_NATIVE_SERVICES_NAME, &token);
    if (ret) {
        fprintf(stderr, "failed to publish hello service\n");
        return -1;
    }

    binder_loop(bs, usr_native_service_handler);

    return 0;
}

static int usr_native_service_handler(struct binder_state *bs,
                              struct binder_transaction_data *txn,
                              struct binder_io *msg,
                              struct binder_io *reply)
{
	uint32_t ret = 0;
	ALOGD("%s, %d", __func__, __LINE__);
	bio_get_uint32(msg);
	switch(txn->code){

		case FUNC_SHELL_CMD_DEAL:{
			ret = server_shell_cmd_deal(bs, txn, msg, reply, get_shell_cmd(msg));
			
			break;
		}
		case FUNC_SHELL_CMD_ONLY_DO:{
			
			ret = server_shell_cmd_only_do(bs, txn, msg, reply, get_shell_cmd(msg));
			bio_put_uint32(reply, ret);
			break;
		}
		default:{
			ALOGE("%s:can't find code=%d", __func__, txn->code);
			break;
		}
	
	}
	return 0;
}



static int svcmgr_publish(struct binder_state *bs, uint32_t target, const char *name, void *ptr)
{
    int status;
    unsigned iodata[512/4];
    struct binder_io msg, reply;

    bio_init(&msg, iodata, sizeof(iodata), 4);
    bio_put_uint32(&msg, 0);  // strict mode header
    bio_put_string16_x(&msg, SVC_MGR_NAME);
    bio_put_string16_x(&msg, name);
    bio_put_obj(&msg, ptr);

    if (binder_call(bs, &msg, &reply, target, SVC_MGR_ADD_SERVICE))
        return -1;

    status = bio_get_uint32(&reply);

    binder_done(bs, &msg, &reply);

    return status;
}


static void free_ptr(char* ptr)
{
	if(ptr !=  NULL){
		free(ptr);
		ptr = NULL;
	}
}

static void set_reply( FILE*pfd, struct binder_io*  reply)
{
	size_t read_len = 0;
	uint32_t cmd_reply_line_count = 0;
	uint32_t tmp_count = 0;
	char *lineptrs[200] = {NULL};

	/* get  context of shell_cmd and count */
	while(getline(&lineptrs[cmd_reply_line_count++], &read_len, pfd) != -1);
	
	cmd_reply_line_count--;

	ALOGD("###############%s,%d:[cmd_reply_line_count]=%d#################", __func__, __LINE__, cmd_reply_line_count);
	/* reply line number */
	bio_put_uint32(reply, cmd_reply_line_count);
	/* reply context of shell_cmd */
	for( tmp_count = 0; tmp_count <  cmd_reply_line_count ; ++tmp_count){
		bio_put_string16_x(reply, lineptrs[tmp_count]);
		ALOGD("%s",lineptrs[tmp_count]);
	}
	ALOGD("###############%s,%d:[cmd_reply_line_count]=%d#################", __func__, __LINE__, cmd_reply_line_count);	
	/* free lineptrs */
	for(tmp_count = 0; tmp_count < cmd_reply_line_count; ++tmp_count)
		free_ptr(lineptrs[tmp_count]);

}


static uint32_t server_shell_cmd_only_do(struct binder_state *bs, struct binder_transaction_data *txn,
                              struct binder_io *msg,
                              struct binder_io *reply,
                              char* shell_cmd)
{

	FILE * pfd = NULL;
	ALOGD("%s,%d:shell_cmd=%s", __func__, __LINE__, shell_cmd);
	
	pfd = popen(shell_cmd, "r");
	if(pfd ==  NULL){
		ALOGE("popen error!!");
		return USR_NATIVE_BINDER_ERROR;
	}
	
	free_ptr(shell_cmd);
	pclose(pfd);
	return USR_NATIVE_BINDER_OK;	
}


static uint32_t server_shell_cmd_deal(struct binder_state *bs, struct binder_transaction_data *txn,
                              struct binder_io *msg,
                              struct binder_io *reply,
                              char* shell_cmd)
{

	FILE * pfd = NULL;
	ALOGD("%s,%d:shell_cmd=%s", __func__, __LINE__, shell_cmd);
	
	pfd = popen(shell_cmd, "r");
	if(pfd ==  NULL){
			ALOGE("popen error!!");
			return USR_NATIVE_BINDER_ERROR;
	}

	set_reply(pfd, reply);
	free_ptr(shell_cmd);
	pclose(pfd);
	return USR_NATIVE_BINDER_OK;	
}


static  char* get_shell_cmd(struct binder_io* msg)
{
	size_t get_length;
	uint16_t* cmd_16 = bio_get_string16(msg, &get_length);
	char* cmd_8  = malloc(get_length+1);
	string16_to_string8(cmd_16, cmd_8, get_length);
	ALOGD("cmd_8=%s", cmd_8);
	return cmd_8;

}


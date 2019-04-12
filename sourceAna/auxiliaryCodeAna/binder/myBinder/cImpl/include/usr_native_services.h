#ifndef		USR_NATIVE_SERVICES_H
#define		USR_NATIVE_SERVICES_H
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/types.h>
#include <stdbool.h>
#include <string.h>
#include <private/android_filesystem_config.h>
#include <cutils/log.h>

#include "include/usr_native_cmd_deal.h"
#include "include/usr_native_cmd_base.h"
#include "include/binder.h"
#include "include/usr_string_translate.h"


#define	USR_NATIVE_SERVICES_BINDER_SIZE	(512*1024)
#define	USR_NATIVE_SERVICES_NAME	"shell_cmd_deal"

/* shell cmd result need be return */
#define	FUNC_SHELL_CMD_DEAL				1
/* shell cmd result needn't be return */
#define	FUNC_SHELL_CMD_ONLY_DO			2

#define	USR_NATIVE_BINDER_ERROR			99
#define	USR_NATIVE_BINDER_OK				0


static int svcmgr_publish(struct binder_state *bs, uint32_t target, const char *name, void *ptr);
static int usr_native_service_handler(struct binder_state *bs,
                              struct binder_transaction_data *txn,
                              struct binder_io *msg,
                              struct binder_io *reply);

static  char* get_shell_cmd(struct binder_io* msg);
static uint32_t shell_cmd_deal(struct usr_native_cmd* cmd);
static uint32_t server_shell_cmd_deal(struct binder_state *bs, 
							  struct binder_transaction_data *txn,
                              struct binder_io *msg,
                              struct binder_io *reply,
                              char* shell_cmd);
static void free_ptr(char* ptr);
static uint32_t server_shell_cmd_only_do(struct binder_state *bs, 
							  struct binder_transaction_data *txn,
                              struct binder_io *msg,
                              struct binder_io *reply,
                              char* shell_cmd);





#endif

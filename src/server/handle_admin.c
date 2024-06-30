#include "../../include/user/user.h"
#include "../../include/server/handle_admin.h"
#include "../../include/server/handle_client.h"
#include "../../include/server/server.h"
#include "../../include/config/config.h"
#include "../../include/status.h"
#include "../../include/log.h"
#include "../../include/utils.h"

#include <string.h>

#define ADMIN_INTERFACE "ADMIN"

void handle_admin_create_user(client_t *cli, int argc, char **argv)
{
	(void)argc;

	char username[99];
	unsigned int access_lvl;
	

	if (!is_numerical_cpy(argv[2],(int *)&access_lvl))
	{
		log_err_message("[%s] %s/%s: Use use an integer for access level parameter!", ADMIN_INTERFACE,cli->ipv4addr,cli->user->username);
		bad_status(cli,NOT_INTEGER,"Put an integer for access level parameter. CREATE_USER <username> <password> <access level>");
		return;
	}
	
	User *user = (User*)calloc(sizeof(User),1);
	strncpy(username,argv[0],sizeof(username));
	strncpy(user->password,argv[1],sizeof(user->password));	
	strncpy(user->username,username,sizeof(user->username));
	user->access_level = access_lvl;
	char *errmsg;
	int useradd_res = add_user(user,&errmsg);
	char re_msg[128];

	switch (useradd_res)
	{
		case USER_CREATED:
			snprintf(re_msg,sizeof(re_msg), "User '%s' was created!", user->username);
			ok_status(cli,USER_CREATED,re_msg);
			log_sys_message("[%s] %s",ADMIN_INTERFACE, re_msg);
		break;

		case USERNAME_TOO_LONG:
			bad_status(cli,USERNAME_TOO_LONG,errmsg);
			log_err_message("[%s] %s",ADMIN_INTERFACE, errmsg);
		break;

		case USERNAME_EXIST:
			bad_status(cli,USERNAME_EXIST,errmsg);
			log_err_message("[%s] %s",ADMIN_INTERFACE, errmsg);
		break;

		default:
			printf("DEFAULT CONDITIOh");
			ok_status(cli,DB_ERROR,errmsg);
			log_sys_message("[%s] %s",ADMIN_INTERFACE, errmsg);
		break;


	};

	if (errmsg != NULL)
	{
		printf("Freed up errmsg\n");
		free(errmsg);
	}


	free(user);
	return;
}

void handle_admin_delete_user(client_t *cli, int argc, char **argv)
{
	(void)argc;
	(void)argv;
	(void)cli;
}

void handle_admin_set_permission(client_t *cli, int argc, char **argv)
{
	(void)argc;
	(void)argv;
	(void)cli;
}

void handle_admin_edit_pwd(client_t *cli, int argc, char **argv)
{
	(void)argc;
	(void)argv;
	(void)cli;
}
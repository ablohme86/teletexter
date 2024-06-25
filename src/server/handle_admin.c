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

	char username[config.userConfig.maxNicknameLength];
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
	// copy the new username with the configured limit to the user object
	strncpy(user->username,username,sizeof(user->username));
	user->access_level = access_lvl;
	int useradd_res = add_user(user);
	
	if (useradd_res == 0)
	{
		log_sys_message("[%s] %s/%s: New user '%s' is created", ADMIN_INTERFACE,cli->ipv4addr,cli->user->username,user->username);
		ok_status(cli,USER_CREATED,"New user created!");
		free(user);
		return;
	}
	if (useradd_res > 0)
	{
		log_sys_message("[%s] %s/%s: Username '%s' is already in use", ADMIN_INTERFACE,cli->ipv4addr,cli->user->username, user->username);
		bad_status(cli,USERNAME_EXIST,"Username already exists!");
		free(user);
		return;
	}
	free(user);
	bad_status(cli,DB_ERROR,"Database error occured!");
	
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
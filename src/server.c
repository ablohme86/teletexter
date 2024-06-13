#include "../include/server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../include/lcd_disp.h"
#include "../include/config.h"
char *get_ip(client_t *cli)
{
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	getpeername(cli->socket, (struct sockaddr *)&addr, &addr_len);
	return inet_ntoa(addr.sin_addr);
	
}

int start_server()
{
	int server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len = sizeof(client_addr);

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) 
	{
		perror("Could not create socket");
		return EXIT_FAILURE;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(config.serverConfig.port);
	
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
	{
		perror("Bind failed");
		lcd_text("Sockets failed!",2,CENTER);
		close(server_socket);
		return EXIT_FAILURE;
	}

	if (listen(server_socket, 3) < 0) 
	{
		perror("Listen failed");
		close(server_socket);
		return EXIT_FAILURE;
	}

	printf("Server listening on port %d\n", config.serverConfig.port);

	while (1) 
	{
		client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
		if (client_socket < 0) 
		{
			perror("Accept failed");
			continue;
		}

		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->socket = client_socket;
		memset(cli->nickname, 0, sizeof(cli->nickname));

		pthread_t tid;
		if (pthread_create(&tid, NULL, client_handler, (void *)cli) != 0) 
		{
			perror("Could not create thread");
			free(cli);
		} 
		else 
		{
			add_client(cli);
		}
	}

	close(server_socket);
	return EXIT_SUCCESS;
}

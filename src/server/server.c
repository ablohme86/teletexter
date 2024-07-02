#include "../../include/server/server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../../include/version.h"
#ifndef DISABLE_LCD
#include "../../include/lcd/lcd_txt.h"
#endif
#include <log.h>
#include "../../include/config/config.h"

int server_socket, client_socket;

char ip[INET_ADDRSTRLEN];

char *get_ip(client_t *cli)
{
	socklen_t addr_len = sizeof(cli->address);
	getpeername(cli->socket, (struct sockaddr *)&cli->address, &addr_len);
	return inet_ntoa(cli->address.sin_addr);
	
}
void close_server()
{
	close(client_socket);
	close(server_socket);

}

void ok_status(client_t *cli, int status_no,char *cust_msg,...)
{

	char st_buff[BUFFER_SIZE];
	if (cust_msg == NULL)
	{
		char desc_buff[50];
		snprintf(desc_buff,sizeof(desc_buff), "OK %d\r\n", status_no);
	}
	else
	{
		snprintf(st_buff,sizeof(st_buff), "OK %d :%s\r\n",status_no,cust_msg);
	}
	send(cli->socket,st_buff,strlen(st_buff),0);
}
void bad_status(client_t *cli, int status_no,char *cust_msg,...)
{

	char st_buff[BUFFER_SIZE];
	if (cust_msg == NULL)
	{
		char desc_buff[50];
		snprintf(desc_buff,sizeof(desc_buff), "ERROR %d\r\n", status_no);
	}
	else
	{
		snprintf(st_buff,sizeof(st_buff), "ERROR %d :%s\r\n",status_no,cust_msg);
	}
	send(cli->socket,st_buff,strlen(st_buff),0);
}

int start_server()
{
    struct addrinfo hints, *res, *p;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char ip[INET_ADDRSTRLEN];

    // Nullstill hints-strukturen og sett ønskede verdier
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // Bruk IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    // Løs opp vertens IP-adresse
    int status = getaddrinfo(config.serverConfig.serverHost, NULL, &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return EXIT_FAILURE;
    }

    // Iterate through the results and bind to the first we can
    for (p = res; p != NULL; p = p->ai_next)
    {
        struct sockaddr_in *server_addr = (struct sockaddr_in *)p->ai_addr;
        server_addr->sin_port = htons(config.serverConfig.port); // Sett portnummer

        server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server_socket == -1)
        {
            continue;
        }

        if (bind(server_socket, (struct sockaddr *)server_addr, sizeof(*server_addr)) == 0)
        {
            break; // Sucessfully bound
        }

        close(server_socket);
    }

    if (p == NULL)
    {
        log_sys_message("Cannot start server: Port %d already in use!", config.serverConfig.port);
        perror("Bind error");
        freeaddrinfo(res);
        return EXIT_FAILURE;
    }

    inet_ntop(AF_INET, &(((struct sockaddr_in *)p->ai_addr)->sin_addr), ip, INET_ADDRSTRLEN);

    freeaddrinfo(res); // Frigjør getaddrinfo-resultatene

    if (listen(server_socket, 3) < 0)
    {
        log_err_message("[SERVER] Listen failed!");
        perror("Listen failed");
        close(server_socket);
        return EXIT_FAILURE;
    }

    log_sys_message("[SERVER] Server listening on %s @ port %d", ip, config.serverConfig.port);

    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0)
        {
            log_err_message("[SERVER] Could not accept client!");
            perror("Accept failed");
            continue;
        }

        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->socket = client_socket;
        cli->user = (User *)calloc(sizeof(User),1); // allocate memory for the new User object, use calloc so all variables will be 0
        cli->ipv4addr = strdup(inet_ntoa(client_addr.sin_addr)); // copy the IP address
        cli->identified = 0;

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, (void *)cli) != 0)
        {
            log_err_message("[SERVER] Could not create thread for the clients!");
            perror("Could not create thread");

            free(cli->user);
            free(cli->ipv4addr); // free the IP address
            free(cli);
            return EXIT_FAILURE;
        }
        else
        {
            add_client(cli);
            send_server_ident(cli);
        }
    }
    close(server_socket);
    return EXIT_SUCCESS;
}
/*int send_client(client_t *cli,char **errmsg, char *msg,...)
{

} */

void send_server_ident(client_t *cli)
{

	char welcome_msg_line1[512];

	snprintf(welcome_msg_line1,sizeof(welcome_msg_line1),"TELETEXTER v%d.%d Build: %d\r\nSERVER_IDENT %s\r\nADDRESS %s\r\n", MAJOR,MINOR,BUILD,config.serverConfig.serverIdentifier,config.serverConfig.serverHost);
	send(cli->socket,welcome_msg_line1,strlen(welcome_msg_line1),0);

}
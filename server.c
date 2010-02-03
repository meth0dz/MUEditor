#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifdef __WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include "pthread.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "netdb.h"
#endif

#define MAX_CLIENTS 10
#define MAX_QUE		5

bool server_init(void);
bool server_create(void);
#ifdef _WIN32
void server_get_connections(SOCKET serv_socket);
#else
void server_get_connections(int serv_socket);
#endif
void server_cleanup(void);
void server_handle_clients(void* client_index);

SOCKET * client_sockets;
struct sockaddr * client_addrs;

int main(int argc, char ** argv)
{
		SOCKET serv_socket;
		if (server_init()) {
			if (server_create()) {
				printf("Server is running.\n");
				while (1);
				return 0;
			}
			server_cleanup();
		}
		return 1;
}

bool server_init(void)
{
	#ifdef _WIN32 
	WSADATA wdata;
	if (WSAStartup(MAKEWORD(2, 2), &wdata)) {
		return false;
	}
	#endif
	return true;
}

bool server_create(void)
{
	struct sockaddr_in serv_addr;
	memset((void*)&serv_addr, 0, sizeof(struct sockaddr_in));
	#ifdef _WIN32
		SOCKET server_socket;
		if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != INVALID_SOCKET) {
			serv_addr.sin_port = htons(1001);
	#else
		int server_socket;
		pthread_t threadid;
		if ((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) != -1) {
			serv_addr.sin_port = (short)1001;
	#endif
			serv_addr.sin_addr.s_addr = INADDR_ANY;
			serv_addr.sin_family = AF_INET;
			if (!bind(server_socket, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in))) {
				if (!listen(server_socket, MAX_QUE)) {
	#if _WIN32
					CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&server_get_connections, (void*)server_socket, 0, 0);
	#else
					pthread_create(&threadid, NULL, &server_get_connections, (void*)server_socket);
	#endif
					return true;
				}
			}
		}
		return false;
}
#ifdef _WIN32
void server_get_connections(SOCKET serv_socket)
#else
void server_get_connections(int serv_socket)
#endif
{
		int client_index;
	#if !defined(_WIN32)
		pthread_t threadid;
	#endif
		if (client_sockets = calloc(MAX_CLIENTS, sizeof(SOCKET))) {
	#ifdef _WIN32
			memset(client_sockets, INVALID_SOCKET, sizeof(SOCKET) * MAX_CLIENTS);
	#else
			memset(client_sockets, -1, sizeof(SOCKET) * MAX_CLIENTS);
	#endif
			if (client_addrs = calloc(MAX_CLIENTS, sizeof(struct sockaddr))) {
				while (true) {
					for (client_index = 0; client_index < MAX_CLIENTS; client_index++) {
	#ifdef _WIN32
						if (client_sockets[client_index] == INVALID_SOCKET) break;
	#else
						if (client_sockets[client_index] == -1) break;
	#endif
					}
					if (client_index != MAX_CLIENTS) {
	#ifdef _WIN32
						if ((client_sockets[client_index] = accept(serv_socket, &(client_addrs[client_index]), NULL)) != INVALID_SOCKET) {
							CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&server_handle_clients, (void*)client_index, 0, 0);
						}
	#else
						if ((client_sockets[client_index] = accept(serv_socket, &(client_addrs[client_index]), NULL)) != -1)
							pthread_create(&threadid, NULL, &server_handle_clients, (void*)client_index);
	#endif
					}
	#ifdef _WIN32
					Sleep (100);
	#else
					usleep(100000);
	#endif
				}
				free (client_addrs);
			}
			free (client_sockets);
		}
	return;
}

void server_handle_clients(void* client_index)
{
		printf("Client #: %d\n", client_index);
		client_sockets[(int)client_index] = 0;
	return;
}

void server_cleanup(void)
{
	#ifdef _WIN32
		for (int i = 0; i < MAX_CLIENTS; i++) 
			if (client_sockets[i] != INVALID_SOCKET) closesocket(client_sockets[i]);
		WSACleanup();
	#else
		int i;
		for (i = 0; i < MAX_CLIENTS; i++)
			if (client_sockets[i] != -1) close(client_sockets[i]);
	#endif
		if (client_addrs)
			free (client_addrs);
		if (client_sockets)
			free (client_sockets);
		return;
}

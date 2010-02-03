#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <winsock2.h>

#define MAX_CLIENTS 10

bool init_server(void);
bool create_server(void);
void handle_clients_server(void* client_index);
void get_connections_server(SOCKET serv_socket);
int get_first_available(SOCKET* list, int list_length);
void cleanup_server(void);

SOCKET * client_sockets;
struct sockaddr * client_addrs;

int main(int argc, char ** argv)
{
	SOCKET serv_socket;
	if (init_server()) {
		if (create_server()) {
			printf("Server is running.\n");
			while (1);
			return 0;
		}
		cleanup_server();
	}
	return 1;
}

// It will be debatable whether we won't to create the server socket in this function or wait until later
bool init_server(void)
{
	#ifdef _WIN32 
	WSADATA wdata;
	if (!WSAStartup(MAKEWORD(2, 2), &wdata)) {
		return true;
	}
	return false;
	#endif
}

bool create_server(void)
{
	#ifdef _WIN32
	SOCKET server_socket;
	struct sockaddr_in serv_addr;
	if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != INVALID_SOCKET) {
		// Won't use ZeroMemory here
		memset((void*)&serv_addr, 0, sizeof(struct sockaddr_in));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(1001);
		if (!bind(server_socket, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in))) {
			if (!listen(server_socket, 5)) {
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&get_connections_server, (void*)server_socket, 0, 0);
				return true;
			}
		}
	}
	#endif
	return false;
}

void get_connections_server(SOCKET serv_socket)
{
	#ifdef _WIN32
	struct sockaddr client_addr;
	int client_index;
	if (client_sockets = calloc(MAX_CLIENTS, sizeof(SOCKET))) {
		memset(client_sockets, INVALID_SOCKET, sizeof(SOCKET) * MAX_CLIENTS);
		if (client_addrs = calloc(MAX_CLIENTS, sizeof(struct sockaddr))) {
			while (true) {
				client_index = get_first_available(client_sockets, MAX_CLIENTS);
				if ((client_sockets[client_index] = accept(serv_socket, &client_addr, NULL)) != INVALID_SOCKET) {
					CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&handle_clients_server, (void*)client_index, 0, 0);
				}
				Sleep (100);
			}
			free (client_addrs);
		}
		free (client_sockets);
	}
	#endif
	return;
}

void handle_clients_server(void* client_index)
{
	printf("Client #: %d\n", client_index);
	client_sockets[(int)client_index] = 0;
	return;
}

int get_first_available(SOCKET* list, int list_length)
{
	for (int i = 0; i < list_length; i++)
		if (list[i] == INVALID_SOCKET) return i;
}

void cleanup_server(void)
{
	WSACleanup();
	return;
}
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <winsock2.h>

#define MAX_CLIENTS 10

bool server_init(void);
bool server_create(void);
void server_get_connections(SOCKET serv_socket);
void server_cleanup(void);
void server_handle_clients(void* client_index);
int get_first_available(SOCKET* list, int list_length);

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

// It will be debatable whether we won't to create the server socket in this function or wait until later
bool server_init(void)
{
	#ifdef _WIN32 
	WSADATA wdata;
	if (!WSAStartup(MAKEWORD(2, 2), &wdata)) {
		return true;
	}
	return false;
	#endif
}

bool server_create(void)
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
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&server_get_connections, (void*)server_socket, 0, 0);
				return true;
			}
		}
	}
	#endif
	return false;
}

void server_get_connections(SOCKET serv_socket)
{
	#ifdef _WIN32
	struct sockaddr client_addr;
	int client_index;
	if (client_sockets = calloc(MAX_CLIENTS, sizeof(SOCKET))) {
		memset(client_sockets, INVALID_SOCKET, sizeof(SOCKET) * MAX_CLIENTS);
		if (client_addrs = calloc(MAX_CLIENTS, sizeof(struct sockaddr))) {
			while (true) {
				for (client_index = 0; client_index < MAX_CLIENTS; client_index++)
					if (client_sockets[client_index] == INVALID_SOCKET) break;
				if (client_index != MAX_CLIENTS) {
					if ((client_sockets[client_index] = accept(serv_socket, &client_addr, NULL)) != INVALID_SOCKET) 
						CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&server_handle_clients, (void*)client_index, 0, 0);
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

void server_handle_clients(void* client_index)
{
	printf("Client #: %d\n", client_index);
	client_sockets[(int)client_index] = 0;
	return;
}

void server_cleanup(void)
{
	WSACleanup();
	if (client_addrs)
		free (client_addrs);
	if (client_sockets)
	free (client_sockets);
	return;
}

int get_first_available(SOCKET* list, int list_length)
{
	for (int i = 0; i < list_length; i++)
		if (list[i] == INVALID_SOCKET) return i;
}


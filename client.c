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

bool setup_client(void);
bool connect_client(char* ip, long port);

#ifdef __WIN32
SOCKET client_socket;
#else
int client_socket;
#endif

int main(int argc, char** argv)
{
	if (setup_client()) {
		if (connect_client("127.0.0.1", 1001)) {
			printf("Connected!\n");
		}
	}
	return 0;
}

bool setup_client(void)
{
	#ifdef _WIN32 
		WSADATA wdata;
		if (WSAStartup(MAKEWORD(2, 2), &wdata)) {
			return false;
		}
	#endif
	return true;
}

bool connect_client(char* ip, long port)
{
		struct sockaddr_in serv_addr;
		memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	#if _WIN32
		if ((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != INVALID_SOCKET) {
			serv_addr.sin_addr.s_addr = inet_addr(ip);
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(port);
	#else 
		if ((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != -1) {
			inet_aton(ip, &(serv_addr.sin_addr));
			serv_addr.sin_family = PF_INET;
			serv_addr.sin_port = port;
	#endif
			if (!connect(client_socket, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in))) {
				return true;
			}
		}
		return false;
}

void cleanup_client(void)
{
	#ifdef _WIN32
		closesocket(client_socket);
		WSACleanup();
	#else
		close(client_socket);
	#endif
	return;
}

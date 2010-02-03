#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <winsock2.h>

bool setup_client(void);
bool connect_client(char* ip, long port);

SOCKET client_socket;

int main(int argc, char** argv)
{
	if (setup_client()) {
		if (connect_client("127.0.0.1", 1001)) {
			printf("Yay!\n");
		}
	}
	return 0;
}

bool setup_client(void)
{
	#ifdef _WIN32 
	WSADATA wdata;
	if (!WSAStartup(MAKEWORD(2, 2), &wdata)) {
		return true;
	}
	return false;
	#endif
}

bool connect_client(char* ip, long port)
{
	struct sockaddr_in serv_addr;
	SOCKET client_socket;
	if ((client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != INVALID_SOCKET) {
		memset(&serv_addr, 0, sizeof(struct sockaddr_in));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = inet_addr(ip);
		serv_addr.sin_port = htons(port);
		if (!connect(client_socket, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in)))
			return true;
	}
	return false;
}
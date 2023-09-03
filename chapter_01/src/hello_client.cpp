#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib ") 
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using SOCKET = int;
using INVALID_SOCKET = -1;
using SOCKET_ERROR = -1;
using closesocket = close;
#endif

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		print_console(PRINT_ERROR, "args not valid!");
		print_console(PRINT_INFOR, "Usage: %s <IP> <port>", argv[0]);
		return -1;
	}

	if (!InitSocketEnv())
	{
		print_console(PRINT_ERROR, "failed to init socket env.");
		return -1;
	}

	SOCKET client_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET)
	{
		print_console(PRINT_ERROR, "failed to get socket!");
		return -1;
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	// AF_INET  IPv4互联网协议族
	// AF_INET6 IPv6互联网协议族
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr); //使用窄字符
	//InetPton 使用宽字符
	server_addr.sin_port = htons(atoi(argv[2]));

	if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		print_console(PRINT_ERROR, "failed to connect server!");
		return -1;
	}

	char message[30];
	int str_len = recv(client_sock, message, sizeof(message) - 1, 0);
	if (str_len == -1)
	{
		print_console(PRINT_ERROR, "failed to get response!");
		return -1;
	}
	print_console(PRINT_INFOR, "Get response from server: %s", message);
	closesocket(client_sock);

	CleanSocketEnv();
	return 0;
}

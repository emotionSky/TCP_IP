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
#define closesocket close
using SOCKET = int;
using INVALID_SOCKET = -1;
using SOCKET_ERROR = -1;
#endif

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		print_console(PRINT_ERROR, "args not valid!");
		print_console(PRINT_INFOR, "Usage: %s <port>", argv[0]);
		return -1;
	}

	if (!InitSocketEnv())
	{
		print_console(PRINT_ERROR, "failed to init socket env.");
		return -1;
	}

	SOCKET server_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (server_sock == INVALID_SOCKET)
	{
		print_console(PRINT_ERROR, "failed to get socket!");
		return -1;
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	// AF_INET  IPv4互联网协议族
	// AF_INET6 IPv6互联网协议族
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr.s_addr); //使用窄字符
	//InetPton 使用宽字符
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		print_console(PRINT_ERROR, "failed to bind port: %s", argv[1]);
		return -1;
	}

	if (listen(server_sock, 5) == SOCKET_ERROR)
	{
		print_console(PRINT_ERROR, "failed to listen port: %s", argv[1]);
		return -1;
	}

	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	SOCKET client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
	if (client_sock == INVALID_SOCKET)
	{
		print_console(PRINT_ERROR, "failed to accept request.");
		return -1;
	}

	char message[] = "Hello World!";
	send(client_sock, message, sizeof(message), 0);
	//write(clnt_sock, message, sizeof(message));
	closesocket(client_sock);
	closesocket(server_sock);

	CleanSocketEnv();
	return 0;
}

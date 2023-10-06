#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#if defined(_WIN32) || defined(_WINDLL)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib ") 
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1024;

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
		print_console(PRINT_ERROR, "InitSocketEnv() error!");
		return -1;
	}

	sock_t server_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (server_sock == invalid_sock)
	{
		print_console(PRINT_ERROR, "failed to get socket!");
		return -1;
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	// AF_INET  IPv4互联网协议族
	// AF_INET6 IPv6互联网协议族
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to bind port: %s", argv[1]);
		return -1;
	}

	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	char message[BUFFER_SIZE] = { 0 };
	int recv_len = 0;

	while (true)
	{
		recv_len = recvfrom(server_sock, message, BUFFER_SIZE - 1, 0, (struct sockaddr*)&client_addr, &client_addr_size);
		if (recv_len > 0)
		{
			message[recv_len] = 0;
			print_console(PRINT_INFOR, "Recv from client: %s", message);

			sendto(server_sock, message, recv_len, 0, (struct sockaddr*)&client_addr, client_addr_size);
		}
		else
		{
			int error = GetSocketError();
			print_console(PRINT_ERROR, "error code: %d", error);
		}
	}
	close_socket(server_sock);

	CleanSocketEnv();
	return 0;
}

#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <cstdio>
#include <cstring>
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

	sock_t client_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (client_sock == invalid_sock)
	{
		print_console(PRINT_ERROR, "failed to get socket!");
		return -1;
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(atoi(argv[2]));

	if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to connect server!");
		return -1;
	}

	char message[BUFFER_SIZE];
	int str_len = 0, recv_len = 0;
	while (true)
	{
		print_console(PRINT_INFOR, "Input message (Q to quit): ");
		fgets(message, BUFFER_SIZE - 1, stdin);

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
		{
			break;
		}

		str_len = send(client_sock, message, static_cast<int>(strlen(message)), 0);
		recv_len = recv(client_sock, message, BUFFER_SIZE - 1, 0);
		message[recv_len] = 0;
		print_console(PRINT_INFOR, "Get message from server: %s", message);
	}
	close_socket(client_sock);

	CleanSocketEnv();
	return 0;
}

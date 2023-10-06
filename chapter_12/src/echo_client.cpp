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
	// AF_INET  IPv4互联网协议族
	// AF_INET6 IPv6互联网协议族
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr); //使用窄字符
	//InetPton 使用宽字符
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
		fgets(message, BUFFER_SIZE, stdin);

		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
		{
			break;
		}

		str_len = send(client_sock, message, (int)strlen(message), 0);
		recv_len = 0;

		// 这里已经结合chapter_05解决了接受不全的问题了，就不再chapter_05赘述
		while (recv_len < str_len)
		{
			int len = recv(client_sock, &message[recv_len], BUFFER_SIZE - recv_len - 1, 0);
			if (len == sock_error)
			{
				print_console(PRINT_ERROR, "recv error!");
				return -1;
			}
			recv_len += len;
		}
		
		message[recv_len] = 0;
		print_console(PRINT_INFOR, "Get response from server: %s", message);
	}
	close_socket(client_sock);

	CleanSocketEnv();
	return 0;
}

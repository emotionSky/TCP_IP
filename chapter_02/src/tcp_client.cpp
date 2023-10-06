#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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
	// AF_INET  IPv4������Э����
	// AF_INET6 IPv6������Э����
	server_addr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr); //ʹ��խ�ַ�
	//InetPton ʹ�ÿ��ַ�
	server_addr.sin_port = htons(atoi(argv[2]));

	if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to connect server!");
		return -1;
	}

	char message[30] = { 0 };
	int read_index = 0, read_len = 0, str_len = 0;
	while (read_len = recv(client_sock, &message[read_index++], 1, 0))
	{
		if (read_len == -1)
		{
			print_console(PRINT_ERROR, "failed to recv data from server.");
			break;
		}

		str_len += read_len;
	}
	print_console(PRINT_INFOR, "Get response from server: %s", message);
	close_socket(client_sock);

	CleanSocketEnv();
	return 0;
}

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
#include <sys/select.h>
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
		print_console(PRINT_ERROR, "failed to init socket env.");
		return -1;
	}

	sock_t server_sock = socket(PF_INET, SOCK_STREAM, 0);
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
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	inet_pton(AF_INET, "0.0.0.0", &server_addr.sin_addr.s_addr); //使用窄字符
	//InetPton 使用宽字符
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to bind port: %s", argv[1]);
		return -1;
	}

	if (listen(server_sock, 5) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to listen port: %s", argv[1]);
		return -1;
	}

	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	sock_t client_sock = invalid_sock;
	int recv_len = 0;
	char message[BUFFER_SIZE] = { 0 };

	fd_set reads;
	FD_ZERO(&reads);
	FD_SET(server_sock, &reads);
	int fd_max = server_sock;
	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	int fd_num = 0;

	while (true)
	{
		fd_set tmp_reads = reads;

		if ((fd_num = select(fd_max + 1, &tmp_reads, 0, 0, &timeout)) == -1)
			break;

		if (fd_num == 0)
			continue;

		for (int i = 0; i < fd_max + 1; i++)
		{
			if (FD_ISSET(i, &tmp_reads))
			{
				if (i == server_sock)     // connection request!
				{
					client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
					FD_SET(client_sock, &reads);
					if (fd_max < client_sock)
						fd_max = client_sock;
					print_console(PRINT_INFOR, "connected client: %d", client_sock);
				}
				else
				{
					recv_len = recv(i, message, BUFFER_SIZE - 1, 0);
					if (recv_len == 0) 
					{
						FD_CLR(i, &reads);
						close_socket(i);
						print_console(PRINT_INFOR, "closed client: %d", i);
					}
					else
					{
						send(i, message, recv_len, 0);
					}
				}
			}
		}
	}
	close_socket(server_sock);

	CleanSocketEnv();
	return 0;
}

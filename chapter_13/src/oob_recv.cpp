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
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1024;

sock_t client_sock = invalid_sock;
#if defined(_WIN32) || defined(_WINDLL)
#else
void urg_handler(int signo)
{
	char message[BUFFER_SIZE];
	int recv_len = recv(client_sock, message, BUFFER_SIZE - 1, MSG_OOB);
	message[recv_len] = 0;
	print_console(PRINT_INFOR, "Urgent message: %s", message);
}
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

#if defined(_WIN32) || defined(_WINDLL)	
	bool bRunning = true;
	fd_set read, except, copy_read, copy_except;
	client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
	if (client_sock == invalid_sock)
	{
		print_console(PRINT_ERROR, "failed to accept request.");
		bRunning = false;
	}
	else
	{
		FD_ZERO(&read);
		FD_ZERO(&except);
		FD_SET(client_sock, &read);
		FD_SET(client_sock, &except);
	}

	int recv_len = 0;
	char message[BUFFER_SIZE] = { 0 };
	while (bRunning)
	{
		copy_read = read;
		copy_except = except;
		struct timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		int res = select(0, &copy_read, 0, &copy_except, &timeout);

		if (res > 0)
		{
			if (FD_ISSET(client_sock, &copy_except))
			{
				recv_len = recv(client_sock, message, BUFFER_SIZE - 1, MSG_OOB);
				message[recv_len] = 0;
				print_console(PRINT_INFOR, "Urgent message: %s", message);
			}

			if (FD_ISSET(client_sock, &copy_read))
			{
				recv_len = recv(client_sock, message, BUFFER_SIZE - 1, 0);
				if (recv_len == 0)
				{
					close_socket(client_sock);
					break;					
				}
				else
				{
					message[recv_len] = 0;
					print_console(PRINT_INFOR, "read message: %s", message);
				}
			}
		}
	}
	close_socket(server_sock);
#else
	struct sigaction act;
	act.sa_handler = urg_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
	if (client_sock == invalid_sock)
	{
		print_console(PRINT_ERROR, "failed to accept request.");
		return -1;
	}

	fcntl(client_sock, F_SETOWN, getpid());
	int state = sigaction(SIGURG, &act, 0);

	int recv_len = 0;
	char message[BUFFER_SIZE] = { 0 };
	while ((recv_len = recv(client_sock, message, BUFFER_SIZE - 1, 0)) != 0)
	{
		if (recv_len == -1)
			continue;
		message[recv_len] = 0;
		print_console(PRINT_INFOR, "read message: %s", message);
	}
	close_socket(client_sock);
	close_socket(server_sock);

#endif

	CleanSocketEnv();
	return 0;
}

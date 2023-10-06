// 水平触发/条件触发

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
#include <sys/epoll.h>
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1024;
constexpr int EPOLL_SIZE = 50;

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

#if defined(_WIN32) || defined(_WINDLL)
	d_handle epoll_fd = nullptr;
#else
	int epoll_fd = 0;
#endif
	epoll_fd = epoll_create(EPOLL_SIZE);
	epoll_event* ep_events = (epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
	struct epoll_event event;
	event.events = EPOLLIN;
#if defined(_WIN32) || defined(_WINDLL)
	event.data.sock = server_sock;
#else
	event.data.fd = server_sock;
#endif

	int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock, &event);
	if (ret < 0)
	{
		SocketError sock_err;
		print_console(PRINT_ERROR, "epoll_ctrl error: %s", sock_err.GetErrorString());
		return -1;
	}

	int event_cnt = 0;
	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	sock_t client_sock = invalid_sock;
	int str_len = 0;
	char buf[BUFFER_SIZE];

	while (true)
	{
		event_cnt = epoll_wait(epoll_fd, ep_events, EPOLL_SIZE, -1);
		if (event_cnt == -1)
		{
			print_console(PRINT_ERROR, "epoll_wait error.");
			break;
		}

		for (int i = 0; i < event_cnt; i++)
		{
#if defined(_WIN32) || defined(_WINDLL)
			sock_t deal_sock = ep_events[i].data.sock;
#else
			sock_t deal_sock = ep_events[i].data.fd;
#endif
			if (deal_sock == server_sock)
			{
				client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);

				event.events = EPOLLIN;
#if defined(_WIN32) || defined(_WINDLL)
				event.data.sock = client_sock;
#else
				event.data.fd = client_sock;
#endif
				epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event);
				print_console(PRINT_INFOR, "connected client: %d", client_sock);
			}
			else
			{
				str_len = recv(deal_sock, buf, BUFFER_SIZE - 1, 0);
				if (str_len == 0)    // close request!
				{
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, deal_sock, nullptr);
					close_socket(deal_sock);
					print_console(PRINT_INFOR, "closed client: %d", deal_sock);
				}
				else
				{
					send(deal_sock, buf, str_len, 0);    // echo!
				}

			}
		}
	}

	close_socket(server_sock);
	epoll_close(epoll_fd);
	CleanSocketEnv();
	return 0;
}


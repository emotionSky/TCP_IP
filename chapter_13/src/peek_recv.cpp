﻿#include <DreamSky/dream_print.h>
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

	bool bRunning = true;
	sock_t client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
	if (client_sock == invalid_sock)
	{
		print_console(PRINT_ERROR, "failed to accept request.");
		bRunning = false;
	}

	int recv_len = 0;
	char message[BUFFER_SIZE] = { 0 };
	while (bRunning)
	{
		int flags = MSG_PEEK;
#ifndef _WIN32
		flags |= MSG_DONTWAIT;
#endif
		recv_len = recv(client_sock, message, BUFFER_SIZE - 1, flags);
		if (recv_len > 0)
			break;
	}

	message[recv_len] = 0;
	print_console(PRINT_INFOR, "Buffering %d bytes: %s", recv_len, message);

	recv_len = recv(client_sock, message, BUFFER_SIZE - 1, 0);
	message[recv_len] = 0;
	print_console(PRINT_INFOR, "Read again: %s \n", message);
	close_socket(server_sock);
	close_socket(client_sock);

	CleanSocketEnv();
	return 0;
}

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
	if (argc != 3)
	{
		print_console(PRINT_ERROR, "args not valid!");
		print_console(PRINT_INFOR, "Usage: %s <IP> <port>", argv[0]);
		return -1;
	}

	if (!InitSocketEnv())
	{
		print_console(PRINT_ERROR, "InitSocketEnv() error!");
		return -1;
	}

	FILE* fp = fopen("./file_client.dat", "wb");
	if (!fp)
	{
		print_console(PRINT_ERROR, "failed to open file!");
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
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(atoi(argv[2]));

	if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to connect to server!");
		return -1;
	}

	char buf[BUFFER_SIZE] = { 0 };
	int recv_len = 0;
	while ((recv_len = recv(client_sock, buf, BUFFER_SIZE, 0)) != 0)
	{
		fwrite(buf, 1, recv_len, fp);
	}

	print_console(PRINT_INFOR, "finish to recv data.");
	const char* response = "Thank you!";
	send(client_sock, response, static_cast<int>(strlen(response)), 0);
	fclose(fp);
	close_socket(client_sock);

	CleanSocketEnv();
	return 0;
}

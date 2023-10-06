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

	FILE* fp = fopen("./file_client.dat", "rb");
	if (!fp)
	{
		print_console(PRINT_ERROR, "failed to open file!");
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
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to bind port: %s", argv[1]);
		return -1;
	}

	// backlog  等待队列的最大长度
	if (listen(server_sock, 5) == sock_error)
	{
		print_console(PRINT_ERROR, "failed to listen port: %s", argv[1]);
		return -1;
	}

	struct sockaddr_in client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	sock_t client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);

	char buf[BUFFER_SIZE] = { 0 };

	while (true)
	{
		size_t read_len = fread(buf, 1, BUFFER_SIZE, fp);
		if (read_len < BUFFER_SIZE)
		{
			send(client_sock, buf, static_cast<int>(read_len), 0);
			break;
		}
		send(client_sock, buf, BUFFER_SIZE, 0);
	}

	// 半关闭的关键处理
#if defined(_WIN32) || defined(_WINDLL)
	shutdown(client_sock, SD_SEND); //win:  SD_RECEIVE  SD_SEND  SD_BOTH
#else
	shutdown(client_sock, SHUT_WR);
#endif

	int read_len = recv(client_sock, buf, BUFFER_SIZE - 1, 0);
	if (read_len > 0)
	{
		buf[read_len] = 0;
		print_console(PRINT_INFOR, "recv: %s", buf);
	}

	close_socket(client_sock);
	close_socket(server_sock);
	fclose(fp);

	CleanSocketEnv();
	return 0;
}

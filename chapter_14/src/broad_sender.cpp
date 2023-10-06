#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <DreamSky/dream_time.h>
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

	sock_t client_sock = socket(PF_INET, SOCK_DGRAM, 0);
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
	inet_pton(AF_INET, "255.255.255.255", &server_addr.sin_addr.s_addr);
	server_addr.sin_port = htons(atoi(argv[1]));

	int ret = setsockopt(client_sock, SOL_SOCKET, SO_BROADCAST, (char*)&server_addr, static_cast<int>(sizeof(server_addr)));
	if (ret == sock_error)
	{
		print_console(PRINT_ERROR, "failed to set broad.");
		return -1;
	}

	char message[BUFFER_SIZE];
	FILE* fp = fopen("news.txt", "r");
	if (!fp)
	{
		print_console(PRINT_ERROR, "failed to open news.txt.");
		return -1;
	}

	while (!feof(fp))   /* Broadcasting */
	{
		size_t read_len = fread(message, 1, BUFFER_SIZE, fp);
		sendto(client_sock, message, static_cast<int>(read_len),
			0, (struct sockaddr*)&server_addr, sizeof(server_addr));
		SleepSe(2);
	}

	close_socket(client_sock);

	CleanSocketEnv();
	return 0;
}

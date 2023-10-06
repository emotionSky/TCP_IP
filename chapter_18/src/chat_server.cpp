#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <DreamSky/dream_thread.h>
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
#include <sys/wait.h>
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1400;
constexpr int MAX_CLIENT_CNT = 256;

int client_count = 0;
sock_t client_socks[MAX_CLIENT_CNT];
DPmutex sock_mutex;

void send_msg(char* msg, int len)   // send to all
{
	sock_mutex.Lock();
	for (int i = 0; i < client_count; i++)
		send(client_socks[i], msg, len, 0);
	sock_mutex.Unlock();
}

void* handle_clnt(void* arg)
{
	sock_t client_sock = *((sock_t*)arg);
	int str_len = 0, i;
	char msg[BUFFER_SIZE];

	while ((str_len = recv(client_sock, msg, sizeof(msg), 0)) != 0)
		send_msg(msg, str_len);

	sock_mutex.Lock();
	for (i = 0; i < client_count; i++)   // remove disconnected client
	{
		if (client_sock == client_socks[i])
		{
			while (i++ < client_count - 1)
				client_socks[i] = client_socks[i + 1];
			break;
		}
	}
	client_count--;
	sock_mutex.Unlock();

	close_socket(client_sock);
	return nullptr;
}


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
	socklen_t client_addr_size = static_cast<int>(sizeof(client_addr));
	sock_t client_sock = invalid_sock;

	while (true)
	{
		client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);

		sock_mutex.Lock();
		client_socks[client_count++] = client_sock;
		sock_mutex.Unlock();

		DPthread handle_thread(handle_clnt, (d_handle)&client_sock);
		handle_thread.DetachThread();

		char str_arr[32] = { 0 };
		inet_ntop(AF_INET, (const void*)&client_addr.sin_addr, str_arr, 32);
		print_console(PRINT_INFOR, "Connected client IP: %s", str_arr);
	}

	close_socket(server_sock);
	CleanSocketEnv();
	return 0;
}



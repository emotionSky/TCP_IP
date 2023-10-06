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
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1024;
constexpr int NAME_SIZE = 50;
char name[NAME_SIZE] = "[DEFAULT]";
char message[BUFFER_SIZE];

void* send_msg(void* arg)   // send thread main
{
	sock_t sock = *((sock_t*)arg);
	char name_msg[NAME_SIZE + BUFFER_SIZE];
	while (true)
	{
		fputs("input message:", stdout);
		fgets(message, BUFFER_SIZE - 1, stdin);
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
		{
			close_socket(sock);
			exit(0); // 这里其实有更优雅的处理方法，比如通过回调/全局标志位等，在外部处理
		}
		sprintf(name_msg, "%s %s", name, message);
		send(sock, name_msg, static_cast<int>(strlen(name_msg)), 0);
	}
	return nullptr;
}

void* recv_msg(void* arg)   // read thread main
{
	sock_t sock = *((sock_t*)arg);
	char name_msg[NAME_SIZE + BUFFER_SIZE];
	while (true)
	{
		int str_len = recv(sock, name_msg, NAME_SIZE + BUFFER_SIZE - 1, 0);
		if (str_len == -1)
			return (void*)-1;
		name_msg[str_len] = 0;
		print_console(PRINT_INFOR, "recv response: %s", name_msg);
	}
	return nullptr;
}

int main(int argc, char* argv[])
{
	if (argc != 4)
	{
		print_console(PRINT_ERROR, "args not valid!");
		print_console(PRINT_INFOR, "Usage: %s <IP> <port> <name>", argv[0]);
		return -1;
	}

	if (!InitSocketEnv())
	{
		print_console(PRINT_ERROR, "failed to init socket env.");
		return -1;
	}

	sprintf(name, "[%s]", argv[3]);
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

	DPthread send_thread(send_msg, (d_handle)&client_sock);
	DPthread recv_thread(recv_msg, (d_handle)&client_sock);
	send_thread.StartThread();
	recv_thread.StartThread();

	send_thread.StopThread();
	recv_thread.StopThread();

	close_socket(client_sock);
	CleanSocketEnv();
	return 0;
}

#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <DreamSky/dream_thread.h>
#include <cstdio>
#include <fcntl.h>
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
#include <sys/wait.h>
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1024;
constexpr int SMALL_BUF = 100;

#define get_str_len(str) static_cast<int>(strlen(str))
using return_type = void*;

return_type request_handler(void* arg);
void send_data(sock_t sock, char* ct, char* file_name);
const char* content_type(char* file);
void send_error(sock_t sock);

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
	socklen_t client_addr_size = static_cast<socklen_t>(sizeof(client_addr));
	sock_t client_sock = invalid_sock;

	while (1)
	{
		client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_size);
		print_console(PRINT_INFOR, "Connection Request : %s:%d",
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		DPthread thread(request_handler, &client_sock);
		thread.DetachThread();
	}

	close_socket(server_sock);
	return 0;
}

return_type request_handler(void* arg)
{
	sock_t client_sock = *((sock_t*)arg);
	char buf[BUFFER_SIZE];
	char method[BUFFER_SIZE];
	char ct[SMALL_BUF];
	char file_name[SMALL_BUF];

	int recv_len = recv(client_sock, buf, BUFFER_SIZE - 1, 0);
	if (strstr(buf, "HTTP/") == nullptr)
	{
		send_error(client_sock);
		close_socket(client_sock);
		return (return_type)1;
	}

	strcpy(method, strtok(buf, " /"));
	if (strcmp(method, "GET"))
		send_error(client_sock);

	strcpy(file_name, strtok(nullptr, " /"));
	strcpy(ct, content_type(file_name));
	send_data(client_sock, ct, file_name);
	return (return_type)0;
}

void send_data(sock_t sock, char* ct, char* file_name)
{
	char protocol[] = "HTTP/1.0 200 OK\r\n";
	char server[] = "Server:Linux Web Server \r\n";
	char cnt_len[] = "Content-length:2048\r\n";
	char cnt_type[SMALL_BUF];
	char buf[BUFFER_SIZE];
	FILE* send_file;

	sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct);
	send_file = fopen(file_name, "r");
	if (send_file == nullptr)
	{
		send_error(sock);
		return;
	}

	send(sock, protocol, get_str_len(protocol), 0);
	send(sock, server, get_str_len(server), 0);
	send(sock, cnt_len, get_str_len(cnt_len), 0);
	send(sock, cnt_type, get_str_len(cnt_type), 0);

	while (fgets(buf, BUFFER_SIZE, send_file) != nullptr)
	{
		send(sock, buf, get_str_len(buf), 0);
	}
	close_socket(sock);
}

const char* content_type(char* file)
{
	char extension[SMALL_BUF];
	char file_name[SMALL_BUF];
	strcpy(file_name, file);
	strtok(file_name, ".");
	strcpy(extension, strtok(nullptr, "."));

	if (!strcmp(extension, "html") || !strcmp(extension, "htm"))
		return "text/html";
	else
		return "text/plain";
}

void send_error(sock_t sock)
{
	char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
	char server[] = "Server:Linux Web Server \r\n";
	char cnt_len[] = "Content-length:2048\r\n";
	char cnt_type[] = "Content-type:text/html\r\n\r\n";
	char content[] = "<html><head><title>NETWORK</title></head>"
		"<body><font size=+5><br>发生错误！查看请求文件名和请求方式！"
		"</font></body></html>";

	send(sock, protocol, get_str_len(protocol), 0);
	send(sock, server, get_str_len(server), 0);
	send(sock, cnt_len, get_str_len(cnt_len), 0);
	send(sock, cnt_type, get_str_len(cnt_type), 0);
	send(sock, content, get_str_len(content), 0);
	close_socket(sock);
}

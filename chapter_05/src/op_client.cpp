#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib ") 
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define closesocket close
using SOCKET = int;
using INVALID_SOCKET = -1;
using SOCKET_ERROR = -1;
#endif

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
		print_console(PRINT_ERROR, "failed to init socket env.");
		return -1;
	}

	SOCKET client_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET)
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

	if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		print_console(PRINT_ERROR, "failed to connect server!");
		return -1;
	}

	char message[BUFFER_SIZE];

	// 这里其实相当于定义了一套非常简单的通讯协议，
	// 根据定义的协议传递需要进行处理的数据信息。
	auto write_32bit = [](char* buf, int value)
		{
			buf[0] = value >> 24;
			buf[1] = value >> 16;
			buf[2] = value >> 8;
			buf[3] = value & 0xff;
		};

	print_console(PRINT_INFOR, "Input the count of numbers: ");
	int num_count = 0, size_int = static_cast<int>(sizeof(int)); // 这里使用 sizeof 则是提高程序稳定性
	scanf("%d", &num_count);
	while (num_count <= 0)
	{
		print_console(PRINT_INFOR, "Input a valid count of numbers: ");
		scanf("%d", &num_count);
	}
	write_32bit(message, num_count);

	for (int index = 0; index < num_count; ++index)
	{
		print_console(PRINT_INFOR, "Input NO.%d number: ", index);
		int tmp_value = 0;
		scanf("%d", &tmp_value);
		write_32bit(message + size_int * (index + 1), tmp_value);
	}

	print_console(PRINT_INFOR, "Input a operator: ");
	scanf("%c", message + size_int * (num_count + 1));
	auto is_char_valid = [](char ch)
		{
			if (ch != '+' && ch != '-' && ch != '*' && ch != '/')
				return false;
			return true;
		};
	while (!is_char_valid(message[size_int * (num_count + 1)]))
	{
		print_console(PRINT_INFOR, "Input a valid operator: ");
		scanf("%c", message + size_int * (num_count + 1));
	}

	int str_len = send(client_sock, message, size_int * (num_count + 1) + 1, 0);
	int recv_len = recv(client_sock, message, BUFFER_SIZE, 0);
	auto read_32bit = [](char* buf)
		{
			int value = buf[0];
			value <<= 8;
			value |= buf[1];
			value <<= 8;
			value |= buf[2];
			value <<= 8;
			value |= buf[3];
			return value;
		};

	print_console(PRINT_INFOR, "Get response from server: %d", read_32bit(message));
	closesocket(client_sock);

	CleanSocketEnv();
	return 0;
}

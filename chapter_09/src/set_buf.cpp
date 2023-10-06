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

int main(int argc, char* argv[])
{
	if (!InitSocketEnv())
	{
		print_console(PRINT_ERROR, "InitSocketEnv() error!");
		return -1;
	}

	sock_t sock = socket(PF_INET, SOCK_STREAM, 0);
	int send_buf_size = 1 << 11;
	socklen_t send_size_len = static_cast<int>(sizeof(int));
	int res_send = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&send_buf_size, send_size_len);
	if (res_send)
	{
		print_console(PRINT_ERROR, "setsockopt() failed");
	}

	res_send = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&send_buf_size, &send_size_len);
	if (res_send)
	{
		print_console(PRINT_ERROR, "getsockopt() failed");
	}
	else
	{
		print_console(PRINT_INFOR, "send_buf_size: %d", send_buf_size);
	}

	int recv_buf_size = 1 << 11;
	socklen_t recv_size_len = static_cast<int>(sizeof(int));
	int res_recv = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&recv_buf_size, recv_size_len);
	if (res_recv)
	{
		print_console(PRINT_ERROR, "setsockopt() failed");
	}

	res_recv = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&recv_buf_size, &recv_size_len);
	if (res_recv)
	{
		print_console(PRINT_ERROR, "getsockopt() failed");
	}
	else
	{
		print_console(PRINT_INFOR, "send_buf_size: %d", recv_buf_size);
	}

	close_socket(sock);

	CleanSocketEnv();
	return 0;
}

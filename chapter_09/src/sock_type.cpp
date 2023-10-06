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

	sock_t tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
	sock_t udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
	print_console(PRINT_INFOR, "SOCK_STREAM: %d", SOCK_STREAM);
	print_console(PRINT_INFOR, " SOCK_DGRAM: %d", SOCK_DGRAM);

	int sock_type = 0;
	socklen_t type_len = static_cast<socklen_t>(sizeof(int));
	int res = getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (char*)&sock_type, &type_len);
	if (res)
	{
		print_console(PRINT_ERROR, "getsockopt() failed");
	}
	else
	{
		print_console(PRINT_INFOR, "tcp_sock: %d", sock_type);
	}

	res = getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (char*)&sock_type, &type_len);
	if (res)
	{
		print_console(PRINT_ERROR, "getsockopt() failed");
	}
	else
	{
		print_console(PRINT_INFOR, "udp_sock: %d", sock_type);
	}

	CleanSocketEnv();
	return 0;
}

#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#if defined(_WIN32) || defined(_WINDLL)
// 为了使用 gethostbyname
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib ") 
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

using namespace dreamsky;

// ./get_host_by_name.exe www.baidu.com
int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		print_console(PRINT_ERROR, "args not valid!");
		print_console(PRINT_INFOR, "Usage: %s <addr>", argv[0]);
		return -1;
	}

	if (!InitSocketEnv())
	{
		print_console(PRINT_ERROR, "InitSocketEnv() error!");
		return -1;
	}
		
	struct hostent* host = gethostbyname(argv[1]);
	if (!host)
	{
		SocketError err;
		print_console(PRINT_ERROR, "gethostbyname() failed, error: %s", err.GetErrorString());
		return -1;
	}

	print_console(PRINT_INFOR, "Official name: %s", host->h_name);

	for (int index = 0; host->h_aliases[index]; ++index)
	{
		print_console(PRINT_INFOR, "Aliases %d: %s", index + 1, host->h_aliases[index]);
	}

	print_console(PRINT_INFOR, "Address type: %s",
		(host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");

	for (int index = 0; host->h_addr_list[index]; ++index)
	{
		char str[32];
		inet_ntop(host->h_addrtype, (const void*)host->h_addr_list[index], str, 20);
		print_console(PRINT_INFOR, "IP addr %d: %s \n", index + 1, str);
	}

	CleanSocketEnv();
	return 0;
}

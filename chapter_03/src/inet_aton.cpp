#include <DreamSky/dream_print.h>

#if defined(_WIN32) || defined(_WINDLL)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib ") 
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

using namespace dreamsky;

int main(int argc, char* argv[])
{
	const char* addr = "127.232.124.79";
	struct sockaddr_in addr_inet;

	// linux 中可以使用 inet_aton(addr, &addr_inet.sin_addr)
	// 为了两个操作系统下的兼容，使用同一个
	if (!inet_pton(AF_INET, addr, &addr_inet.sin_addr.s_addr)) //使用窄字符
	{
		print_console(PRINT_ERROR, "Failed to convert!");
	}
	else
	{
		print_console(PRINT_INFOR, "Network ordered integer addr: %#x", addr_inet.sin_addr.s_addr);
	}
	return 0;
}
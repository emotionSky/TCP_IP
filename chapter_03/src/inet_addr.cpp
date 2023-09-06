#include <DreamSky/dream_print.h>

#ifdef _WIN32
// 使用这个宏，仅仅是为了在Windows下使用 inet_addr
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib ") 
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

int main(int argc, char* argv[])
{
	const char* addr1 = "127.212.124.78";
	const char* addr2 = "127.212.124.256";

	unsigned long conv_addr = inet_addr(addr1);
	if (conv_addr == INADDR_NONE)
		print_console(PRINT_ERROR, "Error occured!");
	else
		print_console(PRINT_INFOR, "Network ordered integer addr: %#lx", conv_addr);

	conv_addr = inet_addr(addr2);
	if (conv_addr == INADDR_NONE)
		print_console(PRINT_ERROR, "Error occureded");
	else
		print_console(PRINT_INFOR, "Network ordered integer addr: %#lx\n", conv_addr);
	return 0;
}
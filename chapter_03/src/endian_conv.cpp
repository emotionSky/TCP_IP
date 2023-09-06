#include <DreamSky/dream_print.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib ") 
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

int main(int argc, char* argv[])
{
	// windows 没有必要调用 WSAStartup 和 WSACleanup，都没有用上 socket环境
	unsigned short host_port = 0x1234;
	unsigned short net_port;
	unsigned long host_addr = 0x12345678;
	unsigned long net_addr;

	net_port = htons(host_port);
	net_addr = htonl(host_addr);

	print_console(PRINT_INFOR, "Host ordered port: %#x", host_port);
	print_console(PRINT_INFOR, "Network ordered port: %#x", net_port);
	print_console(PRINT_INFOR, "Host ordered address: %#lx", host_addr);
	print_console(PRINT_INFOR, "Network ordered address: %#lx", net_addr);
	return 0;
}

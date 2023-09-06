#include <DreamSky/dream_print.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib ") 
#else
#include <unistd.h>
#include <arpa/inet.h>
#endif

int main(int argc, char* argv[])
{
	struct sockaddr_in addr1, addr2;
	char str_arr[20];

	addr1.sin_addr.s_addr = htonl(0x1020304);
	addr2.sin_addr.s_addr = htonl(0x1010101);

	// 使用安全的函数 inet_ntop
	inet_ntop(AF_INET, (const void*)&addr1.sin_addr, str_arr, 20);
	print_console(PRINT_INFOR, "Dotted-Decimal notation1: %s", str_arr);

	inet_ntop(AF_INET, (const void*)&addr2.sin_addr, str_arr, 20);
	print_console(PRINT_INFOR, "Dotted-Decimal notation2: %s", str_arr);

	// 原书中采用 inet_ntoa 仅仅为了展示  “指针” 这个东西的注意事项！
	return 0;
}

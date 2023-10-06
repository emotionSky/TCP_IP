#include <DreamSky/dream_print.h>
#include <DreamSky/dream_socket.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#if defined(_WIN32) || defined(_WINDLL)
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib ") 
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1024;

int main(int argc, char* argv[])
{
	InitSocketEnv();

	fd_set reads;
	FD_ZERO(&reads);
	FD_SET(0, &reads); // 把标准输入作为操作对象

	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	char buf[BUFFER_SIZE];

	while (true)
	{
		fd_set temps = reads;
		
		int result = select(1, &temps, 0, 0, &timeout);
		if (result == -1)
		{
			// Windows一定会进入到这儿，因为Windows无法针对标准输入操作……
			SocketError sock_err;
			print_console(PRINT_ERROR, "select error, %s", sock_err.GetErrorString());
			break;
		}
		else if (result == 0)
		{
			print_console(PRINT_INFOR, "Time-out!");
		}
		else
		{
			if (FD_ISSET(0, &temps))
			{
				int str_len = recv(0, buf, BUFFER_SIZE - 1, 0);
				buf[str_len] = 0;
				print_console(PRINT_INFOR, "message from console: %s", buf);
			}
		}
	}

	CleanSocketEnv();
	return 0;
}

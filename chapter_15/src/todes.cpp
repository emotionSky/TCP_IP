#include <DreamSky/dream_print.h>
#include <stdio.h>
#include <fcntl.h>

#if defined(_WIN32) || defined(_WINDLL)
#include <io.h>
#ifdef _MSC_VER
#pragma  warning( disable: 6031 4996 )
#endif
#endif

using namespace dreamsky;

int main(void)
{
	int fd = open("data.dat", O_WRONLY | O_CREAT | O_TRUNC);
	if (fd == -1)
	{
		print_console(PRINT_ERROR, "file open error.");
		return -1;
	}

	print_console(PRINT_INFOR, "First file descriptor: %d", fd);
	FILE* fp = fdopen(fd, "w");
	fputs("TCP/IP SOCKET PROGRAMMING \n", fp);
	print_console(PRINT_INFOR, "Second file descriptor: %d", fileno(fp));
	fclose(fp);
	return 0;
}
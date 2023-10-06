#include <DreamSky/dream_print.h>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>

#if defined(_WIN32) || defined(_WINDLL)
#include <io.h>
#ifdef _MSC_VER
#pragma  warning( disable: 4267 4996 )
#endif
#endif

using namespace dreamsky;

int main(void)
{
	int fd = open("data.dat", O_WRONLY | O_CREAT | O_TRUNC);
	if (fd == -1)
	{
		print_console(PRINT_ERROR, "file open error");
		return -1;
	}

	FILE* fp = fdopen(fd, "w");
	fputs("Network C programming \n", fp);
	fclose(fp);
	return 0;
}

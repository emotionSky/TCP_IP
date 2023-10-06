#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#if defined(_WIN32) || defined(_WINDLL)
#include <io.h>
#ifdef _MSC_VER
#pragma  warning( disable: 6031 4996 )
#endif
#else
#include <unistd.h>
#endif

#define BUF_SIZE 3

int main(int argc, char* argv[])
{
	char buf[BUF_SIZE];

	int fd1 = open("news.txt", O_RDONLY);
	int fd2 = open("cpy.txt", O_WRONLY | O_CREAT | O_TRUNC);

	int len = 0;
	while ((len = read(fd1, buf, sizeof(buf))) > 0)
		write(fd2, buf, len);

	close(fd1);
	close(fd2);
	return 0;
}

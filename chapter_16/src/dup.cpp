#include <DreamSky/dream_print.h>
#include <cstdio>

#if defined(_WIN32) || defined(_WINDLL)
#include <io.h>
#ifdef _MSC_VER
#pragma  warning( disable: 4267 4996 )
#endif
#else
#include <unistd.h>
#endif

using namespace dreamsky;

int main(int argc, char* argv[])
{
	char str1[] = "Hi~ \n";
	char str2[] = "It's nice day~ \n";

	int cfd1 = dup(1);
	int cfd2 = dup2(cfd1, 7);

	print_console(PRINT_ERROR, "fd1=%d, fd2=%d", cfd1, cfd2);
	write(cfd1, str1, sizeof(str1));
	write(cfd2, str2, sizeof(str2));

	close(cfd1);
	close(cfd2);
	write(1, str1, sizeof(str1));
	close(1);
	write(1, str2, sizeof(str2));
	return 0;
}

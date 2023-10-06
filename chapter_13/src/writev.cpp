#include <DreamSky/dream_print.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#if defined(_WIN32) || defined(_WINDLL)
#else
#include <sys/uio.h>
#endif

using namespace dreamsky;

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WINDLL)
	print_console(PRINT_INFOR, "no implement of writev on windows.");
#else
	struct iovec vec[2];
	char buf1[] = "ABCDEFG";
	char buf2[] = "1234567";

	vec[0].iov_base = buf1;
	vec[0].iov_len = 3;
	vec[1].iov_base = buf2;
	vec[1].iov_len = 4;

	int str_len = writev(1, vec, 2);
	print_console(PRINT_INFOR, "Write bytes: %d", str_len);
#endif
	return 0;
}
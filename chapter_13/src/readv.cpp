#include <DreamSky/dream_print.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#if defined(_WIN32) || defined(_WINDLL)
#else
#include <sys/uio.h>
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1024;

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WINDLL)
	print_console(PRINT_INFOR, "no implement of readv on windows.");
#else
	struct iovec vec[2];
	char buf1[BUFFER_SIZE] = { 0 };
	char buf2[BUFFER_SIZE] = { 0 };

	vec[0].iov_base = buf1;
	vec[0].iov_len = 5;
	vec[1].iov_base = buf2;
	vec[1].iov_len = BUFFER_SIZE;

	int str_len = readv(0, vec, 2);
	print_console(PRINT_INFOR, "Read bytes: %d", str_len);
	print_console(PRINT_INFOR, "First message: %s", buf1);
	print_console(PRINT_INFOR, "Second message: %s", buf2);
#endif
	return 0;
}
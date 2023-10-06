#include <DreamSky/dream_print.h>

#if defined(_WIN32) || defined(_WINDLL)
#else
#include <unistd.h>
#endif

// Windows下没有类似linux的fork实现，并且，
// Windows下的CreateProcess这个使用完全不同。

using namespace dreamsky;
int gval = 10;

int main(int argc, char* argv[])
{
	int lval = 20;
	gval++, lval += 5;

#if defined(_WIN32) || defined(_WINDLL)
	print_console(PRINT_ERROR, "no implement of fork on windows.");
#else
	pid_t pid = fork();
	if (pid == 0)	// pid=0 表示这是子进程
		gval += 2, lval += 2;
	else			// pid!=0 标识这是父进程
		gval -= 2, lval -= 2;

	if (pid == 0)
		print_console(PRINT_INFOR, "Child Proc: [%d, %d]", gval, lval);
	else
		print_console(PRINT_INFOR, "Parent Proc: [%d, %d]", gval, lval);
#endif
	return 0;
}
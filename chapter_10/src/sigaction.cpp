#include <DreamSky/dream_print.h>
#include <DreamSky/dream_time.h>
#include <signal.h>

#if defined(_WIN32) || defined(_WINDLL)
#else
#include <unistd.h>
#endif

using namespace dreamsky;

#if defined(_WIN32) || defined(_WINDLL)
// windows 下没有 SIGALRM 这个信号量
#else
void timeout(int sig)
{
	if (sig == SIGALRM)
		print_console(PRINT_INFOR, "Time out!");

	alarm(2);
}
#endif

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WINDLL)
	print_console(PRINT_INFOR, "no implement of sigaction on windows");
#else
	struct sigaction act;
	act.sa_handler = timeout;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGALRM, &act, 0);

	alarm(2);

	for (int i = 0; i < 3; i++)
	{
		print_console(PRINT_INFOR, "wait...");
		SleepSe(100);
	}
#endif
	return 0;
}
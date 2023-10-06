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

void keycontrol(int sig)
{
	if (sig == SIGINT)
		print_console(PRINT_INFOR, "CTRL+C pressed");
}

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WINDLL)
#else
	signal(SIGALRM, timeout);
#endif
	
	signal(SIGINT, keycontrol);

#if defined(_WIN32) || defined(_WINDLL)
#else
	alarm(2);
#endif

	for (int i = 0; i < 3; i++)
	{
		print_console(PRINT_INFOR, "wait...");
		SleepSe(100);
	}
	return 0;
}
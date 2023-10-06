#include <DreamSky/dream_print.h>
#include <DreamSky/dream_time.h>
#include <cstdlib>
#include <signal.h>

#if defined(_WIN32) || defined(_WINDLL)
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

using namespace dreamsky;

#if defined(_WIN32) || defined(_WINDLL)
#else
void read_childproc(int sig)
{
	int status;
	pid_t id = waitpid(-1, &status, WNOHANG);
	if (WIFEXITED(status))
	{
		print_console(PRINT_INFOR, "Removed proc id: %d", id);
		print_console(PRINT_INFOR, "Child send: %d", WEXITSTATUS(status));
	}
}
#endif

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WINDLL)
	print_console(PRINT_INFOR, "no implement of sigaction on windows");
#else
	pid_t pid;
	struct sigaction act;
	act.sa_handler = read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGCHLD, &act, 0);

	pid = fork();
	if (pid == 0)
	{
		print_console(PRINT_INFOR, "Hi! I'm child process");
		SleepSe(10);
		return 12;
	}
	else
	{
		print_console(PRINT_INFOR, "Child proc id: %d", pid);
		pid = fork();
		if (pid == 0)
		{
			print_console(PRINT_INFOR, "Hi! I'm child process");
			SleepSe(10);
			exit(24);
		}
		else
		{
			int i;
			print_console(PRINT_INFOR, "Child proc id: %d", pid);
			for (i = 0; i < 5; i++)
			{
				print_console(PRINT_INFOR, "wait...");
				SleepSe(5);
			}
		}
	}
#endif
	return 0;
}

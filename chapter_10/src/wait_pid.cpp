#include <DreamSky/dream_print.h>
#include <DreamSky/dream_time.h>

#if defined(_WIN32) || defined(_WINDLL)
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

using namespace dreamsky;

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WINDLL)
	print_console(PRINT_INFOR, "no implemeny of fork on windows");
#else
	int status;
	pid_t pid = fork();

	if (pid == 0)
	{
		SleepMs(15 * 1000);
		return 24;
	}
	else
	{
		while (!waitpid(-1, &status, WNOHANG))
		{
			SleepMs(3 * 1000);
			print_console(PRINT_INFOR, "sleep 3sec.");
		}

		if (WIFEXITED(status))
			print_console(PRINT_INFOR, "Child send %d", WEXITSTATUS(status));
	}
#endif
	return 0;
}

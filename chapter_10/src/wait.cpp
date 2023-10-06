#include <DreamSky/dream_print.h>
#include <DreamSky/dream_time.h>
#include <cstdlib>

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
		return 3;
	}
	else
	{
		print_console(PRINT_INFOR, "Child PID: %d", pid);
		pid = fork();
		if (pid == 0)
		{
			exit(7);
		}
		else
		{
			print_console(PRINT_INFOR, "Child PID: %d", pid);
			wait(&status);
			if (WIFEXITED(status))
				print_console(PRINT_INFOR, "Child send one: %d", WEXITSTATUS(status));

			wait(&status);
			if (WIFEXITED(status))
				print_console(PRINT_INFOR, "Child send two: %d", WEXITSTATUS(status));
			SleepMs(30 * 1000);
		}
	}
#endif
	return 0;
}
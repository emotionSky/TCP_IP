#include <DreamSky/dream_print.h>
#include <DreamSky/dream_process.h>
#include <DreamSky/dream_time.h>
#include <cstring>

#if defined(_WIN32) || defined(_WINDL)
#else
#include <unistd.h>
#endif

using namespace dreamsky;
constexpr int BUFFER_SIZE = 1024;

int main(int argc, char* argv[])
{
	DreamPipe d_pipe;

	if (!d_pipe.CreatePipe())
	{
		print_console(PRINT_ERROR, "failed to create pipe.");
		return -1;
	}
#if defined(_WIN32) || defined(_WINDL)
	print_console(PRINT_ERROR, "no implement of fork on windows.");
#else
	pid_t pid = fork();
	if (pid == 0)
	{
		const char* str = "Who are you?";
		d_pipe.WriteData(str, static_cast<int>(strlen(str)));
		char buf[BUFFER_SIZE] = { 0 };
		d_pipe.ReadData(buf, BUFFER_SIZE);
		print_console(PRINT_INFOR, "child recv: %s", buf);
	}
	else
	{
		char buf[BUFFER_SIZE] = { 0 };
		d_pipe.ReadData(buf, BUFFER_SIZE);
		print_console(PRINT_INFOR, "main recv: %s", buf);
		const char* str = "Thank you for your message";
		d_pipe.WriteData(str, static_cast<int>(strlen(str)));
		SleepSe(3);
	}
#endif
	return 0;
}

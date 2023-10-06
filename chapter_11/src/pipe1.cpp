#include <DreamSky/dream_print.h>
#include <DreamSky/dream_process.h>

#if defined(_WIN32) || defined(_WINDLL)
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
		d_pipe.WriteData(str, static_cast<int>(sizeof(str)));
	}
	else
	{
		char buf[BUFFER_SIZE] = { 0 };
		d_pipe.ReadData(buf, BUFFER_SIZE);
		print_console(PRINT_INFOR, "recv: %s", buf);
	}
#endif
	return 0;
}

#include <DreamSky/dream_print.h>
#include <DreamSky/dream_time.h>
#include <DreamSky/dream_thread.h>

using namespace dreamsky;

#if defined(_WIN32) || defined(_WINDLL)
using return_type = unsigned int;
#else
using return_type = void*;
#endif

return_type thread_func(void* arg)
{
	int cnt = *((int*)arg);
	for (int i = 0; i < cnt; i++)
	{
		SleepSe(1);
		print_console(PRINT_INFOR, "running thread");
	}
	return (return_type)0;
}

int main(int argc, char* argv[])
{
	int param = 5;
#if defined(_WIN32) || defined(_WINDLL)
	DWinThreadEx thread(thread_func, (void*)&param);
#else
	DPthread thread(thread_func, (void*)&param);
#endif

	thread.StartThread();
	//thread.StopThread();
	SleepSe(3);
	print_console(PRINT_INFOR, "end of main");
	return 0;
}

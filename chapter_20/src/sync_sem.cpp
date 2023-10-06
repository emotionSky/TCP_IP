#include <DreamSky/dream_print.h>
#include <DreamSky/dream_thread.h>

using namespace dreamsky;

#if defined(_WIN32) || defined(_WINDLL)
DWinSem sem_one(nullptr, 0);
DWinSem sem_two(nullptr, 1);
using return_type = unsigned int;
#else
DPsem sem_one(false, 0);
DPsem sem_two(false, 1);
using return_type = void*;
#endif
static int num; // 静态变量自动初始化

return_type read(void* arg)
{
	for (int i = 0; i < 5; i++)
	{
		fputs("Input num: ", stdout);

		sem_two.Wait();
		scanf("%d", &num);
		sem_one.Post();
	}
	return 0;
}

return_type accu(void* arg)
{
	int sum = 0;
	for (int i = 0; i < 5; i++)
	{
		sem_one.Wait();
		sum += num;
		sem_two.Post();
	}
	print_console(PRINT_INFOR, "Result: %d", sum);
	return 0;
}

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WINDLL)
	DWinThreadEx t1(read, nullptr);
	DWinThreadEx t2(accu, nullptr);
#else
	DPthread t1(read, nullptr);
	DPthread t2(accu, nullptr);
#endif

	t1.StartThread();
	t2.StartThread();

	t1.StopThread();
	t2.StopThread();
	return 0;
}

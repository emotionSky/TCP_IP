#include <DreamSky/dream_print.h>
#include <DreamSky/dream_thread.h>

using namespace dreamsky;

DPsem sem_one(false, 0);
DPsem sem_two(false, 1);
static int num; // 静态变量自动初始化

void* read(void* arg)
{
	for (int i = 0; i < 5; i++)
	{
		fputs("Input num: ", stdout);

		sem_two.Wait();
		scanf("%d", &num);
		sem_one.Post();
	}
	return nullptr;
}

void* accu(void* arg)
{
	int sum = 0;
	for (int i = 0; i < 5; i++)
	{
		sem_one.Wait();
		sum += num;
		sem_two.Post();
	}
	print_console(PRINT_INFOR, "Result: %d", sum);
	return nullptr;
}

int main(int argc, char* argv[])
{
	DPthread t1(read, nullptr);
	DPthread t2(accu, nullptr);

	t1.StartThread();
	t2.StartThread();

	t1.StopThread();
	t2.StopThread();
	return 0;
}

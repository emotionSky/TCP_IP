#include <DreamSky/dream_print.h>
#include <DreamSky/dream_thread.h>

using namespace dreamsky;

DPmutex dp_mutex;
long long num = 0;
constexpr int thread_count = 10; // 50
constexpr int cycle_count = 500; // 50000000

void* thread_inc(void* arg)
{
	dp_mutex.Lock();
	for (int i = 0; i < cycle_count; i++)
		num += 1;
	dp_mutex.Unlock();
	return nullptr;
}

void* thread_des(void* arg)
{
	for (int i = 0; i < cycle_count; i++)
	{
		dp_mutex.Lock();
		num -= 1;
		dp_mutex.Unlock();
	}
	return nullptr;
}

int main(int argc, char* argv[])
{
	DPthread* dp_threads[thread_count] = { nullptr };

	for (int index = 0; index < thread_count; ++index)
	{
		if (index % 2)
		{
			dp_threads[index] = new DPthread(thread_inc, nullptr);
		}
		else
		{
			dp_threads[index] = new DPthread(thread_des, nullptr);
		}
		dp_threads[index]->StartThread();
	}

	for (int index = 0; index < thread_count; ++index)
	{
		dp_threads[index]->StopThread();
		delete dp_threads[index];
	}

	print_console(PRINT_INFOR, "result: %lld", num);
	return 0;
}

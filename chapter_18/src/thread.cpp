/*****************************************************************//**
 * \file   thread.cpp
 * \brief  这里并没有给出完整的 thread1.cpp - thread4.cpp 的所有实现，
 *         主要是觉得这几个示例其实大同小异，而且对于实际使用来说并没有多少区别，
 *         个人觉得直接给出这样一个综合的示例，然后和 mutex.cpp 进行对比反而更容易明白：
 *         1. 线程的使用。
 *         2. 多线程之间的数据交换。
 *         3. 如何能够确保多线程之间的数据安全。
 * \note   可能会因为数据变量本身的“原子”特性，最后得到和书本示例不同的结论，这就需要自己理解了。
 * 
 * \author 凌佚 emotion_sky
 * \date   October 2023
 *********************************************************************/
#include <DreamSky/dream_print.h>
#include <DreamSky/dream_thread.h>

using namespace dreamsky;

long long num = 0;
constexpr int thread_count = 10; // 50
constexpr int cycle_count = 500; // 50000000

void* thread_inc(void* arg)
{
	for (int i = 0; i < cycle_count; i++)
		num += 1;
	return nullptr;
}

void* thread_des(void* arg)
{
	for (int i = 0; i < cycle_count; i++)
	{
		num -= 1;
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

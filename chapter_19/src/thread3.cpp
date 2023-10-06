#include <DreamSky/dream_print.h>
#include <DreamSky/dream_time.h>
#include <DreamSky/dream_thread.h>
#include <vector>

using namespace dreamsky;
long long num = 0;
constexpr int thread_count = 10; // 50
constexpr int cycle_count = 500; // 50000000

#if defined(_WIN32) || defined(_WINDLL)
using return_type = unsigned int;
#else
using return_type = void*;

class DPthreadManager
{
public:
	DPthreadManager() = default;
	~DPthreadManager()
	{
		for (auto& thread : m_threads)
		{
			delete thread;
		}
		m_threads.clear();
	}

	bool AllocNewThread(DPthread::thread_func func, void* arg)
	{
		DPthread* thread = new DPthread(func, arg);
		thread->StartThread();
		m_threads.push_back(thread);
	}

	void WaitThreads(bool wait_all = true, int timeout_ms = -1)
	{
		for (auto& thread : m_threads)
		{
			thread->StopThread();
		}
	}

private:
	std::vector<DPthread*> m_threads;
};
#endif

return_type thread_inc(void* arg)
{
	for (int i = 0; i < cycle_count; i++)
		num += 1;
	return (return_type)0;
}

return_type thread_des(void* arg)
{
	for (int i = 0; i < cycle_count; i++)
		num -= 1;
	return (return_type)0;
}

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WINDLL)
	DWinThreadExManager thread_manager;
#else
	DPthreadManager thread_manager;
#endif

	for (int index = 0; index < thread_count; ++index)
	{
		if (index % 2)
		{
			thread_manager.AllocNewThread(thread_inc, nullptr);
		}
		else
		{
			thread_manager.AllocNewThread(thread_des, nullptr);
		}		
	}

	thread_manager.WaitThreads();
	print_console(PRINT_INFOR, "result: %lld", num);
	return 0;
}

#include <DreamSky/dream_print.h>
#include <DreamSky/dream_thread.h>

using namespace dreamsky;
constexpr int STR_LEN = 64;

char str[STR_LEN];

#if defined(_WIN32) || defined(_WINDLL)
DWinEvent event(true, false);

unsigned number_of_a(void* arg)
{
	int cnt = 0;
	event.Wait();
	for (int i = 0; str[i] != 0; i++)
	{
		if (str[i] == 'A')
			cnt++;
	}
	print_console(PRINT_INFOR, "Num of A: %d", cnt);
	return 0;
}

unsigned number_of_other(void* arg)
{
	int cnt = 0;
	event.Wait();
	for (int i = 0; str[i] != 0; i++)
	{
		if (str[i] != 'A')
			cnt++;
	}
	print_console(PRINT_INFOR, "Num of others: %d", cnt - 1);
	return 0;
}
#endif

int main(int argc, char* argv[])
{
#if defined(_WIN32) || defined(_WINDLL)
	DWinThreadEx t1(number_of_a, nullptr);
	DWinThreadEx t2(number_of_other, nullptr);

	t1.StartThread();
	t2.StartThread();

	fputs("Input string: ", stdout);
	fgets(str, STR_LEN, stdin);
	event.Set();

	t1.StopThread();
	t2.StopThread();
	event.Reset();
#else
	print_console(PRINT_INFOR, "not support event!");
#endif
	return 0;
}

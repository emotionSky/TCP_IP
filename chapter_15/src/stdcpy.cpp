#include <stdio.h>
#ifdef _MSC_VER
#pragma  warning( disable: 4267 4996 )
#endif

#define BUF_SIZE 3

int main(int argc, char* argv[])
{
	char buf[BUF_SIZE];

	FILE* fp1 = fopen("news.txt", "r");
	FILE* fp2 = fopen("cpy.txt", "w");

	while (fgets(buf, BUF_SIZE, fp1) != nullptr)
		fputs(buf, fp2);

	fclose(fp1);
	fclose(fp2);
	return 0;
}
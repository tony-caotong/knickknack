
#include <time.h>
#include <stdio.h>

void test()
{
	static int test = 5;

	test++;
	printf("%d\n", test);
}


int main(int argc, char** argv)
{
	while(1) {
		test();
		sleep(1);
	}
	return 0;
}

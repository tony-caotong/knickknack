
#include <stdlib.h>
#include <time.h>

#include <sys/ucontext.h>
#include <setjmp.h>

struct test {
	int a;
	int b;
};

struct test t;

void test_func()
{
	t.a=2;
	t.b=2;
}

struct async_fibre_st {
	ucontext_t fibre;
	jmp_buf env;
	int env_init;
};

char* test_buf;

void test4() {
	printf("i am test3() begin.\n");
	test_buf[5] = 1;
	printf("i am test3() end.\n");
}


void test3() {
	printf("i am test3() begin.\n");
	free(test_buf);
	test4();
	printf("i am test3() end.\n");
}


void test2() {
	printf("i am test2() begin.\n");
	test_buf = malloc(16);
	test_buf[5] = 2;
	test3();
	printf("i am test2() end.\n");
}


void test1() {
	printf("i am test1() begin.\n");
	test2();
	printf("i am test1() end.\n");
}

int main(int argc, char** argv)
{
	t.a=1;
	t.b=1;

//	sleep(10);

	test_func();

	struct async_fibre_st st;
	int a;
	ucontext_t fibre;
	jmp_buf env;

	printf("sizeof async_fibre: %d sizeof ucontext_t: %d sizeof jmp_buf: %d sizeof int: %d\n",
		sizeof(st), sizeof(fibre), sizeof(env), sizeof(a));

	test1();

	return 0;
}

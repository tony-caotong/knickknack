#include <stdio.h>
#include <stdint.h>

//typedef int[8] myint;
typedef int myint[8];

enum test{
	AAA,
	BBB
};

#define TRACE(args...) do {printf(args);} while(0)

struct flow{
	uint8_t key1;
	uint16_t key2;
	uint32_t key3;
	uint64_t key4;
};

int main(int argc, char** argv)
{
	myint m;
	enum test t;
	int a = 5;
	int b = -a;
	TRACE("%d\n", -a);
	printf("%d\n", -b);
	TRACE("%d\n", sizeof(AAA));
	printf("%d\n", sizeof(t));
	if (-1)
		printf("-1 means true.\n");

	uint64_t i64 = 0x1000000000000000l;
	uint32_t i32 = 0x10000000;

	printf("64: 0x%016lx\n", i64);
	printf("32: 0x%08x\n", i32);

	uint64_t res = i64 & 0xFF;
	printf("res: 0x%016lx\n", res);

//:	printf("test2 %d\n", test2());

	printf("Key1: %d\n", sizeof(((struct flow*)NULL)->key1));
	printf("Key2: %d\n", sizeof(((struct flow*)NULL)->key2));
	printf("Key3: %d\n", sizeof(((struct flow*)NULL)->key3));
	printf("Key4: %d\n", sizeof(((struct flow*)NULL)->key4));

	return 0;
}

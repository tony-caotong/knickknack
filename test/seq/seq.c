
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char** argv)
{
	uint32_t a, b, c;
	
	a = (uint32_t)atol(argv[1]);
	b = (uint32_t)atol(argv[2]);
	c = a - b;

	printf("MAX: %u\n", 0xffffffffu);
	printf("Half: %u\n",  0xffffffffu/2);
	printf("c: %u\n", c);
	printf("int32_t(c): %d\n", (int32_t)c);

	return 0;
}

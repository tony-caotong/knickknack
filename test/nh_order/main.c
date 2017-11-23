
#include <stdint.h>

int main(int argc, char** argv)
{
	uint16_t a, b;
	a = 80;

	b = htons(a);

	printf("主机a: %d 0x%x\n", a, a);
	printf("网络b: %d 0x%x\n", b, b);
	return 0;
}

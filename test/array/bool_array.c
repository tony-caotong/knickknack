#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	bool* exist;
	int i;

	exist = malloc(7*sizeof(bool));

	printf("size bool: %zu\n", sizeof(bool));

	for (i = 0; i < 7; i++)
		exist[i] = true;

	for (i = 0; i < 7; i++)
		printf("%d\n", exist[i]);

	for (i = 0; i < 7; i++)
		exist[i] = false;

	for (i = 0; i < 7; i++)
		printf("%d\n", exist[i]);

	free(exist);
	return 0;
}

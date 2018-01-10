
#include <stdlib.h>
#include <stdio.h>

int main()
{
	int i;
	char* p, *q;

	p = malloc(16);

	for (i = 0; i < 16-1; i++) {
		p[i] = i + '0';
	}
	p[16-1] = 0;

	printf("%s\n", p);

//	p[16] = 'x';
	q = p - 1;
//	*q = 'y';
	p--;	
	free(p);
	printf("hahahahahha\n");
	return 0;
}

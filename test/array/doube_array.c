#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	int array[10][8];

	printf("Top level : %d\n", sizeof(array)/sizeof(array[0]));
	printf("Second level : %d\n", sizeof(array[0])/sizeof(array[0][0]));
	
	return 0;
}

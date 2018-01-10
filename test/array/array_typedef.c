#include <stdio.h>
#include <stdlib.h>

#define SIZE 10

typedef void* session_t[SIZE];

int main()
{
	ssize_t s;

	s = sizeof(session_t);

	printf("%d\n", s);
}

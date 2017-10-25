
#include <stdio.h>
#include <wordexp.h>

int main(int argc, char** argv)
{
	char* s = "a b c d -1 -e -i xxx -w \"yyy zzz\" ";
	wordexp_t p;
	int i;
	
	wordexp(s, &p, 0);
	perror("wordexp: ");

	for (i = 0; i < p.we_wordc; i++)
		fprintf(stderr, "%d: %s\n", i, p.we_wordv[i]);

	wordfree(&p);

	return 0;
}

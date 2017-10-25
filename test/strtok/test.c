#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
	char* p, *save, *sub;
	char filter_str[] = "10.1.0.0/16, 10.2.0.0/24, 10.3.0.0/24, !10.1.1.0/24, !10.2.2.0/16, !10.0.3.0/24";
	char* filter_str1 = "10.1.0.0/16, 10.2.0.0/24, 10.3.0.0/24, !10.1.1.0/24, !10.2.2.0/16, !10.0.3.0/24";
	
	printf("filter_str: %s\n", filter_str);
	printf("filter_str1: %s\n", filter_str1);

	/* If use filter_str1, program will segmentation fault (core dumped) */
//	for (p = filter_str1;;p = NULL) {
	for (p = filter_str;;p = NULL) {
		sub = strtok_r(p, ",", &save);
		if (sub == NULL)
			break;
		printf("sub: %s\n", sub);
	}
	return 0;
}

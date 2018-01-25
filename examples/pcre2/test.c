
#define PCRE2_CODE_UNIT_WIDTH 8

#include <stdio.h>
#include <string.h>
#include <pcre2.h>


int main(int argc, char** argv)
{
	PCRE2_SPTR pattern;
	PCRE2_SPTR sub;

	if (argc != 3) {
		printf("usage: test <string> <pattern>\n");
		return 0;
	}

	printf("sub: %s\n", argv[1]);
	printf("pattern: %s\n", argv[2]);

	sub = (PCRE2_SPTR)argv[1];
	pattern = (PCRE2_SPTR)argv[2];

	int errorcode;
	PCRE2_SIZE erroroffset;

//	pcre2_compile_context ccontext;

	pcre2_code* re;

	re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode,
			&erroroffset, NULL);
	if (re == NULL) {
		printf("re failed.\n");
		return -1;
	}

	pcre2_match_data* md;

	md = pcre2_match_data_create_from_pattern(re, NULL);
	if (md == NULL) {
		printf("md is NULL.\n");
		return -1;
	}

	int r;
	r = pcre2_match(re, sub, strlen((char*)sub), 0, 0, md, NULL);
	if (r == PCRE2_ERROR_NOMATCH) {
		printf("pcre2_match no match.\n");
		return -1;
	} else if (r < 0) {
		printf("pcre2_match failed.\n");
		return -1;
	}
	printf("matched.\n");

	pcre2_match_data_free(md);
	pcre2_code_free(re);
	
	return 0;
}

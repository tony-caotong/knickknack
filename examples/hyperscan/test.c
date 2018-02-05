/*
*	example for hyperscan.
*/
#include <stdio.h>
#include <string.h>

#include <hs.h>

int callback(unsigned int id, unsigned long long from, unsigned long long to,
		unsigned int flags, void* context)
{
	fprintf(stderr, "id: %u, matched position: %llu\n", id, to);
	return 0;
}

int main(int argc, char** argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <pattern> <string>\n", argv[0]);
		return -1;
	}
	char* pattern = argv[1];
	char* search_str = argv[2];

	hs_database_t* db;
	hs_compile_error_t * cerr;

	if (hs_compile(pattern, HS_FLAG_DOTALL, HS_MODE_BLOCK, NULL, &db,
			&cerr) != HS_SUCCESS) {
		fprintf(stderr, "hs_compile error %s: %s",
				pattern, cerr->message);
		hs_free_compile_error(cerr);
		return -1;
	}

	hs_scratch_t* scratch = NULL;
	if (hs_alloc_scratch(db, &scratch) != HS_SUCCESS) {
		fprintf(stderr, "hs_alloc_scratch error.\n");
		goto out;
	}

	int len = strlen(search_str);
	if (hs_scan(db, search_str, len, 0, scratch, callback, NULL)
			!= HS_SUCCESS) {
		fprintf(stderr, "hs_scan failed.\n");
		goto out;
	}
	hs_free_scratch(scratch);
out:
	hs_free_database(db);
	return 0;
}

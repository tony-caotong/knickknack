/*
*	example for hyperscan.
*/
#include <stdio.h>
#include <string.h>

#include <hs.h>

#define MULTI
#define STREAM

int callback(unsigned int id, unsigned long long from, unsigned long long to,
		unsigned int flags, void* context)
{
	fprintf(stderr, "id: %u, matched position: %llu\n", id, to);
	return 0;
}

int main(int argc, char** argv)
{
	hs_database_t* db;
	hs_compile_error_t * cerr;

#ifndef MULTI
	/* single pattern. */
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <pattern> <string>\n", argv[0]);
		return -1;
	}
	char* pattern = argv[1];
	char* search_str = argv[2];

	if (hs_compile(pattern, HS_FLAG_DOTALL, HS_MODE_BLOCK, NULL, &db,
			&cerr) != HS_SUCCESS) {
		fprintf(stderr, "hs_compile error %s: %s",
				pattern, cerr->message);
		hs_free_compile_error(cerr);
		return -1;
	}
#else
	/* multi pattern. */
#ifdef STREAM
	if (argc != 3) {
#else
	if (argc != 2) {
#endif
		fprintf(stderr, "Usage: %s <string>\n", argv[0]);
		return -1;
	}
	char* search_str = argv[1];
	
	const char* exp[] = {"1234","5678"};
	int flags[] = {HS_FLAG_DOTALL, HS_FLAG_DOTALL};
	unsigned int ids[] = {1, 2};

#ifdef STREAM
	if (hs_compile_multi(exp, flags, ids, 2, HS_MODE_STREAM,
			NULL, &db, &cerr) != HS_SUCCESS) {
#else
	if (hs_compile_multi(exp, flags, ids, 2, HS_MODE_BLOCK,
			NULL, &db, &cerr) != HS_SUCCESS) {
#endif
		fprintf(stderr, "hs_compile error: %s", cerr->message);
		hs_free_compile_error(cerr);
		return -1;
	}
#endif

	hs_scratch_t* scratch = NULL;
	if (hs_alloc_scratch(db, &scratch) != HS_SUCCESS) {
		fprintf(stderr, "hs_alloc_scratch error.\n");
		goto out;
	}

#ifndef STREAM
	int len = strlen(search_str);
	if (hs_scan(db, search_str, len, 0, scratch, callback, NULL)
			!= HS_SUCCESS) {
		fprintf(stderr, "hs_scan failed.\n");
		goto out;
	}
#else
	hs_stream_t* stream;
	if (hs_open_stream(db, 0, &stream) != HS_SUCCESS) {
		fprintf(stderr, "hs_open_stream error.\n");
		goto out;
	}

	int len = strlen(argv[1]);
	if (hs_scan_stream(stream, argv[1], len, 0, scratch, callback, NULL)
			!= HS_SUCCESS) {
		fprintf(stderr, "hs_scan_stream error.\n");
		goto out;
	}

	len = strlen(argv[2]);
	if (hs_scan_stream(stream, argv[2], len, 0, scratch, callback, NULL)
			!= HS_SUCCESS) {
		fprintf(stderr, "hs_scan_stream error.\n");
		goto out;
	}

	if (hs_close_stream(stream, scratch, callback, NULL) != HS_SUCCESS) {
		fprintf(stderr, "hs_close_stream error.\n");
		goto out;
	}
#endif
	hs_free_scratch(scratch);
out:
	hs_free_database(db);
	return 0;
}

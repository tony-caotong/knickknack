
/* content append to openssl/crypto/mem_dbg.c */

struct mdebug {
	uint64_t addr;  // 0 means empty
	char info[128];
	char** backtrace;
	int16_t backtrace_n;
	struct mdebug* next;
};

///////////////////////////////////////////////////////////////////////////////

#include <errno.h>
#include <execinfo.h>

#define HASH_SIZE 65521
#define BT_SIZE_MAX 64
static struct mdebug* mdebug_hash = NULL;
static int64_t mdebug_hash_size = 0;
//static FILE* mdebug_ffile = NULL;

int tong_mdebug_init()
{
	mdebug_hash = malloc(sizeof(struct mdebug) * HASH_SIZE);
	if (!mdebug_hash) {
		return -1;
	}
	return 0;
}

int tong_mdebug_alloc(uint64_t addr, const char* info, int line)
{
	struct mdebug* p;
	uint32_t key;
	void* bt_array[BT_SIZE_MAX];
	int bt_size;
	char** bt_strings;

	key = addr % HASH_SIZE;
	p = mdebug_hash + key;
	if (p->addr != 0) {
		struct mdebug* m;

		m = malloc(sizeof(struct mdebug));
		if (!m) {
			return -1;
		}
		p->next = m;
		p = m;
	}
	p->addr = addr;
	snprintf(p->info, sizeof(p->info), "%s:%d", info, line);
	p->next = NULL;

	/* get backtrace. */
	bt_size = backtrace(bt_array, BT_SIZE_MAX);
	bt_strings = backtrace_symbols(bt_array, bt_size);
	if (bt_strings == NULL) {
		p->backtrace = NULL;
		p->backtrace_n = -1;
	} else {
		p->backtrace = bt_strings;
		p->backtrace_n = bt_size;
	}

	mdebug_hash_size++;
	return 0;
}

int tong_mdebug_free(uint64_t addr)
{
	struct mdebug* m, *p;
	uint32_t key;
	int found;

	key = addr % HASH_SIZE;
	found = 0;
	for (m = mdebug_hash + key, p = NULL; m != NULL && m->addr != 0; p = m, m = m->next) {
		if (m->addr == addr) {
			if (p) {
				p->next = m->next;
				if (m->backtrace)
					free(m->backtrace);
				free(m);
			} else {
				m->addr = 0;
				m->backtrace = NULL;
				m->backtrace_n = 0;
			}
			mdebug_hash_size--;
			found = 1;
			break;
		}
	}
	if (!found) {
		return -1;
	}

	return 0;
}

void tong_mdebug_print(FILE* f)
{
	fprintf(f, "mdebug hash size: %ld\n", mdebug_hash_size);
	fflush(f);
}

void tong_mdebug_dump(FILE* f)
{
	int i, j;
	struct mdebug* p;
	int64_t count;

	count = 0;
	for (i = 0; i < HASH_SIZE; i++) {
		for (p = mdebug_hash + i; p != NULL; p = p->next) {
			if (p->addr != 0) {
				fprintf(f, "mdebug hash dump: addr: %p bt_n: %d info: %s\n",
						(void*)(p->addr), p->backtrace_n, p->info);
				for (j = 0; j < p->backtrace_n; j++) {
					fprintf(f, "\t\t%s\n", p->backtrace[j]);
				}
				count++;
			}
		}
	}
	fprintf(f, "mdebug hash dump: size: %ld count: %ld\n", mdebug_hash_size, count);
	fflush(f);
}


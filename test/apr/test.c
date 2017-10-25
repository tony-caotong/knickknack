
#include <apr_pools.h>
#include <apr_file_io.h>

void err(char* prefix, int rc)
{
	char err[1024];
	apr_strerror(rc, err, 1024);
	fprintf(stderr, "%s:(%d) %s\n", prefix, rc, err);
}

int split(char* conf, char* array, int item_len, int length)
{
	char* p;
	char* sub;
	int i;
	

	for (p = conf, i = 0; ;p = NULL, i++) {
		sub = strtok(p, "\t\n");
		if (sub == NULL || i >= length)
			break;
		apr_cpystrn(array+(i*item_len), sub, item_len);
	}
	return 0;
}

int main(int argc, char** argv)
{
	apr_file_t* f;
	apr_pool_t* pool = NULL;
	apr_status_t rc;
	char buf[2048];

	rc = apr_initialize();
	if (rc != APR_SUCCESS) {
		err("apr_initialize error.", rc);
		return -1;
	}

	rc = apr_pool_create(&pool, NULL);
	if (rc != APR_SUCCESS) {
		err("apr_pool_create error.", rc);
		apr_terminate();
		return -1;
	}
	printf("apr_init finished.\n");
	rc = apr_file_open(&f, "./test.txt", APR_READ | APR_BUFFERED,
		APR_OS_DEFAULT, pool);
	if (rc != APR_SUCCESS) {
		apr_pool_destroy(pool);
		apr_terminate();
		err("apr_file_open error.", rc);
		return -1;
	}

	while (1) {
		char array[4][1024];
		rc = apr_file_gets(buf, sizeof(buf), f);
		if (rc != APR_SUCCESS) {
			if (rc == APR_EOF) {
				printf("EOF read\n");
				break;
			} else {
				err("apr_file_gets error.", rc);
			}
		}
		printf("%s", buf);
		split(buf, array, 1024, 4);
		printf("1:%s, 2%s, 3%s, 4%s\n", array[0], array[1], array[2],
			array[3]);
	}
	
	apr_file_close(f);
	apr_pool_destroy(pool);
	apr_terminate();
	printf("things finished normally.\n");
	return 0;
}

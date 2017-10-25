#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
	int fd;
	long page_size;
	int i;
	char *addr;
	bool wt;

	wt = false;
	if (argc != 1)
		wt = true;

	fd = open("./mem", O_CREAT|O_RDWR, 0644);
	if (fd < 0) {
		perror("open(): ");
		exit(-1);
	}

	page_size = sysconf(_SC_PAGE_SIZE);
	printf("page_size: %ld\n", page_size);
//	addr = mmap(NULL, 32, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
	addr = mmap(NULL, 32, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		perror("mmap(): ");
		goto err;
	}	

	for (i = 0; i < 32; i++) {
		printf("%d: %c\n", i, addr[i]);
//		if (i==5)
//			addr[i] = '\0';
		if (wt)
			addr[i] = getchar();
	}
	printf("\n");
	
	addr[5] = '\0';

	getchar();
	munmap(addr, 32);
	if (addr == MAP_FAILED)
		perror("munmap(): ");
err:
	close(fd);
	return 0;
}

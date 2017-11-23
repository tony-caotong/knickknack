#include <stdio.h>
#include <time.h>

int main(int argc, char** argv)
{
	struct timespec t;
	
	if (clock_getres(CLOCK_REALTIME, &t) < 0)
		perror("clock_getres: ");
	printf("clock_getres(CLOCK_REALTIME): ");
	printf("sec: %llu nsec: %llu\n", t.tv_sec, t.tv_nsec);

	if (clock_gettime(CLOCK_REALTIME, &t) < 0)
		perror("clock_getres: ");
	printf("clock_gettime(CLOCK_REALTIME): ");
	printf("sec: %llu nsec: %llu\n", t.tv_sec, t.tv_nsec);

	if (clock_gettime(CLOCK_REALTIME_COARSE, &t) < 0)
		perror("clock_getres: ");
	printf("clock_gettime(CLOCK_REALTIME_COARSE): ");
	printf("sec: %llu nsec: %llu\n", t.tv_sec, t.tv_nsec);

	printf("MAX %llu\n", -1llu);
	return 0;
}

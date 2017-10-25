
struct test {
	char v1[8];
	char v2[15];
	char v3[33];
};

int main()
{
	struct test t1;
	struct test* t2;

	printf("v1: %d\n", sizeof(t1.v1));
	printf("v2: %d\n", sizeof(t1.v2));
	printf("v3: %d\n", sizeof(t1.v3));
	printf("v1: %d\n", sizeof(t2->v1));
	printf("v2: %d\n", sizeof(t2->v2));
	printf("v3: %d\n", sizeof(t2->v3));
	return 0;
}

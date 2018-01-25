#include <stdio.h>

int list[] = {9,5,4,3,6,1,7,8,2,0};

void print(int* array, int len)
{
	int i;

	for (i = 0; i < len; i++)
		printf("%d ", array[i]);
	printf("\n");
}

void pop(int* array, int len)
{
	int i,j, tmp;

	for (j = len; j > 0; j--) {
		for (i = 1; i < j; i++) {
			if (array[i-1] > array[i]) {
				tmp = array[i-1];
				array[i-1] = array[i];
				array[i] = tmp;
			}
		}
	}
}

void insert(int* array, int len)
{
	int i, j, index, tmp;

	for (i = 1; i < len; i++) {
		tmp = array[i];
		index = i;
		for (j = i - 1; j >= 0; j--) {
			if (array[j] > tmp) {
				array[j+1] = array[j];
				index = j;
			} else {
				break;
			}
		}
		array[index] = tmp;
	}
}

void insert2(int* array, int len)
{
	int i, j, index, tmp;

	for (i = 1; i < len; i++) {
		tmp = array[i];
		index = i;
		for (j = i; j >= 0; j--) {
			if (j == i)
				continue;
			if (array[j] > tmp) {
				array[j+1] = array[j];
				index = j;
			} else {
				break;
			}
		}
		array[index] = tmp;
	}
}

void select(int* array, int len)
{
	int i, j, min, tmp;

	for (i = 0; i < len; i++) {
		min = i;
		for (j = i; j < len; j++) {
			if (array[j] < array[min]) {
				min = j;
			}
		}
		tmp = array[i];
		array[i] = array[min];
		array[min] = tmp;
	}
}

int main(int argc, char** argv)
{
	int len = sizeof(list)/sizeof(int);

	print(list, len);
	//pop(list, len);
	//insert(list, len);
	//insert2(list, len);
	select(list, len);
	print(list, len);
	return 0;
}

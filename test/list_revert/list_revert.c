#include <stdio.h>

struct node_t {
	int v;
	struct node_t* next;
};

/*
	 header->n1->n2->n3->n4->null
*/

// 递归
void revert2(struct node_t* list)
{

}

void revert(struct node_t* list)
{
	struct node_t* h; 
	struct node_t *p, *c, *n;

	if (list == NULL || list->next == NULL)
		return;

	h = list;
	p = NULL;
	c = h->next;

	while (c != NULL) {
		n = c->next;
		c->next = p;
		p = c;
		c = n;
	}
	h->next = p;
}

int main(int argc, char** argv)
{
	struct node_t h, n1, n2, n3, n4;
	struct node_t* p;

	h.next = &n1;
	n1.v = 1;
	n1.next = &n2;
	n2.v = 2;
	n2.next = &n3;
	n3.v = 3;
	n3.next = &n4;
	n4.v = 4;
	n4.next = NULL;

	for (p = h.next; p != NULL; p=p->next) {
		printf("%d ", p->v);
	}
	printf("\n");

	revert(&h);

	for (p = h.next; p != NULL; p=p->next) {
		printf("%d ", p->v);
	}
	printf("\n");
	return 0;
}

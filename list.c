#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>

typedef struct sNode
{
	void *info;
	struct sNode *next;
} Node;
typedef struct
{
	Node *front;
	char (*equals)(void *, void *);
} List;
void push_front(List *, void *);
void push_back(List *, void *);
void insert(List *, void *, int);
void pop_front(List *);
void pop_back(List *);
void remove_elem(List *, int);
int index_of(List *, void *);
void *element_in(List *, int);
void print_list(List *);
void free_list(List *);
char equals_str(char *, char *);

void push_front(List *list, void *info)
{
	Node *new_front = (Node *)malloc(sizeof(Node));
	(*new_front).info = info;
	(*new_front).next = (*list).front;

	(*list).front = new_front;
}

void push_back(List *list, void *info)
{
	if ((*list).front == NULL)
	{
		push_front(list, info);
		return;
	}

	Node *current = (*list).front;
	while ((*current).next != NULL)
		current = (*current).next;

	Node *new_node = (Node *)malloc(sizeof(Node));
	(*new_node).info = info;
	(*new_node).next = NULL;

	(*current).next = new_node;
}

void insert(List *list, void *info, int index)
{
	if (index == 0)
	{
		push_front(list, info);
		return;
	}

	Node *current = (*list).front;
	int i;
	for (i = 1; i < index; i++)
	{
		if (current == NULL)
			return;
		current = (*current).next;
	}

	Node *new_node = (Node *)malloc(sizeof(Node));
	(*new_node).info = info;
	(*new_node).next = (*current).next;

	(*current).next = new_node;
}

void pop_front(List *list)
{
	if ((*list).front == NULL)
		return;

	Node *aux = (*list).front;
	(*list).front = (*(*list).front).next;
	free(aux);
}

void pop_back(List *list)
{
	if ((*list).front == NULL)
		return;

	Node *prev = NULL;
	Node *current = (*list).front;
	while ((*current).next != NULL)
	{
		prev = current;
		current = (*current).next;
	}

	if (prev == NULL)
		(*list).front = NULL;
	else
		(*prev).next = NULL;

	free(current);
}

void remove_elem(List *list, int index)
{
	if (index == 0)
	{
		pop_front(list);
		return;
	}

	Node *current = (*list).front;
	int i;
	for (i = 1; i < index; i++)
	{
		if (current == NULL)
			return;
		current = (*current).next;
	}

	Node *aux = (*current).next;
	if (aux == NULL)
		return;
	(*current).next = (*(*current).next).next;

	free(aux);
}

int index_of(List *list, void *info)
{
	Node *current = (*list).front;
	int i = 0;
	for (; current != NULL; i++, current = (*current).next)
		if ((*(*list).equals)((*current).info, info))
			return i;

	return -1;
}

void *element_in(List *list, int n)
{
	Node *current = (*list).front;
	int i;
	for (i = 0; current != NULL; i++, current = (*current).next)
		if (i == n)
			return (*current).info;
	return NULL;
}

void printListaStr(List *list)
{
	Node *current = (*list).front;
	while (current != NULL)
	{
		printf("%s -> ", (*current).info);
		current = (*current).next;
	}
	printf("null");
}

void free_list(List *list)
{
	Node *current = (*list).front;
	while (current != NULL)
	{
		free((*current).info);
		Node *next = (*current).next;

		free(current);
		current = next;
	}

	free(list);
}

char equals_str(char *str1, char *str2)
{
	return strcmp(str1, str2) == 0;
}

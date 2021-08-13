#include "stack.h"
#include <stdlib.h>

#define CHECK_STACK(STACK)                                                                         \
	if (STACK == NULL)                                                                             \
		return NULL_ERROR;                                                                         \
	if (*STACK == NULL)                                                                            \
		return EMPTY_ERROR;

struct element {
	int data;
	struct element *next;
} typedef Element;


Stack *new_stack()
{
	Stack *stack = (Stack *)malloc(sizeof(Stack));

	if (stack != NULL)
		*stack = NULL;

	return stack;
}


int stack_push(Stack *stack, int new_data)
{
	if (stack == NULL)
		return NULL_ERROR;

	Element *new = (Element *)malloc(sizeof(Element));

	if (new == NULL)
		return NOT_OK;

	new->data = new_data;
	new->next = *stack;
	*stack = new;

	return OK;
}


int stack_pop(Stack *stack)
{
	CHECK_STACK(stack)
	Element *aux = *stack;
	*stack = aux->next;
	free(aux);

	return OK;
}


int stack_get(Stack *stack, int *output)
{
	CHECK_STACK(stack)
	*output = (*stack)->data;

	return OK;
}


void empty_stack(Stack *stack)
{
	if (stack == NULL)
		return;

	Element *aux;

	while (*stack != NULL) {
		aux = *stack;
		*stack = aux->next;
		free(aux);
	}
	// free(stack);
}


int stack_size(Stack *stack)
{
	if (stack == NULL)
		return NULL_ERROR;

	int value = 0;
	Element *aux = *stack;

	while (aux != NULL) {
		aux = aux->next;
		++value;
	}

	return value;
}


// Por meio da função `stack_size` já podemos identificar se a stack está vazia.
// int is_stack_empty(Stack *stack)
// {
// 	if (stack == NULL)
// 		return NULL_ERROR;

// 	return *stack == NULL;
// }

#include "list.h"
#include <stdio.h>
#include <stdlib.h>

//#define MALLOC_ERROR {WARNING("Memória insuficiente!"); return NOT_OK;}
#define CHECK_LIST(LIST)                                                                           \
	if (LIST == NULL) {                                                                            \
		return NULL_ERROR;                                                                         \
	}                                                                                              \
	if (*LIST == NULL) {                                                                           \
		return EMPTY_ERROR;                                                                        \
	}

/* Um elemento único da lista. */
struct element {
	enum Piece piece;
	struct element *next;
};

/* Representação do `struct element` (um elemento único da lista). */
typedef struct element Element;

/* Nota: `List *` representa as seguintes opções:
- struct element **
- Element **
*/

List *new_list()
{
	List *list = (List *)malloc(sizeof(List));

	if (list != NULL)
		*list = NULL;

	return list;
}


int list_add_at_start(List *list, enum Piece new_piece)
{
	if (list == NULL)
		return NULL_ERROR;

	Element *new = (Element *)malloc(sizeof(Element));

	if (new == NULL)
		return NOT_OK;

	new->piece = new_piece;
	new->next = *list;
	*list = new;

	return OK;
}


void empty_list(List *list)
{
	if (list == NULL)
		return;

	Element *aux;

	while (*list != NULL) {
		aux = *list;
		*list = (*list)->next;
		free(aux);
	}
	// free(l); // Opcional: limpa também o ponteiro inicial, não só os elementos.
}


int list_count(List *list, enum Piece piece)
{
	if (list == NULL)
		return NULL_ERROR;

	if (*list == NULL)
		return 0;

	int value = 0;
	Element *aux = *list;

	while (aux != NULL) {

		if (aux->piece == piece)
			value++;

		aux = aux->next;
	}

	return value;
}


int list_has(List *list, enum Piece piece)
{
	if (list == NULL)
		return NULL_ERROR;

	Element *aux = *list;

	while (aux != NULL)

		if (aux->piece == piece)
			return 1; // true
		else
			aux = aux->next;

	return 0; // false
}


int list_remove(List *list, enum Piece piece)
{
	CHECK_LIST(list)
	Element *previous = *list; // Começa como o primeiro elemento da lista.
	// Começa como o segundo elemento da lista, possivelmente NULL.
	Element *current = previous->next;

	if (previous->piece == piece)
		*list = current;

	else if (current == NULL)
		return NOT_OK;

	else {
		while (current->piece != piece && current->next != NULL) {
			previous = current;
			current = current->next;
		}

		if (current->next == NULL && current->piece != piece)
			return NOT_OK;

		previous->next = current->next;
		free(current);
	}

	return OK;
}


/* Métodos depreciados: métodos de lista que não foram usados nesse caso. */

/* //Desnecessário
int cheia(List *list)
{
    return NOT_OK;
}
*/

/* //Retorna a quantidade de elementos da lista.
//Se a lista não existir retorna `NULL_ERROR`, ao invés disso.
int list_size(List *list)
{
    if (list == NULL)
        return NULL_ERROR;

    if (*list == NULL)
        return 0;

    int value = 0;
    Element *aux = *list;

    while (aux != NULL) {
        aux = aux->next;
        value++;
    }

    return value;
}
*/


/* //Insere um elemento no final da lista.
//Retorno: NULL_ERROR, NOT_OK, OK.
int list_add_at_end(List *list, enum Piece new_piece)
{
    if (list == NULL)
        return NULL_ERROR;

    Element *new = (Element *)malloc(sizeof(Element));

    if (new == NULL)
        return NOT_OK;

    new->piece = new_piece;
    new->next = NULL;

    if (*list == NULL) {
        *list = new;
    }
    else {
        Element *aux = *list;

        while (aux->next != NULL)
            aux = aux->next;

        aux->next = new;
    }

    return OK;
}
*/


/* //Insere um elemento de forma 'ordenada', relativo à posição dos elementos na lista.
//Retorno: NULL_ERROR, INPUT_ERROR, NOT_OK, OK.
int list_add_at(List *list, enum Piece new_piece, int index)
{
    if (list == NULL)
        return NULL_ERROR;

    if (index < 0)
        return INPUT_ERROR;

    if (index == 0) {
        Element *new = (Element *)malloc(sizeof(Element));

        if (new == NULL)
            return NOT_OK;
        ;

        new->next = *list;
        new->piece = new_piece;
        *list = new;

        return OK;
    }

    if (*list == NULL)
        return NOT_OK;

    int i = 1;
    Element *previous = *list;
    // Para igualar a verificação com o 'próximo do anterior' iniciamos a contagem pelo i(0) + 1.
    // Usamos o 'próximo do anterior' pois, como devemos inserir um elemento na posição do índice,
    // isso evita armazenar um auxiliar adicional.
    while (previous->next != NULL && i != index) {
        previous = previous->next;
        ++i;
    }

    if (previous->next == NULL)
        return NOT_OK;

    Element *new = (Element *)malloc(sizeof(Element));

    if (new == NULL)
        return NOT_OK;

    new->next = previous->next;
    new->piece = new_piece;
    previous->next = new;

    return OK;
}
*/


/* //Remove o primeiro elemento da lista.
//Retorno: NULL_ERROR, EMPTY_ERROR, OK
int list_remove_from_start(List *list)
{
    CHECK_LIST(list)
    Element *aux = *list;
    *list = aux->next;
    free(aux);

    return OK;
}
*/


/* //Remove o último elemento da lista.
//Retorno: NULL_ERROR, EMPTY_ERROR, OK.
int list_remove_from_end(List *list)
{
    CHECK_LIST(list)
    Element *previous = *list; // Começa como o primeiro elemento da lista.
    // Começa como o segundo elemento da lista, possivelmente `NULL`.
    Element *current = previous->next;

    if (current == NULL) {
        free(previous);
        *list = NULL;
    }
    else if (current->next == NULL) {
        // Fiz esse `esse if` adicional para evitar ficar dando cache em `previous` e `current` ao
        // mesmo tempo durante a iteração do loop abaixo.
        free(current);
        previous->next = NULL;
    }
    else {
        while (current->next->next != NULL)
            current = current->next;

        previous = current;
        current = current->next;
        previous->next = current->next; // Equivalente à `... = NULL`.
        free(current);
    }

    return OK;
}
*/


/* //Remove um elemento de acordo com sua posição na lista.
//Retorno: NULL_ERROR, EMPTY_ERROR, INPUT_ERROR, NOT_OK, OK.
int list_remove_from(List *list, int index)
{
    CHECK_LIST(list)

    if (index < 0)
        return INPUT_ERROR;

    Element *aux;

    if (index == 0) {
        aux = (*list)->next;
        free(*list);
        *list = aux;

        return OK;
    }

    int i = 1;
    Element *previous = *list;
    // Para igualar a verificação com o 'próximo do anterior' iniciamos a contagem pelo i(0) + 1.
    // Usamos o 'próximo do anterior' pois, como devemos inserir um elemento na posição do índice,
    // isso evita armazenar um auxiliar adicional.
    while (previous->next != NULL && i != index) {
        previous = previous->next;
        ++i;
    }

    if (previous->next == NULL)
        return NOT_OK;

    aux = previous->next->next;
    free(previous->next);
    previous->next = aux;

    return OK;
}
*/


// int cheia(List *); // Por ser uma lista dinâmica ela não pode ficar cheia.


/* //Retorna `OK` se a lista estiver vazia, e `NOT_OK`, caso contrário.
//Também retorna `NULL_ERROR`, se a lista for inexistente
int is_list_empty(List *list)
{
    if (list == NULL)
        return NULL_ERROR;

    if (*list == NULL)
        return OK;

    return NOT_OK;
}
*/


/* //Acessa um elemento da lista por índice.
//Retorno: NULL_ERROR, EMPTY_ERROR, INPUT_ERROR, NOT_OK, OK.
int list_get_from(List *list, int index, enum Piece *output)
{
    CHECK_LIST(list);

    if (index < 0)
        return INPUT_ERROR;

    int i = 0;
    Element *aux = *list;

    while (aux != NULL && i != index) {
        aux = aux->next;
        ++i;
    }

    if (aux == NULL)
        return NOT_OK;

    *output = aux->piece;

    return OK;
}
*/


/* //Acessa um elemento da matriz com busca (matrícula).
//Retorno: NULL_ERROR, EMPTY_ERROR, NOT_OK, OK.
int list_get(List *list, enum Piece piece, enum Piece *output)
{
    CHECK_LIST(list)
    Element *aux = *list;

    while (aux != NULL && aux->piece != piece)
        aux = aux->next;

    if (aux == NULL)
        return NOT_OK;

    *output = aux->piece;

    return OK;
}
*/


/* // Função extra que insere com busca.
int inserir_apos(List *list, enum Piece new_piece, enum Piece)
{
    CHECK_LIST(list)

    Element *atual = *list;

    while (atual != NULL && atual->piece.matricula != matricula)
        atual = atual->next;

    if (atual == NULL)
        return NOT_OK;

    Element *next = atual->next;
    Element *novo_element = (Element *)malloc(sizeof(Element));

    if (novo_element == NULL) return NOT_OK;

    novo_element->piece = new_piece;
    novo_element->next = atual->next;
    free(next);

    return OK;
}
*/

int teste(void)
{
	return 1;
}

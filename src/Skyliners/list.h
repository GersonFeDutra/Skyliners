#define OK 1           /* Operação realizada com sucesso. */
#define NOT_OK 0       /* Operação não foi realizada. */
#define NULL_ERROR -1  /* Lista inexistente. */
#define EMPTY_ERROR -2 /* Lista vazia. */
// #define INPUT_ERROR -3 /* Entrada inválida. */

// Lista Dinâmica Simplesmente Encadeada Homogénea.
typedef struct element *List;
/* Usamos `typedef` para definir o tipo de dado e ocultar o `*` da lista */

// Determina os valores que a lista suporta.
enum Piece {
	APARTMENT, // Empilha e pode ser empilhado.
	PARK,      // Não empilha e não pode ser empilhado.
	ROOF,      // Empilha, mas não pode ser empilhado.
}; // Não é o uso mais belo de uma lista, IMHO. Mas, usaremos em prol do trabalho. ¯\_(ツ)_/¯

/* Protótipos */

/* Cria uma lista vazia. */
List *new_list(void);
/* 'List` é um `struct element *` que passaremos por referência: `List *`; isto é, `struct element
**`. Isso permite que o ponteiro inicial da lista seja alterado internamente nas funções. */

/* 'Destrói' uma lista, esvaziando o seu conteúdo. */
void empty_list(List *);

/* Retorna a quantidade de peças do tipo indicado existentes na lista.
Se a lista não existir retorna `NULL_ERROR`, ao invés disso. */
int list_count(List *, enum Piece);

/* Indica se a peça determinada existe na lista. - Retorno booleano.
Se a lista não existir retorna `NULL_ERROR`, ao invés disso. */
int list_has(List *, enum Piece);

/* Insere um elemento no início da lista.
Retorno: NULL_ERROR, NOT_OK, OK. */
int list_add_at_start(List *, enum Piece);

/* Remove o elemento indicado da lista.
Retorno: NULL_ERROR, EMPTY_ERROR, NOT_OK, OK. */
int list_remove(List *, enum Piece);

// O restante dos métodos que não foram usados no projeto estão comentados no '.c' desse TAD.

int teste(void);

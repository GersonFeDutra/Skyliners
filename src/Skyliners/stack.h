#define OK 1           /* Operação realizada com sucesso. */
#define NOT_OK 0       /* Operação não foi realizada. */
#define NULL_ERROR -1  /* Pilha inexistente. */
#define EMPTY_ERROR -2 /* Pilha vazia. */

// Pilha Dinâmica Homogénea. Empilha um único elemento inteiro.
typedef struct element *Stack;
/* Como a pilha é homogénea, comportando apenas um único dado inteiro, não há necessidade de usar
uma struct. */

/* Protótipos */

/* Cria uma pilha vazia. */
Stack *new_stack(void);
/* `Stack` é um nó descritor `struct stack` que armazena dados de acesso aos elementos da pilha
(o elemento inicial e o elemento final). */

/* 'Destrói' uma pilha, esvaziando seu conteúdo. */
void empty_stack(Stack *);

/* Retorna a quantidade de elementos da pilha.
Se a pilha não existir retorna `NULL_ERROR`, ao invés disso. */
int stack_size(Stack *);

/* Insere um dado inteiro na pilha. Note que, pilhas só podem inserir no final.
Retorno: NULL_ERROR, NOT_OK, OK. */
int stack_push(Stack *, int data);

/* Remove um elemento. Note que, pilhas só podem remover do início.
Retorno: NULL_ERROR, EMPTY_ERROR, OK. */
int stack_pop(Stack *);

/* Acessa um dado inteiro, sendo esse o primeiro elemento da pilha.
Retorno: NULL_ERROR, EMPTY_ERROR, OK. */
int stack_get(Stack *, int *data);

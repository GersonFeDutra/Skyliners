#include "stack.h"
// #include "globals.h"

#define SIZE 5 // Tamanho do tabuleiro.

// extern Stack *board[SIZE][SIZE];
/* `board` poderia ter sido definido como um singleton para facilitar o seu acesso nas funções nesse
projeto, mas, ao invés disso, o defini como um Tipo Abastrato de Dado em prol da modularidade. */

// Tabuleiro onde as peças do jogo são empilhadas.
typedef Stack ***Board;
/* Matriz de ponteiros de pilhas (aka.: Vetor de vetores de ponteiros de ponteiros de `struct
element`) :D */
// Um elemento `board` derivado de `Board` nada mais é do quê o primeiro endereço dessa matriz.
/* Poderia ter implementado uma matriz de pilhas ainda mais genérica (sem as regras do jogo), Mas,
para essa aplicação esse TAD já é suficientemente conveniente. */

// typedef Stack (*Board)[SIZE][SIZE];
// Poderia ter implementado o tabuleiro nesse formato, mas decidi implementar como um genérico, anw
// (para fins didáticos). :v

/* Protótipos das funções */

/* Inicializa o tabuleiro, com as peças iniciais já inseridas. */
Board new_board(void);

/* Aplica uma jogada. Retorna um booleano que indica se o movimento foi realizado com sucesso.
Espera-se que o valor entrado seja uma peça válida das seguintes opções:
0 - APARTMENT, 1 - PARK, 2 - ROOF. */
int move(Board board, int column, int line, int piece);
/* O motivo pelo qual não defini essas peças como constantes nesse header, foi por conveniência já
que esses valores correspondem ao valor inteiro do enum da lista nesse projeto. */

/* Imprime o tabuleiro visto de cima. `current_player` deve ser um valor de 0 à 4, correspondendo à
posição do jogador partindo da visão norte, em sentido horário. */
void print_table_topdown(Board board, int current_player);

/* Imprime o tabuleiro pelo ponto de vista de um dos jogadores. `from_player` deve ser um valor
de 0 à 4, correspondendo à posição do jogador partindo da visão norte, em sentido horário. */
void print_table_sideview(Board board, int from_player);

/* Retorna a quantidade de pontos do jogador indicado. `player` deve ser um valor de 0 à 4,
correspondendo à posição do jogador partindo da visão norte, em sentido horário. */
int get_points_from(Board board, int player);

/* Destrói um tabuleiro. */
void free_board(Board);

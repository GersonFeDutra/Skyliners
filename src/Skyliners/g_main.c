#include "game_board.h"
// #include "input_validate.h"
#include "list.h"
// #include <assert.h>
// #include <locale.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
// #include <time.h>

#define PLAYERS 4 // Quantidade de jogadores.
#define INITIAL_PARKS 1
#define INITIAL_ROOFS 2
#define INITIAL_APARTMENTS 12
/* Em casos que a alocação é necessária para o andamento do programa,
apenas encerramos a execução com uma mensagem de erro. */
#define MEMORY_ERROR(POINTER)                                                                      \
	if (POINTER == NULL) {                                                                         \
		printf("Erro: Memória insuficiente!\n");                                                   \
		exit(1);                                                                                   \
	}

/*
#if defined(_WIN32) || defined(_WIN64)
#define CLEAR                                                                                      \
    system("pause");                                                                               \
    system("cls");
#endif
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#define CLEAR                                                                                      \
    printf("Pressione a tecla Enter para continuar...");                                           \
    while (getchar() != '\n') {                                                                    \
    }                                                                                              \
    system("clear");
#endif
#ifndef CLEAR
// ANSI consoles
#define CLEAR                                                                                      \
    printf("Pressione a tecla Enter para continuar...");                                           \
    while (getchar() != '\n') {                                                                    \
    }                                                                                              \
    printf("\e[1;1H\e[2J");
#endif
*/

/* Skyliners - Projeto de algoritmos & Estruturas de Dados I

Regras:
- Começando o jogo:
    1. Um parque é posicionado no centro do tabuleiro;
    2. Seis andares são posicionados no centro do tabuleiro em posições aleatórias;
    3. Cada jogador recebe 12 andares, 1 único parque e 3 coberturas.
- Jogando:
    1. Os jogadores podem posicionar uma peça de cada vez a cada rodada;
    2. Uma rodada acaba quando todos tiverem jogado;
    3. O jogo acaba na rodada em que um dos jogadores tiver esgotado todos os seus andares.
- Peças:
    Andar: pode ser posicionado no chão, ou no topo de outros andares;
    Parque: só pode ser posicionado no chão;
    Cobertura: só pode ser posicionada sobre andares.
- Pontos:
    1. Conforme os jogadores vão posicionando suas peças eles vão adquirindo pontos para cada prédio
    na sua linha de visão;
    2. A visão de cada jogador é determinada pelo lado que estão do tabuleiro: os prédios altos
    anulam a visão dos prédios mais baixos que estiverem mais longe em uma mesma linha de visão
    perpendicular ao jogador;
    3. Ganha o jogador que fizer mais pontos ao final do jogo.
*/
/*
int main()
{
    setlocale(LC_ALL, "Portuguese");
    srand(time(0)); // 'Inicializa' o gerador de números aleatórios.

    void init_players(List * (*)[4]);
    void init_player(List *);
    int play(Board, int player, List *);
    int can_continue(List *[4]);
    int get_winner(int first_player, Board board);

    // Posição de cada jogador em relação ao tabuleiro, listados no sentido horário.
    char VIEWS[][6] = {"norte", "leste", "sul", "oeste"};
    int i;
    int first_player = rand() % PLAYERS; // O primeiro jogador é escolhido aleatoriamente.
    List *players_pieces[PLAYERS];       // Listas das peças de cada jogador.
    Board board = new_board();           // Tabuleiro onde as peças serão empilhadas.
    // first_player = 2;

    MEMORY_ERROR(board)
    init_players(&players_pieces);

    // Game loop
    while (g_can_continue(players_pieces)) {

        for (i = 0; i < PLAYERS; ++i) {
            int player = (first_player + i) % PLAYERS;

            do {
                print_table_topdown(board, player);
                printf("\n");
                printf("Vez do %d° jogador (%s): ", i + 1, VIEWS[player]);
                printf("{%d andar(es), %d cobertura(s), %d parque(s), %d pontos (prédios em sua "
                       "linha de visão)}\n",
                    list_count(players_pieces[player], APARTMENT),
                    list_count(players_pieces[player], ROOF),
                    list_count(players_pieces[player], PARK), get_points_from(board, player));
            } while (!play(board, player, players_pieces[player]));
        }
    }

    int get_winner(first_player, board);
    int last_player = (first_player + i) % PLAYERS;
    // Usamos o valor de `i` que ficou na memória para obter o último jogador, ao final.

    int can_exit;
    do {
        print_table_topdown(board, last_player);
        printf("\nO jogador ao %s é o vencedor, com %d pontos!\nBom Jogo!\n", VIEWS[winner],
            get_points_from(board, winner));

        printf("\nDigite 0 para sair ou:\n");
        printf("1 - ver vista norte\n");
        printf("2 - ver vista leste\n");
        printf("3 - ver vista sul\n");
        printf("4 - ver vista oeste\n");
        i = input_integer_range(
            0, 4, ">>>", "Entrada inválida!\n>>>", ">>>", "Entrada inválida!\n>>>");
        can_exit = i == 0;

        if (!can_exit) {
            print_table_sideview(board, i - 1);
            CLEAR
        }
    }
    while (!can_exit);

    // Não implementei essas funções por quê o programa já vai ser encerrado de qualquer forma.
    //¯\_(ツ)_/¯  Mas, se fosse necessário, bastava fazer alguns loops e chamar as respectivas
funções
    //de esvaziar de pilha e de lista (lembrando de dar `free` nos ponteiros).
    // free_board(board);
    // free_players(players_pieces);

    return 0;
}
*/

/*
int play(Board board, int player, List *player_pieces)
{
    char _E = (char)('A' + SIZE - 1);
    //char _e = (char)('a' + SIZE - 1);
    // Consequências diretas do tamanho do tabuleiro.
    char pieces_names[][12] = {"andar", "parque", "telhado"};
    char invalid_integer_input_message[] = "Entrada inválida! Insira um valor entre 0 e 3.\n>>>";
    int line, column, piece;

    do {
        printf("\nInserir peça (%d - apartamento, %d - parque, %d - cobertura) ", APARTMENT + 1,
            PARK + 1, ROOF + 1);
        printf("ou:\n0 - Vista do jogador\n");

        while (1) {
            piece = input_integer_range(
                0, 3, ">>>", invalid_integer_input_message, ">>>", invalid_integer_input_message);

            if (piece == 0) {
                print_table_sideview(board, player);
                CLEAR
                return 0;
            }
            piece -= 1;

            if (list_count(player_pieces, (enum Piece)(piece)) > 0)
                break;

            printf("O jogador não possui mais peças do tipo %s. Tente novamente:\n",
                pieces_names[piece]);
        }

        printf("\nCoordenada (linha de A - %c | coluna de 1 - %d)[A1 - %c%d]:\n", _E, SIZE, _E,
SIZE); input_alnum_coords(&line, &column, SIZE, SIZE, ">>> ", "Coordenada inválida!\nInsira uma
letra seguida de um número correspondente às " "coordenadas do tabuleiro. Ex.: `B2`.\n");

    } while (!move(board, column, line, piece));

    // A lista sempre terá a peça que iremos remover aqui.
    assert(list_remove(player_pieces, (enum Piece)piece) == OK);
    CLEAR

    return 1;
}
*/


/* Adiciona peças na lista do jogador. */
void init_player(List *player_pieces)
{
	int i;

	for (i = 0; i < INITIAL_APARTMENTS; ++i)
		list_add_at_start(player_pieces, APARTMENT);

	for (i = 0; i < INITIAL_ROOFS; ++i)
		list_add_at_start(player_pieces, ROOF);

	for (i = 0; i < INITIAL_PARKS; ++i)
		list_add_at_start(player_pieces, PARK);
}


void init_players(List *(*players_pieces)[4])
{
	int i;

	for (i = 0; i < PLAYERS; ++i) {
		(*players_pieces)[i] = new_list();
		MEMORY_ERROR((*players_pieces)[i])
		init_player((*players_pieces)[i]);
	}
}


void free_players(List *(*players_pieces)[4])
{
	int i;

	for (i = 0; i < PLAYERS; ++i) {
		empty_list((*players_pieces)[i]);
		free((*players_pieces)[i]);
	}
}


int g_can_continue(List *players_pieces[4])
{
	int i;

	for (i = 0; i < PLAYERS; ++i)
		if (!list_has(players_pieces[i], APARTMENT))
			return 0;

	return 1;
}


int get_winner(int first_player, Board board)
{
	int i;
	int winner, points, max_points = 0;

	for (i = 0; i < PLAYERS; ++i) {
		int player = (first_player + i) % PLAYERS;

		if ((points = get_points_from(board, player)) > max_points) {
			winner = player;
			max_points = points;
		}
	}

	return winner;
}

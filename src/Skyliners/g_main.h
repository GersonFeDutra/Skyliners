#include "list.h"
#include "game_board.h"

/* Inclui algumas das funções do programa principal usada no projeto original. */

/* Inicializa a lista de peças dos jogadores. */
void init_players(List *(*players_pieces)[4]);

/* Libera a lista de peças dos jogadores. */
void free_players(List *(*players_pieces)[4]);

/* Verifica se o jogo pode continuar.
Assume que uma rodada foi completada (todos os jogadores jogaram nesse turno). */
int g_can_continue(List *players_pieces[4]);

/* Verifica o vencedor, considerando-se que foi atingido o final do jogo.
Em caso de empate, o primeiro jogador (dos empatados) é o vencedor, */
int get_winner(int first_player, Board board);

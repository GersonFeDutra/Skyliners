#include "game_board.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "stack.h"

// Peças que serão inseridas no tabuleiro
#define APARTMENT 0 // Empilha e pode ser empilhado.
#define PARK 1      // Não empilha e não pode ser empilhado.
#define ROOF 2      // Empilha, mas não pode ser empilhado.

/* Posições relativas, em relação as bordas. */
#define TOP_DOWN 0   // Norte.
#define RIGHT_LEFT 1 // Leste.
#define DOWN_TOP 2   // Sul.
#define LEFT_RIGHT 3 // Oeste,

// Quantidade inicial de peças lançadas aleatoriamentes no início do jogo
#define INITIAL_PIECES 6

/* Parâmetro inválido. */
#define INVALID_INPUT_ERROR -1

/* Em casos que a alocação é necessária para o andamento do programa,
apenas encerramos a execução com uma mensagem de erro. */
#define MEMORY_ERROR(POINTER)                                                                      \
	if (POINTER == NULL) {                                                                         \
		printf("Erro: Memória insuficiente!\n");                                                   \
		exit(1);                                                                                   \
	}

// Assegura que a entrada é válida.
#define INVALID_PLAYER_ERROR(FROM) assert(FROM >= 0 && FROM < 4);
/* Poderia ter retornado um código de erro, ao invés de usar asserção, mas como esse TAD é muito
específico pra esse projeto não vi necessidade de tanto preciosismo. Assim, é como se assumisse que
nunca passaremos a entrada errada. A asserção já cria um log de erro mais genérico, anw. */

static const int LAST_ROOM = SIZE - 1; // Última "casa" do tabuleiro.
static const int CENTER = SIZE / 2;    // Índice central do tabuleiro.


/* Funções "privadas". */

/* Cria um tabuleiro vazio. */
static Board _create_board()
{
	int i, j;
	Board board = (Stack ***)malloc(sizeof(Stack **) * SIZE);

	if (board == NULL)
		return board;

	// Inicializa o tabuleiro.
	for (i = 0; i < SIZE; ++i) {
		board[i] = (Stack **)malloc(sizeof(Stack *) * SIZE);
		MEMORY_ERROR(board[i])

		for (j = 0; j < SIZE; ++j)
			board[i][j] = NULL;
	}

	return board;
}


/* Busca criar uma nova pilha para a célula nas coordenadas indicadas.
Pode encerrar a execução com uma mensagem de erro, caso não for possível alocar memória. */
static void _create_cell_stack(Board board, int column, int line, int type)
{
	board[column][line] = new_stack();
	MEMORY_ERROR(board[column][line])
	stack_push(board[column][line], type);
}


/* Verifica se a peça está está inserido na posição indicada. */
static int _check_cell(Board board, int column, int line, int piece)
{
	int value;

	if (stack_get(board[column][line], &value) != OK)
		return 0; // falso

	return value == piece;
}


/* Verifica se há alguma das peças indicadas na posição determinada. */
static int _check_cell_in(Board board, int column, int line, int *pieces, int amount)
{
	int value, i;

	if (stack_get(board[column][line], &value) != OK)
		return 1;

	for (i = 0; i < amount; ++i)

		if (value == pieces[i])
			return 1;

	return 0;
}


/* Verifica se é possível adicionar a peça na posição do tabuleiro indicada. */
static int _is_move_invalid(Board board, int column, int line, int piece)
{
	int current_piece;

	if (stack_get(board[column][line], &current_piece) != OK)
		return piece == ROOF;

	switch (piece) {

		case APARTMENT: return current_piece != APARTMENT;
		case ROOF: return current_piece != APARTMENT;
		case PARK: return 1; // É inválido, pois há uma peça já ocupando aquela posição.
		default: return INVALID_INPUT_ERROR;
	}
}


/* Adiciona uma peça na célula indicada.
Se houver algum erro de alocação, o programa será encerrado. */
static void _add_to_cell(Board board, int column, int line, int piece)
{
	if (board[column][line] == NULL)
		_create_cell_stack(board, column, line, piece);

	else if (stack_push(board[column][line], piece) != OK) {
		printf("Erro! Memória insuficiente!\n");
		exit(1);
	}
}


/* Apenas um decorativo. :3 */
static void _print_compass(int line, int current_direction)
{
	if (line > 12 || SIZE < 5)
		return; // Se o tamanho for menor que 5 o desenho não caberá ao lado do tabuleiro.

	/* Segundo essa resposta no Stack Overflow: <https://stackoverflow.com/a/6934050/15327515>
	definir uma constante dessa forma, usando a palavra-chave `static`, impedirá que esses dados
	sejam criados toda vez que a função for chamada. */

	static const char compass[][40] = {
		"|                  N                |",
		"",
		"|                 )|(               |",
		"|               )  |  (             |",
		"|             )    |    (           |",
		"|           )      |      (         |",
		"",
		"|           )      |      (         |",
		"|             )    |    (           |",
		"|               )  |  (             |",
		"|                 )|(               |",
		"",
		"|                  S                |",
	}; // Bússola impressa à direita do mapa.

	/* Conforme o jogador, fazemos uma modificação no desenho da bússola
	 - outra forma de indicar quem está jogando no momento. */
	switch (line) {
		case 1:
			printf("|                  %c                |",
				(current_direction == TOP_DOWN) ? 'A' : '|'); // Norte
			break;
		case 6:
			printf("|     O %c-)--------O--------(-%c L   |",
				(current_direction == LEFT_RIGHT) ? '<' : '-',  // Oeste
				(current_direction == RIGHT_LEFT) ? '>' : '-'); // Leste
			break;
		case 11:
			printf("|                  %c                |",
				(current_direction == DOWN_TOP) ? 'V' : '|'); // Sul
			break;
		default: printf("%s", compass[line]);
	}
}


/* Imprimimos uma faixa com a mensagem "turno" na horizontal para indicar quem está jogando no
momento.
// Poderia ter implementado essa função para uma mensagem genérica de tamanho variável, mas
// preferi evitar a fadiga. :v
*/
static void _hprint_turn_message(int use_message)
{
	static const int spaces_left = 3 * (SIZE / 5);
	/* [('espaço de cada coluna': 6 / 2 :'dividido pela metade' / (5 :'tamanho da mensagem - `\0`')]
	 */
	static const int spaces_right = 3 * (SIZE / 5);
	/* [('espaço de cada coluna': 6 / 2 :'dividido pela metade') / (5 :'tamanho da mensagem -
	`\0`')] */
	static const int leading_spaces = 2 * (SIZE % 5) + 3 * (SIZE % 5);
	char message[6];
	int i, j;
	strcpy(message, use_message ? "Turno" : "     ");

	/* Imprimimos a mensagem com o devido espaçamento. */
	for (i = 0; i < 5; ++i) {
		for (j = 0; j < spaces_left; ++j)
			printf(" ");

		printf("%c", message[i]);

		for (j = 0; j < spaces_right; ++j)
			printf(" ");
	}

	for (i = 0; i < leading_spaces; ++i)
		printf(" ");
}


/* Cria uma pilhas contendo dados que representam a linha de visão de um jogador em relação a uma
faixa do tabuleiro. */
static Stack *_get_line_view(Board board, int column)
{
	int i, j, current_size = 0;
	Stack *value = new_stack();
	MEMORY_ERROR(value)

	// Começamos a leitura da linha de baixo para cima (perpendicular ao jogador).
	for (i = LAST_ROOM; i >= 0; --i) {
		int size;

		if ((size = stack_size(board[column][i])) > current_size /*&& size != NULL_ERROR*/) {
			/* Como sabemos que `NULL_ERROR` é (e sempre será) um número negativo podemos
			remover a verfificação. */

			for (j = 0; j < (size - current_size); ++j)
				/* A _largura_ das edificações na visão de cada faixa será determinado pelo quão
				perto ela está do jogador. O coeficiente será então determinado pelo índice
				rotacionado da posição no tabuleiro perpendicular a ele. */
				stack_push(value, i);

			current_size = size;
		}
	}

	return value;
}


/* Retorna a quantidade de pontos de uma única faixa na linha de visão do jogador. */
static int _get_points_from_line_view(Board board, int column)
{
	int i, j, current_size = 0, points = 0, size;

	// Começamos a leitura da linha de baixo para cima (perpendicular ao jogador).
	for (i = LAST_ROOM; i >= 0; --i)

		if ((size = stack_size(board[column][i])) > current_size /*&& size != NULL_ERROR*/) {
			/* Como sabemos que `NULL_ERROR` é (e sempre será) um número negativo podemos
			remover a verfificação. */

			current_size = size;
			++points;
		}

	return points;
}


/* Retorna uma cópia do tabuleiro rotacionado na direção do jogador.
Exceto se a direção for `DOWN_TOP`, nesse caso apenas retornamos o mesmo. */
static Board _get_rotated_board(Board board, int from_player)
{
	if (from_player == DOWN_TOP)
		/* Já que as operações que faremos na cópia não interfeririam na estrutura da original,
		retornamos o mesmo tabuleiro, para otimização. */
		return board;

	int i, j;
	Board value = _create_board();
	MEMORY_ERROR(value)

	switch (from_player) {
		case TOP_DOWN: {
			for (i = 0; i < SIZE; ++i)
				for (j = 0; j < SIZE; ++j)
					value[LAST_ROOM - i][LAST_ROOM - j] = board[i][j];
		} break;
		case RIGHT_LEFT: {
			for (i = 0; i < SIZE; ++i)
				for (j = 0; j < SIZE; ++j)
					value[LAST_ROOM - j][i] = board[i][j];
		} break;
		// case DOWN_TOP: {
		// 	for (i = 0; i < SIZE; ++i)
		// 		for (j = 0; j < SIZE; ++j)
		// 			value[i][j] = board[i][j];
		// } break;
		case LEFT_RIGHT: {
			for (i = 0; i < SIZE; ++i)
				for (j = 0; j < SIZE; ++j)
					value[j][LAST_ROOM - i] = board[i][j];
		} break;
	}

	return value;
}


/* Libera um tabuleiro rotacionado da memória.
Corresponde a um `Board` que contém cópias das referências à pilhas de outro tabuleiro
(logo, essas referências não serão liberadas aqui). */
static void _free_rotated_board(Board board)
{
	int i;

	// Remove os vetores da matrix.
	for (i = 0; i < SIZE; ++i)
		free(board[i]);

	free(board); // Remove a matriz em si.
}


/* Funções "públicas". */

Board new_board()
{
	Board board = _create_board();
	_create_cell_stack(board, CENTER, CENTER, PARK);

	// Peças aleatórias são adicionadas no meio do tabuleiro no início do jogo.
	int n, i, j;
	for (n = 0; n < INITIAL_PIECES; ++n) {

		do {
			i = rand() % 2 + (CENTER - 1);
			j = rand() % 2 + (CENTER - 1);
		} while (_check_cell(board, i, j, PARK));

		_add_to_cell(board, i, j, APARTMENT);
	}

	return board;
}


// Removi os prints na função copiada abaixo para usar com o motor.
/*
int move(Board board, int column, int line, int piece)
{
	const char pieces_names[][12] = {"andar", "parque", "telhado"};
	int is_invalid = _is_move_invalid(board, column, line, piece);
	assert(is_invalid != INVALID_INPUT_ERROR);
	// Asseguramos, por asserção, que a *entrada* da função seja sempre uma peça válido.

	if (is_invalid) {
		printf("Não é possível mover a peça %s para a posição %c%d)!\n", pieces_names[(int)piece],
			(char)('A' + column), line + 1);

		switch (piece) {
			case APARTMENT:
				printf("Andares não podem ficar acima de parques ou telhados.\n");
				break;
			case PARK: printf("Parques só podem ser posicionados sobre o chão.\n"); break;
			case ROOF: printf("Telhados só podem ser posicionados acima de andares.\n"); break;
		}

		return 0;
	}
	else
		_add_to_cell(board, column, line, piece);

	return 1;
}
*/


int move(Board board, int column, int line, int piece)
{
	const char pieces_names[][12] = {"andar", "parque", "telhado"};
	int is_invalid = _is_move_invalid(board, column, line, piece);
	assert(is_invalid != INVALID_INPUT_ERROR);
	/* Asseguramos, por asserção, que a *entrada* da função seja sempre uma peça válida. */

	if (is_invalid)
		return 0;
	else
		_add_to_cell(board, column, line, piece);

	return 1;
}


void print_table_topdown(Board board, int current_player)
{
	INVALID_PLAYER_ERROR(current_player)
	assert(board != NULL);

	char turn_left[6], turn_right[6];
	int i, j, printed = 0;
	int is_current[4];
	int closing_pieces[] = {ROOF, PARK};

	// Armazenamos essas verificações por quê usaremos novamente no loop abaixo.
	for (i = 0; i < 4; ++i)
		is_current[i] = (current_player == i);

	strcpy(turn_left, is_current[LEFT_RIGHT] ? "Turno" : "     ");
	strcpy(turn_right, is_current[RIGHT_LEFT] ? "Turno" : "     ");
	printf("   ");
	_hprint_turn_message(is_current[TOP_DOWN]);
	printf("      ");
	/* Os espaços são determinados por 2x (espaço restante da linha + espaço da coluna de
	coordenadas + espaço da coluna com a mensagem de turno) */

	_print_compass(printed, current_player); // Imprimimos a linha do desenho da bússola.
	printf("\n");
	++printed;

	// Imprime o tabuleiro + o desenho da bússola, ao lado.
	for (i = 0; i < SIZE; ++i) { // Linha
		printf("   ");

		for (j = 0; j < SIZE; ++j)
			printf("+------"); // Linha de separação entre as casas do tabuleiro.

		printf("+     ");
		_print_compass(printed, current_player);
		printf("\n");

		++printed;
		/* Para não complicar demais a implementação, não fiz a faixa vertical ter tamanho adaptável
		como a horizontal. */
		printf("%c  ", (i < 5) ? turn_left[i] : ' ');

		for (j = 0; j < SIZE; ++j) { // Coluna
			char left, right;
			int size = stack_size(board[j][i]);

			if (size == NULL_ERROR) {
				printf("|      ");
				continue;
			}

			if (_check_cell_in(board, j, i, closing_pieces, 2)) {
				// Fechado
				left = '[';
				right = ']';
			}
			else {
				// Aberto
				left = right = ' ';
			}

			printf("| %c%.2d%c ", left, size, right);
		}

		printf("| %c", 'a' + i); // Faixa indicada as coordenadas do mapa na vertical.
		printf(" %c ", (i < 5) ? turn_right[i] : ' ');
		_print_compass(printed, current_player);
		printf("\n");
		++printed;
	}

	printf("   +");

	for (i = 0; i < SIZE; ++i)
		printf("------+");

	printf("     ");
	_print_compass(printed, current_player);
	printf("\n");
	++printed;
	printf("    ");

	// Faixa indicada as coordenadas do mapa na horizontal.
	for (i = 1; i <= SIZE; ++i)
		printf("   %d   ", i);

	printf("     ");
	_print_compass(printed, current_player);
	printf("\n");
	++printed;
	printf("    ");
	_hprint_turn_message(is_current[DOWN_TOP]);
	printf("\n");
	++printed;
}


void print_table_sideview(Board board, int from_player)
{
	INVALID_PLAYER_ERROR(from_player)
	assert(board != NULL);

	int i, j, k, higher = 0;
	int cached_sizes[SIZE];
	Stack *view_lines[SIZE];
	Board rotated_board = _get_rotated_board(board, from_player);

	// Processa dados das pilhas que usaremos para imprimir a visão do jogador.
	// Começamos a leitura da coluna da direita para a esquerda (perpendicular ao jogador).
	for (i = 0; i < SIZE; ++i) {
		view_lines[i] = _get_line_view(rotated_board, i);
		cached_sizes[i] = 0;

		if ((cached_sizes[i] = stack_size(view_lines[i])) > higher)
			higher = cached_sizes[i];
	}

	// Imprime a visão do jogador, um caractere por vez. Desempilhando as faixas de visão.
	for (i = higher; i > 0; --i) {

		for (j = 0; j < SIZE; ++j)

			if (cached_sizes[j] == i) {
				int width_modifier, spaces;
				assert(stack_get(view_lines[j], &width_modifier) == OK);
				assert(stack_pop(view_lines[j]) == OK);
				// Asseguramos, por asserção, que a pilha existe e que não é vazia.
				spaces = SIZE - width_modifier;
				/* Note que `SIZE - width_modifier` sempre deixará pelo menos um espaço em
				branco, que aproveitamos para dar espaçamento. */
				cached_sizes[j]--;
				/* Vinculamos os tamanhos das pilhas manualmente para evitar chamar o método de
				contagem de pilha, reduzindo custos. */

				/* Essa sequência de `for`s resulta na impressão da visão dos prédios.
				Linha-a-linha. */
				for (k = 0; k < spaces; ++k)
					printf(" ");

				for (k = 0; k < width_modifier; ++k)
					printf("█");

				printf("█");

				for (k = 0; k < width_modifier; ++k)
					printf("█");

				for (k = 0; k < spaces; ++k)
					printf(" ");
			}
			else {
				// Preenchem a linha de uma faixa cujo altura não alcança as outras pilhas.
				int spaces = SIZE * 2 + 1;

				for (k = 0; k < spaces; ++k)
					printf(" ");
			}

		printf("\n");
	}

	// Imprime o "piso".
	for (i = 0; i < SIZE; ++i) {
		printf(" ");

		for (j = 0; j < (SIZE * 2) - 1; ++j)
			printf("-");

		printf(" ");
	}

	printf("\n");

	if (from_player != DOWN_TOP)
		// Lembrando que não fazemos uma cópia quando "rotacionamos" para o jogador ao Sul.
		_free_rotated_board(rotated_board);
}


int get_points_from(Board board, int player)
{
	INVALID_PLAYER_ERROR(player)
	assert(board != NULL);

	int i, points = 0;
	Board rotated_board = _get_rotated_board(board, player);

	for (i = 0; i < SIZE; ++i)
		points += _get_points_from_line_view(rotated_board, i);

	/* //Outra forma, *depreciada*, (mais expensiva) de fazer a contagem de pontos. De forma similar à
	//`print_table_sideview`, criamos pilhas para cada faixa de visão e desempilhamos para processar
	//os dados.
	int j, k;
	Stack *view_lines[SIZE];
	for (i = 0; i < SIZE; ++i)
		view_lines[i] = _get_line_view(rotated_board, i);

	for (i = 0; i < SIZE; ++i) {
		int width, size = stack_size(view_lines[i]), last_width = 0;

		while (size > 0) {
			assert(stack_get(view_lines[i], &width) == OK);
			assert(stack_pop(view_lines[i]) == OK);
			// Asseguramos, por asserção, que a pilha existe e que não é vazia.
			--size;
			// Vinculamos os tamanhos das pilhas manualmente para evitar chamar o método de
			//contagem de pilha, reduzindo custos.

			if (width == last_width)
				continue;

			++points;
			last_width = width;
		}
	}
	*/

	if (player != DOWN_TOP)
		// Lembrando que não fazemos uma cópia quando "rotacionamos" para o jogador ao Sul.
		_free_rotated_board(rotated_board);

	return points;
}


void free_board(Board board)
{
    int i, j;

    // Remove os vetores da matrix.
    for (i = 0; i < SIZE; ++i) {

        // Libera as células da matriz.
        for (j = 0; j < SIZE; ++j) {
            empty_stack(board[i][j]); // Limpa a pilha
            free(board[i][j]); // Remove a pilha
        }

        free(board[i]);
    }

    free(board); // Remove a matriz em si.
}

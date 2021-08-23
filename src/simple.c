#include <gdnative_api_struct.gen.h>

#include "./Skyliners/g_main.h"
#include "./Skyliners/game_board.h"
// #include "./Skyliners/list.h"
#include "./Skyliners/stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Meus defines
#define PLAYERS 4 // Quantidade de jogadores.
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

#define IS_ON_BOUNDS(X, Y) (X >= 0 && X < SIZE) && (Y >= 0 && Y < SIZE)

// Enum com dados da API do motor.
const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;
// Notas pessoais: Por quê constantes?

static const int LAST_ROOM = SIZE - 1; // Última "casa" do tabuleiro.

// Construtor NativeScript
void *simple_constructor(godot_object *p_instance, void *p_method_data);

// Destrutor NativeScript
void simple_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);

godot_variant simple_get_data(godot_object *p_instance, void *p_method_data, void *p_user_data,
	int p_num_args, godot_variant **p_args);

// Minhas funções
/*
godot_variant simple_teste(godot_object *p_instance, void *p_method_data, void *p_user_data,
    int p_num_args, godot_variant **p_args);
*/

godot_variant simple_get_size_at(godot_object *p_instance, void *p_method_data, void *p_user_data,
	int p_num_args, godot_variant **p_args);

godot_variant simple_move_piece(godot_object *p_instance, void *p_method_data, void *p_user_data,
	int p_num_args, godot_variant **p_args);

godot_variant simple_get_points_from(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant simple_game_can_continue(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args);

// :D
godot_variant simple_get_winner(godot_object *p_instance, void *p_method_data, void *p_user_data,
	int p_num_args, godot_variant **p_args);

godot_variant simple_get_pieces_count(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args);

// godot_variant simple_is_inside_limits(godot_object *p_instance, void *p_method_data,
// 	void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant simple_get_rotated_coords(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args);

godot_variant simple_get_next_rotated_coords(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args);

/* Funções auxiliares */

static void _generate_godot_method(
	godot_variant (*method)(godot_object *, void *, void *, int, godot_variant **), void *p_handle,
	char *method_name)
{
	godot_instance_method method_data = {NULL, NULL, NULL};
	method_data.method = method;
	godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};
	nativescript_api->godot_nativescript_register_method(
		p_handle, "Simple", method_name, attributes, method_data);
}


static void _get_rotated_coords(int64_t *x, int64_t *y, int64_t player)
{
	switch (player) {
		case NORTH: {
			*x = LAST_ROOM - *x;
			*y = LAST_ROOM - *y;
		} break; // Sul
		case EAST: {
			uint64_t swap = *x;
			*x = *y;
			*y = LAST_ROOM - swap;
		} break; // Leste
		case WEST: {
			uint64_t swap = *x;
			*x = LAST_ROOM - *y;
			*y = swap;
		} break; // Oeste
	}
}


static void _get_next_rotated_coords(
	int64_t *x, int64_t *y, uint64_t next_x, uint64_t next_y, int64_t player)
{
	_get_rotated_coords(x, y, player);

	switch (player) {
		case NORTH: {
			*x -= next_x;
			*y -= next_y;
		} break; // Sul
		case EAST: {
			*x += next_y;
			*y -= next_x;
		} break; // Leste
		case SOUTH: {
			*x += next_x;
			*y += next_y;
		} break; // Norte
		case WEST: {
			*x -= next_y;
			*y += next_x;
		} break; // Oeste
	}
}


/* Função auxiliar que adiciona um par key-value a um dicionário da Godot. */
static void add_to_godot_dictionary(
	godot_dictionary *self, char *key_name, void *value, enum godot_variant_type type)
{
	godot_string key;
	godot_variant key_var;
	godot_variant value_var;

	api->godot_string_new(&key);
	api->godot_string_parse_utf8(&key, key_name);
	api->godot_variant_new_string(&key_var, &key);

	// Determina o correspondente valor daquela chave
	switch (type) {
		case GODOT_VARIANT_TYPE_BOOL:
			api->godot_variant_new_bool(&value_var, *((godot_bool *)(value)));
			break;
		case GODOT_VARIANT_TYPE_INT:
			api->godot_variant_new_int(&value_var, *((godot_int *)(value)));
			break;
	}

	api->godot_dictionary_set(self, &key_var, &value_var);
}


void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options)
{
	api = p_options->api_struct;

	// Now find our extensions.
	for (int i = 0; i < api->num_extensions; i++) {
		// Adicionamos mais extensões por meio desse `switch`
		switch (api->extensions[i]->type) {
			case GDNATIVE_EXT_NATIVESCRIPT: {
				nativescript_api = (godot_gdnative_ext_nativescript_api_struct *)api->extensions[i];
			}; break;
			default: break;
		}
	}
}


void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options)
{
	// Aqui faremos a limpeza quando o NativeScript for encerrado.
	api = NULL;
	nativescript_api = NULL;
}


void GDN_EXPORT godot_nativescript_init(void *p_handle)
{
	// Essa função torna os objetos da nossa biblioteca disponíveis para o motor.
	// Chamada na inicialização do NativeScript.

	godot_instance_create_func create = {NULL, NULL, NULL};
	create.create_func = &simple_constructor;

	godot_instance_destroy_func destroy = {NULL, NULL, NULL};
	destroy.destroy_func = &simple_destructor;

	// Dizemos ao motor que classes estão sendo implementadas por meio dessa função.
	/* Parâmetros:
	- 'handler' passado para a função atual, - nome da classe do nosso objeto,
	- tipo da classe, - descrições sobre o construtor, - descrições sobre o destrutor.
	*/ // Essa implementação é uma forma de "simular" herança.
	nativescript_api->godot_nativescript_register_class(
		p_handle, "Simple", "Reference", create, destroy);

	godot_instance_method get_data = {NULL, NULL, NULL};
	get_data.method = &simple_get_data;
	// Passamos o ponteiro da função que desejamos expôr em uma struct descritiva.
	// Os outros campos dessa struct permitem especificar dados de uso por método.

	godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};

	/* De forma similar ao método anterior, informamos ao motor que métodos a nossa 'classe' está
	implementando por meio dessa função.
	Parâmetros:
	- `handler` passado para a função atual, - nome da classe do nosso objeto, - nome da função,
	- configurações dos nossos atributos (veja: `godot_method_rpc_mode enum` em
	<godot_headers/nativescript/godot_nativescript.h>) para possíveis valores,
	- enum com a descrição da função que deverá ser chamar pelo motor.
	*/ // Devemos ter uma chamada para essa função para cada método que desejamos expôr.
	nativescript_api->godot_nativescript_register_method(
		p_handle, "Simple", "get_data", attributes, get_data);

	// Struct com informações sobre um método que será registrado no motor.
	struct method {
		godot_variant (*func)(godot_object *, void *, void *, int, godot_variant **);
		char *name;
	};

	const uint8_t LEN = 8;
	uint8_t i;

	// Vetor com os métodos que serão registrados.
	struct method methods[] = {
		// Verificar se está nos limites do mapa
		// {.func = &simple_is_inside_limits, .name = "is_inside_limits"},
		{.func = &simple_get_size_at, .name = "get_size_at"},           // Tamanho da pilha
		{.func = simple_move_piece, .name = "move_piece"},              // Mover peça
		{.func = &simple_get_points_from, .name = "get_player_points"}, // Pontos do jogador
		// Condição de final de jogo
		{.func = &simple_game_can_continue, .name = "can_continue_game"},
		{.func = &simple_get_winner, .name = "get_winner"},             // Verificação do vencedor
		{.func = &simple_get_pieces_count, .name = "get_pieces_count"}, // Contagem de peças
		// Converte as coordenadas conforme a visão dos jogadores
		{.func = &simple_get_rotated_coords, .name = "get_rotated_coords"},
		// Faz o memo que a função acima, considerando os próximos passos.
		{.func = &simple_get_next_rotated_coords, .name = "get_next_rotated_coords"},
	};

	for (i = 0; i < LEN; ++i)
		_generate_godot_method(methods[i].func, p_handle, methods[i].name);
}


// Armazena os dados membros de uma instância da nossa classe GDNative.
typedef struct user_data_struct {
	char data[256];
	int teste;
	Board board;
	List *players_pieces[PLAYERS];
} user_data_struct;


void *simple_constructor(godot_object *p_instance, void *p_method_data)
{
	// Alocamos memória para nossa estrutura e a preenchemos com alguns dados.
	user_data_struct *user_data = api->godot_alloc(sizeof(user_data_struct));
	// Usamos os métodos de gerenciamento de memória da Godot para rastrear esses dados.
	strcpy(user_data->data, "World from GDNative!");
	srand(time(0)); // 'Inicializa' o gerador de números aleatórios.

	// Meus inits
	user_data->board = new_board();
	init_players(&(user_data->players_pieces));

	return user_data;
}


void simple_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data)
{
	// Meus destructors
	user_data_struct *user_data = (user_data_struct *)p_user_data;
	free_board(user_data->board);
	free_players(&(user_data->players_pieces));

	// Limpamos os dados da nossa instância.
	api->godot_free(p_user_data);
}


godot_variant simple_get_data(godot_object *p_instance, void *p_method_data, void *p_user_data,
	int p_num_args, godot_variant **p_args)
{
	godot_string data;
	godot_variant ret;
	user_data_struct *user_data = (user_data_struct *)p_user_data;

	// Primeiro convertemos a nossa string para um tipo `String` da Godot...
	api->godot_string_new(&data);
	api->godot_string_parse_utf8(&data, user_data->data);
	api->godot_variant_new_string(&ret, &data);
	// e então convertemos para um tipo `Variant`.
	api->godot_string_destroy(&data);
	/* Note que destruir uma `String` desse modo apenas diminui a contagem de referências.
	O tipo `Variant` que foi criada a partir dessa string ainda contém uma referência para a
	string original (que foi alocada na heap). */
	// A 'variante' que retornaremos será destruída automaticamente pelo motor.

	return ret; // Dados são sempre retornados para o motor como 'variantes'.
}


// Minhas funções

/*
godot_variant simple_teste(godot_object *p_instance, void *p_method_data, void *p_user_data,
    int p_num_args, godot_variant **p_args)
{
    godot_string data;
    godot_variant ret;
    user_data_struct *user_data = (user_data_struct *)p_user_data;
    user_data->teste = teste();
    api->godot_variant_new_int(&ret, user_data->teste);

    return ret; // Dados são sempre retornados para o motor como 'variantes'.
}
*/


/* Retorna o tamanho da pilha nas coordenadas indicadas. */
godot_variant simple_get_size_at(godot_object *p_instance, void *p_method_data, void *p_user_data,
	int p_num_args, godot_variant **p_args)
{
	godot_variant ret;

	if (p_num_args != 3) {
		api->godot_variant_new_int(&ret, -1);
	}
	else {
		user_data_struct *user_data = (user_data_struct *)p_user_data;
		int64_t x = api->godot_variant_as_int(p_args[0]);
		int64_t y = api->godot_variant_as_int(p_args[1]);
		int64_t player = api->godot_variant_as_int(p_args[2]);
		int64_t out;
		_get_rotated_coords(&x, &y, player);
		out = IS_ON_BOUNDS(x, y) ? stack_size((user_data->board)[x][y]) : -1;

		api->godot_variant_new_int(&ret, out);
	}

	return ret;
}


godot_variant simple_get_next_rotated_coords(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant ret;
	godot_dictionary fetched_data;
	// Dicionário que retornará os valores processados. Se retornar vazio indica falha na operação.
	api->godot_dictionary_new(&fetched_data);

	if (p_num_args == 5) {
		user_data_struct *user_data = (user_data_struct *)p_user_data;
		int64_t x = api->godot_variant_as_int(p_args[0]);
		int64_t y = api->godot_variant_as_int(p_args[1]);
		int64_t next_x = api->godot_variant_as_int(p_args[2]);
		int64_t next_y = api->godot_variant_as_int(p_args[3]);
		int64_t player = api->godot_variant_as_int(p_args[4]);
		godot_string key;
		godot_variant key_var;
		godot_variant value;

		// Realiza as operações necessárias.
		_get_next_rotated_coords(&x, &y, next_x, next_y, player);

		// Adiciona uma chave no dicionário de retorno.
		api->godot_string_new(&key);
		api->godot_string_parse_utf8(&key, "is_on_bounds");
		api->godot_variant_new_string(&key_var, &key);
		api->godot_variant_new_bool(&value, IS_ON_BOUNDS(x, y)); // Valor da chave `is_on_bounds`
		api->godot_dictionary_set(&fetched_data, &key_var, &value);

		// api->godot_string_new(&key);
		// api->godot_string_parse_utf8(&key, "x");
		// api->godot_variant_new_string(&key)
		add_to_godot_dictionary(&fetched_data, "x", &x, GODOT_VARIANT_TYPE_INT);
		add_to_godot_dictionary(&fetched_data, "y", &y, GODOT_VARIANT_TYPE_INT);
	}
	api->godot_variant_new_dictionary(&ret, &fetched_data);

	return ret;
}


godot_variant simple_get_rotated_coords(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant ret;
	godot_dictionary fetched_data;
	// Dicionário que retornará os valores processados. Se retornar vazio indica falha na operação.
	api->godot_dictionary_new(&fetched_data);

	if (p_num_args == 3) {
		user_data_struct *user_data = (user_data_struct *)p_user_data;
		int64_t x = api->godot_variant_as_int(p_args[0]);
		int64_t y = api->godot_variant_as_int(p_args[1]);
		int64_t player = api->godot_variant_as_int(p_args[2]);
		godot_string key;
		godot_variant key_var;
		godot_variant value;

		// Realiza as operações necessárias.
		_get_rotated_coords(&x, &y, player);

		add_to_godot_dictionary(&fetched_data, "x", &x, GODOT_VARIANT_TYPE_INT);
		add_to_godot_dictionary(&fetched_data, "y", &y, GODOT_VARIANT_TYPE_INT);
	}
	api->godot_variant_new_dictionary(&ret, &fetched_data);

	return ret;
}


/* Move uma peça para as coordenadas indicadas. */
godot_variant simple_move_piece(godot_object *p_instance, void *p_method_data, void *p_user_data,
	int p_num_args, godot_variant **p_args)
{
	godot_variant ret;
	int out = -1;

	if (p_num_args == 4) {
		user_data_struct *user_data = (user_data_struct *)p_user_data;
		int64_t x = api->godot_variant_as_int(p_args[0]);
		int64_t y = api->godot_variant_as_int(p_args[1]);
		int64_t piece = api->godot_variant_as_int(p_args[2]);
		int64_t player = api->godot_variant_as_int(p_args[3]);
		_get_rotated_coords(&x, &y, player);

		if (IS_ON_BOUNDS(x, y) && piece >= 0 && piece < 4 &&
			(list_count(user_data->players_pieces[player], (enum Piece)piece) > 0)) {

			if (out = move(user_data->board, x, y, piece))
				list_remove(user_data->players_pieces[player], (enum Piece)piece);
		}
	}
	api->godot_variant_new_int(&ret, out);

	return ret;
}


/* Retorna a pontuação de um jogador indicado. */
godot_variant simple_get_points_from(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant ret;

	if (p_num_args != 1) {
		api->godot_variant_new_int(&ret, -1);
	}
	else {
		user_data_struct *user_data = (user_data_struct *)p_user_data;
		int player = api->godot_variant_as_int(p_args[0]);
		int out = (player >= 0 && player < 4) ? get_points_from(user_data->board, player) : -1;

		api->godot_variant_new_int(&ret, out);
	}

	return ret;
}


/* Verifica se a condição de término do jogo foi atingida. */
godot_variant simple_game_can_continue(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant ret;
	user_data_struct *user_data = (user_data_struct *)p_user_data;
	api->godot_variant_new_bool(&ret, g_can_continue(user_data->players_pieces));

	return ret;
}


/* Retorna o vencedor do jogo. */
godot_variant simple_get_winner(godot_object *p_instance, void *p_method_data, void *p_user_data,
	int p_num_args, godot_variant **p_args)
{
	godot_variant ret;
	int64_t out = -1;

	if (p_num_args == 1) {
		user_data_struct *user_data = (user_data_struct *)p_user_data;
		int first_player = api->godot_variant_as_int(p_args[0]);

		if (first_player >= 0 && first_player < 4)
			out = get_winner(first_player, user_data->board);
	}
	api->godot_variant_new_int(&ret, out);

	return ret;
}


/* Retorna a quantidade de peças do jogador indicado. */
godot_variant simple_get_pieces_count(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant ret;
	int64_t out = -1;

	if (p_num_args == 2) {
		user_data_struct *user_data = (user_data_struct *)p_user_data;
		int piece = api->godot_variant_as_int(p_args[0]);
		int player = api->godot_variant_as_int(p_args[1]);

		if (piece >= 0 && piece < 3 && player >= 0 && player < 4)
			out = list_count(user_data->players_pieces[player], (enum Piece)piece);
	}
	api->godot_variant_new_int(&ret, out);

	return ret;
}

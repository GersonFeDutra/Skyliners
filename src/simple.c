#include <gdnative_api_struct.gen.h>

#include "./Skyliners/g_main.h"
#include "./Skyliners/game_board.h"
// #include "./Skyliners/list.h"
#include "./Skyliners/stack.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Meus defines
#define PLAYERS 4 // Quantidade de jogadores.

// Enum com dados da API do motor.
const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;
// Notas pessoais: Por quê constantes?


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


	// Minhas funções:

	// Teste
	/*
	godot_instance_method list_teste = {NULL, NULL, NULL};
	list_teste.method = &simple_teste;
	godot_method_attributes teste_atributes = {GODOT_METHOD_RPC_MODE_DISABLED};
	nativescript_api->godot_nativescript_register_method(
	    p_handle, "Simple", "test", teste_atributes, list_teste);
	*/

	// Tamanho da pilha
	godot_instance_method size_at = {NULL, NULL, NULL};
	size_at.method = &simple_get_size_at;
	godot_method_attributes get_size_at_atributes = {GODOT_METHOD_RPC_MODE_DISABLED};
	nativescript_api->godot_nativescript_register_method(
		p_handle, "Simple", "get_size_at", get_size_at_atributes, size_at);

	// Mover peça
	godot_instance_method move_piece = {NULL, NULL, NULL};
	move_piece.method = &simple_move_piece;
	godot_method_attributes move_piece_atributes = {GODOT_METHOD_RPC_MODE_DISABLED};
	nativescript_api->godot_nativescript_register_method(
		p_handle, "Simple", "move_piece", move_piece_atributes, move_piece);

	// Pontos do jogador
	godot_instance_method get_player_points = {NULL, NULL, NULL};
	get_player_points.method = &simple_get_points_from;
	godot_method_attributes get_player_points_atributes = {GODOT_METHOD_RPC_MODE_DISABLED};
	nativescript_api->godot_nativescript_register_method(
		p_handle, "Simple", "get_player_points", get_player_points_atributes, get_player_points);

	// Condição de final de jogo
	godot_instance_method can_continue_game = {NULL, NULL, NULL};
	can_continue_game.method = &simple_game_can_continue;
	godot_method_attributes can_continue_atributes = {GODOT_METHOD_RPC_MODE_DISABLED};
	nativescript_api->godot_nativescript_register_method(
		p_handle, "Simple", "can_continue_game", can_continue_atributes, can_continue_game);

	// Verificação do vencedor
	godot_instance_method get_winner = {NULL, NULL, NULL};
	get_winner.method = &simple_get_winner;
	godot_method_attributes get_winner_atributes = {GODOT_METHOD_RPC_MODE_DISABLED};
	nativescript_api->godot_nativescript_register_method(
		p_handle, "Simple", "get_winner", get_winner_atributes, get_winner);

	// Verificação do vencedor
	godot_instance_method get_pieces_count = {NULL, NULL, NULL};
	get_pieces_count.method = &simple_get_pieces_count;
	godot_method_attributes get_pieces_count_atributes = {GODOT_METHOD_RPC_MODE_DISABLED};
	nativescript_api->godot_nativescript_register_method(
		p_handle, "Simple", "get_pieces_count", get_pieces_count_atributes, get_pieces_count);
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

godot_variant simple_get_size_at(godot_object *p_instance, void *p_method_data, void *p_user_data,
	int p_num_args, godot_variant **p_args)
{
	godot_variant ret;

	if (p_num_args != 2) {
		api->godot_variant_new_int(&ret, -1);
	}
	else {
		user_data_struct *user_data = (user_data_struct *)p_user_data;
		int i = api->godot_variant_as_int(p_args[0]);
		int j = api->godot_variant_as_int(p_args[1]);
		int out = (i >= 0 && i < 5) ? stack_size((user_data->board)[i][j]) : -1;

		api->godot_variant_new_int(&ret, out);
	}

	return ret;
}


godot_variant simple_move_piece(godot_object *p_instance, void *p_method_data, void *p_user_data,
	int p_num_args, godot_variant **p_args)
{
	godot_variant ret;
	int out = -1;

	if (p_num_args == 4) {
		user_data_struct *user_data = (user_data_struct *)p_user_data;
		int i = api->godot_variant_as_int(p_args[0]);
		int j = api->godot_variant_as_int(p_args[1]);
		int piece = api->godot_variant_as_int(p_args[2]);
		int player = api->godot_variant_as_int(p_args[3]);

		if (i >= 0 && i < 5 && piece >= 0 && piece < 4 &&
			(list_count(user_data->players_pieces[player], (enum Piece)piece) > 0)) {

			if (out = move(user_data->board, i, j, piece))
				list_remove(user_data->players_pieces[player], (enum Piece)piece);
		}
	}
	api->godot_variant_new_int(&ret, out);

	return ret;
}


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


godot_variant simple_game_can_continue(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant ret;
	user_data_struct *user_data = (user_data_struct *)p_user_data;
	api->godot_variant_new_bool(&ret, g_can_continue(user_data->players_pieces));

	return ret;
}


godot_variant simple_get_winner(godot_object *p_instance, void *p_method_data, void *p_user_data,
	int p_num_args, godot_variant **p_args)
{
	godot_variant ret;
	int out = -1;

	if (p_num_args == 1) {
		user_data_struct *user_data = (user_data_struct *)p_user_data;
		int first_player = api->godot_variant_as_int(p_args[0]);

		if (first_player >= 0 && first_player < 4)
			out = get_winner(first_player, user_data->board);
	}
	api->godot_variant_new_int(&ret, out);

	return ret;
}


godot_variant simple_get_pieces_count(godot_object *p_instance, void *p_method_data,
	void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant ret;
	int out = -1;

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


// // Pega um nó da árvore da cena em tempo de execução
// godot_variant pegar_no(const char *tipo, const godot_variant *caminho)
// {
// 	godot_variant no_recebido;
// 	godot_method_bind *metodo_get_node = api->godot_method_bind_get_method(tipo, "get_node");
// 	godot_variant caminho_no;
// 	godot_variant *argumentos[1];
// 	godot_variant_call_error error;

// 	api->godot_variant_new_node_path(&caminho_no, &caminho);
// 	argumentos[0] = &caminho_no;
// 	godot_variant ret =
// 		api->godot_method_bind_call(metodo_get_node, no_recebido, argumentos, 1, &error);

// 	return no_recebido;
// }

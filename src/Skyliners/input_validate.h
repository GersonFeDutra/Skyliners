/* Persiste na captura e verificação de uma entrada inteira dentro de um determinado intervalo
(inclusiva). */
int input_integer_range(int min, int max, char *message, char *invalid_input_message,
	char *empty_input_message, char *out_of_range_message);

/* Faz a validação persistente de uma entrada de coordenadas no formato 'Número''Valor'. Ex.: A1.
_Apresentando mensagens no terminal e retornando uma entrada válida._
As coordenadas máximas são passadas de forma exclusiva. */
void input_alnum_coords(
	int *alpha, int *num, int to_hcoord, int to_vcoord, char *message, char *invalid_input_message);

/* O restante das funções foram enviadas para um .c para encapsular informações desnecessários às
outras funções. Métodos que não foram usados foram comentadas também nesse arquivo. */

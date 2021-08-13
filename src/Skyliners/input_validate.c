#include "input_validate.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0
#define EMPTY_ERROR -2
#define INVALID_ERROR -1

//#define INT_MAX_DIGITS 21 // Número máximo de dígitos de um inteiro em 64bits + '\0'.
#define INT_MAX_DIGITS 9 // Número máximo de dígitos de um inteiro + '\0'.

/* Faz a validação persistente de uma entrada inteira, apresentando mensagens no terminal.
Retorno: EMPTY_ERROR, INVALID_ERROR, TRUE. */
static int validate_integer(int *value)
{
	// int input, check;
	// check = scanf("%d", &input);
	char input[INT_MAX_DIGITS] = {'\0'};
	int output, i = 0;

	// Trim spaces.
	while ((input[0] = (char)getchar()) == ' ' || input[0] == '\t') {
	}

	// Captura o sinal.
	if (input[0] == '-' || input[0] == '+') {
		++i;
		// Trim spaces, após o sinal.
		while ((input[i] = (char)getchar()) == ' ' || input[i] == '\t') {
		}
	}

	if (input[i] == '\n')
		return EMPTY_ERROR;

	// No final, `fgets` só me atrapalhou :v
	// fgets(&input[1], INT_MAX_DIGITS - 1, stdin);

	// Valida os caracteres entrados, até encontrar '\n' ou atingir o limite do inteiro.
	int consume_buffer = FALSE;
	int out_of_bounds = FALSE;
	int is_invalid = FALSE;

	do {
		if (consume_buffer)
			continue;

		if (!isdigit(input[i])) {

			if (input[i] == ' ' || input[i] == '\t') { // Ignora tudo após o espaço.
				consume_buffer = TRUE;
				continue;
			}
			else {
				consume_buffer = is_invalid = TRUE;
				continue;
			}
		}
		// Se o último elemento foi analisado.
		if (!(i < INT_MAX_DIGITS)) {
			consume_buffer = out_of_bounds = TRUE;
			continue;
		}

		++i;
	} while ((input[i] = (char)getchar()) != '\n');

	if (is_invalid)
		return INVALID_ERROR;

	// Converte a string para inteiro.
	char *buff;
	input[i] = '\0';
	output = (int)strtol(input, &buff, 10);
	*value = output;

	if (out_of_bounds)
		printf("O valor entrado está além dos limites %d, será usado.\n", *value);

	return TRUE;
}


static int input_integer(char *message, char *invalid_input_message, char *empty_input_message)
{
	int value, check;
	printf("%s ", message);

	while ((check = validate_integer(&value)) != TRUE)

		switch (check) {
			case EMPTY_ERROR: printf("%s ", empty_input_message); break;
			case INVALID_ERROR: printf("%s ", invalid_input_message); break;
		}

	return value;
}


int input_integer_range(int min, int max, char *message, char *invalid_input_message,
    char *empty_input_message, char *out_of_range_message)
{
    int value = input_integer(message, invalid_input_message, empty_input_message);

    while (value < min || value > max)
        value = input_integer(out_of_range_message, invalid_input_message, empty_input_message);

    return value;
}


void input_alnum_coords(
	int *alpha, int *num, int to_hcoord, int to_vcoord, char *message, char *invalid_input_message)
{
	char scan;

	while (1) {
		printf("%s ", message);
		scanf("%c%d", &scan, num);
		while (getchar() != '\n') {
		}

		if (scan >= 'A' && scan < (char)('A' + to_vcoord))
			*alpha = scan - 'A';

		else if (scan >= 'a' && scan < (char)('a' + to_vcoord))
			*alpha = scan - 'a';

		else {
			printf("%s", invalid_input_message);
			continue;
		}

		*num -= 1;

		if (*num >= 0 && *num < to_hcoord)
			return;

		printf("%s ", invalid_input_message);
	}
}


// Depreciado: funções que não usei nessa aplicação.


/* // Faz a validação persistente de uma entrada inteira com um tamanho mínimo,
// apresentando mensagens no terminal. Retorna uma entrada válida.
int input_min_integer(int min, char *message, char *invalid_input_message,
	char *empty_input_message, char *out_of_range_message)
{
	int value = input_integer(message, invalid_input_message, empty_input_message);

	while (value < min)
		value = input_integer(out_of_range_message, invalid_input_message, empty_input_message);

	return value;
}
*/

/* //Verifica uma entrada booleana com escolhas baseadas em caracteres.
//Retorno: INVALID_ERROR, TRUE, FALSE.
int validate_boolean_choice(char *options_true, char *options_false)
{
    int i = 0;
    char input;

    scanf(" %c*", &input);
    while (getchar() != '\n') {
    } // Consome a ~maldita~ quebra de linha deixada no buffer pelo `scanf`

    if (input == '\n')
        return EMPTY_ERROR;

    while (options_true[i] != '\0') {

        if (input == options_true[i])
            return TRUE;

        ++i;
    }

    i = 0;

    while (options_false[i] != '\0') {

        if (input == options_false[i])
            return FALSE;

        ++i;
    }

    return INVALID_ERROR;
}
*/


/* //Persiste na captura e verificação de uma entrada booleana baseada em escolhas dentro de um
//determinado conjunto de caracteres. Retorna um booleano
int input_boolean_choice(char *message, char *options_true, char *options_false,
    char *invalid_input_message, char *empty_input_message)
{
    int value;
    printf("%s ", message);

    while (TRUE) {
        value = validate_boolean_choice(options_true, options_false);

        switch (value) {
            case EMPTY_ERROR: printf("%s: ", empty_input_message); break;
            case INVALID_ERROR: printf("%s: ", invalid_input_message); break;
            default: return value;
        }
    }
}
*/

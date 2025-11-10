#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <lexer.h>
#include <string.h>
#include <tokens.h>

//Variáveis Globais
char lexeme[MAXIDLEN + 1];
int lineno = 1;
int columno = 0;

/* Versão extendida de identificador Pascal
 * 
 * Verifica se o lexeme é um identificador (ID)
 * ID = [A-Za-z][A-Za-z0-9]*
 *
 */
int isID(FILE *tape)
{
	if ( isalpha(lexeme[0] = getc(tape)) ) {
		columno++; // Primeira letra
		int i = 1;
		while ( isalnum( lexeme[i] = getc(tape) ) ) {
			columno++;
			i++;
		}
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		if(strcmp(lexeme, "exit") == 0){
			return EXIT;
		}
		if(strcmp(lexeme, "quit") == 0){
			return QUIT;
		}
		return ID;
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

/*
 * DEC = [1-9][0-9]* | '0'
 *                           ------------------------------------------
 *                          |                      digit               |
 *                          |                    --------              |
 *                          |                   |        |             |
 *               digit      |     not zero      V        |  epsilon    V
 * -->(is DEC)--------->(is ZERO)---------->(isdigit)-------------->((DEC))
 *       |
 *       | epsilon
 *       |
 *       V
 *     ((0))
 */
 /*
 * Verifica se o lexeme é um número decimal
 * DEC = [1-9][0-9]* | 0
 */
int isDEC(FILE *tape)
{
	if ( isdigit(lexeme[0] = getc(tape)) ) {
		columno++; // Primeiro dígito
		if (lexeme[0] == '0') {
			lexeme[1] = 0;
			return DEC;
		}
		int i = 1;
		while ( isdigit(lexeme[i] = getc(tape)) ) {
			columno++;
			i++;
		}
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return DEC;
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

/*
 * Função que determina se o lexeme contém uma notação exponencial (EE)
 * EE = [eE]['+''-']?[0-9][0-9]*
*/
int isEE(FILE *tape)
{
	int i = strlen(lexeme);
    if (toupper(lexeme[i] = getc(tape)) == 'E' )
    {
        columno++; // 'E' ou 'e'
        i++;
        // Checagem de sinal opcional
        int hassign = 0;
        if ((lexeme[i] = getc(tape)) == '+' || lexeme[i] == '-') 
		{
            columno++; // sinal
            i++;
            hassign = 1;
        } else {
            ungetc(lexeme[i], tape);
        }
        // Checagem do digito obrigatorio subsequente
        if (isdigit(lexeme[i] = getc(tape))) 
		{
            columno++; // primeiro dígito
            i++;
            while ( isdigit(lexeme[i] = getc(tape))) {
                columno++;
                i++;
            }
            ungetc(lexeme[i], tape);
            lexeme[i] = 0;
            return FLT;
        }
        // Rollback se não houver dígito
        ungetc(lexeme[i], tape);
        i--;
        if(hassign)
		{
            ungetc(lexeme[i], tape);
            columno--; // desfaz contagem do sinal
            i--;
        }
		ungetc(lexeme[i], tape);
		columno--; // desfaz contagem do 'E'
		lexeme[i] = 0;
		return 0;
    }
    ungetc(lexeme[i], tape);
    lexeme[i] = 0;
    return 0;
}

/*
 * Função que determina se o lexeme é um numero
 * podendo ser inteiro ou ponto flutuante 
*/
int isNUM(FILE *tape)
{
	int token = isDEC(tape);
	if (token == DEC) {
		int i = strlen(lexeme);
		if ( (lexeme[i] = getc(tape)) == '.' ) {
			columno++; // '.'
			i++;
			if ( isdigit( lexeme[i] = getc(tape) ) ) {
				columno++; // primeiro dígito após '.'
				i++;
				while ( isdigit( lexeme[i] = getc(tape) ) ) {
					columno++;
					i++;
				}
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
				token = FLT;
			} else {
				// Caso seja apenas DEC seguido de '.' sem dígitos
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
				token = FLT;
			}
		} else {
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
		}
	} else {
		// Tenta reconhecer números começando com '.'
		if ( (lexeme[0] = getc(tape)) == '.' ) {
			if ( isdigit( lexeme[1] = getc(tape) ) ) {
				token = FLT;
				columno += 2; // '.' e primeiro dígito
				int i = 2;
				while ( isdigit( lexeme[i] = getc(tape) ) ) {
					columno++;
					i++;
				}
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
			} else {
				ungetc(lexeme[1], tape);
				ungetc(lexeme[0], tape);
				lexeme[0] = 0;
				return 0; // not a number
			}
		} else {
			ungetc(lexeme[0], tape);
			lexeme[0] = 0;
			return 0; // not a number
		}
	}
	
	if (isEE(tape)) {
		token = FLT;
	}
	return token;
}

int isASGN(FILE *tape) {
	lexeme[0] = getc(tape);
	if(lexeme[0] == ':') 
	{
		lexeme[1] = getc(tape);
		if (lexeme[1] == '=') 
		{
			lexeme[2] = 0;
			columno += 2; // ':' e '='
			return ASGN;
		}
		ungetc(lexeme[1], tape);
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

// Skip spaces
void skipspaces(FILE *tape)
{
	int head;
	while ( isspace(head = getc(tape)) ){
		if(head == '\n'){
			lineno++;
			columno = -1; // Para ignorar o \n do buffer
			break;
		}
		columno++; 
	}
	ungetc(head, tape);
}

// Ignora input de setas
// Arrow = '^' '[' '[' 'A' | 'B' | 'C' | 'D' 
void skipArrow(FILE *tape)
{
	while ((lexeme[0] = getc(tape)) == 0x1B) { // ESC (27)
		lexeme[1] = getc(tape);
		lexeme[2] = getc(tape);
		if (lexeme[1] == '[' && 
			(lexeme[2] == 'A' || lexeme[2] == 'B' || lexeme[2] == 'C' || lexeme[2] == 'D')) {
			columno += 3;
		} else {
			// Não é uma seta, devolve o que foi lido
			ungetc(lexeme[2], tape);
			ungetc(lexeme[1], tape);
			ungetc(lexeme[0], tape);
			lexeme[0] = 0;
			break;
		}
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
}

int gettoken(FILE *source)
{
	int token;
	skipspaces(source);
	skipArrow(source);
	if ( (token = isID(source)) ) return token;
	if ( (token = isNUM(source)) ) return token;
	if ( (token = isASGN(source)) ) return token;
	lexeme[0] = token = getc(source);
	lexeme[1] = 0;
	columno++; // token ASCII (operadores como +, -, *, /, =, etc.)
	// return an ASCII token
	return token;
}
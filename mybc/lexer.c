/****************************************************/
/*
Módulo: Analisador Léxico
----------------------------------------------------------
Descrição geral:
Sua função é ler o código-fonte caractere por caractere e identificar
os **tokens** (símbolos válidos) que serão enviados ao parser
(analisador sintático).

Cada função aqui reconhece um tipo específico de lexema:
 - Identificadores (variáveis e nomes)
 - Números (inteiros e reais)
 - Operadores (atribuição ":=")
 - Espaços e setas (para ignorar entrada inválida)p
 
O analisador léxico é o primeiro estágio da compilação.
Ele abstrai o texto em elementos significativos para o parser LL(1).
*/
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <lexer.h>
#include <string.h>
#include <tokens.h>

/****************************************************
 * Variáveis globais
 * --------------------------------------------------
 * lexeme[] : armazena o texto do token reconhecido.
 * lineno   : linha atual no arquivo de entrada.
 * columno  : coluna atual (posição do último caractere lido).
 ****************************************************/
char lexeme[MAXIDLEN + 1];
int lineno = 1;
int columno = 0;

/****************************************************
 * isID
 * --------------------------------------------------
 * Verifica se o lexema é um **identificador (ID)**.
 * Padrão Pascal: [A-Za-z][A-Za-z0-9]*
 *
 * A função também identifica as palavras "exit" e "quit",
 * que são tokens especiais para encerrar o interpretador.
 ****************************************************/
int isID(FILE *tape)
{
	if ( isalpha(lexeme[0] = getc(tape)) ) {
		columno++; // primeira letra
		int i = 1;
		while ( isalnum( lexeme[i] = getc(tape) ) ) {
			columno++;
			i++;
		}
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;

		// Palavras reservadas de saída
		if(strcmp(lexeme, "exit") == 0){
			return EXIT;
		}
		if(strcmp(lexeme, "quit") == 0){
			return QUIT;
		}
		return ID; // identificador comum
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

/****************************************************
 * isDEC
 * --------------------------------------------------
 * Verifica se o lexema é um número inteiro decimal.
 * Formato:
 *    DEC = [1-9][0-9]* | 0
 * Retorna o token DEC caso o padrão seja reconhecido.
 ****************************************************/
int isDEC(FILE *tape)
{
	if ( isdigit(lexeme[0] = getc(tape)) ) {
		columno++; // primeiro dígito
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

/****************************************************
 * isEE
 * --------------------------------------------------
 * Verifica se há notação científica (exponencial) após um número.
 * Formato:
 *   EE = [eE]['+''-']?[0-9][0-9]*
 * Exemplo válido: 1.5e+10
 * Caso o padrão seja reconhecido, retorna FLT (float).
 ****************************************************/
int isEE(FILE *tape)
{
	int i = strlen(lexeme);
    if (toupper(lexeme[i] = getc(tape)) == 'E' )
    {
        columno++; // 'E' ou 'e'
        i++;
        int hassign = 0;
        // sinal opcional
        if ((lexeme[i] = getc(tape)) == '+' || lexeme[i] == '-') 
		{
            columno++;
            i++;
            hassign = 1;
        } else {
            ungetc(lexeme[i], tape);
        }
        // dígito obrigatório
        if (isdigit(lexeme[i] = getc(tape))) 
		{
            columno++;
            i++;
            while ( isdigit(lexeme[i] = getc(tape))) {
                columno++;
                i++;
            }
            ungetc(lexeme[i], tape);
            lexeme[i] = 0;
            return FLT;
        }
        // rollback em caso de erro
        ungetc(lexeme[i], tape);
        i--;
        if(hassign)
		{
            ungetc(lexeme[i], tape);
            columno--;
            i--;
        }
		ungetc(lexeme[i], tape);
		columno--;
		lexeme[i] = 0;
		return 0;
    }
    ungetc(lexeme[i], tape);
    lexeme[i] = 0;
    return 0;
}

/****************************************************
 * isNUM
 * --------------------------------------------------
 * Reconhece números inteiros (DEC) ou reais (FLT),
 * incluindo aqueles com parte fracionária e/ou notação exponencial.
 * Exemplo:
 *  - 12         → DEC
 *  - 3.14       → FLT
 *  - 0.5e-3     → FLT
 ****************************************************/
int isNUM(FILE *tape)
{
	int token = isDEC(tape);
	if (token == DEC) {
		int i = strlen(lexeme);
		if ( (lexeme[i] = getc(tape)) == '.' ) {
			columno++; // '.'
			i++;
			if ( isdigit( lexeme[i] = getc(tape) ) ) {
				columno++;
				i++;
				while ( isdigit( lexeme[i] = getc(tape) ) ) {
					columno++;
					i++;
				}
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
				token = FLT;
			} else {
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
				token = FLT;
			}
		} else {
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
		}
	} else {
		// números começando com ponto, ex: .25
		if ( (lexeme[0] = getc(tape)) == '.' ) {
			if ( isdigit( lexeme[1] = getc(tape) ) ) {
				token = FLT;
				columno += 2;
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
				return 0;
			}
		} else {
			ungetc(lexeme[0], tape);
			lexeme[0] = 0;
			return 0;
		}
	}
	
	// Verifica se possui parte exponencial
	if (isEE(tape)) {
		token = FLT;
	}
	return token;
}

/****************************************************
 * isASGN
 * --------------------------------------------------
 * Reconhece o operador de atribuição ":=".
 * Retorna o token ASGN caso o padrão seja identificado.
 ****************************************************/
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

/****************************************************
 * skipspaces
 * --------------------------------------------------
 * Ignora espaços em branco e quebras de linha.
 * Atualiza o número de linha e coluna conforme necessário.
 ****************************************************/
void skipspaces(FILE *tape)
{
	int head;
	while ( isspace(head = getc(tape)) ){
		if(head == '\n'){
			lineno++;
			columno = -1; // ajusta para o próximo caractere
			break;
		}
		columno++; 
	}
	ungetc(head, tape);
}

/****************************************************
 * skipArrow
 * --------------------------------------------------
 * Ignora sequências de escape associadas às setas do teclado.
 * Isso evita que as teclas ←, →, ↑, ↓ corrompam o buffer stdin.
 ****************************************************/
void skipArrow(FILE *tape)
{
	while ((lexeme[0] = getc(tape)) == 0x1B) { // ESC (27)
		lexeme[1] = getc(tape);
		lexeme[2] = getc(tape);
		if (lexeme[1] == '[' && 
			(lexeme[2] == 'A' || lexeme[2] == 'B' || lexeme[2] == 'C' || lexeme[2] == 'D')) {
			columno += 3;
		} else {
			// não é uma seta, devolve os caracteres lidos
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

/****************************************************
 * gettoken
 * --------------------------------------------------
 * Função principal do analisador léxico.
 * Lê o próximo token válido do arquivo de entrada (source).
 *
 * Ordem de verificação:
 *  1. Ignora espaços e setas.
 *  2. Tenta reconhecer identificadores.
 *  3. Tenta reconhecer números (inteiros/reais).
 *  4. Tenta reconhecer operador ":=".
 *  5. Caso contrário, retorna o caractere ASCII como token.
 ****************************************************/
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
	columno++; // token ASCII simples
	return token;
}

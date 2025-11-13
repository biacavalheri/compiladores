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
	// Lê o primeiro caractere e verifica se é letra
	if ( isalpha(lexeme[0] = getc(tape)) ) {
		int i = 1;

		// Continua lendo enquanto forem letras ou dígitos
		while ( isalnum( lexeme[i] = getc(tape) ) ) {
			i++;
		}

		// O último caractere lido NÃO faz parte do identificador.
        // Devolvemos para a entrada para que outro token o processe
		ungetc(lexeme[i], tape);

		// Finaliza o identificador com terminador nulo '\0'
        // para transformar lexeme[] em uma string válida de C.
		lexeme[i] = 0;

		// Caso o identificador seja exatamente "exit", retorna o token EXIT.
		if(strcmp(lexeme, "exit") == 0){
			return EXIT;
		}

		// Caso seja "quit", retorna o token QUIT.
		if(strcmp(lexeme, "quit") == 0){
			return QUIT;
		}

		// Qualquer outro texto válido é um identificador comum.
		return ID; 
	}

	ungetc(lexeme[0], tape); // devolve o caractere lido
	lexeme[0] = 0;           // limpa a string
	return 0;                // indica "não reconheci ID"
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
	//  Lê o primeiro caractere e verifica se é um dígito. Se NÃO for dígito, não pode ser DEC
	if ( isdigit(lexeme[0] = getc(tape)) ) {

		/* -------------------------------------------------------
         * Caso especial: número começa com '0'
         * -------------------------------------------------------
         * Pela definição da gramática, o número 0 sozinho é válido,
         * mas "012" não é tratado como inteiro normal aqui.
         *
         * Então:
         *   - Se o número começar com '0', retornamos o token DEC
         *     imediatamente, sem ler mais dígitos.
         * ------------------------------------------------------- */
		if (lexeme[0] == '0') {
			lexeme[1] = 0;  // Finaliza string: "0"
			return DEC;
		}

		/* -------------------------------------------------------
         * Caso geral: número começa com [1-9]
         * Agora lemos todos os próximos dígitos enquanto forem
         * válidos, formando números como 12, 345, 78956 etc.
         * ------------------------------------------------------- */
		int i = 1;
		while ( isdigit(lexeme[i] = getc(tape)) ) {
			i++;
		}

		// O último caractere lido NÃO faz parte do identificador.
        // Devolvemos para a entrada para que outro token o processe
		ungetc(lexeme[i], tape);

		// Finaliza o identificador com terminador nulo '\0'
        // para transformar lexeme[] em uma string válida de C.
		lexeme[i] = 0;

		// Retorna o token DEC (número inteiro)
		return DEC;
	}
	ungetc(lexeme[0], tape); 	// devolve o caractere lido
	lexeme[0] = 0; 		 		// limpa a string
	return 0; 			  		// indica "não reconheci DEC"
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
        i++;
        int hassign = 0;
        // sinal opcional
        if ((lexeme[i] = getc(tape)) == '+' || lexeme[i] == '-') 
		{
            i++;
            hassign = 1;
        } else {
            ungetc(lexeme[i], tape);
        }
        // dígito obrigatório
        if (isdigit(lexeme[i] = getc(tape))) 
		{
            i++;
            while ( isdigit(lexeme[i] = getc(tape))) {
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
            i--;
        }
		ungetc(lexeme[i], tape);
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
			i++;
			if ( isdigit( lexeme[i] = getc(tape) ) ) {
				i++;
				while ( isdigit( lexeme[i] = getc(tape) ) ) {
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
				int i = 2;
				while ( isdigit( lexeme[i] = getc(tape) ) ) {
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
            break;
        }
        // columno++; 
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
			// columno += 3;
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
    
    // O Lexer deve garantir que columno e lineno estão certos antes de analisar.
    skipspaces(source); 
    skipArrow(source);

    // 1. Tenta reconhecer tokens complexos
    if ( (token = isID(source)) ) {
        return token;
    }
    if ( (token = isNUM(source)) ) {
		columno += strlen(lexeme);
        return token;
    }
    if ( (token = isASGN(source)) ) {
        columno += 2; 
        return token;
    }
    
    // 2. Tenta reconhecer token ASCII simples
    lexeme[0] = token = getc(source);

    if (token == EOF) {
        return EOF; 
    }

	if (token == '\n') {
        lineno++;
        columno = 0; // A linha avança e a coluna RESETA para 0
    } else {
		columno++; // Aumenta a coluna para o caractere ASCII simples (EX: +, *, ;)
	}

    lexeme[1] = 0;
	// return an ASCII token
    return token;
}
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
	// Começa lendo o lexeme já parcialmente preenchido
	int i = strlen(lexeme);

	// Tenta ler um 'E' ou 'e'
    if (toupper(lexeme[i] = getc(tape)) == 'E' )
    {
        i++;
        int hassign = 0; // indica se houve sinal (+ ou -)

        /*******************************************************
         * Verifica SE houver sinal opcional após o 'e'
         * Ex.: 1e+10   ou   2.5E-3
         *******************************************************/
        if ((lexeme[i] = getc(tape)) == '+' || lexeme[i] == '-') 
		{
            i++;
            hassign = 1;
        } else {
			// Não havia sinal → devolve caractere lido indevidamente
            ungetc(lexeme[i], tape);
        }

        /*******************************************************
         * Agora precisamos OBRIGATORIAMENTE de pelo menos um dígito.
         * Sem um dígito após 'e' ou 'e+', não é notação científica.
         *******************************************************/
        if (isdigit(lexeme[i] = getc(tape))) 
		{
            i++;
			// Lê todos os dígitos do expoente
            while ( isdigit(lexeme[i] = getc(tape))) {
                i++;
            }
			
			// O último caractere lido NÃO faz parte do identificador.
        	// Devolvemos para a entrada para que outro token o processe
            ungetc(lexeme[i], tape);

			// Finaliza o identificador com terminador nulo '\0'
        	// para transformar lexeme[] em uma string válida de C.
            lexeme[i] = 0;

			// Notação científica válida → retorna FLT
            return FLT;
        }

		/*******************************************************
         * Se NÃO houver dígito após 'e' ou 'e+' → erro.
         * Precisamos desfazer tudo o que foi lido erroneamente.
         *
         * Exemplo: "1e+" → não é válido → rollback.
         *******************************************************/
        ungetc(lexeme[i], tape); // devolve o não-dígito
        i--;

        if(hassign) // se havia sinal, desfaz
		{
            ungetc(lexeme[i], tape);
            i--;
        }

		
		ungetc(lexeme[i], tape);	// desfaz o 'E'
		lexeme[i] = 0;				// limpa a string
		return 0;                   // não é notação científica
    }
    ungetc(lexeme[i], tape); 	// desfaz o 'E'
    lexeme[i] = 0;				// limpa a string
    return 0;         		  	// não é notação científica	
}

/****************************************************
 * isNUM
 * --------------------------------------------------
 * Reconhece números inteiros (DEC) ou números reais (FLT).
 *
 * Formatos aceitos:
 *   - 12              (inteiro)
 *   - 3.14            (float com parte fracionária)
 *   - 0.5e-3          (notação científica)
 *   - .25             (float que começa com '.')
 *
 * Funciona como um “wrapper” que combina:
 *   • isDEC() → reconhece parte inteira
 *   • leitura opcional do '.'
 *   • leitura opcional de dígitos após '.'
 *   • isEE() → reconhece parte exponencial
 ****************************************************/
int isNUM(FILE *tape)
{
	// Primeiro tentamos reconhecer um inteiro (DEC)
	int token = isDEC(tape);

	/****************************************************
	 * CASO 1 — Começa com DEC
	 * Exemplos: 12, 0, 345
	 ****************************************************/
	if (token == DEC) {
		int i = strlen(lexeme);

		/************************************************
		 * Tenta reconhecer ponto decimal '.'
		 * Ex.: 12.34   ou   0.75
		 ************************************************/
		if ( (lexeme[i] = getc(tape)) == '.' ) {
			i++;

			/********************************************
			 * Após o '.' tentamos ler pelo menos um dígito.
			 ***************************************/
			if ( isdigit( lexeme[i] = getc(tape) ) ) {
				i++;

				// Lê todos os dígitos após o ponto
				while ( isdigit( lexeme[i] = getc(tape) ) ) {
					i++;
				}

				// O último caractere lido NÃO faz parte do identificador.
        		// Devolvemos para a entrada para que outro token o processe
				ungetc(lexeme[i], tape);

				// Finaliza o identificador com terminador nulo '\0'
				// para transformar lexeme[] em uma string válida de C.
				lexeme[i] = 0;

				// Retorna token FLT (número real)
				token = FLT;
			} else {
				/********************************************
				 * Caso especial: "12." (sem dígitos depois)
				 * Aceitamos como float, seguindo estilo do C.
				 ********************************************/
				ungetc(lexeme[i], tape); 	// desfaz o caractere não-dígito
				lexeme[i] = 0;  	  		// finaliza string
				token = FLT; 				// considera como float (ex.: "12.")
			}
		} else {
			/************************************************
			 * Não havia ponto → devolve caractere indevido.
			 ************************************************/
			ungetc(lexeme[i], tape); 	// desfaz o caractere não '.'
			lexeme[i] = 0;            // finaliza string
		}

	/********************************************************
	 * CASO 2 — Não começou com DEC, tenta ".digit"
	 * Ex.: .5   .123
	********************************************************/
	} else {
		
		 // Olhamos se o primeiro caractere é '.'
		if ( (lexeme[0] = getc(tape)) == '.' ) {¨

			// Para ser float, DEVE ter um dígito depois de '.'
			if ( isdigit( lexeme[1] = getc(tape) ) ) {
				token = FLT;
				int i = 2;

				// Lê os próximos dígitos
				while ( isdigit( lexeme[i] = getc(tape) ) ) {
					i++;
				}

				// O último caractere lido NÃO faz parte do identificador.
				// Devolvemos para a entrada para que outro token o processe
				ungetc(lexeme[i], tape);

				// Finaliza o identificador com terminador nulo '\0'
				// para transformar lexeme[] em uma string válida de C.
				lexeme[i] = 0;

			} else {
				// "." sozinho → não é número
				ungetc(lexeme[1], tape); 	// desfaz o caractere não-dígito
				ungetc(lexeme[0], tape); 	// desfaz o '.'
				lexeme[0] = 0;  	  		// limpa a string
				return 0; 					// not a number
			}
		} else {
			ungetc(lexeme[0], tape);  	// desfaz o caractere não '.'
			lexeme[0] = 0;            	// limpa a string  
			return 0; 					// not a number
		}
	}
	
	/****************************************************
	 * Tenta reconhecer notação exponencial (isEE)
	 * Ex.: 1.5e10, 3e-2, 0.01E+5
	 ****************************************************/
	if (isEE(tape)) {
		token = FLT;
	}
	return token;
}

/****************************************************
 * isASGN
 * --------------------------------------------------
 * Reconhece o operador de atribuição ":=".
 *
 * Funcionamento:
 *   1. Tenta ler ':'
 *   2. Se der certo, tenta ler '='
 *   3. Se formar ":=", retorna token ASGN
 *   4. Caso contrário, desfaz leituras com ungetc()
 *
 * Exemplo válido:
 *     x := 10
 ****************************************************/
int isASGN(FILE *tape) {

	// Lê o primeiro caractere e guarda no lexeme
	lexeme[0] = getc(tape);

	// Primeiro caractere deve ser ':'
	if(lexeme[0] == ':') 
	{

		// Lê o segundo caractere
		lexeme[1] = getc(tape);

		// Se for '=', temos o operador completo ":="
		if (lexeme[1] == '=') 
		{
			lexeme[2] = 0; 	// finaliza a string ':='
			return ASGN;  	// reconheceu o token
		}

		// Não era '=', desfaz a leitura do segundo caractere
		// Devolve esse caractere para a entrada.
		ungetc(lexeme[1], tape);
	}

	ungetc(lexeme[0], tape); // desfaz a leitura do ':'
	lexeme[0] = 0;           // limpa a string
	return 0;                // indica "não reconheci ASGN"
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

	/****************************************************
     * Lê caracteres enquanto forem isspace():
     *
     *   isspace inclui: ' ', '\t', '\n', '\r', '\f', '\v'
     *
     * Mas queremos parar se for '\n', pois o parser usa
     * '\n' como separador entre comandos.
     ****************************************************/

    while ( isspace(head = getc(tape)) ){

		// Se for quebra de linha, paramos.
        if(head == '\n'){
            break;
        }
		// Descomentar a linha abaixo caso queira contar os espaços como colunas
        // columno++; 
    }

	// O último caractere lido NÃO é espaço (ou é '\n'),
    // então devolvemos para que outros reconhecedores o tratem.
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
	// Enquanto o próximo caractere for ESC (0x1B),
    // tentamos verificar se é realmente uma sequência de seta
	while ((lexeme[0] = getc(tape)) == 0x1B) { // ESC (27)
		// Lê os dois caracteres seguintes da sequência ESC [ X
		lexeme[1] = getc(tape);
		lexeme[2] = getc(tape);

		/****************************************************
         * Verifica se os três caracteres formam:
         *
         *   ESC [ A   → seta para cima
         *   ESC [ B   → seta para baixo
         *   ESC [ C   → seta para direita
         *   ESC [ D   → seta para esquerda
         ****************************************************/
		if (lexeme[1] == '[' && 
			(lexeme[2] == 'A' || lexeme[2] == 'B' || lexeme[2] == 'C' || lexeme[2] == 'D')) {
			// Descomentar a linha abaixo caso desajar contar as setas como colunas
			// columno += 3; 
		} else {
			/************************************************
             * Se não era uma seta (alguma outra sequência ESC),
             * então desfazemos TODAS as leituras com ungetc.
             ****************************************************/
			ungetc(lexeme[2], tape);
			ungetc(lexeme[1], tape);
			ungetc(lexeme[0], tape);
			lexeme[0] = 0;
			break; // sai do while — não é sequência de seta
		}
	}

	// Devolve o último caractere lido que não era ESC,
    // para que o lexer trate normalmente.
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
    
    /****************************************************
     * PASSO 1 — Limpeza da entrada
     * --------------------------------------------------
     * Ignora:
     *   - espaços, tabs, etc.
     *   - sequências "ESC [ X" das setas do teclado
     *
     * Isso garante que o Lexer SEMPRE começa lendo
     * o próximo caractere significativo.
     ****************************************************/
    skipspaces(source); 
    skipArrow(source);

	/****************************************************
     * PASSO 2 — Tentativa de reconhecer tokens complexos
     ****************************************************/

    // Identificadores, variáveis e palavras-chave exit/quit
    if ( (token = isID(source)) ) {
        return token;
    }

	// Números (inteiros, floats, ponto flutuante, notação científica)
    if ( (token = isNUM(source)) ) {
		columno += strlen(lexeme);
        return token;
    }

	// Números (inteiros, floats, ponto flutuante, notação científica)
    if ( (token = isASGN(source)) ) {
        columno += 2; 
        return token;
    }
    
    /****************************************************
     * PASSO 3 — Token ASCII simples
     * --------------------------------------------------
     * Se nenhum reconhecedor complexo funcionou, significa
     * que o próximo token é um símbolo único (ex.: +, -, *,
     * /, (, ), ;, etc.)
     ****************************************************/
    lexeme[0] = token = getc(source);

	// Fim de arquivo
    if (token == EOF) {
        return EOF; 
    }

    /****************************************************
     * PASSO 4 — Atualização de posição na entrada
     ****************************************************/
	if (token == '\n') {
		// Nova linha → incrementa linha e zera coluna
        lineno++;
        columno = 0; // A linha avança e a coluna RESETA para 0
    } else {
		// Qualquer outro caractere ocupa 1 coluna
		columno++; // Aumenta a coluna para o caractere ASCII simples (EX: +, *, ;)
	}

	 // Finaliza lexeme como string de 1 caractere
    lexeme[1] = 0;
	
	// return an ASCII token
    return token;
}
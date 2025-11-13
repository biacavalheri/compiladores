/****************************************************/
/*
OBJETIVO:
Este programa implementa um interpretador de expressões aritméticas
com variáveis e comandos simples. Ele realiza análise léxica e sintática
utilizando uma gramática LL(1) e executa as expressões diretamente.

-----------------------------------------------------
GRAMÁTICA SIMPLIFICADA (LL(1)):

mybc -> cmd { cmdsep cmd } EOF
cmdsep -> ';' | '\n'

cmd -> E 
     | exit
     | quit
     | <epsilon>

E -> [ '+' | '-' ] T { ('+' | '-') T }
T -> F { ('*' | '/') F }
F -> '(' E ')'
    | DEC
    | FLT
    | ID [ ':=' E ]

-----------------------------------------------------
DESCRIÇÃO DAS FUNÇÕES PRINCIPAIS:

mybc()   : ponto de entrada do interpretador. Gerencia comandos múltiplos.
cmd()    : executa um comando, que pode ser uma expressão ou “exit/quit”.
E()      : avalia expressões aritméticas com + e -.
match()  : verifica se o token atual é o esperado; trata erros sintáticos.
recall() : busca valores de variáveis na tabela de símbolos.
store()  : armazena valores de variáveis na memória virtual.
handleSigint(): trata o sinal Ctrl+C, garantindo saída limpa.
-----------------------------------------------------
DIFERENCIAIS DO CÓDIGO:

- Usa um analisador LL(1) com lookahead (variável global “lookahead”).
- Implementa tratamento de erros detalhado, incluindo linha e coluna.
- Mantém uma tabela de símbolos (variáveis) em memória.
- Interpreta expressões em tempo real, exibindo o resultado imediatamente.
- Trata interrupções do usuário (Ctrl+C) e comandos múltiplos separados
  por ponto e vírgula ou nova linha.
*/
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>
#include <tokens.h>
#include <parser.h>
#include <signal.h>


int lookahead; // este é o olho do compilador (token atual em análise)
int parse_error = 0; // flag que indica se ocorreu erro no comando atual

/****************************************************
 * handleSigint
 * --------------------------------------------------
 * Manipulador de sinal SIGINT (Ctrl + C).
 * Quando o usuário pressiona Ctrl+C, imprime apenas uma quebra de linha,
 * sem encerrar abruptamente o programa. Isso evita corrupção no buffer
 * e mantém o interpretador responsivo.
 ****************************************************/
void handleSigint(int sig)
{
    if (sig == SIGINT) {
        printf("\n");
        fflush(stdout);
    }
}

/****************************************************
 * mybc
 * --------------------------------------------------
 * Função principal do interpretador de comandos.
 * mybc -> cmd { cmdsep cmd } EOF
 * Onde cmdsep pode ser ';' ou '\n'.
 * 
 * Executa comandos em sequência até o final do arquivo (EOF).
 ****************************************************/
void mybc(void)
{
	cmd(); // Executa o primeiro comando digitado pelo usuário

	// Se depois do comando vier um ';' ou uma quebra de linha, significa que existe outro comando para executar.
	while(lookahead == ';' || lookahead == '\n'){
		// cmdsep -> ';' | '\n'
		match(lookahead); // Consume esse separador (tira da entrada).
		cmd(); // Executa o próximo comando.
	}

	match(EOF);
}

/****************************************************
 * cmd
 * --------------------------------------------------
 * cmd -> E | exit | quit | <epsilon>
 * 
 * Esta função reconhece um comando do usuário.
 * Pode ser uma expressão, ou os comandos de saída (exit/quit).
 * Caso haja erro de parsing, limpa o estado de erro antes de continuar.
 ****************************************************/
void cmd(void)
{
	switch(lookahead) {
		case EXIT:
		case QUIT:
			exit(0);

		// FIRST(E):
		case '+':
		case '-':
		case '(':
		case DEC:
		case FLT:
		//case HEX:
		//case OCT:
		case ID:
			E();
			if (!parse_error) {
    			printf("%lg\n", acc); // imprime o resultado da expressão
			} else {
    			parse_error = 0; // reseta o erro para o próximo comando
			}
			break;
		default:
			; // epsilon (não faz nada)
	}
}

extern FILE *objcode;

double acc; // acumulador principal (resultado da expressão atual)
#define STACKSIZE 1024
double stack[STACKSIZE]; // pilha auxiliar para operadores binários
int sp = -1; // stack pointer (inicialmente vazia)

/****************************************************
 * Tabela de Símbolos
 * --------------------------------------------------
 * Implementada como um dicionário simples (vetores paralelos)
 * que mapeia identificadores (variáveis) para valores numéricos.
 ****************************************************/
#define MAXSTENTRIES 4096
char symtab[MAXSTENTRIES][MAXIDLEN+1]; // nomes das variáveis
int symtab_next_entry = 0; // índice da próxima posição livre
double vmem[MAXSTENTRIES]; // memória virtual (valores associados)

int address;

/****************************************************
 * recall
 * --------------------------------------------------
 * Busca o valor associado a uma variável.
 * Caso a variável não exista, cria uma nova entrada com valor 0.0.
 * Retorna o valor encontrado (ou 0.0 se nova variável).
 ****************************************************/
double recall(char const *name) {
	for (address = symtab_next_entry - 1; address > -1; address--) {
		if (strcmp(symtab[address], name) == 0) {
			return vmem[address];
		}
	}
	// variável ainda não existe — cria entrada nova
	address = symtab_next_entry++;
	strcpy(symtab[address], name);
	return 0.e+00; // retorna zero (float)
}

/****************************************************
 * store
 * --------------------------------------------------
 * Atualiza o valor de uma variável existente (ou cria nova).
 * Utiliza a função recall para localizar o endereço.
 ****************************************************/
void store(char const *name) {
	recall(name);
	vmem[address] = acc;
}

/****************************************************
 * E
 * --------------------------------------------------
 * E é o símbolo inicial da gramática LL(1) de expressões.
 * 
 * E -> [ ominus ] T { oplus T }
 * onde:
 *   ominus = ['+' | '-']
 *   oplus  = ['+' | '-']
 * 
 * Essa função implementa avaliação recursiva e iterativa
 * (usando GOTO para simular loops entre T e F),
 * respeitando a precedência e associatividade dos operadores.
 ****************************************************/
void E(void)
{
	/* variáveis auxiliares de controle */
	int varname[MAXIDLEN+1]; // guarda o nome da variável antes do match()
	int isnegate = 0; // isnegate → guarda se há um sinal unário '-' no início
	int isotimes = 0; // isotimes → guarda se o próximo operador é * ou /
	int isoplus = 0; // isoplus  → guarda se o próximo operador é + ou -

	// Verifica sinal unário inicial
	if(lookahead == '+' || lookahead == '-') {
		if (lookahead == '-') {
			isnegate = lookahead;
		}
		match(lookahead);
	}

	_Tbegin:

	_Fbegin:

	/* -----------------------------------------------------------
	 * Parte responsável por reconhecer um F (operando)
	 * F pode ser:
	 *   - expressão entre parênteses
	 *   - número inteiro
	 *   - número real
	 *   - variável (com ou sem atribuição)
	 * ----------------------------------------------------------- */
	switch(lookahead) {
		case '(':
			match('('); E(); match(')');
			break;
		case DEC:
			acc = atoi(lexeme); // número inteiro
			match(DEC); break;
		case FLT:
			acc = atof(lexeme); // número real
			match(FLT); break;
		default:
			// F -> ID [ := E ]
			strcpy(varname, lexeme); // salva o nome antes do match
			match(ID);
			if (lookahead == ASGN) {
				match(ASGN);
				E(); // resultado vem em 'acc'
				store(varname); // salva na memória virtual
			} else {
				acc = recall(varname); // busca valor da variável
			}
	}
	
	/* =============================================================
     * 1. APLICA OPERADORES MULTIPLICATIVOS PENDENTES (* ou /)
     * -------------------------------------------------------------
     * Quando encontramos um * ou /, não podemos aplicar imediatamente,
     * porque ainda precisamos ler o próximo operando (outro F).
     *
     * Então guardamos o operador em isotimes e empilhamos o valor anterior.
     * Quando chegamos aqui, já temos:
     *   - valor anterior no stack[sp]
     *   - novo operando em acc
     *   - operador guardado em isotimes
     *
     * Agora podemos finalmente aplicar a operação.
     * ============================================================= */
	if(isotimes) {
		if(isotimes == '*'){
			stack[sp] = stack[sp] * acc;
		} else {
			stack[sp] = stack[sp] / acc;
		};
		acc = stack[sp--];
		isotimes = 0;
	}

	/* =============================================================
     * 2. DETECTA NOVO OPERADOR * OU /
     * -------------------------------------------------------------
     * Se o próximo token é * ou /, significa que um novo termo
     * multiplicativo começa. Então:
     *   - guardamos o operador em isotimes
     *   - empilhamos o valor atual (acc)
     *   - consumimos o operador
     *   - voltamos para ler um novo F (goto _Fbegin)
     * ============================================================= */
	if(lookahead == '*' || lookahead == '/') {
		isotimes = lookahead;
		stack[++sp] = acc;
		match(lookahead); goto _Fbegin;
	}

	/* =============================================================
     * 3. APLICA SINAL UNÁRIO NEGATIVO (ex.: -3 ou -(a+2))
     * -------------------------------------------------------------
     * Esse sinal é lido lá no começo da expressão, mas só pode ser
     * aplicado depois que a parte multiplicativa for resolvida.
     * ============================================================= */
	if(isnegate){
		acc = -acc;
		isnegate = 0;
	}

	/* =============================================================
     * 4. APLICA OPERADORES ADITIVOS PENDENTES (+ ou -)
     * -------------------------------------------------------------
     * Assim como em * e /, operações de soma/subtração também são
     * adiadas, mas apenas até que * e / acabem.
     *
     * Se havia um + ou - guardado em isoplus, aplicamos agora:
     *   stack[sp]  → valor anterior
     *   acc        → novo termo
     * ============================================================= */
	if(isoplus){
		if(isoplus == '+'){
			stack[sp] = stack[sp] + acc;
		} else {
			stack[sp] = stack[sp] - acc;
		};
		acc = stack[sp--];
		isoplus = 0;
	}

	 /* =============================================================
     * 5. DETECTA NOVOS OPERADORES ADITIVOS (+ ou -)
     * -------------------------------------------------------------
     * Se o próximo token é + ou -, então uma nova parte da expressão
     * começa. Então:
     *   - guardamos o operador em isoplus
     *   - empilhamos o valor atual
     *   - consumimos o operador
     *   - voltamos para ler mais termos (goto _Tbegin)
     * ============================================================= */
	if(lookahead == '+' || lookahead == '-') {
		isoplus = lookahead;
		stack[++sp] = acc;
		match(lookahead); goto _Tbegin;
	}
	// Fim da expressão
}

/****************************************************
 * match
 * --------------------------------------------------
 * Verifica se o token atual (lookahead) corresponde ao esperado.
 * Caso sim, consome o token chamando gettoken().
 * Caso contrário, emite mensagem de erro sintático detalhada,
 * incluindo linha, coluna, token esperado e token recebido.
 * 
 * Implementa recuperação simples: avança até ';' ou '\n'
 * para evitar abortar o interpretador.
 ****************************************************/
int lookahead;
void match(int expected)
{
	if (parse_error) return; // Se já houve erro antes, não tenta validar mais nada. Evita cascata de erros

	/****************************************************
     * CASO 1 — Sem erro: lookahead == expected
     * --------------------------------------------------
     * Então consome o token da entrada e lê o próximo.
     ****************************************************/
    if (lookahead == expected) {
        lookahead = gettoken(source);
        return;
    }

	/****************************************************
     * CASO 2 — Erro sintático: token não corresponde
     * --------------------------------------------------
     * Precisamos gerar uma mensagem clara e amigável,
     * dizendo qual token esperávamos e qual realmente veio.
     ****************************************************/
	char expectedName[64];
    char receivedName[64];

    // Nome legível do token esperado
    switch (expected) {
        case ID:   strcpy(expectedName, "identifier"); break;
        case DEC:  strcpy(expectedName, "integer");    break;
        case FLT:  strcpy(expectedName, "float");      break;
        case ASGN: strcpy(expectedName, "':=' (assignment)"); break;
        case EXIT: strcpy(expectedName, "'exit'");     break;
        case QUIT: strcpy(expectedName, "'quit'");     break;
        case EOF:  strcpy(expectedName, "end of input"); break;
        default:
            if (expected >= 32 && expected <= 126) {
                snprintf(expectedName, sizeof(expectedName), "'%c'", expected);
            } else {
                snprintf(expectedName, sizeof(expectedName), "token %d", expected);
            }
            break;
    }

	// Converte o token esperado em texto legível
    if (lookahead == EOF) {
        strcpy(receivedName, "end of input");
    } else if (lookahead >= 32 && lookahead <= 126) {
        snprintf(receivedName, sizeof(receivedName), "'%c'", lookahead);
    } else {
        switch (lookahead) {
            case ID: strcpy(receivedName, "identifier"); break;
            case DEC: strcpy(receivedName, "integer"); break;
            case FLT: strcpy(receivedName, "float"); break;
            case ASGN: strcpy(receivedName, "':=' (assignment)"); break;
            case EXIT: strcpy(receivedName, "'exit'"); break;
            case QUIT: strcpy(receivedName, "'quit'"); break;
            default: snprintf(receivedName, sizeof(receivedName), "token %d", lookahead); break;
        }
    }

    // Mensagem de erro padronizada e clara
    fprintf(stderr, "\nERROR at line %d, column %d.\n", lineno, columno);
    fprintf(stderr, "Token mismatch: expected %s but received %s.\n", expectedName, receivedName);

	parse_error = 1;

	/****************************************************
     * RECUPERAÇÃO DE ERRO
     * --------------------------------------------------
     * Ignora tokens até encontrar o fim de comando:
     *    ';'   ou   '\n'
     *
     * Assim o interpretador continua funcionando
     * para o próximo comando da entrada.
     ****************************************************/
	while (lookahead != EOF && lookahead != ';' && lookahead != '\n') {
        lookahead = gettoken(source);
    }
	return;
}

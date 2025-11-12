/****************************************************/
/*
Arquivo: parser.h
Módulo: Analisador Sintático e Interpretador de Expressões
----------------------------------------------------------
Descrição geral:
Conecta a interface
entre o analisador léxico (lexer) e o parser, permitindo que tokens,
valores e contexto de parsing sejam compartilhados entre os módulos.

As funções aqui declaradas implementam uma gramática LL(1) que analisa
expressões aritméticas com atribuições, variáveis e comandos simples.

Principais funções:
- mybc()   → executa o loop principal do interpretador
- cmd()    → processa comandos individuais
- E()      → avalia expressões aritméticas
- match()  → realiza verificação de tokens e tratamento de erros

Constantes e variáveis externas:
- lookahead  → armazena o token atual
- lexeme     → texto do token atual
- acc        → acumulador principal do interpretador
- parse_error → flag de erro sintático
- lineno, columno → localização do erro no código-fonte
*/
/****************************************************/

#pragma once
#define ERRTOKEN -0x10000000  // Token especial usado para indicar erro léxico

/****************************************************
 * Variáveis externas compartilhadas entre o parser e o lexer.
 * O uso de extern permite que o parser acesse o estado atual
 * do analisador léxico sem duplicar dados globais.
 ****************************************************/
extern int lookahead;   // token atual (fornecido pelo lexer)
extern double acc;      // acumulador de resultado numérico
extern char lexeme[];   // cadeia de caracteres do token reconhecido
extern int lineno;      // número da linha atual (controle de erro)
extern int columno;     // número da coluna atual (controle de erro)
extern int parse_error; // flag global de erro de parsing
extern FILE *source;    // ponteiro para o arquivo de entrada (ou stdin)

/****************************************************
 * Protótipos das funções principais do parser
 ****************************************************/

// Compara o token atual com o esperado e realiza tratamento de erro
extern void match(int);

// Lê o próximo token da entrada
extern int gettoken(FILE *);

// Manipulador de sinal para Ctrl+C (SIGINT)
extern void handleSigint(int);

// Funções que implementam a gramática (símbolos não-terminais)
extern void mybc(void); // regra inicial: interpretador de comandos
extern void cmd(void);  // comando individual
extern void E(void);    // expressão aritmética

// Protótipos extras (não necessariamente usados neste parser)
extern void T(void);
extern void F(void);
extern void Q(void);
extern void R(void);

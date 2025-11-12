/****************************************************/
/*
----------------------------------------------------------
Descrição geral:
Este cabeçalho declara as funções e variáveis globais utilizadas
no módulo principal (`main.c`) e interligadas aos demais módulos
do projeto (lexer e parser).

Ele serve como ponto de conexão entre as etapas do compilador:
- Lexer  (gettoken)
- Parser (mybc)
- Tratamento de sinais (handleSigint)
*/
/****************************************************/

#pragma once

/****************************************************
 * Declarações externas compartilhadas com o parser
 * e o analisador léxico.
 ****************************************************/
extern int lookahead;          // Token atual (fornecido pelo lexer)
extern int gettoken(FILE *);   // Função de obtenção de tokens (lexer)
extern double acc;             // Acumulador global (resultado de expressões)

/****************************************************
 * Protótipos de funções do interpretador
 ****************************************************/
extern void mybc(void);        // Função principal do parser (interpretação)
extern void handleSigint(int); // Manipulador do sinal Ctrl+C

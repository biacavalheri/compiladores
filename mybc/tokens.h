/****************************************************/
/*
Arquivo: tokens.h
Módulo: Definições de Tokens
----------------------------------------------------------
Descrição geral:
Define o conjunto de tokens reconhecidos pelo analisador léxico
e utilizados pelo parser. Cada token é identificado por um número
inteiro único, a partir de 1024, para evitar conflito com os
caracteres ASCII (que vão até 127).

Esses tokens representam as categorias léxicas da linguagem
interpretada pelo mini compilador, como identificadores, números,
atribuições e comandos especiais.

*/
/****************************************************/

enum {
	ID = 1024,   // Identificador (nome de variável)
	DEC,         // Número inteiro decimal
	OCT,         // Número octal (não usado neste parser)
	HEX,         // Número hexadecimal (não usado neste parser)
	FLT,         // Número de ponto flutuante
    ASGN,        // Operador de atribuição ":="
	EXIT,        // Comando "exit" — encerra o interpretador
	QUIT,        // Comando "quit" — encerra o interpretador
};

/****************************************************/
/*
Módulo: Definição de Palavras-Chave
----------------------------------------------------------
Descrição geral:

Esses tokens especiais são reconhecidos durante a análise léxica
e utilizados pelo parser para determinar a estrutura sintática
do programa fonte (gramática LL(1)).

----------------------------------------------------------
Conjunto de palavras reservadas suportadas:
BEGIN - END
IF - THEN - ELSE
REPEAT - UNTIL
WHILE - DO
PROGRAM, PROCEDURE, FUNCTION, VAR, TYPE, etc.
*/
/****************************************************/

#pragma once

/****************************************************
 * Enumeração das palavras-chave.
 * Cada elemento do enum representa o código único de um token.
 *
 * A faixa de valores começa em 0x10000000 para garantir que
 * os tokens de palavras-chave não colidam com os tokens de
 * caracteres ASCII (como ';', '+', etc.).
 ****************************************************/
enum {
    BEGIN = 0x10000000,  // início de bloco
    PROGRAM,             // definição principal do programa
    PROCEDURE,           // declaração de procedimento
    FUNCTION,            // declaração de função
    INTEGER,             // tipo inteiro
    REAL,                // tipo real
    BOOLEAN,             // tipo booleano
    VAR,                 // declaração de variável
    IF,                  // comando condicional
    THEN,                // parte 'então' do IF
    ELSE,                // parte alternativa do IF
    REPEAT,              // início de laço tipo REPEAT
    UNTIL,               // final de laço REPEAT
    WHILE,               // início de laço WHILE
    DO,                  // corpo do laço WHILE
    OR,                  // operador lógico OR
    DIV,                 // divisão inteira
    MOD,                 // operador módulo
    AND,                 // operador lógico AND
    TRUE,                // constante booleana verdadeira
	FALSE,               // constante booleana falsa
    END,                 // final de bloco
};

/****************************************************
 * Constantes auxiliares para controle de iteração.
 * Permitem saber facilmente o primeiro e o último token
 * de palavra-chave, úteis para loops de verificação.
 ****************************************************/
#define KEYWORD_FIRST BEGIN
#define KEYWORD_LAST  END
#define KEYWORDS_COUNT (KEYWORD_LAST - KEYWORD_FIRST + 1)

/****************************************************
 * Declarações externas
 * --------------------------------------------------
 * - keywords[] : tabela de strings com os nomes das palavras-chave.
 * - iskeyword() : função que verifica se um identificador é keyword.
 ****************************************************/
extern const char *keywords[];
int iskeyword(const char *name);

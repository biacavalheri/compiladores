/****************************************************/
/*

Módulo: Cabeçalho do Analisador Léxico
----------------------------------------------------------
Descrição geral:
Define as variáveis e constantes utilizadas pelo módulo `lexer.c`.

O analisador léxico é responsável por ler o código fonte e
identificar tokens. Ele converte o fluxo de caracteres em
símbolos significativos para o parser.

Constantes:
 - MAXIDLEN : tamanho máximo permitido para um identificador.

Variáveis globais:
 - lexeme[] : contém o texto do último token reconhecido.
 - lineno   : controla a linha atual do código-fonte.
 - columno  : controla a coluna atual.
*/
/****************************************************/

#pragma once

#define MAXIDLEN 256  // tamanho máximo de um identificador

extern char lexeme[];  // armazena o texto do token atual

extern int lineno;     // linha atual (para mensagens de erro)
extern int columno;    // coluna atual (para mensagens de erro)

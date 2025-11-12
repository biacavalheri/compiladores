/****************************************************/
/*
Módulo: Dicionário de Palavras-Chave
----------------------------------------------------------
Descrição geral:
Implementa a tabela de palavras-chave reservadas
da linguagem. Durante a análise léxica, ele é utilizado para
distinguir identificadores comuns (variáveis, funções) de
palavras que possuem significado fixo na gramática (como IF, END, BEGIN).

O reconhecimento de palavras-chave é feito comparando o lexema
atual com as strings armazenadas em `keywords[]`.
Se houver correspondência, é retornado o código numérico do token
definido em `keywords.h`; caso contrário, retorna 0 (não é keyword).
*/
/****************************************************/

#include <string.h>
#include <keywords.h>

/****************************************************
 * keywords[]
 * --------------------------------------------------
 * Dicionário estático de palavras-chave.
 * Cada string representa uma palavra reservada reconhecida
 * pela linguagem. A posição no vetor está relacionada ao
 * valor numérico definido no enum de `keywords.h`.
 *
 * Obs.: É declarada como const para indicar que a tabela
 * não deve ser alterada em tempo de execução.
 ****************************************************/
const char *keywords[] = {   // <- const
    "begin","program","procedure","function","integer","real","boolean","var",
    "if","then","else","repeat","until","while","do","or","div","mod",
    "and","true","false","end",
};

/****************************************************
 * iskeyword
 * --------------------------------------------------
 * Função de verificação de palavras-chave.
 * Recebe uma string (lexema) e verifica se ela corresponde
 * a alguma das palavras reservadas da linguagem.
 *
 * Retorno:
 *   - Se o nome for uma keyword → retorna o código do token.
 *   - Caso contrário → retorna 0 (identificador comum).
 *
 * Implementação:
 *   - Percorre o intervalo de tokens entre BEGIN e END,
 *     comparando o texto com as strings do vetor keywords[].
 ****************************************************/
int iskeyword(char const *name)
{
    int i;
    for (i = BEGIN; i <= END; i++) {
        if (strcmp(name, keywords[i - BEGIN]) == 0) return i;
    }
    return 0; // não é palavra-chave
}

//keywords.h

/* 
vamos trabalhar com as palavras-chaves a seguir:
BEGIN - END
IF - THEN - ELSE
REPEAT - UNTIL
WHILE - DO
*/
#pragma once

enum {
    BEGIN = 0x10000000,
    PROGRAM,
    PROCEDURE,
    FUNCTION,
    INTEGER,
    REAL,
    BOOLEAN,
    VAR,
    IF,
    THEN,
    ELSE,
    REPEAT, 
    UNTIL,
    WHILE,
    DO,
    OR,
    DIV,
    MOD,
    AND,
    TRUE,
	FALSE,
    END,
};

// extern int iskeyword(char const *name);
#define KEYWORD_FIRST BEGIN
#define KEYWORD_LAST  END
#define KEYWORDS_COUNT (KEYWORD_LAST - KEYWORD_FIRST + 1)

extern const char *keywords[];   // <- adicione isto
int iskeyword(const char *name);
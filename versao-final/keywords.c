//keywords.c
#include <string.h>
#include <keywords.h>

//dicionário
const char *keywords[] = {   // <- const
    "begin","program","procedure","function","integer","real","boolean","var",
    "if","then","else","repeat","until","while","do","or","div","mod",
    "and","true","false","end",
};


int iskeyword(char const *name)
{
    int i;
    for (i = BEGIN; i <= END; i++) {
        if (strcmp(name, keywords[i - BEGIN]) == 0) return i;

    }
    return 0;
}

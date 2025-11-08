#include <stdio.h>
#include <stdlib.h>
#include <main.h>

FILE *source, *objcode;

int main(void)
{
	lookahead = gettoken(source = stdin);
	objcode = stdout;
	printf("Iniciando a compilação...\n");
	program();
	printf("Compilação concluída.\n");

	return 0;
}
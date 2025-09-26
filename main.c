#include <stdio.h>
#include <stdlib.h>
#include <main.h>
#include <parser.h>

FILE *source;

int main(void)
{
	lookahead = gettoken(stdin);

	E();

	return 0;
}

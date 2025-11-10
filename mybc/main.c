#include <stdio.h>
#include <stdlib.h>
#include <main.h>
#include <signal.h>

FILE *source, *objcode;

int main(void)
{
	signal(SIGINT, handleSigint); // Manipulador do sinal Crtl+C

	lookahead = gettoken(source = stdin);
	objcode = stdout;

	mybc();

	return 0;
}
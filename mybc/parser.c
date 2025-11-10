#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>
#include <tokens.h>
#include <parser.h>
#include <signal.h>


int lookahead; // este é o olho do compilador
int parse_error = 0; // indica se ocorreu erro no comando atual

// Manipulador para Ctrl+C (SIGINT)
void handleSigint(int sig)
{
    if (sig == SIGINT) {
        printf("\n");
        fflush(stdout);
    }
}

// Interpretador de comando
//
// mybc -> cmd { cmdsep cmd} EOF
void mybc(void)
{
	cmd();

	while(lookahead == ';' || lookahead == '\n'){
		// cmdsep -> ';' | '\n'
		match(lookahead);

		cmd();

	}

	match(EOF);
}

// cmd -> E | exit | quit | <epsilon>
void cmd(void)
{
	switch(lookahead) {
		case EXIT:
		case QUIT:
			exit(0);

		// FIRST(E):
		case '+':
		case '-':
		case '(':
		case DEC:
		case FLT:
		//case HEX:
		//case OCT:
		case ID:
			E();
			if (!parse_error) {
    			printf("%lg\n", acc);
			} else {
    			parse_error = 0; // Reseta parse_error para o próximo comando
			}
			break;
		default:
			;
	}
}

extern FILE *objcode;

double acc; //acumulador
#define STACKSIZE 1024
double stack[STACKSIZE];
int sp = -1;

// tabela de símbolos comom dicionário dos valores armazenados na memória virtual
#define MAXSTENTRIES 4096
char symtab[MAXSTENTRIES][MAXIDLEN+1];
int symtab_next_entry = 0; //uso: strcpy(symtab[symtab_next_entry], name)
double vmem[MAXSTENTRIES];

int address;
double recall(char const *name) {
	// busca bottom-up a variável em name
	for (address = symtab_next_entry - 1; address > -1; address--) {
		if (strcmp(symtab[address], name) == 0) {
			return vmem[address];
		}
	}
	// variável ainda não existe
	address = symtab_next_entry++;
	strcpy(symtab[address], name);
	return 0.e+00;//só para enfatizar que é ponto flutuante
}

void store(char const *name) {
	// vai localizar o endereço da variável na memória
	recall(name);
	vmem[address] = acc;
}

// E é o símbolo inicial da gramática LL(1) de expressões simplificadas
// E -> [ ominus ] T { oplus T }
// ominus = ['+''-']
// oplus = ['+''-']
void E(void)
{
	/*0*/int varname[MAXIDLEN+1];/**/
	/*1*/int isnegate = 0;/**/
	/*2*/int isotimes = 0;/**/
	/*3*/int isoplus = 0;/**/

	if(lookahead == '+' || lookahead == '-') {
		if (lookahead == '-') {
			isnegate = lookahead;
		}
		match(lookahead);
	}

	_Tbegin:

	_Fbegin:

	switch(lookahead) {
		case '(':
			match('('); E(); match(')');
			break;
		case DEC:
		/*4*/acc = atoi(lexeme);/**/
			match(DEC); break;
		case FLT:
		/*7*/acc = atof(lexeme);/**/
			match(FLT); break;
		default:
			// F -> ID [ := E ]
			/**/strcpy(varname, lexeme);/**/ // tem que salvar antes do match senão perde o nome
			match(ID);
			if (lookahead == ASGN) {
				match(ASGN);
				E(); //trás o resultado no acumulador (acc)
				/**/ store(varname);/**/ //armazena no endereço associado à varname
			} else {
				/**/ acc = recall(varname);/**/
			}
	}
	
	/*9*/
	if(isotimes) {
		if(isotimes == '*'){
			stack[sp] = stack[sp] * acc;
		} else {
			stack[sp] = stack[sp] / acc;
		};
		acc = stack[sp--];
		isotimes = 0;
	}

	/*10*/
	if(lookahead == '*' || lookahead == '/') {
		/*10*/isotimes = lookahead;/**/
		/*10a*/stack[++sp] = acc;/**/
		match(lookahead); goto _Fbegin;
	}
    // term end
	
	/*11*/
	if(isnegate){
		acc = -acc;
		isnegate = 0;
	}

	/*12*/
	if(isoplus /* isoplus :: oplus.t */){
		if(isoplus == '+'){
			stack[sp] = stack[sp] + acc;
		} else {
			stack[sp] = stack[sp] - acc;
		};
		acc = stack[sp--];
		isoplus = 0;
	}

	/**/
	if(lookahead == '+' || lookahead == '-') {
		/**/isoplus = lookahead;/**/
		/**/stack[++sp] = acc;/**/
		match(lookahead); goto _Tbegin;
	}
	// expression end
}

//////////////////////////// parser components /////////////////////////////////
//TODO: implementar signalhandler para comandos
int lookahead;
void match(int expected)
{

	if (parse_error) return; // já houve erro — ignora novas verificações

    if (lookahead == expected) {
        lookahead = gettoken(source);
        return;
    }


	char expectedName[64];
    char receivedName[64];
    // Nome legível do token esperado
	// Tenta traduzir tokens conhecidos
    switch (expected) {
        case ID:   strcpy(expectedName, "identifier"); break;
        case DEC:  strcpy(expectedName, "integer");    break;
        case FLT:  strcpy(expectedName, "float");      break;
        case ASGN: strcpy(expectedName, "':=' (assignment)"); break;
        case EXIT: strcpy(expectedName, "'exit'");     break;
        case QUIT: strcpy(expectedName, "'quit'");     break;
        case EOF:  strcpy(expectedName, "end of input"); break;
        default:
            if (expected >= 32 && expected <= 126) {
                snprintf(expectedName, sizeof(expectedName), "'%c'", expected);
            } else {
                snprintf(expectedName, sizeof(expectedName), "token %d", expected); //Token desconhecido
            }
            break;
    }
	// Nome legível do token recebido
    if (lookahead == EOF) {
        strcpy(receivedName, "end of input");
    } else if (lookahead >= 32 && lookahead <= 126) {
        snprintf(receivedName, sizeof(receivedName), "'%c'", lookahead);
    } else {
        // Tenta traduzir tokens conhecidos
        switch (lookahead) {
            case ID: strcpy(receivedName, "identifier"); break;
            case DEC: strcpy(receivedName, "integer"); break;
            case FLT: strcpy(receivedName, "float"); break;
            case ASGN: strcpy(receivedName, "':=' (assignment)"); break;
            case EXIT: strcpy(receivedName, "'exit'"); break;
            case QUIT: strcpy(receivedName, "'quit'"); break;
            default: snprintf(receivedName, sizeof(receivedName), "token %d", lookahead); break; //Token desconhecido
        }
    }

    // Mensagem de erro padronizada
    fprintf(stderr, "\nERROR at line %d, column %d.\n", lineno, columno);
    fprintf(stderr, "Token mismatch: expected %s but received %s.\n", expectedName, receivedName);

	parse_error = 1;
	while (lookahead != EOF && lookahead != ';' && lookahead != '\n') {
        lookahead = gettoken(source);
    }
	return;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lexer.h>
#include <tokens.h>
#include <keywords.h>
#include <parser.h>

int lookahead;  // Este é o olho do compilador
extern FILE *objcode;

// tabela de símbolos como dicionário dos valores armazenados na memória virtual
#define MAXSTENTRIES 4096
char symtab[MAXSTENTRIES][MAXIDLEN+1];
int symtab_next_entry = 0; // uso: strcpy(symtab[symtab_next_entry], name);

// program -> PROGRAM ID '(' idlist ')' ';' block '.'
void program(void)
{
	match(PROGRAM); match(ID); match('('); idlist(); match(')'); match(';'); block(); match('.');
}

// idlist -> ID (',' ID)
void idlist(void)
{
	match(ID);
	while (lookahead == ','){
		match(',');
		match(ID);
	}
}

//void idlist(void)
//{
//_idhead:
//	match(ID);
//	while (lookahead == ','){
//		match(',');
//		goto _idhead;
//	}
//}

// block -> vardeclaration
//			sbprogram
//			begin_end
void block(void)
{
	vardeclaration();
	sbprogram();
	begin_end();
}

//begin_end -> BEGIN stmtlsit END
void begin_end(void)
{
	match(BEGIN); stmtlist(); match(END);
}

// colocar a gramática aqui
void stmtlist(void)
{
_stmtlist_head:
	stmt();
	if (lookahead == ';') {
		match(';'); goto _stmtlist_head;
	}
}


// vardeclaration -> [ VAR idlist ':' type ';' ( idlist ':' type ';' ) ]
void vardeclaration(void)
{
	if (lookahead == VAR) {
		match(VAR); 
_idlist_head:
		idlist();
		match(':');
		type();
		match(';');
		if (lookahead == ID) {
			goto _idlist_head;
		}
	}
}

// type -> INTEGER | REAL | BOOLEAN
void type(void)
{
	switch (lookahead) {
		case INTEGER:
			match(INTEGER); break;
		case REAL:
			match(REAL); break;
		default:
			match(BOOLEAN);
	}
}

// sbprogram -> { PROCEDURE ID parmlist ';' block ';' | FUNCTION ID parmlist ':' type ';' block ';' }
void sbprogram(void)
{
	while (lookahead == PROCEDURE || lookahead == FUNCTION) {
		if (lookahead == PROCEDURE) {
			match(PROCEDURE); match(ID); parmlist(); match(';'); block(); match(';');
		} else {
			match(FUNCTION); match(ID); parmlist(); match(':'); type(); match(';'); block(); match(';');
		}
	}
}

// definir parmlist
void parmlist(void)
{
    if (lookahead == '(') {
        match('(');
      _parmlist_head:
		if (lookahead == VAR) {
			match(VAR);
		}
		idlist();
		match(':');
		type();
		if (lookahead == ';') { 
			match(';');
			goto _parmlist_head;
		}
		match(')');
    }
}

void arglist(void)
{
	if (lookahead == '(') {
		match('(');
_exprlist_head:
		expression();
		if (lookahead == ',') {
			match(',');
			goto _exprlist_head;
		}
		match(')');
	}
}

// void idstmt(void)
// {
// 	match(ID); arglist();
// }

void idstmt(void)
{
    char varname[MAXIDLEN+1];
    strcpy(varname, lexeme);
    match(ID);

    if (lookahead == ASGN) {
        match(ASGN);
        expression();
        fprintf(objcode, "\tmovl %%eax, %s\n", varname);
    } else {
        arglist();
    }
}


void ifstmt(void)
{
	match(IF); expression(); match(THEN); stmt(); 
	if (lookahead == ELSE) {
		match(ELSE); stmt();
	}
}

void whilestmt(void)
{
	match(WHILE); expression(); match(DO); stmt();
}

void repstmt(void)
{
	match(REPEAT); stmtlist(); match(UNTIL); expression();
}


void stmt(void)
{
	switch (lookahead) {
		case ID: // pode ser ID de variável, ID de funcao ou ID de procedimento
			idstmt(); break;
		case BEGIN:
			begin_end(); break;
		case IF:
			ifstmt(); break;
		case WHILE:
			whilestmt(); break;
		case REPEAT:
			repstmt(); break;
		default:
		; // empty statment
	}
}

int isrelop(void)
{
	switch(lookahead) {
	case '<':
	case LEQ:
	case NEQ:
	case '>':
	case GEQ: 	
	return lookahead;
	}
	return 0;
}

void expression(void)
{
	simple_expression();
	if (isrelop()) { 
		match(lookahead);
		simple_expression();
	}
}
// ominus = ['+''-']
// oplus = ['+''-']
void simple_expression(void)
{
	/*0*/char varname[MAXIDLEN+1];/**/
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
			match('('); expression(); match(')');
			break;
		case DEC:// int32
			/**/fprintf(objcode, "\tmovl $%s, %%eax\n", lexeme);/**/
			match(DEC); break;
		case FLT://flt32
			match(FLT); break;
		default:
			// F -> ID [ := E ] 
			/**/strcpy(varname, lexeme);/**/ // tem que salvar antes do match senão perde o
							 				// nome
			match(ID);
			if (lookahead == ASGN) {
				match(ASGN);
				expression(); //trás o resultado no acumulador (acc), também conhecido como RVAL
			}else{
				fprintf(objcode, "\tmovl %s, %%eax\n", varname);
			}
	}
	// factor end
	
	/*9*/
	if(isotimes) {
		if (isotimes == '*') {
			fprintf(objcode, "\timull (%%esp)\n");
			fprintf(objcode, "\taddl $4, %%esp\n");
		} else {
			fprintf(objcode, "\tmovl %%eax, %%ecx\n");
			fprintf(objcode, "\tpopl %%eax\n");
			fprintf(objcode, "\tcltq\n");
			fprintf(objcode, "\tidivl %%ecx\n");
		}
		isotimes = 0;
	}
	/**/

	if(lookahead == '*' || lookahead == '/') {
		/**/isotimes = lookahead;/**/
		fprintf(objcode, "\tpushl %%eax\n");
		match(lookahead); goto _Fbegin;
	}
	// term end

	/*11*/
	if (isnegate) {
		fprintf(objcode, "\tnegl %%eax\n");
		isnegate = 0;
	}
	/**/

	/**/
	if(isoplus) {
		if (isoplus == '+') {
			fprintf(objcode, "\taddl %%eax, (%%esp)\n");
			// stack[sp] = stack[sp] + acc ;
		} else {
			fprintf(objcode, "\tsubl %%eax, (%%esp)\n");
			// stack[sp] = stack[sp] - acc ;
		}
		// acc = stack[sp--];
		fprintf(objcode, "\tpopl %%eax\n");

		isoplus = 0;
	}
	/**/

	if(lookahead == '+' || lookahead == '-') {
		/**/ isoplus = lookahead; /**/
		// stack[++sp] = acc;
		/**/ fprintf(objcode, "\tpushl %%eax\n"); /**/
		match(lookahead); goto _Tbegin;
	}
	// expression end

}

//////////////////////////// parser components /////////////////////////////////
int lookahead;
void match(int expected)
{
	if (lookahead == expected) {
		lookahead = gettoken(source);
	} else {
		fprintf(stderr, "token mismatch at line %d\n", lineno);
		exit(ERRTOKEN);
	}
}
#pragma once
#define ERRTOKEN -0x10000000

extern int lookahead;
extern double acc;

extern char lexeme[];

extern int lineno;
extern int columno;
extern int parse_error;

extern FILE *source;

extern void match(int);

extern int gettoken(FILE *);

extern void handleSigint(int);
extern void mybc(void);
extern void cmd(void);
extern void E(void);
extern void T(void);
extern void F(void);
extern void Q(void);
extern void R(void);

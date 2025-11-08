#define ERRTOKEN -0x10000000

extern int lookahead;
extern double acc;

extern char lexeme[];

extern int lineno;

extern FILE *source;

extern void match(int);

extern int gettoken(FILE *);

/* extern void mybc(void);
extern void cmd(void);
extern void T(void);
extern void F(void);
extern void Q(void);
extern void R(void); */

extern void program(void);
extern void type(void);
extern void parmlist(void);
extern void vardeclaration(void);
extern void sbprogram(void);
extern void begin_end(void);
extern void stmtlist(void);
extern void stmt(void);
extern void idlist(void);
extern void block(void);
extern void simple_expression(void);
extern void expression(void);
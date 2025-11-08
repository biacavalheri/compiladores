HDIR = .
CFLAGS = -g -I$(HDIR)

mypas: lexer.o main.o parser.o keywords.o symtab.o
	$(CC) $^ -o $@

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~

targz: # pra entregar pro prof
	tar zcvf mypas_grp0_`date "+%Y%m%d"`.tar.gz Makefile *.[ch]
# pra extrair:
# 	tar zxvf mypas.tar.gz

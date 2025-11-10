HDIR = .
CFLAGS = -g -I$(HDIR)

mybc: lexer.o main.o parser.o
	$(CC) $^ -o $@

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~

targz: # pra entregar pro prof
	tar zcvf mybc_`date "+%Y%m%d"`.tar.gz Makefile *.[ch] ./versioned
# pra extrair:
# 	tar zxvf mybc.tar.gz
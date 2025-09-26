HDIR = .
CFLAGS = -g -I$(HDIR)

mybc: lexer.o main.o parser.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) =.o

mostlyclean: clean
	$(RM) *~

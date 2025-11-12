##############################################################
# Makefile do projeto "mybc" — Interpretador de Expressões
# ------------------------------------------------------------
# Este arquivo define o processo automatizado de compilação,
# limpeza e empacotamento do projeto.
#
# O `make` é uma ferramenta utilizada para gerenciar dependências
# e simplificar a recompilação de programas compostos por vários
# arquivos-fonte.
#
# Alvos principais:
#   - mybc        : compila o interpretador completo
#   - clean       : remove arquivos objeto (.o)
#   - mostlyclean : remove temporários e backups
#   - targz       : empacota tudo em um .tar.gz para entrega
##############################################################

# Diretório de cabeçalhos (.h)
HDIR = .

# Opções de compilação:
# -g       → inclui informações de debug (útil para gdb)
# -I$(HDIR) → adiciona o diretório de cabeçalhos ao include path
CFLAGS = -g -I$(HDIR)

##############################################################
# Alvo principal: compila o interpretador mybc
# ------------------------------------------------------------
# O interpretador é formado pelos módulos:
#   lexer.o  → analisador léxico
#   main.o   → programa principal
#   parser.o → analisador sintático / interpretador
#
# A variável $^ representa todas as dependências listadas,
# e $@ representa o nome do alvo (mybc).
##############################################################
mybc: lexer.o main.o parser.o
	$(CC) $^ -o $@

##############################################################
# Alvo de limpeza:
# Remove os arquivos objeto (.o) gerados pela compilação.
# Útil para recompilar o projeto do zero.
##############################################################
clean:
	$(RM) *.o

##############################################################
# Alvo de limpeza mais completa:
# Remove arquivos temporários e de backup (como *~).
##############################################################
mostlyclean: clean
	$(RM) *~

##############################################################
# Alvo de empacotamento (.tar.gz)
# ------------------------------------------------------------
# Cria um arquivo compactado contendo:
#   - Makefile
#   - todos os arquivos .c e .h
#   - diretório ./versioned (se existir)
#
# O nome do arquivo inclui a data atual (YYYYMMDD).
# Exemplo de saída: mybc_20251111.tar.gz
#
# Também há instrução de como extrair o pacote.
##############################################################
targz: # pra entregar pro prof
	tar zcvf mybc_`date "+%Y%m%d"`.tar.gz Makefile *.[ch] ./versioned

# pra extrair:
#   tar zxvf mybc.tar.gz

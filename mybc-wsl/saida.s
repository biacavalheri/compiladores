/****************************************************/
/*
Arquivo: saida.s
Módulo: Saída Simulada de Código Assembly
----------------------------------------------------------
Descrição geral:
Este arquivo representa a saída (geração de código) produzida
pelo interpretador/compilador após a análise de expressões
aritméticas. Ele não é executado diretamente pelo parser, mas
serve como exemplo ilustrativo da tradução de expressões
de alto nível para código Assembly (fase de geração de código).

O conteúdo mostra instruções x86 simplificadas geradas a partir
de uma expressão, simulando a manipulação de variáveis na pilha
e nos registradores.

----------------------------------------------------------
Durante a execução de um compilador completo, o processo de
tradução é dividido em:
  1. Análise léxica → identificação de tokens
  2. Análise sintática → verificação da estrutura da expressão
  3. Análise semântica → interpretação dos significados
  4. Geração de código → tradução para Assembly

O arquivo `saida.s` representa o resultado final da etapa (4).
*/
/****************************************************/

Iniciando a compilação...

    movl x, %eax          # Move o valor da variável x para o registrador %eax
    pushl %eax            # Empilha o valor atual de %eax (salva na pilha)
    movl y, %eax          # Move o valor da variável y para %eax
    addl %eax, (%esp)     # Soma o valor de y com o valor no topo da pilha
    popl %eax             # Desempilha o topo da pilha de volta em %eax
    movl %eax, y          # Armazena o resultado da soma na variável y
    movl x, %eax          # Move novamente x para %eax (início de outra expressão)
    movl y, %eax          # Move y para %eax (possível reutilização de resultado)

Compilação concluída.

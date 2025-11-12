/****************************************************/
/*
----------------------------------------------------------
Descrição geral:
Ponto de entrada (`main`) do interpretador.

Sua função é inicializar o ambiente de execução do compilador,
configurar o tratamento de sinais (Ctrl+C), preparar os fluxos de
entrada e saída (source e objcode), e invocar o analisador sintático
principal (`mybc()`), que inicia o processo de interpretação.

Fluxo geral de execução:
1. Configura o manipulador de interrupção (Ctrl+C → handleSigint).
2. Lê o primeiro token da entrada (fornecido pelo lexer).
3. Chama o analisador sintático `mybc()`, que interpreta os comandos.
4. Finaliza a execução com retorno 0.

----------------------------------------------------------
Observações:
- O programa lê da entrada padrão (`stdin`) e escreve na saída padrão (`stdout`).
- Ao receber SIGINT (Ctrl+C), imprime apenas uma quebra de linha
  sem encerrar o processo, garantindo robustez interativa.
*/
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <main.h>
#include <signal.h>

/****************************************************
 * Variáveis globais:
 * --------------------------------------------------
 * source  → ponteiro para o arquivo de entrada (programa-fonte).
 * objcode → ponteiro para o arquivo de saída (geração simbólica, se houver).
 ****************************************************/
FILE *source, *objcode;

/****************************************************
 * Função principal do interpretador (main)
 * --------------------------------------------------
 * Configura o ambiente e inicia o processo de interpretação.
 ****************************************************/
int main(void)
{
	signal(SIGINT, handleSigint); // Instala manipulador para Ctrl+C

	lookahead = gettoken(source = stdin); // Lê o primeiro token da entrada
	objcode = stdout;                     // Saída padrão (pode gerar assembly ou resultado)

	mybc(); // Inicia o analisador sintático principal (interpretador)

	return 0; // Execução concluída com sucesso
}

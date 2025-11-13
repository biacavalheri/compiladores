/****************************************************/
/*
Trabalho de Compiladores – Interpretador de Expressões

Autores: Beatriz Cavalheri, Eduardo Bahia, Larissa Ferrari e Maysa Marques
Data da ultima atualização:  13/11/2025
Disciplina: Compiladores
Professor: Eraldo Marinho

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
 * source  → fluxo de entrada do interpretador (ex.: stdin).
 * objcode → fluxo de saída do interpretador (ex.: stdout).
 *
 * Observação: são streams FILE*, não caminhos de arquivo.
 ****************************************************/
FILE *source, *objcode;

/****************************************************
 * Função principal do interpretador (main)
 * --------------------------------------------------
 * Configura o ambiente e inicia o processo de interpretação.
 ****************************************************/
int main(void)
{
	// SIGINT é o sinal enviado quando você pressiona Ctrl+C no terminal.
	// handleSigint é a função que trata esse sinal.
	signal(SIGINT, handleSigint); // Instala manipulador para Ctrl+C. Se o usuário apertar Ctrl+C, em vez de fechar o programa, chame a função handleSigint()

	lookahead = gettoken(source = stdin); // Lê o primeiro token da entrada
	objcode = stdout;                     // Saída padrão (pode gerar assembly ou resultado)

	mybc(); // Inicia o analisador sintático principal (interpretador)

	return 0; // Execução concluída com sucesso
}

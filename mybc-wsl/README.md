<h1> Documentação Suplementar — Interpretador <code>mybc</code></h1>

<h2>1. Visão Geral do Projeto</h2>

<p>
O projeto <strong>mybc</strong> implementa um <em>interpretador interativo</em> de expressões aritméticas, inspirado na ferramenta <code>bc</code> do Unix.
Ele é estruturado em módulos que representam as principais etapas de um compilador tradicional:
</p>

<ol>
  <li><strong>Análise Léxica (Lexer)</strong> — identifica tokens a partir do texto de entrada;</li>
  <li><strong>Análise Sintática (Parser)</strong> — valida a estrutura das expressões conforme uma gramática LL(1);</li>
  <li><strong>Tratamento de Erros</strong> — informa linha, coluna e tokens esperados/encontrados;</li>
  <li><strong>Interpretação/Semântica</strong> — avalia as expressões e gerencia variáveis em tempo de execução.</li>
</ol>

<p>
Embora o projeto seja um <em>interpretador</em>, ele adota princípios de compiladores acadêmicos, como pilha de avaliação, tabela de símbolos e análise recursiva descendente.
</p>

<hr/>

<h2>2. Estrutura Modular</h2>

<h3>2.1. Módulo Léxico (<code>lexer.c</code> / <code>lexer.h</code>)</h3>
<p>
Responsável por transformar a sequência de caracteres digitada pelo usuário em <strong>tokens</strong> compreensíveis pelo parser.
</p>
<p>Reconhece:</p>
<ul>
  <li>Identificadores (<code>ID</code>) — nomes de variáveis;</li>
  <li>Números (<code>DEC</code>, <code>FLT</code>);</li>
  <li>Operador de atribuição <code>:=</code>;</li>
  <li>Palavras especiais <code>exit</code> e <code>quit</code>;</li>
  <li>Operadores <code>+</code>, <code>-</code>, <code>*</code>, <code>/</code> e delimitadores <code>;</code>, <code>(</code>, <code>)</code>.</li>
</ul>
<p>
O lexer também controla linha e coluna, filtra setas do teclado (← → ↑ ↓) e trata interrupções <kbd>Ctrl+C</kbd>.
</p>

<h3>2.2. Módulo de Palavras-Chave (<code>keywords.c</code> / <code>keywords.h</code>)</h3>
<p>
Mantém o dicionário de palavras reservadas (ex: <code>BEGIN</code>, <code>IF</code>, <code>WHILE</code>, <code>END</code>).
Serve como base para futuras expansões da linguagem.
A função <code>iskeyword()</code> compara o lexema reconhecido com o vetor <code>keywords[]</code>.
</p>

<h3>2.3. Módulo Sintático (<code>parser.c</code> / <code>parser.h</code>)</h3>
<p>
O parser realiza a análise sintática das expressões, verificando se obedecem à gramática LL(1):
</p>

<pre><code>
mybc -> cmd { cmdsep cmd } EOF
cmd  -> E | exit | quit | &lt;ε&gt;
E    -> [ '+' | '-' ] T { ('+' | '-') T }
T    -> F { ('*' | '/') F }
F    -> '(' E ')' | DEC | FLT | ID [ ':=' E ]
</code></pre>

<p>
Cada regra é implementada como uma função recursiva. A função <code>E()</code> aplica a precedência de operadores usando uma pilha de avaliação e um acumulador global (<code>acc</code>).
</p>
<p>
A função <code>match()</code> valida tokens e emite mensagens de erro descritivas com linha, coluna e tokens esperados/recebidos. O parser implementa <strong>recuperação de erros</strong>, descartando tokens até o final do comando.
</p>

<h3>2.4. Tabela de Símbolos</h3>
<p>
Implementada com vetores paralelos:
</p>
<ul>
  <li><code>symtab[]</code> — nomes das variáveis;</li>
  <li><code>vmem[]</code> — valores correspondentes;</li>
  <li><code>recall()</code> — busca variável existente;</li>
  <li><code>store()</code> — cria/atualiza variável.</li>
</ul>
<p>
Simula uma memória persistente onde os valores são armazenados entre execuções de comandos.
</p>

<h3>2.5. Módulo Principal (<code>main.c</code> / <code>main.h</code>)</h3>
<p>
Inicializa o interpretador, configurando o manipulador de <kbd>Ctrl+C</kbd>, obtendo o primeiro token e chamando <code>mybc()</code>.
Lê da entrada padrão e escreve na saída padrão.
</p>

<h3>2.6. Arquivo de Saída (<code>saida.s</code>)</h3>
<p>
Simula a geração de código Assembly x86 a partir das expressões analisadas.
Serve como exemplo didático da etapa de geração de código de um compilador completo.
</p>

<h3>2.7. Makefile</h3>
<p>
Automatiza a compilação e empacotamento do projeto.
</p>
<ul>
  <li><code>make mybc</code> — compila o interpretador;</li>
  <li><code>make clean</code> — remove arquivos objeto;</li>
  <li><code>make targz</code> — cria o pacote de entrega.</li>
</ul>

<hr/>

<h2>3. Métodos e Decisões de Implementação</h2>

<ul>
  <li><strong>Abordagem LL(1) recursiva descendente:</strong> simples, direta e ideal para aprendizado de compiladores.</li>
  <li><strong>Função <code>match()</code>:</strong> fornece mensagens de erro detalhadas e recuperação sem abortar a execução.</li>
  <li><strong>Tabela de símbolos em arrays fixos:</strong> prioriza clareza didática; pode ser substituída por hash em versões futuras.</li>
  <li><strong>Pilha de avaliação:</strong> respeita a precedência dos operadores sem construir árvores sintáticas.</li>
  <li><strong>Tratamento de sinais <kbd>Ctrl+C</kbd>:</strong> evita encerramentos abruptos e mantém a interatividade.</li>
  <li><strong>Filtro de setas do teclado:</strong> evita corrupção do buffer de entrada (<code>stdin</code>).</li>
</ul>

<hr/>

<h2>4. Considerações Finais</h2>

<p>
O projeto <strong>mybc</strong> demonstra a aplicação prática dos conceitos fundamentais de compiladores — análise léxica, sintática e semântica — em um ambiente interativo.
</p>

<p>
Ele combina clareza didática com robustez técnica, apresentando:
</p>

<ul>
  <li>Parser LL(1) funcional;</li>
  <li>Tratamento de erros com recuperação;</li>
  <li>Gerenciamento de variáveis simbólicas;</li>
  <li>Integração modular entre lexer, parser e main;</li>
  <li>Potencial expansão para geração de código Assembly.</li>
</ul>

<p>
Como trabalho acadêmico, o <em>mybc</em> serve não apenas como produto final funcional, mas também como material de estudo sobre o fluxo de tradução de linguagens e a arquitetura de interpretadores.
</p>

Iniciando a compilação...
	movl x, %eax
	movl y, %eax
	movl x, %eax
	pushl %eax
	movl y, %eax
	addl %eax, (%esp)
	popl %eax
	movl %eax, y
	movl x, %eax
	pushl %eax
	movl y, %eax
	subl %eax, (%esp)
	popl %eax
	movl %eax, x
Compilação concluída.

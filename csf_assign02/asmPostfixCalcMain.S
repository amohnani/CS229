/*
 * postfix calculator (assembly language version)
 * main function
 */

.section .rodata

mArgv1Msg: .string "argv[1] is %s\n"
mResult:	.string "Result is: %ld\n"
mBadArguments:	.string "Wrong number of arguments.\n"
	
.section .text

/*
 * main function: program entry point
 *
 * Params:
 *   argc - number of command line arguments (including program name)
 *   argv - array of pointers to command line arguments
 *
 * Returns:
 *   exit code
 */
	.globl main
main:
	subq $8, %rsp                /* align stack pointer */

	/*
	 * The code below uses printf to print the first command line
	 * argument (argv[1]).  You should replace this code with your
	 * actual postfix calculator implementation.
	 */
	
	//movq $mArgv1Msg, %rdi
	cmpq $2, %rdi
	je .LCalc
	movq $mBadArguments, %rdi
	call printf
	movq $1, %rax
	addq $8, %rsp
	ret
	
.LCalc:
	
	movq 8(%rsi), %rsi //gets string input
	movq %rsi, %rdi //puts string in first arg register
	call eval //calls evaluation function
	movq %rax, %rsi //puts result of eval in second argument
	movq $mResult, %rdi //puts output string in first argument
	call printf //calls printf

	/*
	 * Return 0 from main.
	 */
	movq $0, %rax

	addq $8, %rsp                /* restore stack pointer */

	ret

/* vim:ft=gas:
 */

/*
 * postfix calculator (assembly language version)
 * function implementation
 */

.section .rodata

/* Example string constant, refer to its address using $eUnknownTokenType */
eUnknownTokenType: .string "Error: Unknown token type"
eInvalidExpression: .string "Error: Invalid Expression.\n"
eLargeOperandStack:	.string "Error: Operand stack is too large.\n"
eNotEnoughOperands:	.string "Error: Not enough operands.\n"
eInvalidChar:	.string "Error: Invalid character.\n"

	
.section .bss
	.align 16
stack:	 .space (20*8)
pCount:	 .space (8)
pVal:	 .space (8)
	
.section .text

	
	.globl eval
	
eval:
	subq $8, %rsp
	pushq %r15
	pushq %r13
	pushq %r12
	pushq %r9
	pushq %rbp
	pushq %rbx
	
	movq %rdi, %r15 //r15 now stores string pointer
	

	//movq $0, %rbp //stores intsover

	jmp .LCheck
.LMainLoop:
	movq %r15, %rdi //sends string pointer to rdi
	call skipws //calls skipws with rdi
	movq %rax, %r15 //puts new string pointer into r15
	movq %r15, %rdi //moves string pointer back to rdi
	call strlen //calls strlen with string pointer

	cmpq $0, %rax //checks if strlen returned 0
	movq %r15, %rdi //restores rdi to string pointer
	je .LReturnVal //if so, jumps to end of function
	movq $0, %rax //moves 0 to rax
	call tokenType //checks tokenType
	movq %rax, %r9
	cmpq $0, %r9 //checks if token type is 0

	je .LTokenDigit //if equal to 0, jump to case digit
	jg .LTokenOperand //if greater, jump to case operator
	movq $eInvalidChar, %rdi //otherwise, set invalid char
	jmp .LError //and call error
	
.LTokenDigit:
	/*cmpq $1, %rbp //checks if intsover is false
	movq %rdi, %r10 //stores rdi in r10
	movq $eInvalidExpression, %rdi //stores invalid expression in rdi
	je .LError //if intsover is false, jumps to Error
	movq %r10, %rdi //restores rdi*/

	cmpq $20, pCount //checks if operand stack has reached limit
	movq %rdi, %r10 //stores rdi in r10
	movq $eLargeOperandStack, %rdi //stores invalid expression in rdi
	je .LError //checks if stack has reached limit
	movq %r10, %rdi //restores rdi 

	movq %r15, %rdi //sets rdi to string pointer
	movq $pVal, %rsi //sets rsi, second arg, to address of pVal
	call consumeInt //calls consumeInt
	movq %rax, %r15 //moves new string pointer to r15

	movq $stack, %rdi //moves stack to rdi
	movq $pCount, %rsi //sets rsi, second arg, to address of pCount
	movq pVal, %rdx //moves pval to rdx, third arg
	call stackPush //calls stackPush
	jmp .LCheck //jumps to condition of while loop


.LTokenOperand:
	//movq $1, %rbp //sets intsover to true
	cmpq $1, pCount //checks how many operands are on stack
	movq %rdi, %r10 //stores rdi in r10
	movq $eNotEnoughOperands, %rdi //sets rdi to not enough operands
	jbe .LError //if less than or equal to 1 operand, error
	movq %r10, %rdi //restores rdi

	movq $stack, %rdi //moves stack into first argument
	movq $pCount, %rsi //moves address of pCount into second argument
	call stackPop //calls stackPop
	movq %rax, %rbx //stores val 1 in rbx

	movq $stack, %rdi //moves stack into first argument
	movq $pCount, %rsi //moves address of pCount into second argument
	call stackPop //calls stackPop
	movq %rax, %r12 //stores val 2 in r12

	cmpq $1, %r9 //case of addition
	je .LAdd

	cmpq $2, %r9 //case of subtraction
	je .LSubtract

	cmpq $3, %r9 //case of multiplication
	je .LMultiply

	cmpq $4, %r9 //case of division
	je .LDivide
.LAdd:
	addq %rbx, %r12 //adds two values and stores result in r12
	jmp .LEndOfOperand
.LSubtract:
	subq %rbx, %r12 //subtracts r12 - rbx and stores result in r12
	jmp .LEndOfOperand
.LMultiply:
	imulq %rbx, %r12 //multiplies rbx by r12 and stores result in r12
	jmp .LEndOfOperand
.LDivide:	
	movq $0, %rdx //moves 0 to rdx
	movq %r12, %rax //moves r12 to dividend
	divq %rbx //divides rax by rbx and stores reuslt in rax
	movq %rax, %r12 //moves rax to r12 to store result there
	jmp .LEndOfOperand
.LEndOfOperand:
	movq $0, %rdx //turns rdx to 0
	movq $stack, %rdi //puts stack in first argument
	movq $pCount, %rsi //puts address of pCount in second argument
	movq %r12, %rdx //puts result of computation in third argument
	call stackPush //calls stackPush

	incq %r15 //moves string pointer
	
	jmp .LCheck
	
.LCheck:
	movq %r15, %rdi //moves string pointer to rdi
	call strlen //calls strlen
	movq %r15, %rdi //restores rdi
	cmpq $0, %rax //compares length of string to 0
	ja .LMainLoop //if greater, return to loop

.LReturnVal:

	movq %rdi, %r10 //temporarily stores rdi
	cmpq $1, pCount //checks if length of stack is 1
	movq $eInvalidExpression, %rdi //stores invalid string in rdi
	je .LFinishUp //if length of stack is 1, moves to return val



.LError: //function for any error
	call printf //calls printf with already set rdi
	movq $1, %rax //sets rdi to 1
	call exit //calls exit with 1

.LFinishUp:
	movq %r10, %rdi //restores rdi 
	movq $stack, %rdi //moves stack to rdi
	movq $pCount, %rsi //stores address of pCount in rsi
	call stackPop //calls stackPop and stores result in rax
	
	
	popq %rbx //pops local vars used in function
	popq %rbp
	popq %r9
	popq %r12
	popq %r13
	popq %r15
	addq $8, %rsp //aligns stack pointer
	ret //returns rax which was calculated by stackPop


	
	.globl stackPush
stackPush:
	subq $8, %rsp //subtract stack pointer

	pushq %r13 //store local var on stack
	
	movq (%rsi), %r13 //move value of pCount into r13
	imulq $8, %r13 //multiply by 8 to get number of bytes
	addq %r13, %rdi //add r13 to rdi to get &stack[pCount]
	movq %rdx, (%rdi) //put val into stack[pCount]
	incq (%rsi) //increment pCount
	
	popq %r13 //pop local var from stack
	addq $8, %rsp //align stack pointer
	ret //exit function

	
	.globl stackPop
stackPop:	
	subq $8, %rsp //align stack 
	pushq %r13 //store local var on stack
	
	addq $-1, (%rsi) //decrement pCount
	movq (%rsi), %r13 //store the value of pCount in r13
	imulq $8, %r13 //multiply r13 by 8 to get number of bytes
	addq %r13, %rdi //add r13 to rdi to get &stack[pCount]
	movq (%rdi), %rax //move value at stack[pCount] to rax

	popq %r13 //remove local var from stack
	addq $8, %rsp //align stack pointer
	ret //return rax

	
	
	.globl consumeInt
consumeInt:
	subq $8, %rsp //align stack pointer

	pushq %rbx //store local vars in stack
	pushq %rbp
	pushq %r14
	pushq %r15
	pushq %r13

	movq $0, %rbx //rbp is counter
	movq $0, %rbp //rbp is temp that stores eventual integer
	movq %rdi, %r13 //store value of rdi in r13
	call strlen //gets length of string
	movq %r13, %rdi //restore rdi
	movq %rax, %r14 //r14 stores length of string
	movq $0, %rax //set rax back to 0
	movq $0, %r15 //stores character at current address
	movb (%rdi), %r15b //sets r15 equal to the s[0]
	jmp .LLoopCondition

.LLoopBody:
	imulq $10, %rbp //multiplies temp value by 10
	subq $48, %r15 //calculates char of r15 - '0'
	addq %r15, %rbp //adds value of r15 to temp
	movq %rdi, %r13 //stores rdi in r13
	incq %rbx //increments counter
	addq %rbx, %rdi //adds counter to rdi to get &s[counter]
	movb (%rdi), %r15b //moves s[counter] to r15
	movq %r13, %rdi //restores value of rdi
	

.LLoopCondition:
	cmpq %r14, %rbx //compares counter to length of string
	jae .LNewString //if the end has been reached, jumps to end of func
	movq %rdi, %r13 //stores rdi in r13
	movq %r15, %rdi //moves value of s[counter] into rdi
	call isDigit //calls isDigit
	movq %r13, %rdi //restores rdi
	cmpq $0, %rax //compares result of isDigit with 0
	je .LNewString //if the character is not an int, jumps to end of func
	jmp .LLoopBody //if conditions are met, loop continues

.LNewString:
	movq %rbp, (%rsi) //moves temp to *pVal
	addq %rbx, %rdi //adds counter to rdi to get &s[counter]
	movq %rdi, %rax //sets rax to s + counter

	popq %r13 //removes local vars from stack
	popq %r15
	popq %r14
	popq %rbp
	popq %rbx
	
	addq $8, %rsp //aligns stack
	ret //returns rax


	
	.globl skipws
skipws:
	subq $8, %rsp
	pushq %rbx //pushes local variables to stack
	pushq %r13
	pushq %r14
	pushq %r15
	movq $0, %rbx //sets counter to 0
	movq %rdi, %r14
	call strlen
	movq %rax, %rbp //puts length of string in rbp
	movq $0, %rax
	movq $0, %rdi
	movq %r14, %rdi //restores rdi
	jmp .LCheckCond
.LLoop:
	incq %rbx //body of loop - increments rbx
.LCheckCond:
	cmpq %rbp, %rbx //compares counter and length of string
	jae .LFini //jumps if length is greater than or equal to counter
	movq %rdi, %r12 //stores string pointer in r12
	addq %rbx, %rdi //adds the counter to the string pointer
	movq %rdi, %r13 //stores s + counter in r13
	movq $0, %rdi //stores 0 in rdi
	movb (%r13), %dil //stores s[counter] in lower byte of rdi
	call isSpace //checks isSpace
	movq %r12, %rdi //restores rdi
	cmpq $0, %rax //checks if the char is not a space
	je .LFini //if so, ends loop
	jmp .LLoop //otherwise, starts the loop again
.LFini:
	addq %rbx, %rdi //adds counter to original string pointer
	movq %rdi, %rax //moves rdi to return value

	popq %r15 //pops local variables from stack
	popq %r14
	popq %r13
	popq %rbx
	addq $8, %rsp //align stack pointer
	ret


	
	.globl tokenType
tokenType:
	subq $8, %rsp //align stack pointer
	pushq %rbp
	movq %rdi, %rbp //store address of array in rbp
	movq $0, %rdi //set rdi to 0
	movb (%rbp), %dil //store value of s[0] in lower byte of rdi
	call isDigit //call isDigit with s[0]
	
	movq %rax, %r11 
	cmpq $1, %r11 //check if isDigit returned true
	je .LDigit
	movq %rbp, %rdi //return rbp to rdi
	movb (%rdi), %r9b //move the value of s[0] to r9b
	cmp $43, %r9b //check if s[0] = '+'
	je .LPlus
	cmp $45, %r9b //check if s[0] = '-'
	je .LMinus
	cmp $42, %r9b // check if s[0] = '*'
	je .LMult
	cmp $47, %r9b //check if s[0] = '/'
	je .LDiv
	jmp .LOther
.LPlus:
	movq $1, %rax //case for +
	jmp .LFinish
.LMinus:
	movq $2, %rax //case for -
	jmp .LFinish
.LMult:
	movq $3, %rax //case for *
	jmp .LFinish
.LDiv:
	movq $4, %rax //case for /
	jmp .LFinish
.LDigit:
	movq $0, %rax //case for digit
	jmp .LFinish
.LOther:
	movq $-1, %rax //if the character is invalid
	
.LFinish:
	popq %rbp
	addq $8, %rsp //aligns stack pointer and returns
	ret
	


	
	.globl isSpace
isSpace:
	subq $8, %rsp //align stack
	cmpq $9, %rdi //check if char is a tab
	je .LSpace
	cmpq $32, %rdi //check if char is a space
	je .LSpace
	movq $0, %rax //if not, set return value to 0
	jmp .LOut
.LSpace:
	movq $1, %rax //if space, set return value to 1
.LOut:
	addq $8, %rsp //align stack and return
	ret


	.globl isDigit
isDigit:
	subq $8, %rsp
	cmpq $47, %rdi //checks if ascii code of char is greater than '0'
	ja .LConditionOne //jumps to condition one if true
	movq $0, %rax //if not, sets return value to 0
	jmp .LDone
.LConditionOne:
	cmpq $58, %rdi //checks if ascii code of char is less than '9'
	jb .LConditionTwo //jumps to condition two if true
	movq $0, %rax //if not, sets return val to 0
	jmp .LDone
.LConditionTwo:
	movq $1, %rax //if both cases are true, sets return val to 1
.LDone:
	addq $8, %rsp //aligns stack pointer and returns
	ret
	
/*
 * addLongs function (this is just an example function)
 */
	.globl addLongs
addLongs:
	subq $8, %rsp                /* align stack pointer */

	movq %rsi, %rax              /* move second parameter to %rax */
	addq %rdi, %rax              /* add first parameter to second */
	/* the sum is now in %rax */

	addq $8, %rsp                /* restore stack pointer */
	ret

/* vim:ft=gas:
 */

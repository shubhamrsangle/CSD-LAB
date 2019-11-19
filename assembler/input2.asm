// Program 2 
// Program with nearly 30 different C instructions
// ASM code to perform Modulo opervar1ion  b = mod(a, m)

@100
D=A
@R0
M=D

@initial
0;JMP

(modulus)
	@R0
	M=M-1
	A=M
	D=M
	@var2
	M=D
	
	@R0
	M=M-1
	A=M
	D=M
	@var1
	M=D
	
	(LOOP)
		@var2
		D=M
		@var1
		D=D-M
		@END1
		D;JGT
		
		@var1
		D=M
		@var2
		D=D-M
		@var1
		M=D
		
		@LOOP
		0;JMP
	(END1)
	
	@var1
	D=M
	@R0
	M=M-1
	A=M
	A=M
	M=D	
	
	@R0
	M=M-1
	A=M
	A=M
	0;JMP

(initial)

@function
D=A
@R0
A=M
M=D
@R0
M=M+1

@b
D=A
@R0
A=M
M=D
@R0
M=M+1

@a
D=M
@R0
A=M
M=D
@R0
M=M+1

@m
D=M
@R0
A=M
M=D
@R0
M=M+1

@modulus
0;JMP

(function)
@function
0;JMP

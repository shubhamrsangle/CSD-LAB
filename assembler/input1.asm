// Program 1 to test assembler
// Program with 5 A type instruction

@69
D=A

@var
D=M

@R0
M=M+1

@function
D=A

@SCREEN
D=A

(initial)

@KBD
D=M
@initial
D;JEQ

@store
M=D
@initial
0;JMP
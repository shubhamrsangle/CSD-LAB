// Program 3 to Test Assembler
// Program with nearly 15 different type of jump instruction

@EQUAL
D;JEQ

@GREAT
D;JGT

@LESS
D;JLT

@EQUAL
D;JGT

@GREAT
D;JLT

@LESS
D;JEQ

@EQUAL
D;JGE

@GREAT
D;JNE

@LESS
D;JGE

@EQUAL
D;JNE

@GREAT
D;JMP

@LESS
D;JNE

(EQUAL)
@0
D=A
@c
M=D
@END
0;JMP

(GREAT)
@1
D=A
@c
M=D
@END
0;JMP

(LESS)
@2
D=A
@c
M=D
@END
0;JMP
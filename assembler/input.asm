

@11
D=A
@i
M=D
@11
D=A
@j
M=D
@i
D=M
@j
D=D-M
@EQUAL
D;JEQ

@GREAT
D;JGT

@LESS
D;JLT


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

(END)
@0
M=D



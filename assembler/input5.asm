// Program 5 to test assembler
// Any Program from 5b. Ramcheck

@17
D=A
@i
M=D

(LOOP)
	@i
	D=M
	@16384
	D=D-A
	@ENDLOOP
	D;JGE
	
	@11111
	D=A
	@i
	A=M
	M=D
	
	@i
	M=M+1
	
	@LOOP
	0;JMP
(ENDLOOP)
@ENDLOOP
0;JMP
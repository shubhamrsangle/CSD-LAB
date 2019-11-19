#Default values
#R0-R15 -> 0-15
#Screen -> 16384
#KBD    -> 24576
'''
SP      -> 0
LCL     -> 1
ARG     -> 2
THIS    -> 3
THAT    -> 4
'''
#Start Assigning Space in Ram for Variable from 16

#  dest = comp ; jump
#  1 1 1 a c1 c2 c3 c4 c5 c6 d1 d2 d3 j1 j2 j3
from collections import defaultdict as dd
builtIn={'R0':0,'R1':1,'R2':2,'R3':3,'R4':4,'R5':5,'R6':6,'R7':7,'R8':8,'R9':9,'R10':10,'R11':11,'R12':12,'R13':13,'R14':14,'R15':15,'SCREEN':16384,'KBD':24576,'SP':0,'LCL':1,'ARG':2,'THIS':3,'THAT':4}
import re
variables=dd(int)
current=[16]
Labels=[]
label_code={}
binary_dest={"":'000',"M":"001","D":"010","MD":"011", "A":"100","AM":"101", "AD":"110" }
binary_cmp={'0':'101010', '1':'111111', '-1':"111010", 'D':'001100', 'A':'110000', 'M':'110000',
                        '!D':'001101', '!A':'110001', '!M':'110001', '-D':'001111', '-A':'110011', '-M':'110011',
                        'D+1':'011111', 'A+1':'110111', 'M+1':'110111', 'D-1':'001110', 'A-1':'110010',
                        'M-1':'110010', 'D+A':'000010','A+D':'000010', 'D+M':'000010', 'M+D':'000010','D-A':'000010', 'D-M':'000010',
                        'A-D':'000111', 'M-D':'000111', 'D&A':'000000','A&D':'000000', 'D&M':'000000','M&D':'000000', 'D|A':'010101','A|D':'010101',
                        'D|M':'010101','M|D':'010101'}
binary_jmp={'':'000', 'JGT':'001', 'JEQ':'010', 'JGE':'011', 'JLT':'100', 'JNE':'101', 'JLE':'110', 'JMP':'111'}
'''binary_a={'0':'0', '1':'0', '-1':"0", 'D':'0', 'A':'0', 'M':'1',
                        '!D':'0', '!A':'0', '!M':'1', '-D':'0', '-A':'0', '-M':'1',
                        'D+1':'0', 'A+1':'0', 'M+1':'1', 'D-1':'0', 'A-1':'0',
                        'M-1':'1', 'D+A':'0', 'D+M':'1', 'D-A':'0', 'D-M':'1',
                        'A-D':'0', 'M-D':'1', 'D&A':'0', 'D&M':'1', 'D|A':'0',
                        'D|M':'1'}'''

#Remove single line and multiline comments from code 
#builtIn={'R0':0,'R1':1,'R2':2,'R3':3,'R4':4,'R5':5,'R6':6,'R7':7,'R8':8,'R9':9,'R10':10,'R11':11,'R12':12,'R13':13,'R14':14,'R15':15}
def comment_remover(text):
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " " # note: a space and not an empty string
        else:
            return s
    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)

#Removes Empty lines from code and Remove white spaces as they don't indicate anythong in asm
def preProcess(statements):
	final=[]
	for statement in statements:
		statement = ''.join(statement.split())
		if statement=='':
			continue
		else:
			final.append(statement)
	return final

def binary15bit(value):
	binary_rep=bin(value)[2:]
	if len(binary_rep)>15:
		print("Value Overflow")
		return "000000000000000"
	elif len(binary_rep)==15:
		return binary_rep
	else:
		ans='0'*(15-len(binary_rep))+binary_rep
		return ans
	return "000000000000000" #Dummy Default return


def aTranslator(value):
	if value.isdigit():
		return binary15bit(int(value))
	elif value in Labels:
		value_code=label_code[value]
		return binary15bit(value_code)
		
	else:
		try:
			value_code=builtIn[value]

		except:
			#print(value)
			if variables[value]==0:
				variables[value]=current[0]
				value_code=current[0]
				current[0]+=1
			else:
				value_code=variables[value]

		return binary15bit(value_code)
	return "000000000000000" #Dummy Default return

def cTranslator(statement):
	binary_code="111"
	dest=""
	cmp1=""
	jmp=""
	if '=' in statement:
		if ';' in statement:
			c=statement.index("=")
			d=statement.index(";")
			dest=statement[:c]
			cmp1=statement[c+1:d]
			jmp=statement[d+1:]
		else:
			c=statement.index("=")
			dest=statement[:c]
			cmp1=statement[c+1:]
	elif ';' in statement :
		d=statement.index(";")
		cmp1=statement[:d]
		jmp=statement[d+1:]

	#Handling "a" bit
	if 'M' in cmp1:
		a='1'
	else:
		a='0'

	#Handling dest
	try:
		dest_bits=binary_dest[dest]
	except:
		print("Invalid Destination in Instruction")
		dest_bits="555"

	#Handling cmp
	try:
		cmp_bits=binary_cmp[cmp1]
	except:
		print("Invalid Comparison value in Instruction")
		cmp_bits="555555"

	#Handling Jmp

	try:
		jmp_bits=binary_jmp[jmp]
	except:
		print("Invalid Jump value in Instruction")
		jmp_bits="555"

	#Final finary instruction
	binary_code=binary_code+a+cmp_bits+dest_bits+jmp_bits

	return binary_code


#Main Function
input_file=input("Input File Name\n")
f=open(input_file,'r')
content=f.read()
f.close()
f=open("output.hack",'w')
stripped_code=comment_remover(content)
statements=stripped_code.strip().split('\n')
final_statements=preProcess(statements)
hackCode=[]
#First Pass Finding Labels and Line numbers
line_num=0
for statement in final_statements:
	if statement[0]=='@':
		#A type instruction
		line_num+=1
	elif "=" in statement or ";" in statement:
		#C type instruction
		line_num+=1
	elif '(' in statement and ')' in statement:

		label=statement[1:len(statement)-1]
		Labels.append(label)
		try:
			dummy=label_code[label]
			print("Label Repeated after insruction number ",line_num)
		except:

			label_code[label]=line_num
	else:
		print("Invalid Instruction, Please check syntax of Instruction number ", line_num+1)

#print("Labels and their line_num  are: ", label_code)
#Seconf Pass for Translation
for statement in final_statements:
	if statement[0]=='@':
		#A type instruction
		translated='0'+aTranslator(statement[1:])
		hackCode.append(translated)
		f.write(translated)
		f.write('\n')
		#print(statement, translated)
	elif "=" in statement or ";" in statement:
		#C type instruction
		translated=cTranslator(statement)
		hackCode.append(translated)
		f.write(translated)
		f.write('\n')
		#print(statement,translated)
	else:
		continue
f.close()



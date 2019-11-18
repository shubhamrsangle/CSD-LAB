#include<iostream>
#include<string>

#include<fstream>
#include<unordered_map>



using namespace std;

/*std::tr1::unordered_map<string,string>addr;
std::tr1::unordered_map<string,string>mInst;
std::tr1::unordered_map<string,string>inst;*/

unordered_map<string,string>addr;
unordered_map<string,string>mInst;
unordered_map<string,string>inst;
int bcount;
int callcount;
string fname; //file name


void initialize()
{
	
	bcount=0;
	callcount=0;
	addr["local"]="LCL";
	addr["argument"]="ARG";
	addr["this"]="THIS";
	addr["that"]="THAT";
	addr["pointer"]="3";
	addr["temp"]="5";
	addr["static"]="16";



	mInst["push"] = "\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
	mInst["pop"] = "\n@SP\nM=M-1\nA=M\nD=M\n"; 



	inst["add"] = "\nM=M+D\n";
	inst["sub"] = "\nM=M-D\n";
	inst["and"] = "\nM=M&D\n";
	inst["or"] = "\nM=M|D\n";
	inst["neg"] = "\n@SP\nA=M\nM=-M";
	inst["not"] = "\n@SP\nA=M\nM=!M";
	inst["eq"]="\nD;JEQ\n";
	inst["lt"]="\nD;JLT\n";
	inst["gt"]="\nD;JGT\n";
}

string preValue()
{
	//return(mInst["pop"]+"@SP"+"\n"+"M=M-1"+"\n"+"@SP"+"\n"+"A=M"+"\n");
	return("\n@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\n@SP\nA=M\n");
}
string arithmetic(string first)
{
	
	string result="\n";
	if(first!="not" && first!="neg")
	{
		result+=preValue();
	}
	
	
	if(first=="eq" || first=="lt" || first=="gt")
	{
		
		result+="\nD=M-D\n@BOOL"+to_string(bcount)+inst[first]+"\n@SP\n"+"A=M\nM=0\n"+"@ENDBOOL"+to_string(bcount)+"\n0;JMP\n";
		
		result+="(BOOL"+to_string(bcount)+")\n";
		result+="@SP\nA=M\n";
		result+="M=-1\n";
		result+="(ENDBOOL"+to_string(bcount)+")\n";
		bcount++;
	}
	else
	{
		result+=inst[first];
	}
	return(result);
	
}


string push_pop(string first,string second,string third)
{
	string result="";
	if(first=="push")
	{
		if(second=="constant")
		{
			result+="\nD=A\n";
			//cout<<"-------------------"<<endl;
		}
		else
		{
			result+="A=D\n";
		}
		result+=mInst["push"];
		//cout<<"inside"<<result<<endl;
		//result+="@SP A=M M=D @SP M=M+1";
	}
	else if(first=="pop")
	{
		result+="\nD=A\n@R13\nM=D\n"+mInst["pop"];
		//cout<<mInst["pop"]<<endl;
		//result+="D=A @R13 M=D @SP M=M-1 A=M D=M";
		result+="\n@R13\nA=M\nM=D\n";
	}
	
	return(result);
}
string memoryInst(string first,string second,string third)
{
	/*int pos = vmline.find(' ');
	int pos2 = vmline.find(' ',pos+1);
	int pos3 = vmline.find(' ',pos2+1);
	
	string first = vmline.substr(0,pos);
	string second = vmline.substr(pos+1,pos2-pos-1);
	string third = vmline.substr(pos2+1,pos3-pos2-1);
	//cout<<"data"<<pos<<" "<<pos2<<" "<<pos3<<endl;
	//cout<<"s "<<second<<endl;*/
	string result="\n";
	if(second =="constant")
	{
		
		result+="\n@"+third;
		
	}
	else if(second == "static")
	{
		result+="\n@"+fname+"."+third;
	}
	else if(second=="pointer" || second=="temp")
	{
		result+="\n@R"+addr[second]+third;
	}
	else if(second=="local" || second =="argument" || second=="this" || second=="that")
	{
		result+="\n@"+addr[second];
		result+="\nD=M\n";
		result+="@"+third;
		result+="\nA=D+A\n";
	}
	
	result+=push_pop(first,second,third);
	//cout<<result<<endl;
	return(result);
	
}

string callInst(string second,string third)
{
	string result="\n";
	result+="@"+second+".RETURN"+to_string(callcount);
	
	result+="\nD=A"+mInst["push"];
	result+="@LCL\nD=M"+mInst["push"]; //push LCL
	result+="@ARG\nD=M"+mInst["push"];//push ARG
	result+="@THIS\nD=M"+mInst["push"];//push this
	result+="@THAT\nD=M"+mInst["push"];//push That
	result+="@SP\nD=M\n@"+third+"\nD=D-A\n@5\nD=D-A\n@ARG\nM=D\n";//ARG=SP-N-5
	result+="@SP\nD=M\n@LCL\nM=D\n";//LCL=SP
	result+="@"+second+"\n0;JMP\n";
	result+="("+second+".RETURN"+to_string(callcount)+")";
	callcount++;
	return(result);
}

string functionInst(string second,string third)
{
	string result="\n";
	result+="("+second+")\n";
	result+="@"+third;
	result+="\nD=A\n@"+second+".count\nM=D\n";
	result+="("+second+".loop)\n@"+second+".count\nD=M\n@"+second+".END\nD;JLE\n";
	result+="@0\nD=A\n"+mInst["push"];
	result+="@"+second+".count\nM=M-1\n";
	result+="@"+second+".loop\n0;JMP\n";
	result+="("+second+".END)";
	return(result);
}

string convertToASM(string vmline)
{
	int pos = vmline.find(' ');
	string first = vmline.substr(0,pos);
	int pos2 = vmline.find(' ',pos+1);
	string second = vmline.substr(pos+1,pos2-pos-1);
	int pos3 = vmline.find(' ',pos2+1);
	string third = vmline.substr(pos2+1,pos3-pos2-1);
	
	if(first=="add" || first=="sub" || first=="and" || first=="or")
	{
		return(arithmetic(first));
	}
	else if(first=="push" || first=="pop")
	{
		//cout<<vmline<<endl;
		return(memoryInst(first,second,third));
	}
	else if(first=="neg" || first=="not")
	{
		return(arithmetic(first));
	}
	else if(first=="eq" || first=="gt" || first=="lt")
	{
		return(arithmetic(first));
	}
	else if(first=="label")
	{
		
		return("("+second+")");
	}
	else if(first=="goto")
	{
		
		return("\n@"+second+"\n0;JMP");
	}
	else if(first=="if-goto")
	{
		return(mInst["pop"]+"@"+second+"\nD;JNE\n");
	}
	else if(first=="function")
	{
		return(functionInst(second,third));
	}
	else if(first=="call")
	{
		return(callInst(second,third));
	}
	/*else if(first=="return")
	{
		return(returnInst(vmline));
	}*/
	
}
void Vmtranslate(string vmfile)
{
	int pos=vmfile.find('.');
	string suffix = vmfile.substr(pos+1);
	if(suffix != "vm" || pos==0)
	{
			cout<<"Entered file is not compatible"<<endl;
			return;
	}
	fname = vmfile.substr(0,pos);
	//fname="vaishali";
	fname+=".asm";
	
	ifstream fp;
	fp.open(vmfile.c_str());
	
	string vmline;
	
	ofstream fw;
	fw.open(fname.c_str());
	fw<<"@256"<<endl<<"D=A"<<endl<<"@SP"<<endl<<"M=D"<<endl<<"@main"<<endl<<"0;JMP"<<endl;
	
	int l=0;
	while(fp)
	{
		getline(fp,vmline);
		l++;
	}
	
	int i=0;
	fp.close();
	ifstream fp1;
	fp1.open(vmfile.c_str());
	
	while(i<(l-1))
	{
		getline(fp1,vmline);
		cout<<"vmline "<<vmline<<endl;
		string asmline = convertToASM(vmline);
		fw<<asmline<<endl;
		cout<<"asm line "<<asmline<<endl;
		
		i++;
	}
	//cout<<lcount<<endl;
	fw<<"(END)"<<endl<<"@END"<<endl<<"0;JMP"<<endl;
	fp1.close();
	
	
	return;
}
int main()
{

	cout<<"Enter Vm file name for translation :";
	string Vmfile;
	cin>>Vmfile;
	
	initialize();
	
	Vmtranslate(Vmfile);
	
	return 0;
	
	
	
	
	
}

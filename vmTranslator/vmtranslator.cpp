#include<iostream>
#include<string>
#include<fstream>
#include<unordered_map>

using namespace std;



unordered_map<string,string>addr;
unordered_map<string,string>mInst;
unordered_map<string,string>inst;
int bcount;		//comparison count
int callcount; //call count
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
	return(mInst["pop"]);
}
string arithmetic(string first)
{
	
	string result="\n";
	if(first!="not" && first!="neg")
	{
		result+=preValue();
	}
	
	result+="@SP\nM=M-1\n@SP\nA=M";
	if(first=="eq" || first=="lt" || first=="gt")
	{
		
		result+="\nD=M-D\n@COMP"+to_string(bcount)+inst[first]+"\n@SP\n"+"A=M\nM=0\n"+"@ENDCOMP"+to_string(bcount)+"\n0;JMP\n";
		
		result+="(COMP"+to_string(bcount)+")\n";
		result+="@SP\nA=M\n";
		result+="M=1\n";
		result+="(ENDCOMP"+to_string(bcount)+")\n";
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
			
		}
		else
		{
			result+="D=M\n";
		}
		result+=mInst["push"];
		
	}
	else if(first=="pop")
	{
		result+="\nD=A\n@R13\nM=D\n"+mInst["pop"];
		result+="\n@R13\nA=M\nM=D\n";
	}
	
	return(result);
}
string memoryInst(string first,string second,string third)
{
	
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
		result+="\n@R"+addr[second]+" \n"+third;
	}
	else if(second=="local" || second =="argument" || second=="this" || second=="that")
	{
		result+="\n@"+addr[second];
		result+="\nD=M\n";
		result+="@"+third;
		result+="\nA=D+A\n";
	}
	else
	{
		cout<<"Unknown Memory command"<<endl;
	}
	
	result+=push_pop(first,second,third);
	
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

string returnInst()
{
	string result="\n";
	result+="@LCL\nD=M\n@FRAME\nM=D\n";
	result+="@FRAME\nD=M\n@5\nD=D-A\nA=D\nD=M\n@RET\nM=D\n";
	result+="@SP\nM=M-1\nA=M\nD=M\n@ARG\nA=M\nM=D\n";
	result+="@ARG\nD=M\n@SP\nM=D+1\n";
	result+="@FRAME\nM=M-1\nA=M\nD=M\n@THAT\nM=D\n";
	result+="@FRAME\nM=M-1\nA=M\nD=M\n@THIS\nM=D\n";
	result+="@FRAME\nM=M-1\nA=M\nD=M\n@ARG\nM=D\n";
	result+="@FRAME\nM=M-1\nA=M\nD=M\n@LCL\nM=D\n";
	result+="@RET\nA=M\n0;JMP\n";
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
	int pos4 = vmline.find(' ',pos3+1);
	string fourth = vmline.substr(pos3+1,pos4-pos3-1);
	cout<<"================="<<first<<" "<<second<<" "<<third<<" "<<fourth<<"0 "<<pos<<" "<<pos2<<" "<<pos3<<" "<<pos4<<endl;
	if(first=="add" || first=="sub" || first=="and" || first=="or")
	{
		if(pos!=pos2 || pos!=-1)
		{
			cout<<"Wrong arithmetic instruction syntax"<<endl;
			return("Wrong arithmetic instruction syntax\n");
		}
		return(arithmetic(first));
	}
	else if(first=="push" || first=="pop")
	{
		//cout<<vmline<<endl;
		if(pos2==-1 || pos==-1 || pos4==-1)
		{
			cout<<"Wrong memory instruction syntax"<<endl;
			return("Wrong memory instruction syntax\n");
		}
		return(memoryInst(first,second,third));
	}
	else if(first=="neg" || first=="not")
	{
		if(pos!=pos2 || pos!=-1)
		{
			cout<<"Wrong not or neg instruction syntax"<<endl;
			return("Wrong not or neg instruction syntax\n");
		}
		return(arithmetic(first));
	}
	else if(first=="eq" || first=="gt" || first=="lt")
	{
		if(pos!=pos2 || pos!=-1)
		{
			cout<<"Wrong comparison instruction syntax"<<endl;
			return("Wrong comparison instruction syntax\n");
		}
		return(arithmetic(first));
	}
	else if(first=="label")
	{
		if(pos3==-1 || pos==-1 || (pos4==-1 && pos2!=-1))
		{
			cout<<"Wrong label syntax"<<endl;
			return("Wrong label syntax\n");
		}
		
		return("("+second+")");
	}
	else if(first=="goto")
	{
		if(pos3==-1 || pos==-1)
		{
			cout<<"Wrong goto syntax"<<endl;
			return("Wrong goto syntax\n");
		}
		
		return("\n@"+second+"\n0;JMP");
	}
	else if(first=="if-goto")
	{
		if(pos3==-1 || pos==-1)
		{
			cout<<"Wrong if-goto syntax"<<endl;
			return("Wrong if-goto syntax\n");
		}
		
		return(mInst["pop"]+"@"+second+"\nD;JNE\n");
	}
	else if(first=="function")
	{
		if(pos2==-1 || pos==-1 || pos4==-1)
		{
			cout<<"Wrong function syntax"<<endl;
			return("Wrong function syntax\n");
		}
		return(functionInst(second,third));
	}
	else if(first=="call")
	{
		if(pos2==-1 || pos==-1 || pos4==-1)
		{
			cout<<"Wrong call syntax"<<endl;
			return("Wrong call syntax\n");
		}
		return(callInst(second,third));
	}
	else if(first=="return")
	{
		if(pos!=pos2 || pos!=-1)
		{
			cout<<"Wrong return syntax"<<endl;
			return("Wrong return syntax\n");
		}
		return(returnInst());
	}
	else
	{
		cout<<"unrecognized command"<<endl;
		return("unrecognized command\n");
	}
	
}
/*string removeComments(string vmline)
{
	int pos=vmline.find(" ");
	int npos=vmline.find(" ",pos+1);
	while(pos!=npos)
	{
		pos=npos;
		npos=vmline.find(" ",npos+1);
	}
	string n=s.substr(0,npos-1);
	return(n);
	
	
}*/
void Vmtranslate(string vmfile)
{
	int pos=vmfile.find('.');
	string suffix = vmfile.substr(pos+1);
	if(suffix != "vm" || pos==0)
	{
			cout<<"Entered file is not vm file"<<endl;
			return;
	}
	fname = vmfile.substr(0,pos);
	
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
		/*cout<<"Vmline----------------> "<<vmline<<endl;
		vmline=removeComments(vmline);
		cout<<"vmline===============> "<<vmline<<endl;*/
		if(vmline!="")
		{
			string asmline = convertToASM(vmline);
			fw<<"/* "<<vmline<<" */"<<endl;
			fw<<asmline<<endl;
		}
		
		
		i++;
	}
	
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

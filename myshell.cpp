/* 20162023 NIKITA WANI */

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <vector>
#include <signal.h>
using namespace std;

vector<string> vec;
volatile sig_atomic_t cflag = 0;
static int no=1000;

void executebang(int,int);
void executebang(string);

void sigintHandler1(int sig_num)
{
	cflag = 1; 
}

/* Write history in a history file, load history and execute the history command */
void writehistory(string cmd)
{
	ofstream historyfile;
	char *home;
	string path;

	home=getenv("HOME");
	path=home;
	path=path+ "/history.txt";
    historyfile.open (path.c_str(), ios_base::app);
    historyfile<<cmd<<"\n";
}

void loadhistory()
{
	string line;
	char *home;
	string path;

	home=getenv("HOME");
	path=home;
	path=path+ "/history.txt";
	
	ifstream historyfile (path.c_str());

    if(historyfile.is_open())
    {
        while(getline(historyfile,line))
        {
     	   vec.push_back(line);
        }
        historyfile.close();
    }   
    else
        cout<<"Error opening file"<<endl;
}

void executehistory()
{
	loadhistory();
	
	for(vector<string>::iterator it=vec.begin();it!=vec.end();it++)
	{
		cout<<no<< "  "<< *it<<"\n";
		no++;
	}
}

void executehistory(int num)
{
	loadhistory();
	int n;
	n=1000+vec.size()-num;
	for(vector<string>::iterator it=vec.end()-num;it!=vec.end();it++)
	{
		
		cout<<n<< "  "<< *it<<"\n";
		n++;
	}
}

/***Function to execute pwd command***/
void executepwd()
{
	char buffer[255];
	char *cwd = getcwd(buffer, sizeof(buffer));
	cout<<cwd;
	cout<<"\n";
}

/***Function to execute cd command***/
void executecd(string path)
{
	int n;
	n=chdir(path.c_str()); 
	if(n<0)
		cout<<"bash: cd: "<<path<<" :No such file or directory\n";
}

/***Function to execute echo command***/
int executeecho(string command, char text[])
{
	int i,len,flag1,flag2,k,pos;
	len=command.length();
	k=0;
	flag1=0;
	flag2=0;
	pos=command.find("o");
	command.erase(0,pos+1);
	pos=command.find_first_not_of(" \t");


	command.erase(0,pos);
	len=command.length();
	if(command[0]=='$' && command[1]=='$')
	{
		cout<<getppid();
		return 0;
	}
	else
	{
		for(i=0;i<=len;i++)
		{
			if(command[i]==34 && flag1==0 && flag2!=1)
			{
				flag1=1;
				continue;
			}
			else if(command[i]==34 && flag1==1 && flag2!=1)
			{
				flag1=0;
				continue;
			}
			else if((command[i]==34 && flag2==1 ))
			{
				text[k]=command[i];
				k++;
			}
			else if(command[i]==39 && flag2==0 && flag1!=1)
			{
				flag2=1;
				continue;
			}
			else if(command[i]==39 && flag2==1 && flag1!=1)
			{
				flag2=0;
				continue;
			}
			else if((command[i]==39 && flag1==1 ))
			{
				text[k]=command[i];
				k++;
			}
			else if(command[i]=='$')
			{
				int z;
				string variable="";
				char *envar;
				
				z=0;
				int j=0;
				i++;
				while((command[i]!=34)&&(command[i]!=39)&&(command[i]!=32) &&(i!=len))
				{

					variable=variable+command[i];
					z++;
					i++;
				}
				i--;
				envar=getenv(variable.c_str());
				
				if(envar!=NULL)
				{
					string envvariable(envar);
					int length=envvariable.length();
					while(j<length)
					{
						text[k]=envvariable[j];
						j++;
						k++;
					}
				}
			}
			else if((command[i]==92) && (command[i+1]==34 || command[i+1]==36 || command[i+1]==92))
			{
				i++;
				text[k]=command[i];
				k++;
			}
			else
			{
				text[k]=command[i];
				k++;
			}
		}
	}
	text[k]='\0';

	if(flag1!=0 || flag2!=0)
		return -1;
	else
		return 0;
	
}

/***Function to remove leading spaces***/
string removespace(string command)
{
	int i=0;
	while(command[i]==32)
		i++;
	command.erase(0,i);
	return command;
}

/***Function to remove quotes***/
string removequotes(string command)
{
	int i,len,k;
	k=0;
	len=command.length();
	for(i=0;i<=len;i++)
	{
		if((command[i]!=34) && (command[i]!=39) && (command[i]!=60) && (command[i]!=62))
		{	
			command[k]=command[i];
			k++;
		}
	}
	command[k]='\0';
	return command;
}

/***Function to check whether IO redirection present***/
int searchinput(string command,char file[])
{
	int i,len;
	string c;
	len=command.length();
	for(i=0;i<=len;i++)
	{
		c=command[i];
		if(c=="<")
		{
			break;
		}
	}
	if(i==len+1)
		i=0;
	int k=i+1;
	while(command[k]==32)
		k++;
	int m=0;
	if(i!=0)
	{
		while(command[k]!=32 && k<len)
		{
			file[m]=command[k];
			k++;
			m++;
		}
	}
	file[m]='\0';
	return i;
}


int searchoutput(string command,char file[])
{
	int i,len;
	string c;
	len=command.length();
	for(i=0;i<=len;i++)
	{
		c=command[i];
		if(c==">")
		{
			break;
		}
	}
	if(i==len+1)
		i=0;
	int k=i+1;
	while(command[k]==32)
		k++;
	int m=0;
	if(i!=0)
	{
		while(command[k]!=32&& k<len)
		{
			file[m]=command[k];
			k++;
			m++;
		}
	}
	file[m]='\0';
	return i;
}

/***Function to replace space with pipe for parsing after each command is extracted separately***/
string replacepipe(string command)
{
	int i,len,k,flag1,flag2;
	k=0;
	len=command.length();
	if(k==1)
	cout<<" ";
	for(i=0;i<=len;i++)
	{
		if(command[i]=='\'' && flag1==0)
		{
			flag1=1;
			continue;
		}	
		if (command[i]=='\"' && flag2==0)
 		{
			flag2=1;
			continue;
		}
		if(command[i]=='\'' && flag1==1)
		{
			flag1=0;
			continue;
		}
		if(command[i]=='\"' && flag2==1)
		{				
			flag2=0;
			continue;
		}
		if(command[i]==32 && flag1==0 && flag2==0)
		{
			command[i]=124;
		}
	}
	return command;
}

/***Function to execute command. It checks whether the command is built in or not and execute accordingly.***/
void execute(string command[], int num)
{
	char *parts;
	char *home;
	char *oldpwd;
	char *buf2;
	char *buf3[200];
	
	pid_t  pid;
    int fd_in=0;
    int fd[2];
    string one;
 	int in,out;
 	int filedes1,filedes2;
 	char opfile[100];
 	char ipfile[100];
 	string history[100];
 	int k;
 	for(k=0;k<=num;k++)
 	history[k]=command[k];
 
    for(k=0;k<=num;k++)
    {
    	int found;
    	char text[100];
    	filedes1=filedes2=0;
    	pipe(fd);
    	command[k]=removespace(command[k]);
    	found=command[k].find("echo");

    	out=searchoutput(command[k],opfile);
 		if(out!=0)
 		command[k]=command[k].erase(out,command[k].length());

    	in=searchinput(command[k],ipfile);
    	if(in!=0){
    		command[k]=command[k].erase(in,command[k].length());	
    	}
 		
 		
 		
 		if(!(found>=0))
 		{	
 			command[k]=replacepipe(command[k]);
 			command[k]=removequotes(command[k]);
 		}	
 		
		buf2=const_cast <char *>(command[k].c_str());
		parts =strtok(buf2,"|"); //to extract each command separated by pipe from the whole command
		int i=0;
		while (parts != NULL)
  		{
    		buf3[i]=parts;
    		parts = strtok (NULL, "|");
    		i++;
  		}
  		buf3[i]=NULL;
  		if(!strcmp(buf3[0],"cd"))     //Implementing cd 
		{
			if(num==0)
			{
			if(buf3[1]=='\0')
			{
				home=getenv("HOME");
				buf3[1]=home;
			}
			one =buf3[1];
			if( one=="~" || one == "#")
			{
				home=getenv("HOME");
				buf3[1]=home;
			}
			else if(one == "-") 
			{
				oldpwd=getenv("OLDPWD");
				buf3[1]=oldpwd;
			}
			executecd(buf3[1]);
			}
			
		}
		else if(!strcmp(buf3[0],"export"))  //Implementing export
		{
			char * var;
			string variable=buf3[1];
			string varname,varvalue;
			var=const_cast <char *>(variable.c_str());
			varname =strtok(var,"=");
			varvalue=strtok(NULL,"=");
			//cout<<varname<<varvalue;
				if(setenv(varname.c_str(),varvalue.c_str() ,1)<0)
			cout<<"Error setting variable\n";
		}
  		else
  		{
  			if ((pid = fork())==-1) 
   			{
    			cout<<"ERROR:forking child process failed\n";
       			exit(1);
   			}
	   		else if (pid == 0) 
	   		{ 
	   			if (in!=0) 
	   			{ 
	    			filedes1 = open(ipfile, O_RDONLY );
	   				fd_in=filedes1;
				}
				if (out!=0) 
				{ 
					filedes2 = open(opfile,O_CREAT|O_TRUNC|O_WRONLY,S_IRWXU);
					dup2(filedes2,1);
					close(fd[1]);
				}
	    		
				dup2(fd_in, 0);
				if(!command[k+1].empty())
					dup2(fd[1],1);
				close(fd[0]);
				if(!strcmp(buf3[0],"pwd"))			//Implementing pwd
				{
					executepwd();
					exit(1);
				}

				else if(!strcmp(buf3[0],"history")) //Implemrnting history
				{
					if(buf3[1]!=NULL)
					{
						int number;
						number=atoi(buf3[1]);
						executehistory(number);
					}
					else
					executehistory();
					exit(1);
				}
				else if(buf3[0][0]==33) //Implementing bang operator
				{
					int cmdno;
					string bang=buf3[0];
					if(bang[1]=='!')
					{
						executebang(1,1);
					}
					else if(bang[1]=='-')
					{
						bang.erase(0,2);
						cmdno=atoi(bang.c_str());
						executebang(cmdno,1);
					}
					else if(bang[1]>='0' && bang[1]<='9' )
					{
						bang.erase(0,1);
						cmdno=atoi(bang.c_str());
						executebang(cmdno,0);
					}
					else
					{
						bang.erase(0,1);
						executebang(bang);
					}
					exit(1);
				}
				else if(found>=0)  //Implementing echo
				{
					if(executeecho(command[k],text)>=0)
					{
						if(text!=NULL)
						cout<<text<<"\n";
						
					}
					else
					{
						cout<<"Error: invalid command\n";
						
					}
					exit(1);
				}
			    else
			    {                                  	
					if (execvp(buf3[0], buf3) < 0) 
					{     
						if(strcmp(buf3[0],"exit"))
	   				cout<<"No command found\n";
	       			exit(1);
					}	
				}
			}
			else 
			{                                 
				wait(NULL);
				close(fd[1]);
				fd_in=fd[0];
			}
		}
	}
    bzero(buf3,sizeof(buf3));
}

/***Function to parse the command given.. it stores the command separated by pipes in command array.***/
int parse(string cmd,string command[])
{
	int i,k,pos=0,len;
	int flag1,flag2;
	flag1=0,flag2=0,k=0;
	len=cmd.length();
	for(i=0;i<len;i++)
	{
		if(cmd[i]=='\'' && flag1==0)
		{
			flag1=1;
			continue;
		}	
		if (cmd[i]=='\"' && flag2==0)
			{
			flag2=1;
			continue;
		}
		if(cmd[i]=='\'' && flag1==1)
		{
			flag1=0;
			continue;
		}
		if(cmd[i]=='\"' && flag2==1)
		{				
			flag2=0;
			continue;
		}
		if(cmd[i]=='|' && flag1==0 && flag2==0)
		{
			command[k]=cmd.substr(pos,i-pos);
			pos=i+1;
			k++;
		}
	}

	command[k]=cmd.substr(pos);
	return k;
}

/***Function to execute bang command***/
void executebang(int cmdno,int status)
{
	int cmdno2,k;
	string it2[100];
	loadhistory();
	vector<string>::iterator it;
	if(status==0)
	{
		
		cmdno2=cmdno-1000;
		it=vec.begin()+cmdno2;		
	}
	else if(status==1)
	{
		it=vec.end()-cmdno;

	}
	it2[0]=*it;
	cout<<*it<<"\n";
	if(status!=1 || cmdno!=1)
	writehistory(*it);
	k=parse(*it2,it2);
	execute(it2,k);	
}

void executebang(string bang)
{
	int len,k;
	string it2[100];
	loadhistory();
	vector<string>::iterator it;
	len=bang.length();
	for(vector<string>::iterator it=vec.end();it!=vec.begin();it--)
	{
		string cm=*it;
		if(cm.substr(0,len)==bang)
		{
			cout<<*it<<"\n";
			writehistory(*it);
			it2[0]=*it;
			k=parse(*it2,it2);
			execute(it2,k);
			break;
		}
	}
}

/*** Main function***/
int main()
{
	string cmd;
	int len,k;
	char buffer[255];
	do
	{
		string command[100];
		
		char *cwd = getcwd(buffer, sizeof(buffer));

		signal(SIGINT, sigintHandler1);
		//signal(SIGTSTP, sigintHandler2);
		if(cflag==1)
		{
			cflag=0;
			cout<<"\n";
			continue;
		}
		
		cout<<"myShell@Nikita:~"<<cwd<<"$ ";	
		getline(cin,cmd);
		cmd=removespace(cmd);
		len=cmd.length();
		if(len==0)
			continue;
		if(cmd[0]!=33)
		writehistory(cmd);
		k=parse(cmd,command);
		execute(command,k);
	}
	while(cmd!="exit");
	cout<<"Bye..";
	return 0;
}

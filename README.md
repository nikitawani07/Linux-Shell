# LinuxShell

Implemented a Linux shell for running different commands that supports i/o redirection, piping, history, environment variables, external-internal commands using various system calls.

## Functionality implemented

a) Executes all the commands (ls, clear, vi etc) 

b) Shell built-ins (cd, pwd, export) 

c) Print environment variables and text using echo

d) Allow for the piping of several tasks :  Pipes “|” (multiple) 

e) Input and Output redirection: (<, >) 

f) Support for history and '!' operator 

g) Handle Interrupt Signal (Control-C)

## Function Description

* Write history in a history file, load history and execute the history command 
    void writehistory(string cmd) : It writes the command in the history file.
    void loadhistory() : It loads the content of the history file line by line in vector. 
    void executehistory() : To implement history command
    void executehistory(int num) : To implement history command with a number e.g. history 5

* void executepwd() : Function to execute pwd command

c) void executecd(string path) : Function to execute cd command

d) int executeecho(string command, char text[]) : Function to execute echo command.

e) string removespace(string command) : Function to remove leading spaces from the command.

f) string removequotes(string command) : Function to remove quotes from the command.

g) Function to check whether IO redirection present i.e. whether it is reading from the file or writing in the file.
  int searchinput(string command,char file[])
  int searchoutput(string command,char file[])

h) string replacepipe(string command) : Function to replace space with pipe for parsing after each command is extracted separately

i) void execute(string command[], int num) : Function to execute command. It checks whether the command is built in or not and execute accordingly.

j) int parse(string cmd,string command[]) : Function to parse the command given. It stores the each command separated by pipes in command array.

i) Function to execute bang command
  void executebang(int cmdno,int status) : It executes command containing bang operator e.g. !!, !-1, !1000  
  void executebang(string bang) : It executes command containing bang operator given the string e.g. !v. This will execute the last command starting with v in the history file. 


## Program flow

Initially the command is taken from the stdin. 
The complete command is tokenized on the basis of pipes and each token or individual commmand is stored in command array.
Then the execute function is called which is responsible for executing each command stored in command array one by one.
Execute function: This function removes leading spaces, replace space with pipe and check for IO redirection. After that it check whether it is built in or not and execute accordingly.
For exiting from the shell type exit.

## To Run

 Compile:	
      
      g++ -Wall myshell.cpp -o myshell

  Execute:
      
      ./myshell

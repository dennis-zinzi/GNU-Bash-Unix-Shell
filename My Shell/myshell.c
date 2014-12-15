/*
 * Dennis Zinzi - 130301136
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>


//Assign exit value to 127
#define EXIT_VALUE 127
#define EXIT_SUCESS 0
#define EXIT_CHANGE 10
#define EXIT_DIR 25


//User input
char *input;
//String to break down user input
char *token;
//User command
char *cmd;
//User parameters
char *params[10];
//Number of parameters
int paramsize = 0;
//Process ID
int pid;
//Command prompt string
char *symbol;
//Pointer to command prompt string
char **symbolp = &symbol;

//Declaration of methods used
void welcome();
void readInput();
void help();
void csymbol(char *);


int main(int argc, char *argv[]){
	//Allocate memory for command prompt string
	symbol = (char *)malloc(sizeof(char)*11);
	//Assign command prompt string default value of "##"
	strcpy(*symbolp, "##");
	//Display welcoming output
	welcome();
	//Repeat forever
	while(true){
		//Read user input
		readInput();
		//Fork processes
		pid = fork();
		//If process not found restart
		if(pid<0){
			puts("Process not found");
		}
		//Parent code
		else if(pid != 0){
			//Process status
			int status;
			//Wait for child process
			waitpid(-1, &status, 0);

			//printf("Child exited with status: %d\n", WEXITSTATUS(status));

			//Determine exit status of child
			if(WIFEXITED(status))
			{
				//If exit command typed stop program execution
				if(WEXITSTATUS(status)==EXIT_VALUE){
					free(symbol);
					puts("Exited shell");
					return 0;
				}

				//If user wants to change command prompt string
				if(WEXITSTATUS(status)==EXIT_CHANGE){
					csymbol(params[0]);
				}

				//If user wants to change current working directory
				if(WEXITSTATUS(status)==EXIT_DIR){
					//Assign space for current working directory path
					char cwd[1024];
					if (getcwd(cwd, sizeof(cwd)) != NULL){
						char *dir;
						//If user wants to access root directory
						if(strcmp(params[0],"/")==0){
							dir = "/";
							chdir(dir);
						}

						//If user wants to access any other directory
						if(strcmp(params[0],"/")!=0){
							//Append directory to be accessed to current working directory
							dir = strcat(cwd,"/");
							dir = strcat(dir,params[0]);
							chdir(dir);
						}
					}
				}
			}
			else{
				// Program exited abnormally
				puts("Unexpected exit");
			}
		}
		//Child code
		else{
			//Assign user command
			argv[1]=cmd;
			//If user entered whitespace or returned repeat
			if(argv[1] == NULL){
				exit(errno);
			}

			//Assign user parameters if any
			if(paramsize>0){
				int k = 0;
				while(k<paramsize){
					argv[2+k]=params[k];
					k++;
				}
			}

			//If user typed exit command exit program
			if((strcmp(argv[1],"q")==0 && argv[2]==NULL) || (strcmp(argv[1],"Q")==0 && argv[2]==NULL) || (strcmp(argv[1],"exit")==0 && argv[2]==NULL)){
				puts("Exiting...");
				exit(EXIT_VALUE);
			}

			//If user typed help command display help information
			if((strcmp(argv[1],"help")==0 && argv[2]==NULL)){
				help();
				exit(EXIT_SUCCESS);
			}

			//If useer wants to change command prompt string
			if((strcmp(argv[1],"csymbol")==0 && argv[2]!=NULL && argv[3]==NULL)){
				exit(EXIT_CHANGE);
			}

			//If user wants to change current working directory
			if(strcmp(argv[1],"cd")==0 && argv[2] != NULL && argv[3]==NULL){
				exit(EXIT_DIR);
			}

			//Try executing user command and parameters
			int x = execvp(argv[1], &argv[1]);
			//If command non-existent return error and repeat
			if(x==-1){
				puts("Error: Command not found");
				exit(errno);
			}

		}
	}
	return 0;
}

/**
 * Welcome to program output
 */
void welcome(){
	puts(" ---------------------------------");
	puts("|---------------------------------|");
	puts("|\tWelcome to my Shell\t  |");
	puts("|---------------------------------|");
	puts("|   Type 'q' or \"exit\" to exit    |");
	puts("|---------------------------------|");
	puts("|      Type \"help\" for help\t  |");
	puts("|---------------------------------|");
	puts(" ---------------------------------");
}

/**
 * Method to read user input and determine command and parameters
 */
void readInput(){
	int bytesread;
	size_t linebytes = 60;
	printf("%s: ",symbol);
	//Allocate space for user input
	input = (char *) malloc(linebytes+1);
	//Assign value of user input
	bytesread = getline(&input, &linebytes, stdin);

	//Breakdown words inputed
	char *word;
	word = strtok(input," \n");
	//First word is the command
	cmd = word;
	//Other words are the arguments
	int i = 0;
	while(word != NULL){
		word = strtok(NULL, " \n");
		params[i]=word;
		i++;
		paramsize++;
	}
	//Free memory allocated for user input line
	free(input);
}

/**
 * Method which provides user with list of available commands
 */
void help(){
	puts("Available commands:");
	puts("\tcat\t-\tCopies user input");
	puts("\t\t\t\tWARNING: only exits with Ctrl+C thus exiting program completely");
	puts("\tcc\t-\tCompiles C/C++ Program specified");
	puts("\tcd\t-\tChanges current working directory to specified directory");
	puts("\t\t\t\t\"/\" changes current directory to root directory");
	puts("\t\t\t\t\"..\" moves to parent directory");
	puts("\tcsymbol\t-\tChanges default symbol (\"##\") for commands");
	puts("\tdate\t-\tDisplays current date");
	puts("\techo\t-\tPrints out arguments after command");
	puts("\texit\t-\tExits program");
	puts("\tls\t-\tDisplays list of files under current working directory");
	puts("\tps\t-\tDisplays list of current processes");
	puts("\tpwd\t-\tDisplays path of current working directory");
	puts("\tQ\t-\tExits program");
	puts("\tq\t-\tExits program");
	puts("\trm\t-\tRemoves specified file");
	puts("\tw\t-\tDisplays summary of every logged in user, process currently running,\n"
			"\t\t\tand load of activity imposed on computer");
}

/**
 * Method to change command string
 */
void csymbol(char *newsymbol){
	strcpy(*symbolp, newsymbol);
}


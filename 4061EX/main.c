#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>

#include "util.h"

//This function will parse makefile input from user or default makeFile. 
int parse(char * lpszFileName)
{
	regex_t whiteSpace ;
    makeRegex (&whiteSpace, "^\\s*$") ;
    regex_t command ;
    makeRegex (&command, "^\t[^\n]*\n"); //if start with /t, extract but not tokenize 
    regex_t lineComment ;
    makeRegex (&lineComment, "^#[^\n]*\n"); //match any line comment, first character must be #
	int nLine=0;
	int nTargets=0; 
	int nDepedencies=0;
	char szLine[1024];
	char * lpszLine="123";
	bool MatchSkip;
	bool BlankSkip;
	bool CommandSkip;
    bool nameTaken=false;

	FILE * fp = file_open(lpszFileName);

	if(fp == NULL)
	{
		return -1;
	}
/* The following code is for building the structre array where each element contains a "node" */
	while(file_getline(szLine, fp) != NULL) {
		nLine++;
		MatchSkip=matchRegex(&lineComment, szLine) ; 
		BlankSkip=matchRegex(&whiteSpace, szLine) ; 
		if(MatchSkip==false && BlankSkip==false){
				nameTaken=false;
				CommandSkip=matchRegex(&command, szLine);	
				if (CommandSkip==false){ 
					lpszLine = strtok(szLine, " \n"); 
					if(targetTree[nTargets].name[0]=='\0'){
					strncpy(targetTree[nTargets].name,lpszLine,strlen(lpszLine)-1);
					}
					else{nTargets++; nameTaken=true; nDepedencies=0;}
					if(strstr(lpszLine,":")==NULL){
						printf("syntax error\n");
						exit(0);
					}
					strncpy(targetTree[nTargets].name,lpszLine,strlen(lpszLine)-1);
					lpszLine = strtok(NULL, " \n");
					while (lpszLine != NULL) {
						lNode* newDepedency = (lNode*) malloc(sizeof(lNode));
						strcpy(newDepedency->name,lpszLine);
						targetTree[nTargets].depedency[nDepedencies]=newDepedency;
						lpszLine = strtok(NULL, " \n");		
						nDepedencies++;
					}
				}
				else if (CommandSkip==true) {
					
					if (szLine[0] == '\t') {
					memmove(szLine, szLine+1, strlen(szLine));}
					strcpy(targetTree[nTargets].commandline,szLine);
					if(nameTaken==false){
					nTargets++;}
					nDepedencies=0;
				}
		}
	 
    }
	fclose(fp);

	return 0;
}

void show_error_message(char * lpszFileName)
{
	fprintf(stderr, "Usage: %s [options] [target] : only single target is allowed.\n", lpszFileName);
	fprintf(stderr, "-f FILE\t\tRead FILE as a maumfile.\n");
	fprintf(stderr, "-h\t\tPrint this message and exit.\n");
	fprintf(stderr, "-n\t\tDon't actually execute commands, just print them.\n");
	fprintf(stderr, "-B\t\tDon't check files timestamps.\n");
	fprintf(stderr, "-m FILE\t\tRedirect the output to the file specified .\n");
	exit(0);
}

int main(int argc, char **argv) 
{
	// Declarations for getopt
	
	extern int optind;
	extern char * optarg;
	int ch;
	char * format = "f:hnBm:"; 
	char szMakefile[64] = "Makefile";
	char szTarget[64];
	char szLog[64];
	bool redir=false;
	bool execute=true;
	bool timeStamp=true;
	int log;
	char * mainTarget;
	bool Specific_Target=false;
	

	while((ch = getopt(argc, argv, format)) != -1) //obtain the character option 
	{ //argv would be ./make4061 ..etc 
		switch(ch) 
		{
			case 'f':
				strcpy(szMakefile, strdup(optarg));
				break;
			case 'n':
				execute=false;
				break;
			case 'B':
				break;
			case 'm':
				redir = true;
				strcpy(szLog, strdup(optarg));
				break;
			case 'h':
			default:
				show_error_message(argv[0]);
				exit(1);
		}
	}

	argc -= optind;
	argv += optind;

	
	if(redir==true)
	{
		log = open(szLog, O_CREAT|O_TRUNC|O_WRONLY, 0644);
		dup2(log, 1);
	}
	if(argc > 1)
	{
		show_error_message(argv[0]);
		return EXIT_FAILURE;
	}

	if(argc == 1)
	{
		mainTarget=argv[0];
		Specific_Target=true;
	}
	else
	{
		Specific_Target=false;
	}

	
	/* Parse graph file or die */
	if((parse(szMakefile)) == -1) 
	{
		return EXIT_FAILURE;
	}
	   build_depedency(targetTree);
		execute_tree(targetTree, mainTarget,Specific_Target,execute);

	//after parsing the file, you'll want to check all dependencies (whether they are available targets or files)
	//then execute all of the targets that were specified on the command line, along with their dependencies, etc.
	return EXIT_SUCCESS;
}

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
	enum MatchType { numMatch, wordMatch, noMatch, bootMatch} matchType ;
	bool MatchSkip;
	bool BlankSkip;
	bool CommandSkip;

	FILE * fp = file_open(lpszFileName);

	if(fp == NULL)
	{
		return -1;
	}

	while(file_getline(szLine, fp) != NULL) {
		//printf("beginning is %s\n",lpszLine);
		nLine++;
		MatchSkip=matchRegex(&lineComment, szLine) ; //determine if there is any comment exist, if there is, skip tokenizing and move on 
		BlankSkip=matchRegex(&whiteSpace, szLine) ; 
		if(MatchSkip==false && BlankSkip==false){
			CommandSkip=matchRegex(&command, szLine);	//determine if there is tab at the beginning, if not, tokenize the string and break it down 
			if (CommandSkip==false){ //if not a command
				//Remove newline character at end if there is one
				lpszLine = strtok(szLine, " \n"); 
				printf("lpszLine is %s\n",lpszLine);
				strcpy(targetTree[nTargets].name,lpszLine);
				//targetTree[nTargets].name=lpszLine;
				printf("target name given as %s",targetTree[0].name);
				while (lpszLine != NULL) {   		
				//targetTree[nTargets].depedency[nDepedencies]=lpszLine;
				nDepedencies++;
				lpszLine = strtok(NULL, " \n");
				}
				//lpszLine now empty
				
			}
			else if (CommandSkip==true) {
				//printf("%s",szLine);
				nTargets++;
				//printf("it is now %s",targetTree[0].name);
			}
		}
	 printf("it is now! %s\n",targetTree[0].name);
	 	 printf("it is now! %s\n",targetTree[1].name);

    }
    //printf("it is %s\n",targetTree[0].name);
    //printf("abc %s\n",targetTree[1].name);
    //printf("%s\n",targetTree[2].name);

		//You need to check below for parsing.
		//Skip if blank or comment.
		//Remove leading whitespace.
		//Skip if whitespace-only.
		//Only single command is allowed.
		//If you found any syntax error, stop parsing. 
		//If lpszLine starts with '\t' it will be command else it will be target.
		//It is possbile that target may not have a command as you can see from the example on project write-up. (target:all)
		//You can use any data structure (array, linked list ...) as you want to build a graph
	

	//Close the makefile. 
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
	char * format = "f:hnBm:"; //if there is semicolon after it, you need specify a file
	
	// Default makefile name will be Makefile
	char szMakefile[64] = "Makefile";
	char szTarget[64];
	char szLog[64];
	

	while((ch = getopt(argc, argv, format)) != -1) //obtain the character option 
	{ //argv would be ./make4061 ..etc 
		switch(ch) 
		{
			case 'f':
				strcpy(szMakefile, strdup(optarg));
				break;
			case 'n':
				break;
			case 'B':
				break;
			case 'm':
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

	// at this point, what is left in argv is the targets that were 
	// specified on the command line. argc has the number of them.
	// If getopt is still really confusing,
	// try printing out what's in argv right here, then just running 
	// with various command-line arguments.
	 
    //for command ./make4061, nothing left, ./make4061 123 left with 123..etc
	
	printf("argc is %d \n", argc);

	if(argc > 1)
	{
		show_error_message(argv[0]);
		return EXIT_FAILURE;
	}

	//You may start your program by setting the target that make4061 should build.
	//if target is not set, set it to default (first target from makefile)
	if(argc == 1)
	{
	}
	else
	{
	}


	/* Parse graph file or die */
	if((parse(szMakefile)) == -1) 
	{
		return EXIT_FAILURE;
	}

	//after parsing the file, you'll want to check all dependencies (whether they are available targets or files)
	//then execute all of the targets that were specified on the command line, along with their dependencies, etc.
	return EXIT_SUCCESS;
}

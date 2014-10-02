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
	int target_length;
	bool MatchSkip;
	bool BlankSkip;
	bool CommandSkip;

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
			CommandSkip=matchRegex(&command, szLine);	
			if (CommandSkip==false){ 
				lpszLine = strtok(szLine, " \n"); 
				strncpy(targetTree[nTargets].name,lpszLine,strlen(lpszLine)-1);
				lpszLine = strtok(NULL, " \n");
				while (lpszLine != NULL) {
				lNode* newDepedency = (lNode*) malloc(sizeof(lNode));
				strcpy(newDepedency->name,lpszLine);
				targetTree[nTargets].depedency[nDepedencies]=newDepedency;
				//printf("targetTree[%d].depedency[%d] is %s\n",nTargets,nDepedencies,targetTree[nTargets].depedency[nDepedencies]->name);  	
				lpszLine = strtok(NULL, " \n");
				//printf("lpszLine is now! %s\n",lpszLine);  		
				//printf("lpszLine is now! %s\n",targetTree[nTargets].depedency[nDepedencies]);  		
				nDepedencies++;
				}
				
				
			}
			else if (CommandSkip==true) {
				if (szLine[0] == '\t') {
				memmove(szLine, szLine+1, strlen(szLine));}
				strcpy(targetTree[nTargets].commandline,szLine);
				//printf("%s\n",targetTree[nTargets].commandline);
				nTargets++;
				nDepedencies=0;
			}
		}
	 
    }
   
   /*build complete */ 
   
   
   //second iteration, run through the array, give index to each of the depedencies which points to the location of the array//
    int i=0;int j=0; int Dindex; 
    while (targetTree[i].name[0]!='\0'){
			j=0;
			if (targetTree[i].depedency[0]!=NULL){
				while(targetTree[i].depedency[j]->name!=NULL){
					Dindex=Search(targetTree[i].depedency[j]->name, targetTree);
					if(Dindex==-1){
						printf("syntax error,found depedency file that doesnt exist, compile stop\n"); //found syntax error 
						return -1;
					}
					else{
						targetTree[i].depedency[j]->index=Dindex; 
					}
					//printf("targetTree[%d].depedency[%d]: The depedency name is %s \n",i,j,targetTree[i].depedency[j]->name);
					//printf("targetTree[%d].depedency[%d]: The depedency matches %d in the array \n",i,j,targetTree[i].depedency[j]->index);
					j++;
					
				}
			}
			else{ 
				targetTree[i].indepedent=true; //indicate that it is a leave node 
				//printf("targetTree[%d]\n",i);
				//printf("this is a indepedent node\n");
				}
				i++;
				
	}
	
    //printf("the number of targets is %d", nTargets);


    //second iteration is over//

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

/************************
 * util.c
 *
 * utility functions
 *
 ************************/

#include "util.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/***************
 * These functions are just some handy file functions.
 * We have not yet covered opening and reading from files in C,
 * so we're saving you the pain of dealing with it, for now.
 *******/
FILE * file_open(char* filename) 
{
	FILE* fp = fopen(filename, "r");
	if(fp == NULL) 
	{
		fprintf(stderr, "make4061: %s: No such file or directory.\n", filename);
		exit(1);
	}

	return fp;
}

//This function will return the line.
char* file_getline(char* buffer, FILE* fp) 
{
	buffer = fgets(buffer, 1024, fp);
	return buffer;
}


//Return -1 if file does not exist
int is_file_exist(char * lpszFileName)
{
	return access(lpszFileName, F_OK); 
}

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace(*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}

//return -1 if file does not exist. 
//return last modified time of file
//bigger number means that it is newer (more recently modified). 
int get_file_modification_time(char * lpszFileName)
{
	if(is_file_exist(lpszFileName) != -1)
	{
		struct stat buf;
		int nStat = stat(lpszFileName, &buf);
		return buf.st_mtime;
	}
	
	return -1;
}

//Compare the last modified time between two files.
//return -1, if any one of file does not exist. 
//return 0, if both modified time is the same.
//return 1, if first parameter is bigger (more recent)
//return 2, if second parameter is bigger (more recent)
int compare_modification_time(char * lpsz1, char * lpsz2)
{	
	int nTime1 = get_file_modification_time(lpsz1);
	int nTime2 = get_file_modification_time(lpsz2);

	if(nTime1 == -1 || nTime2 == -1)
	{
		return -1;
	}

	if(nTime1 == nTime2)
	{
		return 0;
	}
	else if(nTime1 > nTime2)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

// makeargv
/* Taken from Unix Systems Programming, Robbins & Robbins, p37 */
int makeargv(const char *s, const char *delimiters, char ***argvp) {
   int error;
   int i;
   int numtokens;
   const char *snew;
   char *t;

   if ((s == NULL) || (delimiters == NULL) || (argvp == NULL)) {
      errno = EINVAL;
      return -1;
   }
   *argvp = NULL;
   snew = s + strspn(s, delimiters);
   if ((t = malloc(strlen(snew) + 1)) == NULL)
      return -1;
   strcpy(t,snew);
   numtokens = 0;
   if (strtok(t, delimiters) != NULL)
      for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++) ;

   if ((*argvp = malloc((numtokens + 1)*sizeof(char *))) == NULL) {
      error = errno;
      free(t);
      errno = error;
      return -1;
   }

   if (numtokens == 0)
      free(t);
   else {
      strcpy(t,snew);
      **argvp = strtok(t,delimiters);
      for (i=1; i<numtokens; i++)
         *((*argvp) +i) = strtok(NULL,delimiters);
   }

   *((*argvp) + numtokens) = NULL;
   return numtokens;
}

//You should call this function when you done with makeargv()
void freemakeargv(char **argv) {
   if (argv == NULL)
      return;
   if (*argv != NULL)
      free(*argv);
   free(argv);
}
//ALL functions in the following are custom-made functions


void makeRegex (regex_t *re, const char* pattern) {
  int rc ;

  /* "Compile" the regular expression.  This sets up the regex to do
     the matching specified by the regular expression given as a
     character string.
   */
  rc = regcomp(re, pattern, REG_EXTENDED) ;

  if (rc!= 0) {
      printf ("Error in compiling regular expression.\n");
      size_t length = regerror (rc, re, NULL, 0) ;
      char *buffer = (char *) malloc( sizeof(char) * length ) ;
      (void) regerror (rc, re, buffer, length) ;
      printf ("%s\n", buffer);
  }
}


bool matchRegex (regex_t *re, const char *text) {
    int status ;
    const int nsub=1 ;
    regmatch_t matches[nsub] ;

  /* execute the regular expression match against the text.  If it
     matches, the beginning and ending of the matched text are stored
     in the first element of the matches array.
   */
    status = regexec(re, text, (size_t)nsub, matches, 0); 

    if (status==REG_NOMATCH) {
        return false ;
    }
    else {
        return true ;
    }
}




int testFn(const char *str)
{
    return (str && *str && str[strlen(str) - 1] == '\n') ? 0 : 1;
}
bool file_exists(const char * filename)
{
	FILE * file;
    if (file = fopen(filename, "r"))
    {
        fclose(file);
        return true;
    }
    return false;
}
int Search(char * name, target_t *targetTree ){
	int i=0;
	while (targetTree[i].name[0]!='\0'){
		//printf("node is %d\n",i);
		//printf("first compare %s\n",name);
		//printf("second compare %s\n",targetTree[i].name);
		if  (strcmp(targetTree[i].name,name)==0){
			return i;
		}
		i++;
	}
	return -1;
}

void build_depedency(target_t *targetTree, bool Time){
	int i=0; 
	int j=0;
	int Dindex;
	
	while (targetTree[i].name[0]!='\0'){
			j=0; 
			targetTree[i].status=INELIGIBLE; //initialize the status as not ready for compile 
			if (targetTree[i].depedency[0]!=NULL){  
				while(targetTree[i].depedency[j]->name!=NULL){
					Dindex=Search(targetTree[i].depedency[j]->name, targetTree);
					if(Dindex==-1){
						if(file_exists(targetTree[i].depedency[j]->name)==false){   //if cannot find file in tree, and if these depedencies doesn't exist as file
							printf("syntax error,found depedency file that doesnt exist, compile stop\n"); //found syntax error 
							exit(0);
						}
						else{ //for case such as make4061: main.c parse.c whereas these are existing files 
							if(Time==false){
							targetTree[i].indepedent=true; //indicate that it is a leave node 
							targetTree[i].status=READY;} //indicate that it is a leave node 
							else if(compare_modification_time(targetTree[i].depedency[j]->name, targetTree[i].name)!=2){
								targetTree[i].status=NEW;
							}
						}
					}
					else{
						targetTree[i].depedency[j]->index=Dindex; 
					}
					j++;
					
				}
			}
			else{ 
				targetTree[i].indepedent=true; //indicate that it is a leave node 
				targetTree[i].status=READY; //indicate that it is a leave node 
				}
				i++;
				
	}

}

void execute_tree(target_t *targetTree, char *main,bool st, bool exe,bool timeS){
	int childPid;
	int i=0;
    int j=0;
    int Dindex=0;
	int  nTargets=0;
	int indexChecker=0;
	int completedProgress=0;
	int execount=0;
	pid_t child_pid[15];
    bool go; //determine whether all node children have completed compiling 
    if(exe==false){
		 while (targetTree[execount].name[0]!='\0'){
			if(targetTree[execount].commandline[0]!='\0'){
			printf("following commland line would be run : %s \n", targetTree[execount].commandline);}
			execount++; 
		}
		exit(0);
	}
    while (targetTree[nTargets].name[0]!='\0'){
		nTargets++; //update uTargets to fix the issue of tc 6 
	}
	if(st==true){
		Dindex=Search(main, targetTree);
		if(targetTree[Dindex].status!=READY || Dindex==-1){
			printf("cannot execute this target, either it is not a valid target or there is depedency require to be completed\n");
			exit(0);
		}
		else {
				char ** myargv;
				char *Ecommand=targetTree[Dindex].commandline;
				int countn=makeargv(Ecommand, " \n", &myargv);
				execvp(myargv[0], myargv);    
				printf("*** ERROR: exec failed\n");
				exit(0); 
		}
		
	}
    while (completedProgress<nTargets){ //while not all of the progress has been complied
		char *Tcommand=targetTree[i].commandline;
		if(strstr(Tcommand,"rm")!=NULL || targetTree[i].status==NEW){
			completedProgress++;
			targetTree[i].status=FINISHED; //mark it as finished 
			i++; 
			continue;
		}
		if(targetTree[i].status==INELIGIBLE){
				go=true;
				while(targetTree[i].depedency[j]->name!=NULL){
					indexChecker=targetTree[i].depedency[j]->index; 
					if (targetTree[indexChecker].status!=FINISHED){
						go=false;
					}
					j++;
				}  
				j=0;      
				if (go==true){
					targetTree[i].status=READY;
				}
				
			} 
		else if(targetTree[i].status==READY ){ 
			char ** myargv;
			int status;
			childPid=fork();
			if(childPid == -1){
				perror("ERROR: Failed to fork\n");
				exit(0);
			}
			else if (childPid == 0){
				char *Ecommand=targetTree[i].commandline;
				int countn=makeargv(Ecommand, " \n", &myargv);
				execvp(myargv[0], myargv);    
				printf("*** ERROR: exec failed\n");
				exit(0); 
			}
			else {                                  /* for the parent:      */
				  while (wait(&status) != childPid)       /* wait for completion  */
					   ;
			 }
			completedProgress++;
			targetTree[i].status=FINISHED; //mark it as finished 
		}
			
			i++; 
			if(targetTree[i].name[0]=='\0'){
				i=0;
			}
	
	
}
}


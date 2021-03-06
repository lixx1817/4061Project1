/********************
 * util.h
 *
 * You may put your utility function definitions here
 * also your structs, if you create any
 *********************/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>
#include <stdbool.h>

// the following ifdef/def pair prevents us from having problems if 
// we've included util.h in multiple places... it's a handy trick
#ifndef _UTIL_H_
#define _UTIL_H_

//You can change any value in this file as you want. 
/*#define INELIGIBLE 0
#define READY 1
#define RUNNING 2
#define FINISHED 3
*/
#define MAX_LENGTH 1024
#define MAX_DEPENDENCIES ar10
#define MAX_TARGETS 10

// This stuff is for easy file reading
FILE * file_open(char*);
char * file_getline(char*, FILE*);
char *trimwhitespace(char *str);
int is_file_exist(char *);
int get_file_modification_time(char *);
int compare_modification_time(char *, char *);
int makeargv(const char *s, const char *delimiters, char ***argvp);
void freemakeargv(char **argv);

// hand made functions below 
void makeRegex (regex_t *re, const char* pattern);
bool matchRegex (regex_t *re, const char *text);
int consumeWhiteSpaceAndComments (regex_t *whiteSpace, 
                                  regex_t *blockComment,
                                  regex_t *lineComment,
                                  const char *text) ;
int testFn(const char *str);
//You will need to fill this struct out to make a graph.


typedef struct listNode {
    char  name[30];
    int index; 
} lNode;
typedef struct target{
	char  name[30]; 
	char  commandline[50]; 
	char  options[20]; 
	bool targetExists;
    lNode * depedency[10];
    bool indepedent; 
    enum STATUS {INELIGIBLE, READY, NEW, FINISHED} status ;
}target_t;

target_t targetTree[1024];

int Search(char * name, target_t *targetTree );
bool file_exists(const char * filename);
void build_depedency(target_t *targetTree, bool Time);
void execute_tree(target_t *targetTree, char *main,bool st,bool exe,bool TimeS);


#endif

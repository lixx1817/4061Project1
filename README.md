/* CSci4061 F2014 Assignment 1
* login: cselabs login name (login used to submit)
* date: 10/03/14
* name: Hai Li, Hang Li, Zixiang Ma 
* id: 4454480, id for second name, 4644999 */ 
Purpose
To create a simple make utility similar to make in Linux. It could parse a file and generate a graph using different data structure to hold the dependencies information, and execute given command.

How to compile
Target without dependencies, meaning have no child, will be compile first. A parent will only be compiled when all its child has be compiled. 

How to use the program from the shell
-f filename: file specified will be renamed as makefile. 
-n: only show command 
-B: ignore timestamps and compile everything
-m log.txt: information will be output to file log.txt

What the program does
General algorithm starts with reading makefile line by line in the Parse function. Whitespace and comments will be neglected. Syntax will be checked and program will print out error message if coming across syntax error. Only command line and target line will be stored in an array temporarily, and a directed acyclic graph will be built based on dependencies relationship. Files will be complied based on the graph, and fork (), exec (), and wait () were used to execute command in specific order. 

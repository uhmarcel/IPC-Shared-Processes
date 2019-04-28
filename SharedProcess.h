/*
	  File name: SharedProcess.h
	     Author: Marcel Riera

	Description: Header file containing libraries, constants and
                 function prototypes required for the program.

    Requirements: The following files must be in the same folder for
      			  proper compilation:
      			- SharedMain.c
      			- SharedChild.c
      			- SharedProcess.h
*/

/* Included libraries */

#include <stdio.h>		// Required for input and output
#include <stdlib.h>		// Required for validation (atoi)
#include <string.h>		// Required for validation (strcmp, strcat)
#include <unistd.h>		// Required for fork
#include <sys/shm.h>	// Required for shared memory
#include <sys/wait.h>	// Required for wait() function

/* Declared constants */
#define MIN_ARGUMENTS 2		 // Minimum amount of arguments expected
#define MAX_ARGUMENTS 8 	 // Maximum amount of arguments expected
#define FIRST_ARGUMENT 1	 // First argument, disregarding path arg
#define ARG_MIN_VALUE 0		 // Minimum value for a given argument
#define ARG_MAX_VALUE 9		 // Maximum value for a given argument
#define INVALID_ARGUMENTS -1 // Return code for invalid arguments
#define NO_ERRORS 0			 // Return code for exit succesful
#define FAILED -1			 // Return code for exit failed
#define INDEX_OFFSET 1		 // Offset to compensate for array index 0
#define TRUE 1				 // True for boolean expressions
#define FALSE 0				 // False for boolean expressions
#define SHM_PROTOCOL 0666    // Protocol required for shared memory
#define FORK_CHILD 0		 // Fork ID return for a child process
#define BUFFER_SIZE 32		 // Arbitrary buffer size for strings
#define TAB_SIZE 2			 // Indentation spaces for trace displays

static const char* ORDER[] = {"", "first", "second", "third", "fourth",
							  "fifth", "sixth", "seventh"};

/* Function prototypes */

int validateCommandLine(int argc, char* argv[]);
int isUnique(int toTest, int arrayToTest[], int index);
char* toString(int arrayToDisplay[], int size);
void fillSharedMemory(int sharedMemory[], int argc, char* argv[]);
int parentForkChildProcesses(int parentPID, int childAmount, int* childID);
void parentWaitAllChildren(int argc);
int childMain(int childID, int* sharedMemory, const int size);
char* getPrefix(int childID);

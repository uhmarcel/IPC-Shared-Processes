
/*
   Source code:  SharedMain.c
        Author:  Marcel Riera

      Language:  C
   Compile/Run:  make build
                 ./SharedProcess.out 1 2 3 4 5 6 7

   Alternative:  gcc SharedMain.c SharedChild.c -o SharedProcess.out
                 ./SharedProcess.out 1 2 3 4 5 6 7

                 Where command arguments must be between 1 to 7 unique numbers
                 in the range 0-9.

  --------------------------------------------------------------------

  Description:  This program showcases the use of forks and multi process
  				communication with shared memory. The program traces the
  				steps of the parent process and subsequent childs in the
  				execution.

        Input:  Input is given upon execution as command line arguments,
        	    which must be between 1 to 7 unique numbers in the range 0-9.

       Output:  The program outputs the trace of current operation for each
       			process, displays the status of shared memory, modifies
       			main memory, and then shows the updated value.

      Process:  The program's steps are as follows
                1. parent starts
                2. parent validates command line
                3. parent request shared memory
                4. parent receives shared memory
                5. parent attaches shared memory
                6. parent fills shared memory
                7. parent displays shared memory
                8. parent forks all child
                	for each child:
                	1. child starts
                	2. child displays shared memory
                	3. child displays unique ID
                	4. child updates shared memory
                	5. child displays shared memory
                	6. child terminates
                9. parent waits for child
                10. parent displays child PID and exit code
                11. parent displays shared memory
                12. parent detaches shared memory
                13. parent removes shared memory
                14. parent terminates

  Requirements: The following files must be in the same folder for
      			proper compilation:
      			- SharedMain.c
      			- SharedChild.c
      			- SharedProcess.h

*/

#include "SharedProcess.h"  // Required program header

int main(int argc, char* argv[]) {
	/* Variable initialization */
	int* sharedMemory = NULL;
	int parentPID = 0;
	int childID = 0;
	int shmID = 0;
	int count = 0;

	/* Process */
	puts("Parent: starts");
	setbuf(stdout, NULL);
	puts("Parent: validates command line\n");
	if (validateCommandLine(argc, argv) == INVALID_ARGUMENTS) {
		return FAILED;
	} //endif

	puts("Parent: requests shared memory");
	shmID = shmget(IPC_PRIVATE, argc * sizeof(int), IPC_CREAT | SHM_PROTOCOL);
	if (shmID < 0) {
		return FAILED;
	} //endif
	puts("Parent: receives shared memory");

	puts("Parent: attaches shared memory");
	sharedMemory = (int*) shmat(shmID, NULL, 0);
	if (sharedMemory[0] == FAILED) {
		return FAILED;
	} //endif

	puts("Parent: fills shared memory");
	fillSharedMemory(sharedMemory, argc, argv);
	printf("Parent: displays shared memory -> %s\n\n", toString(sharedMemory,
		argc));
	parentPID = getpid();
	if (parentForkChildProcesses(parentPID, argc, &childID) == FAILED) {
		return FAILED;
	} //endif
	if (getpid() != parentPID) {
		childMain(childID, sharedMemory, argc);
		return NO_ERRORS;
	} //endif

	parentWaitAllChildren(argc);
	printf("\nParent: displays shared memory -> %s\n", toString(sharedMemory,
		argc));
	puts("Parent: detaches shared memory");
	shmdt((void*) sharedMemory);
	sharedMemory = NULL;
	puts("Parent: removes shared memory");
	shmctl(shmID, IPC_RMID, NULL);
	puts("Parent: finished");

	/* Exit succesful */
	return NO_ERRORS;
} //endmain

/* FUNCTIONS */


/**
 * Function parentForkChildProcesses
 * Creates a given amount of child process by the use of the fork function.
 * It also assigns each child a unique ID
 * @param parentPID  the process ID of the parent
 * @param childAmount  the amount of childs to spawn
 * @param *childID  pointer to a variable to store assigned child ID
 * @return 	NO_ERRORS if succesful, FAILED otherwise
 **/
int parentForkChildProcesses(int parentPID, int childAmount, int* childID) {
	int forkID = 0;
	int childCount = 0;

	for (childCount = INDEX_OFFSET; childCount < childAmount; childCount++) {
		if (getpid() == parentPID) {
			printf("Parent: forks %s child process\n", ORDER[childCount]);
			forkID = fork();
			if (forkID < 0) {
				puts("Parent: ERROR - Fork failed. Child couldn't be created.");
				return FAILED;
			} //endif
			else if (forkID == FORK_CHILD) {
				*childID = childCount;
			} //endif
		} //endif
	} //endfor
} //endfunction

/**
 * Function parentWaitAllChildren
 * This functions when running on parent waits for all children to finish,
 * then corresponding trace messages, child process id, and exit code.
 * @param argc  command line input arguments amount
 **/
void parentWaitAllChildren(int argc) {
	int childCount = 0;
	int status = 0;
	int childPID = 0;

	for (childCount = INDEX_OFFSET; childCount < argc; childCount++) {
		printf("Parent: waits for %s child\n", ORDER[childCount]);
		childPID = wait(&status);
		printf("Parent: detects %s child completion\n", ORDER[childCount]);
		printf("Parent: displays %s child PID %d & exit code %d\n",
			ORDER[childCount], childPID, status);
	} //endfor
} //endif

/**
 * Function fillSharedMemory
 * Fills the given array shared memory with the values corresponding
 * to the command line input.
 * @param *sharedMemory  pointer to the program's shared memory
 * @param argc  command line input arguments amount
 * @param *argv[]  command line input arguments as an array of strings
 **/
void fillSharedMemory(int* sharedMemory, int argc, char* argv[]) {
	int index = 0;

	for (index = INDEX_OFFSET; index < argc; index++) {
		sharedMemory[index] = atoi(argv[index]);
	} //endfor
} //endfunction


/**
 * Function validateCommandLine
 * Validates this execution's command line input to match the requirements
 * of argument amount, argument format, argument range, and argument
 * uniqueness.
 * @param argc  command line input arguments amount
 * @param *argv[]  command line input arguments as an array of strings
 * @return  NO_ERRORS if is valid, INVALID_ARGUMENTS otherwise
 **/
int validateCommandLine(int argc, char* argv[]) {
	if (argc < MIN_ARGUMENTS || argc > MAX_ARGUMENTS) {
		puts("Error: Invalid amount of arguments.");
		return INVALID_ARGUMENTS;
	} //endif

	int validatedValues[argc]; // Can't initialize variable length array
	int count = 0;

	for (count = FIRST_ARGUMENT; count < argc; count++) {
		int parsedInt = atoi(argv[count]);
		if (parsedInt == 0 && strcmp(argv[count],"0")) {
			puts("Error: Non-numeric argument found.");
			return INVALID_ARGUMENTS;
		} //endif
		if (parsedInt < ARG_MIN_VALUE || parsedInt > ARG_MAX_VALUE) {
			puts("Error: Argument out of range found.");
			return INVALID_ARGUMENTS;
		} //endif
		if (!isUnique(parsedInt, validatedValues, count)) {
			puts("Error: Non-unique argument found.");
			return INVALID_ARGUMENTS;
		} //endif
		validatedValues[count] = parsedInt;
	} //endfor
	return NO_ERRORS;
} //endfunction

/**
 * Function isUnique
 * Boolean function to determine wether the given value already exists
 * on the given array up to a certain index.
 * @param toTest  integer to check if exists on array
 * @param arrayToCheck[]  array to perform the check on
 * @param upTo  last index to check for uniqueness
 * @return  TRUE if number is not on array, FALSE otherwise
 **/
int isUnique(int toTest, int arrayToCheck[], int upTo) {
	int index = 0;

	for (index = 0; index < upTo; index++) {
		if (arrayToCheck[index] == toTest) {
			return FALSE;
		} //endif
	} //endfor
	return TRUE;
} //endfunction

/**
 * Function toString
 * Returns an string representation of a given array for display.
 * @param arrayToDisplay[]  the given array to be make string
 * @param size  the size of the input array
 * @return  a string representing the array
 **/
char* toString(int arrayToDisplay[], int size) {
	static char arrayStr[BUFFER_SIZE] = "";
	char currentStr[BUFFER_SIZE] = "";
	int index = 0;

	strcpy(arrayStr, "");
	for (index = INDEX_OFFSET; index < size; index++) {
		sprintf(currentStr, "%d ", arrayToDisplay[index]);
		strcat(arrayStr, currentStr);
	} //endfor
	return arrayStr;
} //endfunction

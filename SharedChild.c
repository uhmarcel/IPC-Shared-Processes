
/*
	Source code: SharedChild.c
	     Author: Marcel Riera

	Description: Source code containing the functions for the
				 child processes.

	Requirements: The following files must be in the same folder for
      			  proper compilation:
      			- SharedMain.c
      			- SharedChild.c
      			- SharedProcess.h
*/

#include "SharedProcess.h"  // Required program header

/**
 * Function childMain
 * Function equivalent to main but for child processes created with
 * the fork function. It displays shared memory, the childs ID,
 * performs and update to the memory, and finally displays the updated
 * value.
 * @param childID  the unique ID assigned to the child upon creation
 * @param *sharedMemory  pointer to the program's shared memory
 * @param size  the size of the shared memory array
 * @return  the childs execution exit code
 **/
int childMain( int childID, int* sharedMemory, const int size) {
	printf("%s starts\n", getPrefix(childID));

	printf("%s displays shared memory -> %s\n", getPrefix(childID),
		toString(sharedMemory, size));
	printf("%s displays private unique ID -> %d\n", getPrefix(childID),
		childID);

	printf("%s updates shared memory\n", getPrefix(childID));
	sharedMemory[childID] *= childID;

	printf("%s displays shared memory -> %s\n", getPrefix(childID),
		toString(sharedMemory, size));

	printf("%s exits with code %d\n", getPrefix(childID), NO_ERRORS);
	return NO_ERRORS;
} //endfunction

/**
 * Function getPrefix
 * Constructs the expected prefix for a child process to display its
 * trace messages.
 * @param childID  the unique ID assigned to the child upon creation
 * @return  a string representing the child's display prefix
 **/
char* getPrefix(int childID) {
	static char prefix[BUFFER_SIZE] = "";
	char indent[BUFFER_SIZE] = "";
	char childName[BUFFER_SIZE] = "";
	int index = 0;

	for (index = 0; index < TAB_SIZE * childID; index++) {
		indent[index] = ' ';
	} //endif
	sprintf(childName, "Child ID: %d", childID);
	strcat(indent, childName);
	strcpy(prefix, indent);
	return prefix;
} //endfunction

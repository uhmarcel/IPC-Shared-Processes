
# Makefile for SharedProcess program 
# Author: Marcel Riera				 


# Files required for compilation
FILES = SharedMain.c SharedChild.c

# Name of executable
OUT = SharedProcess.out

build: $(FILES)
	gcc $(FILES) -o $(OUT)
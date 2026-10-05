#include <stdio.h>
#include <stdlib.h>
#include "hash.h"


char* extractFileToMem(char* fileName){

	FILE* rainbowTableFile = NULL;
	char* myRainbowTable = NULL;
	long sizeOfFile = 0;

	rainbowTableFile = fopen(fileName, "r");

	if(!rainbowTableFile){ printf("File can't open"); }

	fseek(rainbowTableFile, 0, SEEK_END); // Set cursor to EOF
	sizeOfFile=ftell(rainbowTableFile); // Read number of byte the cursor is from SOF
	printf("sizeOfFile = %ld \n", sizeOfFile);
	rewind(rainbowTableFile); // Reset Cursor

	myRainbowTable = (char *) malloc(sizeOfFile * sizeof(char) + 1);
	fread(myRainbowTable, 1, sizeOfFile, rainbowTableFile);
	return myRainbowTable;
}

int main() {

	int i = 0;
	char* hashTable;

	hashTable = extractFileToMem("./rainbow.txt");
	printf("%s", hashTable);
//	for (int j = 0; j < (sizeof(hashtable) / 8), j++){
		printf("%d", sizeof(hashTable) / 8);	
//	}
	free(hashTable); //free the extractFileToMem's allocated var 
	return i;

}

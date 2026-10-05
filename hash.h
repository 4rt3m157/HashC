#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define HASH_SIZE 32
#define MAX_PASSWORD_SIZE 32
#define NUMBER_OF_PASSWORD 10


typedef struct{
    char* word;
	unsigned char hash[HASH_SIZE];
}Entry;

Entry* createEntries(char* fileName, char* algo);
void freeWords(Entry* entryList, uint count);

char* readLine(char* text);
char* readHash(char* line);
int compareHash(char* knownHash, char* hash );
char* splitHashText(char* hashTable);


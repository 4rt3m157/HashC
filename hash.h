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

Entry* createEntries(char* fileName, const EVP_MD * algo_hashage);

int hashWord(char * mot_original, u_char * hash_du_mot, u_int taille_hash, const EVP_MD * algo_hashage );
int read_and_hash(char * fileName, Entry* entryList, const EVP_MD * algo_hashage);

void freeWords(Entry* entryList, uint count);
void printHash(const unsigned char* hash, unsigned int len);

char* readLine(char* text);
char* readHash(char* line);
int compareHash(char* knownHash, char* hash );
char* splitHashText(char* hashTable);


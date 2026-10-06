#include <openssl/sha.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <sys/types.h>



// taille max de entryList
#define NUMBER_OF_PASSWORD 1000

// taille d'un hash SHA-256
#define HASH_SIZE 32

// un mot + son hash
typedef struct {
	char *word;
	u_char hash[HASH_SIZE];
} Entry;

int hashWord(char *mot, u_char *hash_du_mot, u_int *taille_hash, const EVP_MD *algo_hashage);
void printHash(const unsigned char *hash, unsigned int len);
int read_and_hash(char *fileName, Entry *entryList, const EVP_MD *algo_hashage, u_int *count);
void freeWords(Entry *entryList, uint *count);

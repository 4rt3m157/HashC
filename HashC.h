#ifndef HASHC_H
#define HASHC_H

#include <openssl/evp.h>
#include <sys/types.h>
#include <openssl/bio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>



// taille max de entryList
#define NUMBER_OF_PASSWORD 1000

// taille d'un hash SHA-256
#define SHA256_HASH_SIZE 32
#define MD5_HASH_SIZE 16

// un mot + son hash
typedef struct {
    char *word;
    u_char hash[SHA256_HASH_SIZE];
} Entry;

int hashWord(char *mot, u_char *hash_du_mot, u_int *taille_hash, const EVP_MD *algo_hashage);
void printHash(const unsigned char *hash, unsigned int len);
int read_and_hash(char *fileName, Entry **entryList, const EVP_MD *algo_hashage, u_int *count);
void freeWords(Entry *entryList, uint *count);
void transformStringtoHex(char *hashString, char *hashHex);
#endif /* HASHC_H */

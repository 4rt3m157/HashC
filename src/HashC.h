#ifndef HASHC_H
#define HASHC_H

// INCLUDE
//OPENSSL
#include <openssl/evp.h>
#include <openssl/core_dispatch.h>
//STD
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
//OTHERS
#include <string.h>
#include <sys/types.h>
#include <getopt.h>

//CONSTANTES
#define MAX_HASHTABLE 65536

//STRUCT
typedef struct Entry {
    char *word;
    unsigned int sizeHash;
    unsigned char hash[EVP_MAX_MD_SIZE];
    struct Entry *suivant;
} Entry;

typedef struct {
    Entry *field[MAX_HASHTABLE];
} HashTable;

//Utilitaires
HashTable *table_new(void);
unsigned int get_field_index(unsigned char *hash, unsigned int hashSize);
int hashWord(char *mot, u_char *hash_du_mot, u_int *taille_hash,EVP_MD *algo_hashage);
unsigned int hashTableIndex(unsigned char *hash, unsigned int hash_len);
char LetterToHexa(char car);
void transformStringtoHex(char *hashString, char *hashHex);
void printHash(unsigned char *hash, unsigned int len);
//G
int reaload_Password(char *passwordsPath, EVP_MD *algo, HashTable **ptrTable);
int add_Entry_to_Table(HashTable *table_de_hash, char *mot, EVP_MD *algo);
int create_Binary_Dictionary(char *path, HashTable *hashTable);
int check_Generation(char *passwordsPath, char *outputPath, EVP_MD *algo);
//L
int load_Table(char *path, HashTable **ptrTable);
int find_Hash(char *tablePath, char *hashPath);
//FREE
void free_Table(HashTable *t);
void free_Words(Entry *entrie);

#endif
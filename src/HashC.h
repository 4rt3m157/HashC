#ifndef HASHC_H
#define HASHC_H

// INCLUDE

// OPENSSL
#include <openssl/core_dispatch.h>
#include <openssl/evp.h>
// STD
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
// OTHERS
#include <getopt.h>
#include <string.h>
#include <sys/types.h>

// CONSTANTES
#define MAX_HASHTABLE 65536
#define MAX_PASSWORD 15000000
// STRUCT
typedef struct Entry {
  char *word;
  unsigned int sizeHash;
  unsigned char hash[EVP_MAX_MD_SIZE];
  struct Entry *suivant;
} Entry;

typedef struct {
  Entry *field[MAX_HASHTABLE];
} HashTable;

// Utilitaires
HashTable *table_new(void);
unsigned int get_field_index(unsigned char *hash);
int hashWord(char *mot, u_char *hash_du_mot, u_int *taille_hash,
             EVP_MD *string_algo_hashage);
unsigned int hashTableIndex(unsigned char *hash, unsigned int hash_len);
char LetterToHexa(char car);
void transformStringtoHex(char *hashString, int hashSize, char *hashHex);
void printHash(unsigned char *hash, unsigned int len);
// G
int reaload_Password(char *passwordsPath, EVP_MD *algo, HashTable **ptrTable);
int add_Entry_to_Table(HashTable *table_de_hash, char *mot, EVP_MD *algo);
int create_Binary_Dictionary(char *binaryPath, HashTable *hashTable);
int check_Generation(char *passwordsPath, char *outputPath, EVP_MD *algo);
// L
int load_Binary_Dictionnary(char *binaryPath, HashTable **ptrTable);
int find_Hash(HashTable *tablePath, char *hashPath);
// FREE
void free_Table(HashTable *t);
void free_Words(Entry *entrie);

#endif

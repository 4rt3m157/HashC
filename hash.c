#include "hash.h"
#include <inttypes.h>
#include <openssl/bio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

Entry* createEntries(char* fileName, const EVP_MD * algo_hashage){
    
    /*
        VARIABLES POUR :
    */
    //compteur
        int i = 0;
	Entry* entryList = NULL;
	entryList = calloc(NUMBER_OF_PASSWORD,sizeof(Entry));
	read_and_hash(fileName, entryList, algo_hashage);
	
	return entryList;

}
int read_and_hash(char * fileName, Entry* entryList, const EVP_MD * algo_hashage){
	size_t len = 0; // buffer géré par getline()
	char* lineBuffer = NULL; // buffer qui prend la ligne
	long sizeOfLine = 0; // taille de la ligne
	int i = 0;

	//open file
	FILE* rainbowTableFile = NULL;
    rainbowTableFile = fopen(fileName, "r");
	
	//lire fichier ligne / ligne
	while ((sizeOfLine = getline(&lineBuffer, &len, rainbowTableFile)) != -1){
		lineBuffer[--sizeOfLine] = '\0';
		entryList[i].word = strdup(lineBuffer); // ajouter le mot de la ligne dans l'entry i
		i++;
		if (entryList[i].word) {
			if (hashWord(entryList[i].word,entryList[i].hash, sizeof(entryList[i].hash),algo_hashage)) printf("hash bien gén");
			return 1;
		}

	}
	return 0;
}
// Calcul du hash SHA-256 avec EVP (openssl)
int hashWord(char * mot_original, u_char * hash_du_mot, u_int taille_hash,const EVP_MD * algo_hashage ){
	//retour en code d'err parce que EVP_Digest le mets directement où il faut
	return EVP_Digest(mot_original, strlen(mot_original), hash_du_mot, &taille_hash, algo_hashage, NULL);
}
void printHash(const unsigned char* hash, unsigned int len) {
    for (unsigned int k = 0; k < len; k++)
        printf("%02x", hash[k]);
    printf("\n");
}
void freeWords(Entry* entryList, uint count){
    if (entryList){
        for (uint i = 0; i < count; i++) 
            free(entryList[i].word);     
        free(entryList);
    }
}
int main(int argc,char** argv) {

    int i = 0;
    Entry* entryList;
    
    char *filename = argv[1];
    char *algo_name = argv[2];
    
    if (argc < 3) {
            printf("Write the hashing Algorithm you want :\n SHA-256, MD5, SHA-1,... ");
            scanf("%s", algo_name);
    }


    else algo_name = argv[2];
	const EVP_MD * algo_hashage = EVP_get_digestbyname(algo_name);

    entryList = createEntries(filename, algo_hashage);
    printf(" \n entryList[0].word = %s \n hash = ", entryList[1].word);
	printHash(entryList[1].hash, 32);
    free(entryList); //free the list of all struct 'word+hash' allocated  
    return i;

}
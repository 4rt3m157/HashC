#include "hash.h"
#include <inttypes.h>
#include <openssl/bio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Entry* createEntries(char* fileName, char* algo){
    
    /*
        VARIABLES POUR :
    */
    //compteur
        int i = 0;
	Entry* entryList = NULL;
	entryList = malloc(NUMBER_OF_PASSWORD*sizeof(Entry));
	printf("assez de ram visibly");

    //ouverture fichier
    FILE* rainbowTableFile = NULL;
    rainbowTableFile = fopen(fileName, "r");
    if(rainbowTableFile){
		printf("ca s'ouvre");
        //lecture fichier
        char* randBuffer = NULL;
        size_t len = 0;
        char* lineBuffer = NULL;
        long sizeOfLine = 0;
        
        // création struct hash + passwd
        if (entryList){
            printf("no entryList");
            //hash
            //création du pointeur vers l'algo depuis l'argument donné au programme
            const EVP_MD * algo_hashage = EVP_get_digestbyname(algo);
			
            if (algo_hashage) {
				printf("alors l'algo");
                uint taille_hash;
                // pointeur vers la structure qui gère la crypto
                EVP_MD_CTX *addr_contxt_crypto = EVP_MD_CTX_new();

                // tant que getline lis le nombre de char sur la ligne
                while ((sizeOfLine = getline(&lineBuffer, &len, rainbowTableFile)) != -1)
                {
                    if (i >= NUMBER_OF_PASSWORD) {
                        printf("maximum capacité de hashage atteinte, \ndiviser votre fichier en fichier de %d Lignes (parce que j'ai décidé)", NUMBER_OF_PASSWORD);
                        break;    
                    }
                    //ajouter \0 a la place du \n pour que le strdup() fonctionne
                    lineBuffer[--sizeOfLine] = '\0';
                    
                    //mettre le passwd dans une struct Entry 
                    entryList[i].word = strdup(lineBuffer);
                    if (entryList[i].word) {
                        // Calcul du hash SHA-256 avec l'API EVP (openssl)
                        EVP_DigestInit_ex(addr_contxt_crypto, algo_hashage, NULL);
                        EVP_DigestUpdate(addr_contxt_crypto, entryList[i].word, strlen(entryList[i].word));
                        EVP_DigestFinal_ex(addr_contxt_crypto, entryList[i].hash, &taille_hash );
                        i++;
                    }
                    else {
                        printf("entryList[i].word == NULL");
                        freeWords(entryList, i);
                        fclose(rainbowTableFile);
					}    
                }
                    fclose(rainbowTableFile);
                    EVP_MD_CTX_free(addr_contxt_crypto);    
            }
            else {
                printf("algo_hashage == NULL");
                freeWords(entryList, i);
                fclose(rainbowTableFile);
			}
			return entryList;

		}    
    }
	return entryList;

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

    entryList = createEntries(filename, algo_name);
    printf(" entryList[0].word = %s \t entryList[0].hash = %s\t", entryList[0].word, entryList[0].hash);
    free(entryList); //free the list of all struct 'word+hash' allocated  
    return i;

}
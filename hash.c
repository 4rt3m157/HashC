#include "hash.h"
#include <inttypes.h>
#include <openssl/bio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/**
 * @brief Calcul du hash avec EVP (openssl)
 *
 * @param mot          mot du fichier à hasher
 * @param hash_du_mot  bufferdans lequel EVP_Digest écrit directement le hash calculé
 * @param taille_hash  pointeur où EVP_Digest écrit la taille du hash calculé // mettre dans Entry pour possibilité d'évolution
 * @param algo_hashage algorithme à utiliser 
 *
 * @return 1 si le hash a été calculé avec succès, 0 sinon 
 */

// Calcul du hash SHA-256 avec EVP (openssl)
int hashWord(char * mot, u_char * hash_du_mot, u_int *taille_hash,const EVP_MD * algo_hashage ){
	
	int result = 0;

	if(EVP_Digest(mot, strlen(mot), hash_du_mot, taille_hash, algo_hashage, NULL)) result = 1;
	
	/*
	Retour en code int parce que EVP_Digest met le hash directement où il faut. 
	donc possibilité de prendre directment la sortie de la fonction en condition.
	on pourrait aussi la mettre en return, ou directement dans la code, mais c'est plus agréable à lire comme ca.
	*/
	return result;
}
/**
 * @brief affiche en hexa octet / octet le hash passé en paramètre
 *
 * @param hash pointeur vers le buffer contenant le hash à afficher
 * @param len  taille du hash à afficher
 *
 * @return void
 */
void printHash(const unsigned char* hash, unsigned int len) {
	
	for (unsigned int k = 0; k < len; k++)
		printf("%02x", hash[k]);
	printf("\n");
}

/**
 * @brief Stock les mots d'un fichier formaté tel que : 1 mot = 1 ligne et stocke chaque mot lu 
 *			dans une Entry stocké dans entryList, une liste d'entry, puis calcule le hash correspondant pour chaque mot
 *
 * @param fileName     chemin vers le fichier à lire
 * @param entryList    tableau d'Entry dans lequel sont stockés les mots lus et leurs hashs
 * @param algo_hashage algorithme de hashage à utiliser (ex: SHA-256, MD5, SHA-1...)
 * @param count        pointeur utilisé pour compter le nombre de mots ajoutés (sert ensuite pour freeWords)
 *
 * @return le nombre d'entries pour lesquelles le hash a été calculé avec succès
 */
int read_and_hash(char * fileName, Entry* entryList, const EVP_MD * algo_hashage, u_int* count){

	int i = 0;
	int result = 0;

	size_t len = 0; // buffer géré par getline()
	char* lineBuffer = NULL; // buffer qui prend la ligne
	long sizeOfLine = 0; // taille de la ligne

	//open file
	FILE* rainbowTableFile = NULL;
	rainbowTableFile = fopen(fileName, "r");

	//lire fichier ligne / ligne
	while ((sizeOfLine = getline(&lineBuffer, &len, rainbowTableFile)) != -1){
		lineBuffer[--sizeOfLine] = '\0';
		entryList[i].word = strdup(lineBuffer); // ajouter le mot de la ligne dans l'entry i
		if (entryList[i].word != NULL) {
			// augment le count qui est utilisé pour freeWords
			(*count)++;
			u_int taille = 0;	
			// Si le hash est entré, le Entry[i] est OK, increment result	
			if (hashWord(entryList[i].word,entryList[i].hash, &taille,algo_hashage))
			{
				printf("entryList[%d].word = %s\n hash du mot = ",i, entryList[i].word);
				printHash(entryList[i].hash, 32);
				result++;
			}
		}	
		i++;
	}
	return result;
}

/**
 * @brief Libère la mémoire allouée pour les mots (word) de chaque Entry,
 *        ainsi que le tableau entryList lui-même
 *
 * @param entryList tableau d'Entry dont les mots et le tableau doivent être libérés
 * @param count     pointeur du nombre d'entries dans entryList
 *
 * @return void
 */
void freeWords(Entry* entryList, uint *count){
	if (entryList){
		for (uint i = 0; i < *count; i++) 
			free(entryList[i].word);     
		free(entryList);
	}
}

int main(int argc,char** argv) {
	char *filename = argv[1];
	char *algo_name = argv[2];
	u_int count = 0;

	int i = 0;
	
	// liste d'Entry & allocation
	Entry* entryList;
	entryList = calloc(NUMBER_OF_PASSWORD,sizeof(Entry));
	// nombre d'entry créée
	int entryNumber = 0;
	// u_int count = 0;

	if (argc < 3) {
		printf("Write the hashing Algorithm you want :\n SHA-256, MD5, SHA-1,... \n");
		scanf("%s", algo_name);
	}
	else algo_name = argv[2];
	
	printf("Algo choice : %s \n" ,algo_name);

	const EVP_MD * algo_hashage = EVP_get_digestbyname(algo_name);
	entryNumber = read_and_hash(filename, entryList, algo_hashage, &count);
	
	Entry personnal_entry;

	u_int taille = 32;
	//demande le mdp qu'on veut trouver
	char passwd_to_find[32] = "";
	printf("Write the password you need to find : \n"); 
	scanf("%s", passwd_to_find);
	
	personnal_entry.word = passwd_to_find;

	if ((hashWord(passwd_to_find, personnal_entry.hash, &taille , algo_hashage))) {	
		// memcpy( personnal_entry.hash, personnal_entry.hash, 32);
		printf("personnal_entry.word : %s \t hash :",personnal_entry.word);
		printHash(personnal_entry.hash, 32);
		int j = 0;
		int retour;
		printf("Nombre de hash à vérifier : %d\n", count);
		for (j = 0; j <= count; j++) {
			retour = memcmp(entryList[j].hash, &personnal_entry.hash, 32);
			if(retour == 0){
				printf("\n password = %s\n", entryList[j].word);
				break;
			}
		}
	}
	
	freeWords(entryList, &count); //free the list of all struct 'word+hash' allocated  
	return i;

}

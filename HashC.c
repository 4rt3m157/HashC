#include "hashC.h"

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
int hashWord(char *mot, u_char *hash_du_mot, u_int *taille_hash, const EVP_MD *algo_hashage) {
    int result = 0;
    if (EVP_Digest(mot, strlen(mot), hash_du_mot, taille_hash, algo_hashage, NULL))
        result = 1;
    /*
    Retour en code int parce que EVP_Digest met le hash directement où il faut.
    donc possibilité de prendre directment la sortie de la fonction en condition.
    on pourrait aussi la mettre en return, ou directement dans la code, mais c'est plus agréable à lire comme ca.
    */
    return result;
}

/**
 * @brief convertit les char hexa ('0-9' 'A-F') en octets Hexa.
 * "3F4231EA4D"[10] -> 3F 42 31 EA 4D [5]
 *
 * @param char car en ascii à convertir
 * @return char l'octet en binaire
 */
char LetterToHexa(char car) {
    char octet;
    if (car <= '9' && car >= '0') {
        octet = car - '0';
    } else if (car <= 'F' && car >= 'A') {
        octet = car - 'A' + 10;
    } else if (car <= 'f' && car >= 'a') {
        octet = car - 'a' + 10;
    }
    return octet;
}

/**
 * @brief convertit String char * "5d41402abc4b2a76b9719d911017c592" en octets char * 5d41402abc4b2a76b9719d911017c592
 * divise donc par 2 le nombre de mémoire prise pour stocker un hash
 * @param hashString la chaine de charactère representant le hash
 * recherché
 * @param hashHex pointeur vers le résultat
 * @return void
 */
void transformStringtoHex(char *hashString, char *hashHex) {
    for (size_t i = 0; i < 64; i += 2) {
        hashHex[i / 2] = (LetterToHexa(hashString[i]) << 4);
        hashHex[i / 2] = hashHex[i / 2] | LetterToHexa(hashString[i + 1]);
    }
}

/**
 * @brief affiche en hexa octet / octet le hash passé en paramètre
 *
 * @param hash pointeur vers le buffer contenant le hash à afficher
 * @param len  taille du hash à afficher
 *
 * @return void
 */
void printHash(const unsigned char *hash, unsigned int len) {
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
int read_and_hash(char *fileName, Entry **entryListPtr, const EVP_MD *algo_hashage, u_int *count) {
    int result = 0;
    size_t len = 0;
    char *lineBuffer = NULL;
    long sizeOfLine = 0;

    size_t capacity = 16;
    Entry *entryList = malloc(capacity * sizeof(Entry));
    if (entryList == NULL) return -1;

    FILE *rainbowTableFile = fopen(fileName, "r");
    if (rainbowTableFile == NULL) {
        free(entryList);
        return -1;
    }

    while ((sizeOfLine = getline(&lineBuffer, &len, rainbowTableFile)) != -1) {
        if (*count >= capacity) {
            capacity *= 2;
            Entry *tmp = realloc(entryList, capacity * sizeof(Entry));
            if (tmp == NULL) {
                result = -1;
                break;
            }
            entryList = tmp;
        }

        if (sizeOfLine > 0 && lineBuffer[sizeOfLine - 1] == '\n')
            lineBuffer[--sizeOfLine] = '\0';

        if (sizeOfLine == 0) continue; // ligne vide

        entryList[*count].word = strdup(lineBuffer);
        u_int taille = 0;
        if (hashWord(entryList[*count].word, entryList[*count].hash, &taille, algo_hashage)) {
            (*count)++;
            result++;
        } else {
            free(entryList[*count].word);
        }
    }

    free(lineBuffer);
    fclose(rainbowTableFile);
    *entryListPtr = entryList;
    return result;
}

int create_binary_dictionary(char *fileName, Entry *entryList, u_int *count) {
    int result = 0;
    FILE *dictionaryFile = NULL;
    dictionaryFile = fopen(fileName, "wb");

    if (dictionaryFile != NULL) {
        for (int i = 0; i < *count; ++i) {
            if (fwrite(entryList[i].word, sizeof(entryList[i].word), 1, dictionaryFile) != -1) {
                result = -1;
                break;
            }
            if (fwrite(entryList[i].hash, sizeof(entryList[i].hash), 1, dictionaryFile) != -1) {
                result = -1;
                break;
            }
        }
        fclose(dictionaryFile);
    } else
        result = -1;
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
void freeWords(Entry *entryList, uint *count) {
    if (entryList) {
        for (uint i = 0; i < *count; i++)
            free(entryList[i].word);
        free(entryList);
    }
}

int main(int argc, char **argv) {
    char *filename = argv[1];
    char *algo_name = argv[2];
    u_int count = 0;
    int retour;

    int i = 0;

    // liste d'Entry & allocation
    Entry *entryList;

    entryList = calloc(NUMBER_OF_PASSWORD, sizeof(Entry));

    // nombre d'entry créée
    int entryNumber = 0;

    // u_int count = 0;

    if (argc < 3) {
        printf("Write the hashing Algorithm you want :\n SHA-256, MD5, SHA-1,... \n");
        scanf("%s", algo_name);
    } else algo_name = argv[2];

    printf("Algo choice : %s \n", algo_name);

    const EVP_MD *algo_hashage = EVP_get_digestbyname(algo_name);
    entryNumber = read_and_hash(filename, &entryList, algo_hashage, &count);

    if (create_binary_dictionary(filename, entryList, &count))
        printf("Creation of dictionary returned successfully\n");

    else
        printf("Creation of dictionary returned error\n");

    Entry personnal_entry;
    personnal_entry.word = "";
    u_int taille = 32;
    //demande le mdp qu'on veut trouver
    char hash_to_find[64] = "";
    printf("Write the hash you need to find : \n");
    fgets(hash_to_find, 64, stdin);
    if (hash_to_find[-1] == '\n')
        hash_to_find[-1] = '\0';
    transformStringtoHex(hash_to_find, personnal_entry.hash);

    if ((hashWord(hash_to_find, personnal_entry.hash, &taille, algo_hashage))) {
        // memcpy( personnal_entry.hash, personnal_entry.hash, 32);
        printf("personnal_entry.hash : %s \t word : <to find>", personnal_entry.hash);
        printHash(personnal_entry.hash, 32);
        int j = 0;
        int retour;
        printf("Nombre de hash à vérifier : %d\n", count);
        for (j = 0; j <= count; j++) {
            retour = memcmp(entryList[j].hash, &personnal_entry.hash, 32);
            if (retour == 0) {
                printf("\n password = %s\n", entryList[j].word);
                break;
            }
        }
    }

    freeWords(entryList, &count); //free the list of all struct 'word+hash' allocated
    return i;
}


#include "HashC.h"

unsigned int get_field_index(unsigned char *hash) {
  unsigned int index = 0;
  index = (unsigned int)hash[0] << 8;
  index = index | hash[1];
  return index;

}
/**
 * @brief Calcul du hash avec EVP (openssl)
 *
 * @param mot mot du fichier à hasher
 * @param hash_du_mot  bufferdans lequel EVP_Digest écrit directement le hash
 * calculé
 * @param taille_hash  pointeur où EVP_Digest écrit la taille du hash calculé //
 * mettre dans Entry pour possibilité d'évolution
 * @param algo_hashage algorithme à utiliser
 *
 * @return 1 si le hash a été calculé avec succès, 0 sinon
 */
int hashWord(char *mot, u_char *hash_du_mot, u_int *taille_hash,EVP_MD *algo) {
  int result = -1;
  if (EVP_Digest(mot, strlen(mot), hash_du_mot, taille_hash, algo, NULL))
    result = 1;
  /*
  Retour en code int parce que EVP_Digest met le hash directement où il faut.
  donc possibilité de prendre directment la sortie de la fonction en condition.
  on pourrait aussi la mettre en return, ou directement dans la code, mais c'est
  plus agréable à lire comme ca.
  */
  return result;
}
unsigned int hashTableIndex(unsigned char *hash, unsigned int hash_len) {
  int index = 0;
  index = (unsigned int)hash[0] << 8;
  index = index | hash[1];
  return index;
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
  if (car <= '9' && car >= '0')
    octet = car - '0';
  else if (car <= 'F' && car >= 'A')
    octet = car - 'A' + 10;
  else if (car <= 'f' && car >= 'a')
    octet = car - 'a' + 10;
  else
    octet = -1;
  return octet;
}

/**
 * @brief convertit String char * "5d41402abc4b2a76b9719d911017c592" en octets
 * char * 5d41402abc4b2a76b9719d911017c592 divise donc par 2 le nombre de
 * mémoire prise pour stocker un hash
 * @param hashString la chaine de charactère representant le hash
 * recherché
 * @param hashHex pointeur vers le résultat
 * @return void
 */
void transformStringtoHex(char *hashString, int hashSize, char *hashHex) {
  for (size_t i = 0; i < hashSize; i += 2) {
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
void printHash(unsigned char *hash, unsigned int len) {
  for (unsigned int k = 0; k < len; k++)
    printf("%02x", hash[k]);
  printf("\n");
}

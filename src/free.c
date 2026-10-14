#include "HashC.h"
/**
 * @brief free les mots de chaque Entry,
 *        ainsi que le tableau entryList lui-même
 *
 * @param entryList* adresse d'une entrie dans un field de la hashTable.
 * Les mots doivent être libérés
 *
 * @return void
 */
void free_Words(Entry *entrie) {
  if (entrie->word != NULL) {
    free(entrie->word);
    free(entrie);
  }
}

void free_Table(HashTable *hashTable) {
  unsigned int i = 0;
  while (i < MAX_HASHTABLE) {
    if (hashTable->field[i] != NULL) {
      Entry *entrie = hashTable->field[i];
      if (entrie->suivant != NULL) {
        Entry *suivant;
        suivant = entrie->suivant;
        free_Words(entrie);
        entrie = suivant;
      }
    }
    i++;
  }
}
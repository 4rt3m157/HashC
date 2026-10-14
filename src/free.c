#include "HashC.h"
/**
 * @brief free les mots de chaque Entry et le tableau entryList
 * @param entryList* adresse d'une entrie dans un field de la hashTable.
 * chaque mot est libéré
 * @return void
 */
void free_Words(Entry *entrie) {
  free(entrie->word);
  free(entrie);
}

void free_Table(HashTable *hashTable) {
  unsigned int i = 0;
  while (i < MAX_HASHTABLE) {
    Entry *entrie = hashTable->field[i];
    while (entrie != NULL) {
      Entry *suivant = entrie->suivant;
      free_Words(entrie);
      entrie = suivant;
    }
    i++;
  }
  free(hashTable);
}
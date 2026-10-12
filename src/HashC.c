#include "HashC.h"
#include <stdio.h>
#include <string.h>

// read and load, reaload ? ca m'fait rire je gardes
int reaload_Password(char *passwordsPath, EVP_MD *algo,
                     HashTable **ptrHashTable) {
  FILE *passwordFile = fopen(passwordsPath, "r");
  if (!passwordFile)
    return -1;

  // creation de la table si le fichier est ok
  HashTable *hashTable = table_new();
  char *ligne = NULL;
  size_t cap = 0;
  int ok = 1;

  while (ok && getline(&ligne, &cap, passwordFile) != -1) {
    ligne[strcspn(ligne, "\r\n")] = '\0';
    if (ligne[0] == '\0')
      continue;
    if (add_Entry_to_Table(hashTable, ligne, algo) != 0)
      ok = 0;
  }

  free(ligne);
  fclose(passwordFile);

  if (!ok)
    free_Table(hashTable);
  else
    *ptrHashTable = hashTable;
  return ok;
}

int add_Entry_to_Table(HashTable *tableHash, char *mot, EVP_MD *algo) {
  int ok;
  Entry *entrie = calloc(1, sizeof *entrie);
  if (entrie) {
    entrie->word = strdup(mot);
    if (!entrie->word ||
        !hashWord(mot, entrie->hash, &entrie->sizeHash, algo)) {
      free_Words(entrie);
      ok = -1;
    }
    if (ok) {
      unsigned int index = get_field_index(entrie->hash, entrie->sizeHash);
      entrie->suivant = tableHash->field[index];
      tableHash->field[index] = entrie;
    }
  } else
    ok = -1;
  return ok;
}

int create_Binary_Dictionary(char *fileName, HashTable *tableHash) {
  FILE *dictionaryFile = fopen(fileName, "wb+");
  int ok;

  for (size_t i = 0; i < MAX_HASHTABLE; i++) {
    Entry *entrie = tableHash->field[i];
    while (ok && entrie) {
      unsigned int sizeMot = (unsigned int)strlen(entrie->word);
      unsigned char sizeHash = (unsigned char)entrie->sizeHash;

      // OK = 1 si il a réussit à tout écrire
      ok = fwrite(&sizeMot, sizeof sizeMot, 1, dictionaryFile) == 1 &&
           fwrite(entrie->word, 1, sizeMot, dictionaryFile) == sizeMot &&
           fwrite(&sizeHash, sizeof sizeHash, 1, dictionaryFile) == 1 &&
           fwrite(entrie->hash, 1, sizeHash, dictionaryFile) == sizeHash;

      entrie = entrie->suivant;
    }
  }
  fclose(dictionaryFile);
  return ok;
}

int check_Generation(char *passwordsPath, char *outputPath, EVP_MD *algo) {
  HashTable *hashTable = NULL;

  if (reaload_Password(passwordsPath, algo, &hashTable) != 0) {
    fprintf(stderr, "erreur lecture/hashage %s\n", passwordsPath);
    return -1;
  }

  int retour = create_Binary_Dictionary(outputPath, hashTable);
  free_Table(hashTable);

  if (retour != 0) {
    fprintf(stderr, "Erreur écriture %s\n", outputPath);
    return -1;
  }
  printf("HashTable écrite dans %s\n", outputPath);
  return 0;
}
int get_Option(int argc, char **argv) {
  char help[] =
      "Usage :\n./t3c G -i <dictionnaire> -o <t3c_output> [-a "
      "<algo>]\n\t-i / --input <path> : "
      "dictionnaire (obligatoire)\n\t-o / --output <path> : sortie T3C "
      "(obligatoire)\n\t-a / --algo <nom> : algorithme (optionnel).\n\n./t3c L "
      "-t <t3c_path>";

  char *algo_name = argv[2];
  char *input = NULL, *output = NULL, *algo = "MD5", *table = NULL;

  struct option option_longue[] = {{"input", required_argument, 0, 'i'},
                                   {"output", required_argument, 0, 'o'},
                                   {"algo", required_argument, 0, 'a'},
                                   {"table", required_argument, 0, 't'},
                                   {0, 0, 0, 0}};

  if (argc < 2) {
    fprintf(stderr, "%s\n", help);
    return 1;
  }

  char mode = (argv[1][1] == '\0') ? argv[1][0]
                                   : '?'; // une seule lettre uniquement (G|L)

  switch (mode) {
  case 'G':
    if (!input || !output) {
      fprintf(stderr, "G : -i et -o sont obligatoires\n%s\n", help);
      return 1;
    }
    printf("Mode G : input=%s output=%s algo=%s\n", input, output, algo);
    //
    break;

  case 'L':
    if (!table) {
      fprintf(stderr, "L : -t est obligatoire\n%s\n", help);
      return 1;
    }
    printf("Mode L : table=%s\n", table);
    // load_Table()
    break;

  default:
    fprintf(stderr, "Mode inconnu : %s\n%s\n", argv[1], help);
    return 1;
  }
}

int main(int argc, char **argv) { get_Option(argc, argv); }

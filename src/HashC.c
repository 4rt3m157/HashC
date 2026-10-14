#include "HashC.h"
#include <string.h>

HashTable *table_new(void) {
  HashTable *hashTable = calloc(1, sizeof *hashTable);
  return hashTable;
}

// read and load, reaload ? ca m'fait rire je gardes
int reaload_Password(char *passwordsPath, EVP_MD *algo,
                     HashTable **ptrHashTable) {
  FILE *passwordFile = fopen(passwordsPath, "r");
  if (!passwordFile)
    return -1;

  // creation de la table si le fichier est ok
  HashTable *hashTable = table_new();
  if (!hashTable) {
    fclose(passwordFile);
    return -1;
  }

  char *ligne = NULL;
  size_t size = 0;
  int ok = 1;
  int nbLigne = 0;
  while (ok && getline(&ligne, &size, passwordFile) != -1) {
    ligne[strcspn(ligne, "\r\n")] = '\0';
    if (ligne[0] == '\0')
      continue;
    if (add_Entry_to_Table(hashTable, ligne, algo) != 0) {
      ok = 0;
      continue;
    }
    nbLigne++;
    if (nbLigne >= MAX_PASSWORD) {
      fprintf(stderr, "RAM Protection : Only %d password loaded.\n", nbLigne);
      break;
    }
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
  int ok = 0;
  Entry *entrie = calloc(1, sizeof *entrie);
  if (entrie) {
    entrie->word = strdup(mot);
    if (!entrie->word ||
        !hashWord(mot, entrie->hash, &entrie->sizeHash, algo)) {
      free_Words(entrie);
      ok = -1;
    }
    if (ok == 0) {
      unsigned int index = get_field_index(entrie->hash);
      entrie->suivant = tableHash->field[index];
      tableHash->field[index] = entrie;
    }
  } else
    ok = -1;
  return ok;
}

int find_Hash(HashTable *tableHash, char *hashToFind) {
  int hexLen = (int)strlen(hashToFind);

  // condensat hexa valide : longueur paire, au moins MD5 (32), au plus SHA-512
  // (128)
  if (hexLen < 32 || hexLen % 2 != 0 || hexLen / 2 > EVP_MAX_MD_SIZE) {
    printf("%s : hash invalide\n", hashToFind);
    return -1;
  }

  unsigned char hash[EVP_MAX_MD_SIZE];
  int sizeHash = hexLen / 2;
  transformStringtoHex(hashToFind, hexLen, (char *)hash);

  unsigned int index = get_field_index(hash);

  int found = 0;
  for (Entry *entrie = tableHash->field[index]; entrie != NULL;
       entrie = entrie->suivant) {
    if (entrie->sizeHash == (unsigned int)sizeHash &&
        memcmp(entrie->hash, hash, sizeHash) == 0) {
      printf("%s : %s\n", hashToFind, entrie->word);
      found = 1;
      break; // une seule ligne de sortie par condensat lu
    }
  }
  if (!found)
    printf("\n");

  return found ? 0 : 1;
}

int create_Binary_Dictionary(char *fileName, HashTable *tableHash) {
  FILE *dictionaryFile = fopen(fileName, "wb+");
  if (!dictionaryFile)
    return 0; // 0 = echec, meme convention que le reste de la fonction

  int ok = 1;

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

int load_Binary_Dictionnary(char *binaryPath, HashTable **ptrTable) {
  FILE *dictionaryFile = fopen(binaryPath, "rb");
  if (dictionaryFile == NULL)
    return -1;

  HashTable *tableHash = table_new();
  if (!tableHash) {
    fclose(dictionaryFile);
    return -1;
  }

  // true seulement si une entree a ete entamee (sizeMot lu) puis n'a pas pu
  // etre finie : une fin de fichier entre deux entrees est normale, une coupure
  // au milieu d'une entree non.
  int corrompu = 0;

  while (1) {
    unsigned int sizeMot = 0;
    unsigned char sizeHash = 0;

    if (fread(&sizeMot, sizeof sizeMot, 1, dictionaryFile) != 1) {
      if (!feof(dictionaryFile))
        corrompu = 1; // erreur de lecture, pas juste une fin de fichier
      break;
    }

    Entry *entrie = calloc(1, sizeof *entrie);
    if (!entrie) {
      corrompu = 1;
      break;
    }

    entrie->word = malloc((size_t)sizeMot + 1);
    if (!entrie->word) {
      free(entrie);
      corrompu = 1;
      break;
    }

    int ok_entree =
        fread(entrie->word, 1, sizeMot, dictionaryFile) == sizeMot &&
        fread(&sizeHash, sizeof sizeHash, 1, dictionaryFile) == 1 &&
        sizeHash <= EVP_MAX_MD_SIZE &&
        fread(entrie->hash, 1, sizeHash, dictionaryFile) == sizeHash;

    if (!ok_entree) {
      free(entrie->word);
      free(entrie);
      corrompu = 1; // sizeMot promettait une entree complete : elle est coupee
      break;
    }

    entrie->word[sizeMot] = '\0';
    entrie->sizeHash = sizeHash;

    unsigned int index = get_field_index(entrie->hash);
    entrie->suivant = tableHash->field[index];
    tableHash->field[index] = entrie;
  }

  fclose(dictionaryFile);

  if (corrompu) {
    free_Table(tableHash);
    return -1;
  }

  *ptrTable = tableHash;
  return 0;
}

int check_Generation(char *passwordsPath, char *outputPath, EVP_MD *algo) {
  HashTable *hashTable = NULL;

  if (reaload_Password(passwordsPath, algo, &hashTable) != 1) {
    fprintf(stderr, "erreur lecture/hashage %s\n", passwordsPath);
    return -1;
  }

  int ecrit_ok = create_Binary_Dictionary(outputPath, hashTable);
  free_Table(hashTable);

  if (!ecrit_ok) {
    fprintf(stderr, "Erreur écriture %s\n", outputPath);
    return -1;
  }

  fprintf(stderr, "HashTable écrite dans %s\n", outputPath);
  return 0;
}

int main(int argc, char **argv) {
  /* MD5 par défaut, comme l'impose la consigne */
  EVP_MD *algo = EVP_get_digestbyname("MD5");

  char help[] =
      "Usage :\n./t3c G -i <dictionnaire> -o <t3c_output> [-a <algo>]\n"
      "\t-i / --input <path>  : dictionnaire (obligatoire)\n"
      "\t-o / --output <path> : sortie T3C (obligatoire)\n"
      "\t-a / --algo <nom>    : algorithme (optionnel) MD5 SHA1 SHA256 "
      "SHA512\n\n"
      "./t3c L -t <t3c_path>";

  if (argc < 2) {
    fprintf(stderr, "%s\n", help);
    return 1;
  }

  char *passwordPath = NULL, *binaryPath = NULL, *table = NULL;

  struct option option_longue[] = {{"input", required_argument, 0, 'i'},
                                   {"output", required_argument, 0, 'o'},
                                   {"algo", required_argument, 0, 'a'},
                                   {"table", required_argument, 0, 't'},
                                   {0, 0, 0, 0}};

  /* L'analyse des options commence après le mode (argv[1]) */
  optind = 2;

  int opt;
  while ((opt = getopt_long(argc, argv, "i:o:a:t:", option_longue, NULL)) !=
         -1) {
    switch (opt) {
    case 'i':
      passwordPath = optarg;
      break;
    case 'o':
      binaryPath = optarg;
      break;
    case 'a':
      if (strcmp(optarg, "MD5") == 0 || strcmp(optarg, "SHA1") == 0 ||
          strcmp(optarg, "SHA256") == 0 || strcmp(optarg, "SHA512") == 0) {
        algo = EVP_get_digestbyname(optarg);
      } else {
        fprintf(stderr, "Algorithme inconnu, MD5 par défaut\n");
      }
      break;
    case 't':
      table = optarg;
      break;
    case '?':
    default:
      fprintf(stderr, "%s\n", help);
      return 1;
    }
  }

  char mode = (argv[1][1] == '\0') ? argv[1][0] : '?'; // Garder qu'une lettre

  switch (mode) {
  case 'G':
    if (!passwordPath || !binaryPath) {
      fprintf(stderr, "G : -i et -o\n%s\n", help);
      return 1;
    }
    fprintf(stderr, "Mode G : input=%s output=%s\n", passwordPath, binaryPath);

    if (check_Generation(passwordPath, binaryPath, algo) != 0) {
      fprintf(stderr, "Erreur génération\n");
      return 1;
    }
    break;

  case 'L': {
    if (!table) {
      fprintf(stderr, "L : -t <file> \n%s\n", help);
      return 1;
    }

    HashTable *hashTable = NULL;
    if (load_Binary_Dictionnary(table, &hashTable) != 0) {
      fprintf(stderr, "erreur chargement %s\n", table);
      return 1;
    }

    char *ligne = NULL;
    size_t cap = 0;
    while (getline(&ligne, &cap, stdin) != -1) { // lecture hash
      ligne[strcspn(ligne, "\r\n")] = '\0';
      if (ligne[0] == '\0')
        continue;
      find_Hash(hashTable, ligne);
    }

    free(ligne);
    free_Table(hashTable);
    break;
  }

  default:
    fprintf(stderr, "Erreur mode : %s\n%s\n", argv[1], help);
    return 1;
  }

  return 0;
}
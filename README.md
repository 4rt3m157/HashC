# HashC

Génération et recherche dans une table de correspondance chaîne/condensat.

## Compilation

### Local

```
make
```

Nécessite `libssl-dev` (headers OpenSSL).
### Exemple usage :
 ./t3c G -i ../wordlist.txt -o ../word.bin
 ./t3c L -t ../word.bin
 e0deff349b2c61f5f796ccaa344a4930
### Docker

```
docker build -t hashc .
docker run --rm -it hashc
```

## Architecture

- Table de hachage artisanal contenant des listes chainées en cas d'entré multiple. Index calculé à partir des deux premiers octets du condensat.

- Taille fixe plutôt que redimensionnement dynamique parce que plus simple à mettre en place et assez efficace pour un projet tel que celui-ci (j'espère...).

- Sérialisation binaire par concaténation d'entrées (longueur + données), relue séquentiellement jusqu'à `EOF`. Limite la taille du fichier et de la mémoire. Lecture/Ecriture plus rapide.

## Fichiers

`HashC.h` structures (`Entry`, `HashTable`), prototypes 


`HashC.c` génération, chargement, recherche, `main` 


`utils.c` calcul de condensat, conversion hexa, affichage


`free.c` libération mémoire

#define RAINBOW "./rainbow.txt"

char* extractFileToMem(char* fileName);
char* readLine(char* text);
char* readHash(char* line);
bool compareHash(char* knownHash, char* hash );
char* splitHashText(char* hashTable);

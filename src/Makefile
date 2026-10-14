hash: HashC.o utils.o free.o
	sudo apt install libssl-dev
	gcc -o t3c HashC.o utils.o free.o -lcrypto

HashC.o: HashC.c HashC.h
	gcc -Wall -Wextra -c HashC.c

utils.o: utils.c HashC.h
	gcc -Wall -Wextra -c utils.c

free.o: free.c HashC.h
	gcc -Wall -Wextra -c free.c


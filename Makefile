Install:
	sudo apt install libssl-dev
	gcc ./hash.c -o hash -lcrypto -lssl
	./hash ./wordlist.txt SHA-256

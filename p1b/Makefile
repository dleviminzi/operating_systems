default: client server

client:
	gcc -g lab1b-client.c -Wall -Wextra -o lab1b-client

server:
	gcc -g lab1b-server.c -Wall -Wextra -o lab1b-server

clean:
	rm -f *.o lab1b-client lab1b-server *.txt *.tar.gz

dist: default
	tar -czf lab1b-004923946.tar.gz README Makefile lab1b-server.c lab1b-client.c

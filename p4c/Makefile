default:
	gcc -o lab4c_tcp -Wall -Wextra -lm -lmraa -lpthread -g lab4c_tcp.c
	gcc -o lab4c_tcp -Wall -Wextra -lm -lmraa -lssl -lcrypto -lpthread -g lab4c_tls.c
clean: 
	rm -f *.tar.gz lab4c_tcp lab4c_tls

dist:
	tar -czvf lab4c-004923946.tar.gz README Makefile lab4c_tcp.c lab4c_tls.c
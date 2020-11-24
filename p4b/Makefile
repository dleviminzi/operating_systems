default:
	gcc -g -lm -lmraa -lpthread lab4b.c -Wall -Wextra -o lab4b

check: default
	chmod +x check.sh
	./check.sh

clean:
	rm lab4b *.o *.tar.gz

dist:
	tar -czvf lab4b-004923946.tar.gz README Makefile lab4b.c
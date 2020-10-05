default:
	gcc -g lab0.c -Wall -Wextra -o lab0

check: default ioTest iTest oTest segTest badOptTest
	if [ -s log.txt ]; then \
		echo "Errors occured during smoke test..."; \
		cat log.txt; \
	fi; \
	rm -f log.txt

clean:
	rm -f *.o lab0 *.txt *.tar.gz

dist: default
	tar -czf lab0-004923946.tar.gz README Makefile lab0.c *.png

ioTest:
	echo "My name is Daniel Levi-Minzi" > i.txt; \
	touch o.txt; \
	./lab0 --input=i.txt --output=o.txt; \
	if [ $$? -ne 0 ]; then \
		echo "Failed w/ valid i/o files." >> log.txt; \
	fi; \
	cmp -s o.txt i.txt > /dev/null; \
	if [ $$? -ne 0 ]; then \
		echo "Copy incorrect w/ valid i/o files." >> log.txt; \
	fi; \
	rm -f i.txt o.txt

iTest:
	echo "My name is Daniel Levi-Minzi" > i.txt; \
	./lab0 --input=i.txt --output=o.txt; \
	if [ $$? -ne 0 ]; then \
		echo "Failed w/ valid i file and created o file." >> log.txt; \
	fi; \
	cmp -s o.txt i.txt > /dev/null; \
	if [ $$? -ne 0 ]; then \
		echo "Copy incorrect w/ valid i file and created o file." >> log.txt; \
	fi; \
	rm -f i.txt o.txt	

oTest:
	touch o.txt; \
	echo "My name is Daniel Levi-Minzi" | ./lab0 --output=o.txt; \
	if [ $$? -ne 0 ]; then \
		echo "Failed w/ valid o files and kb i." >> log.txt; \
	fi; \
	echo "My name is Daniel Levi-Minzi" > i.txt; \
	cmp -s o.txt i.txt > /dev/null; \
	if [ $$? -ne 0 ]; then \
		echo "Copy incorrect w/ valid o files and kb i." >> log.txt; \
	fi; \
	rm -f i.txt o.txt

segTest:
	./lab0 --catch --segfault &> seg.txt; \
	if [ $$? -ne 4 ]; then \
		echo "Incorrect exist value for seg/catch." >> log.txt; \
	elif [ ! -s seg.txt ]; then \
		echo "catch didnt send error message to stderr."; \
	fi; \
	rm -f seg.txt

badOptTest:
	./lab0 --what --input &> badOpt.txt; \
	if [ $$? -ne 1 ]; then \
		echo "Did not catch bad option." >> log.txt; \
	elif [ ! -s badOpt.txt ]; then \
		echo "Bad option didnt send error message to stderr."; \
	fi; \
	rm -f badOpt.txt

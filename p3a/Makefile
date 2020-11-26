default:
	gcc lab3a.c -o lab3a -Wall -Wextra

test: default
	cp trivial.img ./trivial_temp.img
	./lab3a trivial_temp.img
	rm trivial_temp.img
	

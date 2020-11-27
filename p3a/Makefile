default:
	gcc lab3a.c -o lab3a -Wall -Wextra

test_trivial: default
	cp trivial.img ./trivial_temp.img
	./lab3a trivial_temp.img
	rm trivial_temp.img

test_EXT2_test: default
	cp EXT2_test.img ./EXT2_test_temp.img
	./lab3a EXT2_test_temp.img
	rm EXT2_test_temp.img

default:
	gcc -g -lpthread lab2_list.c SortedList.c -Wall -Wextra -o lab2_list
clean:
	rm -rf *tar* lab2_list lab2_add *.o *.dSYM *.csv *.png
tests: clean default
	touch lab2_list.csv

# 1000 iterations for 1,2,4,8,12,16,24 threads w/ mutex and spin locking
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=1 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2_list.csv 

graphs: tests
	gnuplot lab2_plotter.gp
dist: graphs lab2_list.c Makefile README SortedList.c SortedList.h
	tar -czvf lab2a-004923946.tar.gz README Makefile lab2_list.c SortedList.h SortedList.c *.gp *.png *.csv 
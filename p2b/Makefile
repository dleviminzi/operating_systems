default:
	gcc -g -lpthread -lprofiler lab2_list.c SortedList.c -Wall -Wextra -o lab2_list
def_mac: 
	gcc -Wl,-no_pie -g -lpthread -lprofiler lab2_list.c SortedList.c -Wall -Wextra -o lab2_list
profile: clean default
	CPUPROFILE_FREQUENCY=800 ./lab2_list --threads=12 --iterations=1000 --sync=s --profile >> lab2_list.csv
	pprof --text ./lab2_list profile.prof > profile.out
prof_mac: clean def_mac
	CPUPROFILE_FREQUENCY=800 ./lab2_list --threads=12 --iterations=1000 --sync=s --profile >> lab2_list.csv
	pprof --text ./lab2_list profile.prof > profile.out
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

# running with 4 lists and no sync
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=1 >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=1 >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=1 >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=1 >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=1 >> lab2_list.csv 
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=2 >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=2 >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=2 >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=2 >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=2 >> lab2_list.csv 
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=4 >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=4 >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=4 >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=4 >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=4 >> lab2_list.csv 
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=8 >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=8 >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=8 >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=8 >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=8 >> lab2_list.csv 
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=16 >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=16 >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=16 >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=16 >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=16 >> lab2_list.csv 

# running with 4 lists and syncing
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=10 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=10 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=10 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=10 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=10 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=20 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=20 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=20 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=20 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=20 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=40 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=40 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=40 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=40 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=40 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=80 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=80 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=80 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=80 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=80 --sync=m >> lab2_list.csv 
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=10 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=10 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=10 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=10 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=10 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=20 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=20 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=20 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=20 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=20 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=40 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=40 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=40 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=40 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=40 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=1 --lists=4 --yield=id --iterations=80 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=4 --lists=4 --yield=id --iterations=80 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=8 --lists=4 --yield=id --iterations=80 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=12 --lists=4 --yield=id --iterations=80 --sync=s >> lab2_list.csv 
	-./lab2_list --threads=16 --lists=4 --yield=id --iterations=80 --sync=s >> lab2_list.csv 

# 1000 it w/ 1, 4, 8, 16 lists and diff sync options
	./lab2_list --threads=1 --lists=1 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=2 --lists=1 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=4 --lists=1 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=8 --lists=1 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=12 --lists=1 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=1 --lists=4 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=2 --lists=4 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=4 --lists=4 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=8 --lists=4 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=12 --lists=8 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=1 --lists=8 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=2 --lists=8 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=4 --lists=8 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=8 --lists=8 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=12 --lists=8 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=1 --lists=16 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=2 --lists=16 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=4 --lists=16 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=8 --lists=16 --iterations=1000 --sync=m >> lab2_list.csv 
	./lab2_list --threads=12 --lists=16 --iterations=1000 --sync=m >> lab2_list.csv 

	./lab2_list --threads=1 --lists=1 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=2 --lists=1 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=4 --lists=1 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=8 --lists=1 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=12 --lists=1 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=1 --lists=4 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=2 --lists=4 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=4 --lists=4 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=8 --lists=4 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=12 --lists=8 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=1 --lists=8 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=2 --lists=8 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=4 --lists=8 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=8 --lists=8 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=12 --lists=8 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=1 --lists=16 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=2 --lists=16 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=4 --lists=16 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=8 --lists=16 --iterations=1000 --sync=s >> lab2_list.csv 
	./lab2_list --threads=12 --lists=16 --iterations=1000 --sync=s >> lab2_list.csv 

graphs: tests
	gnuplot lab2_plotter.gp
dist: graphs lab2_list.c Makefile README SortedList.c SortedList.h
	tar -czvf lab2a-004923946.tar.gz README Makefile lab2_list.c SortedList.h SortedList.c *.gp *.png *.csv 
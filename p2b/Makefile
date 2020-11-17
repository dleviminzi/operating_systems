default:
	gcc -g -lpthread -lprofiler lab2_list.c SortedList.c -Wall -Wextra -o lab2_list
def_mac: 
	gcc -Wl,-no_pie -g -lpthread -lprofiler lab2_list.c SortedList.c -Wall -Wextra -o lab2_list

profile: clean default
	CPUPROFILE_FREQUENCY=800 ./lab2_list --threads=12 --iterations=1000 --sync=s --profile >> lab2_list.csv
	touch profile.out
	pprof --text ./lab2_list profile.prof >> profile.out
	echo ------- ANALYSIS OF FUNCTION TAKING MOST RESOURCES: LOCK ------- >> profile.out
	pprof --list=lock ./lab2_list profile.prof >> profile.out

prof_mac: clean def_mac
	CPUPROFILE_FREQUENCY=800 ./lab2_list --threads=12 --iterations=1000 --sync=s --profile >> lab2_list.csv
	touch profile.out
	pprof --text ./lab2_list profile.prof >> profile.out
	pprof --list=lock >> profile.out

clean:
	rm -rf *tar* lab2_list *.o *.dSYM *.csv *.png *.out

tests: clean default
	chmod +x test.sh
	./test.sh

graphs: tests
	gnuplot lab2_plotter.gp

dist: graphs lab2_list.c Makefile README SortedList.c SortedList.h
	tar -czvf lab2b-004923946.tar.gz README Makefile lab2_list.c SortedList.h SortedList.c test.sh *.gp *.png *.csv *.out 
default:
	gcc -g -lpthread lab2_add.c -Wall -Wextra -o lab2_add
clean:
	rm -f lab2_add *.png *.csv
tests: default
	touch lab2_add.csv

	./lab2_add --threads=1  --iterations=10 >> lab2_add.csv
	./lab2_add --threads=1  --iterations=20 >> lab2_add.csv
	./lab2_add --threads=1  --iterations=40 >> lab2_add.csv
	./lab2_add --threads=1  --iterations=80 >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100 >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=20 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=40 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=80 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10 >> lab2_add.csv
	./lab2_add --threads=4  --iterations=20 >> lab2_add.csv
	./lab2_add --threads=4  --iterations=40 >> lab2_add.csv
	./lab2_add --threads=4  --iterations=80 >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100 >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10 >> lab2_add.csv
	./lab2_add --threads=8  --iterations=20 >> lab2_add.csv
	./lab2_add --threads=8  --iterations=40 >> lab2_add.csv
	./lab2_add --threads=8  --iterations=80 >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100 >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10 >> lab2_add.csv
	./lab2_add --threads=12  --iterations=20 >> lab2_add.csv
	./lab2_add --threads=12  --iterations=40 >> lab2_add.csv
	./lab2_add --threads=12  --iterations=80 >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100 >> lab2_add.csv
	./lab2_add --threads=12  --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100000 >> lab2_add.csv

	./lab2_add --threads=1  --iterations=10 --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=20 --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=40 --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=80 --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10 --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=20 --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=40 --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=80 --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10 --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=20 --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=40 --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=80 --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10 --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=20 --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=40 --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=80 --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10 --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=20 --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=40 --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=80 --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100000 --yield >> lab2_add.csv

	./lab2_add --threads=1  --iterations=10 --sync=m >> lab2_add.csv
	./lab2_add --threads=1  --iterations=20 --sync=m >> lab2_add.csv
	./lab2_add --threads=1  --iterations=40 --sync=m >> lab2_add.csv
	./lab2_add --threads=1  --iterations=80 --sync=m >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100 --sync=m >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000 --sync=m >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000 --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10 --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=20 --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=40 --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=80 --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100 --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000 --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000 --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10 --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=20 --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=40 --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=80 --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100 --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000 --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000 --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10 --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=20 --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=40 --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=80 --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100 --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000 --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 --sync=m >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10 --sync=m >> lab2_add.csv
	./lab2_add --threads=12  --iterations=20 --sync=m >> lab2_add.csv
	./lab2_add --threads=12  --iterations=40 --sync=m >> lab2_add.csv
	./lab2_add --threads=12  --iterations=80 --sync=m >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100 --sync=m >> lab2_add.csv
	./lab2_add --threads=12  --iterations=1000 --sync=m >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100000 --sync=m >> lab2_add.csv

	./lab2_add --threads=1  --iterations=10 --sync=s >> lab2_add.csv
	./lab2_add --threads=1  --iterations=20 --sync=s >> lab2_add.csv
	./lab2_add --threads=1  --iterations=40 --sync=s >> lab2_add.csv
	./lab2_add --threads=1  --iterations=80 --sync=s >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100 --sync=s >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000 --sync=s >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000 --sync=s >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10 --sync=s >> lab2_add.csv
	./lab2_add --threads=2  --iterations=20 --sync=s >> lab2_add.csv
	./lab2_add --threads=2  --iterations=40 --sync=s >> lab2_add.csv
	./lab2_add --threads=2  --iterations=80 --sync=s >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100 --sync=s >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000 --sync=s >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000 --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10 --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=20 --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=40 --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=80 --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100 --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000 --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000 --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10 --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=20 --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=40 --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=80 --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100 --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000 --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 --sync=s >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10 --sync=s >> lab2_add.csv
	./lab2_add --threads=12  --iterations=20 --sync=s >> lab2_add.csv
	./lab2_add --threads=12  --iterations=40 --sync=s >> lab2_add.csv
	./lab2_add --threads=12  --iterations=80 --sync=s >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100 --sync=s >> lab2_add.csv
	./lab2_add --threads=12  --iterations=1000 --sync=s >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100000 --sync=s >> lab2_add.csv

	./lab2_add --threads=1  --iterations=10 --sync=c >> lab2_add.csv
	./lab2_add --threads=1  --iterations=20 --sync=c >> lab2_add.csv
	./lab2_add --threads=1  --iterations=40 --sync=c >> lab2_add.csv
	./lab2_add --threads=1  --iterations=80 --sync=c >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100 --sync=c >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000 --sync=c >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000 --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10 --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=20 --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=40 --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=80 --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100 --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000 --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000 --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10 --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=20 --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=40 --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=80 --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100 --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000 --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000 --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10 --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=20 --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=40 --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=80 --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100 --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000 --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 --sync=c >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10 --sync=c >> lab2_add.csv
	./lab2_add --threads=12  --iterations=20 --sync=c >> lab2_add.csv
	./lab2_add --threads=12  --iterations=40 --sync=c >> lab2_add.csv
	./lab2_add --threads=12  --iterations=80 --sync=c >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100 --sync=c >> lab2_add.csv
	./lab2_add --threads=12  --iterations=1000 --sync=c >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100000 --sync=c >> lab2_add.csv

	./lab2_add --threads=1  --iterations=10 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=20 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=40 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=80 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=20 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=40 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=80 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=20 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=40 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=80 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=20 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=40 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=80 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=20 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=40 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=80 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=1000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100000 --sync=m --yield >> lab2_add.csv

	./lab2_add --threads=1  --iterations=10 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=20 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=40 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=80 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=20 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=40 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=80 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=20 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=40 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=80 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=20 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=40 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=80 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=20 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=40 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=80 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=1000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10000 --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100000 --sync=s --yield >> lab2_add.csv

	./lab2_add --threads=1  --iterations=10 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=20 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=40 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=80 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=20 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=40 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=80 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=20 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=40 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=80 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=20 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=40 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=80 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=20 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=40 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=80 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=1000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=10000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=12  --iterations=100000 --sync=c --yield >> lab2_add.csv

graph: clean tests
	gnuplot lab2_add.gp
dist: default 
	tar -czf lab2a-004923946.tar.gz README Makefile lab2_add.c

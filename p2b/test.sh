rm -f lab2_list.csv
touch lab2_list.csv

for i in 1, 2, 4, 8, 12, 16, 24
do 
    ./lab2_list --threads=$i --sync=m --iterations=1000 >> lab2b_list.csv
done

for i in 1, 2, 4, 8, 12, 16, 24
do
    ./lab2_list --threads=$i --sync=s --iterations=1000 >> lab2b_list.csv
done

for i in 1, 4, 8, 12, 16
do 
    for j in 1, 2, 4, 8, 16
    do
        ./lab2_list --threads=$i --iterations=$j --yield=id --lists=4 >> lab2b_list.csv
    done
done

for i in 1, 4, 8, 12, 16
do 
    for j in 10, 20, 40, 80
    do
        ./lab2_list --threads=$i --iterations=$j --sync=m --yield=id --lists=4 >> lab2b_list.csv
    done
done

for i in 1, 4, 8, 12, 16
do 
    for j in 10, 20, 40, 80
    do
        ./lab2_list --threads=$i --iterations=$j --sync=s --yield=id --lists=4 >> lab2b_list.csv
    done
done

for i in 1, 2, 4, 8, 12, 16
do 
    for j in 4, 8, 16
    do
        ./lab2_list --threads=$i --iterations=1000 --yield=id --lists=$j --sync=m >> lab2b_list.csv
    done
done

for i in 1, 2, 4, 8, 12, 16
do 
    for j in 4, 8, 16
    do
        ./lab2_list --threads=$i --iterations=1000 --yield=id --lists=$j --sync=s >> lab2b_list.csv
    done
done
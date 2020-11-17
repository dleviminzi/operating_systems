#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#   8. throughput operations per second (s)
#
# output:
#	lab2_list-1.png ... cost per operation vs threads and iterations
#	lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#	lab2_list-4.png ... cost per operation vs number of threads
#

# general plot parameters 
set terminal png
set datafile separator ","

# plotting thread number vs operations per second
set title "Operations per Second vs. Thread Count"
set xlabel "Thread Count"
set xrange [0.75:32]
set logscale x 2
set ylabel "Operations per Second"
set logscale y 10
set output 'lab2b_1.png'

plot \
    "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2_list.csv" using ($2):($8) \
    title 'spin lock' with linespoints lc rgb 'blue', \
    "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2_list.csv" using ($2):($8) \
    title 'mutex lock' with linespoints lc rgb 'red', 

# Plotting wait for lock and avg time per operation vs number of threads
set title "Lock Wait Time and Time per Operation vs. Thread Count"
set xlabel "Thread Count"
set xrange [0.75:32]
set logscale x 2
set ylabel "Time"
set logscale y 10
set output 'lab2b_2.png'

plot \
    "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2_list.csv" using ($2):($9) \
    title 'lock wait time' with linespoints lc rgb 'blue', \
    "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2_list.csv" using ($2):($7) \
    title 'time per operation' with linespoints lc rgb 'red', 


# Plotting multiple lists with synchronization
set title "Successful Iterations for Different Sync Options"
set xlabel "Thread Count"
set xrange [0.75:32]
set logscale x 2
set ylabel "Iterations"
set logscale y 10
set output 'lab2b_3.png'

plot \
    "< grep -e 'list-id-m,[0-9]*,[0-9]*,4,' lab2_list.csv" using ($2):($3) \
    title 'mutex lock' with points lc rgb 'blue', \
    "< grep -e 'list-id-s,[0-9]*,[0-9]*,4,' lab2_list.csv" using ($2):($3) \
    title 'sync lock' with points lc rgb 'red', \
    "< grep -e 'list-id-none,[0-9]*,[0-9]*,4,' lab2_list.csv" using ($2):($3) \
    title 'no lock' with points lc rgb 'green', 

# Plotting ops per sec vs thread count per list num (sync m)
set title "Operations per Second vs. Thread Count w/ sync=m"
set xlabel "Thread Count"
set xrange [0.75:32]
set logscale x 2
set ylabel "Operations per Second"
set logscale y 10
set output 'lab2b_4.png'

plot \
    "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2_list.csv" using ($2):($9) \
    title '1 list' with linespoints lc rgb 'red', \
    "< grep -e 'list-none-m,[0-9]*,1000,4,' lab2_list.csv" using ($2):($9) \
    title '4 list' with linespoints lc rgb 'blue', \
    "< grep -e 'list-none-m,[0-9]*,1000,8,' lab2_list.csv" using ($2):($9) \
    title '8 list' with linespoints lc rgb 'green', \
    "< grep -e 'list-none-m,[0-9]*,1000,16,' lab2_list.csv" using ($2):($9) \
    title '16 list' with linespoints lc rgb 'yellow', 

# Plotting ops per sec vs thread count per list num (sync s)
set title "Operations per Second vs. Thread Count w/ sync=s"
set xlabel "Thread Count"
set xrange [0.75:32]
set logscale x 2
set ylabel "Operations per Second"
set logscale y 10
set output 'lab2b_5.png'

plot \
    "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2_list.csv" using ($2):($9) \
    title '1 list' with linespoints lc rgb 'red', \
    "< grep -e 'list-none-m,[0-9]*,1000,4,' lab2_list.csv" using ($2):($9) \
    title '4 list' with linespoints lc rgb 'blue', \
    "< grep -e 'list-none-m,[0-9]*,1000,8,' lab2_list.csv" using ($2):($9) \
    title '8 list' with linespoints lc rgb 'green', \
    "< grep -e 'list-none-m,[0-9]*,1000,16,' lab2_list.csv" using ($2):($9) \
    title '16 list' with linespoints lc rgb 'yellow', 
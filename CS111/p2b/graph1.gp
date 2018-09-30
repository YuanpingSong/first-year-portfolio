# general plot parameters
set terminal png
set datafile separator ","

# a plot of the total number of operations per second for each synchronization method m/s

set title "List-1: Total number of Operations per Second vs Threads"
set xlabel "Threads"
set xrange [0:25]
set ylabel "Total Number of Operations per Second"
set logscale y 10
set output 'lab2b_1.png'

# grep out synchronized, non-yielding results, with 1000 iterations
plot \
	"< grep -e 'list-none-m,[0-9]*,1000,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'list w/mutex' with linespoints lc rgb 'blue', \
	"< grep -e 'list-none-s,[0-9]*,1000,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'list w/spin-lock' with linespoints lc rgb 'green'


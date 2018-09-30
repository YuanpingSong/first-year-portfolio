# general plot parameters
set terminal png
set datafile separator ","

set title "List-3: Multiple List with and w/o Synchronization"
set xlabel "Threads"
set xrange [0:18]
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'

# note that unsuccessful runs should have produced no output
plot \
	"< grep list-id-s lab2b_list.csv" using ($2):($3) \
	title 'spin lock' with points lc rgb 'violet', \
	"< grep list-id-m lab2b_list.csv" using ($2):($3) \
	title 'mutex lock' with points lc rgb 'red', \
	"< grep list-id-none lab2b_list.csv" using ($2):($3) \
	title 'unprotected' with points lc rgb 'green'

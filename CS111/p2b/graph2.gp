# general plot parameters
set terminal png
set datafile separator ","

# A plot of average time per op and waittime vs threads

set title "List-2: Average time per op and average time per lock op vs Threads"
set xlabel "Threads"
set xrange [0:25]
set ylabel "Time"
set logscale y 10
set output 'lab2b_2.png'

# grep out mutex-synchronized, non-yielding results, with 1000 iterations
plot \
	"< grep -e 'list-none-m,[0-9]*,1000,' lab2b_list.csv" using ($2):($8) \
	title 'avg waittime' with linespoints lc rgb 'blue', \
	"< grep -e 'list-none-m,[0-9]*,1000,' lab2b_list.csv" using ($2):($7) \
	title 'avg time per op' with linespoints lc rgb 'red'

# general plot parameters
set terminal png
set datafile separator ","

# mutex: throughput vs threads, 1 curve for each number of lists. 

set title "List-4: Mutex Protected Multiple Lists Performance"
set xlabel "Threads"
set xrange [0:18]
set ylabel "Total Number of Operations per Second"
set logscale y 10
set output 'lab2b_4.png'

# grep out mutex protected, non-yielding results, with 1000 iterations
plot \
	"< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'blue', \
	"< grep -e 'list-none-m,[0-9]*,1000,4' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '4 lists' with linespoints lc rgb 'green', \
	"< grep -e 'list-none-m,[0-9]*,1000,8' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '8 lists' with linespoints lc rgb 'red', \
	"< grep -e 'list-none-m,[0-9]*,1000,16' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '16 lists' with linespoints lc rgb 'violet'


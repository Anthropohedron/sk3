#!/usr/bin/env gnuplot -c
set key top left
set title "Signal Kanban Instability Example"
set xlabel "Time"
set ylabel "Deficit"
set mytics 10
set mxtics 10
set logscale y
set logscale x
#set terminal x11 persist
set terminal pdf color solid
set output "/tmp/unstableSK3/unstable.pdf"
plot \
	"/tmp/unstableSK3/t1.log" using 1:((stringcolumn(3) eq "BufferChg") ? -$5 : 1/0) title "Task 1" with lines, \
	"/tmp/unstableSK3/t2.log" using 1:((stringcolumn(3) eq "BufferChg") ? -$5 : 1/0) title "Task 2" with lines, \
	"/tmp/unstableSK3/t3.log" using 1:((stringcolumn(3) eq "BufferChg") ? -$5 : 1/0) title "Task 3" with lines, \
	"/tmp/unstableSK3/t4.log" using 1:((stringcolumn(3) eq "BufferChg") ? -$5 : 1/0) title "Task 4" with lines

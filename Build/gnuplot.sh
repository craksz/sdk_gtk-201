#! /bin/bash
cd 'gnuplot'
ls *.dat | sed "s/.dat/.dat=1/" > list

cd '..'

	gcc `pkg-config --cflags --libs gtk+-2.0 opencv` pipeC.c -o gnuplot.out 
	./gnuplot.out

cd 'gnuplot'
rm list
cd '..'


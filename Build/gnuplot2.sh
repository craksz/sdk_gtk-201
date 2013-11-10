#! /bin/bash
cd 'gnuplot'
ls *.dat | sed "s/.dat/.dat/" > list
for i in `cat list` ; do
   #echo $i
   cd '..'
   sed -e "s/filename/'$i'/" \
   	gnuplot/scriptG.p | gnuplot 
   cd 'gnuplot'
done
#rm list
cd '..'

#	Proyecto: Control de un Quadrotor por VA
#	Script Gnuplot para graficar el error de X, Y, Z y Yaw.

#	Iniciamos Multiplot.

cd 'gnuplot'	
#//filename = "grafica - 2.30,3.50,1.00 - 2.30,3.50,1.00 - 0.10,0.05,1.00 - 2.00,3.00,1.00.dat";


set multiplot;



set size 1.0,0.33;

set origin 0.0,0.66;
set title "Error X, Y \n filename";
set xlabel '[frames]';
set ylabel 'pixeles';
	plot filename u 1:2 t 'Error x' w lines,\
	     filename u 1:3 t 'Error y' w lines;
	     

set origin 0.0,0.33;
set title "Error de giro en Yaw \n filename";
set ylabel 'ángulo';
	plot filename u 1:6 t 'Error Yaw' w lines;

set origin 0.0,0.0;
set title "Error Z \n filename";
set ylabel 'mm'
	plot filename u 1:4 t 'Altura' w lines,\
	     filename u 1:5 t 'Alt Ref' w lines;


cd '..'
#unset multiplot;       

pause mouse key


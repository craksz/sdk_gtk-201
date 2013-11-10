#	"scriptG.p"
#	Proyecto: Control de un Quadrotor por VA
#	Script Gnuplot para graficar el error de X, Y, Z y Yaw.

#	Iniciamos Multiplot.

set multiplot;
set size 1.0,0.33;

set origin 0.0,0.66;
set title 'Error X, Y';
set xlabel '$0';
set ylabel 'pixeles';
	plot "$0" u 1:2 t 'Error x' w lines,\
	     "$0" u 1:3 t 'Error y' w lines;
	     
set origin 0.0,0.33;
set title 'Error de giro en Yaw';
set ylabel 'ángulo';
	plot "$0" u 1:6 t 'Error Yaw' w lines;

set origin 0.0,0.0;
set title 'Error Z'
set ylabel 'mm'
	plot "$0" u 1:4 t 'Altura' w lines,\
	     "$0" u 1:5 t 'Alt Ref' w lines;

unset multiplot;

pause mouse key
if (MOUSE_KEY != 13) reread


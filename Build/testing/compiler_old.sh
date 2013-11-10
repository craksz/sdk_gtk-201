#!/bin/bash
#sudo bash compiler.sh testing12.c 2
clear
if [ "$2" == "0" ]; then
	gcc `pkg-config --cflags --libs gtk+-2.0 opencv` $1
fi
if [ "$2" == "1" ];	then
	./a.out
fi
if [ "$2" == "2" ];	then
	gcc `pkg-config --cflags --libs gtk+-2.0 opencv` $1
	./a.out
fi

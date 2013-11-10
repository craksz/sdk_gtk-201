#!/bin/bash
if [ "$3" ]; then
	if [ "$2" == "0" ]; then
		g++ `pkg-config --cflags --libs gtk+-2.0 opencv` $1
	fi
	if [ "$2" == "1" ];	then
		./a.out
	fi
	if [ "$2" == "2" ];	then
		g++ `pkg-config --cflags --libs gtk+-2.0 opencv` $1
		./a.out
	fi
else
	

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

fi

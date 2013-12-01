
#!/bin/bash

if [[ -z "$1" ]]; then
	echo "gotta type the path ..."
	exit
fi

if [[ ! -e "$1" ]]; then
	echo "$1 doesn't exist..."
	exit
fi

if [[ -z "$2" ]]; then
	echo "gotta type the color ..."
	exit
fi

color="$2"
theDir="$1"

for image in $theDir/*; do
	./ImgProcTest $image $color
	#file $image
done

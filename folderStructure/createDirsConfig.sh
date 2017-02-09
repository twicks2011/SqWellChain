#!/bin/bash
mkdir outputFiles/$1/
mkdir VMD/$1/

mkdir outputFiles/$1/occupancies/
mkdir outputFiles/$1/restartInfo/
mkdir outputFiles/$1/collated/

mkdir outputFiles/$1/configs/
for i in {0..16}
do
    mkdir outputFiles/$1/configs/T$i/
    for j in {0..450}
    do
	mkdir outputFiles/$1/configs/T$i/E$j/
    done
done

#!/bin/bash
createDirs.sh $1
sed  -e "s/<NAME>/$1/g" masterSub.sh > $1.sh
sed  -e "s/<NCPU>/$2/g" $1.sh > $1.tmp
mv $1.tmp $1.sh

#Run this part last!!
cd inputFiles
checkOutputFolder.sh $1.dat
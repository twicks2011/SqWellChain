#!/bin/bash

#Check correct number of command line arguments
if (( $# < 2 )); then
   echo "Generates output folders and submission scripts for an hpc SqWellChain run" >&2
   echo "Usage  :  setUpRun.sh [name of the input file (no .dat)]] [number of hpc processors]" >&2
   echo "Example:  ./setUpRun.sh N10TestRun 12" >&2
  exit 1
fi

#====Extract arguments====
NAME=$1
nProcessors=$2



createDirs.sh $NAME
sed  -e "s/<NAME>/$NAME/g" masterSub.sh > $NAME.sh
sed  -e "s/<NCPU>/$nProcessors/g" $NAME.sh > $NAME.tmp
mv $NAME.tmp $1.sh

#Run this part last!!
cd inputFiles
checkOutputFolder.sh $NAME.dat
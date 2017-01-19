#!/bin/bash
#Check correct number of command line arguments
if (( $# < 1 )); then
   echo "Makes output folders for a SqWell simulation" >&2
   echo "Usage  :  createDirs.sh [folder name (these folders will be created)]" >&2
   echo "Example:  createDirs.sh N40TestRun" >&2
  exit 1
fi

mkdir outputFiles/$1/
mkdir VMD/$1/

mkdir outputFiles/$1/occupancies/
mkdir outputFiles/$1/restartInfo/
mkdir outputFiles/$1/collated/

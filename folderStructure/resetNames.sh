#A Script for reseting the names of configuration files, removing the DONE prefix.

for i in DONE*
do
    mv "$i" "`echo $i | sed 's/DONE//'`"
done
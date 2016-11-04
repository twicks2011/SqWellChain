for i in DONE*
do
    mv "$i" "`echo $i | sed 's/DONE//'`"
done
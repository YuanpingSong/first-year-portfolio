#!/bin/bash

declare -a filenames

ls -la $1 |
# obtain a detailed list of files under directory 

grep "^-" |
# select from the list only regular files 

sort -k 5n -k 9 |
# order the remaining list first by file size and then by file name

tr -s ' '|
# eliminate duplicate spaces so that the ninth fields appears after the eighth delimiter 

cut -d " " -f 9 > temp1
# obtain the last field with file names


#mapfile filenames < temp1

count=0

while read name ; do
    filenames[$count]="$name"
    ((count=count+1))
done < temp1


idx=0
for i in "${filenames[@]}"
do
    ((j=idx+1))
    while [ "$j" -lt "${#filenames[@]}" ] 
    do 

#printf "I: %s J: %s\n" $i ${filenames[$j]}
	if cmp -s $i ${filenames[$j]}
	then 
#	    echo "The files match"
	    name=${filenames[$j]}
	    rm $name
	    ln $i $name
	    filenames[$j]="temp1"
	else 
	    echo "The files are different"
	fi
	((j=j+1))
    done
    ((idx=idx+1))
done

rm "temp1"









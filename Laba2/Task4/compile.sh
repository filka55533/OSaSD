#!/bin/bash 
if gcc $1 -g -o "$1.exe";
then
    s=""
    for i in $@
    do
       [ $1 != $i ] && s="$s $i"
    done
     ./$1.exe $s && `rm -r "$1.exe"`
fi

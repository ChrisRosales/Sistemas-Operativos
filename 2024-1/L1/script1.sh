#!/bin/bash


x=$1
final=$2
patron=$3



while [ $x -le $final ]
do
  num=$x
  
  while [ $num -gt 0 ]
  do
    digito=$(($num % 10))
    
    if [ $digito -eq $patron ]; then
        echo $x
        break
    fi
    
    num=`expr $num / 10`
  done
  
  x=`expr $x + 1`
  
done    

#!/bin/bash

   num1=$RANDOM
   num2=$RANDOM
   str=$(cat /dev/urandom | tr -cd 'a-f0-9' | head -c100)
   spc=" "
   echo "$num1$spc$num2$spc$str" > $1.txt

for (( c=1; c<=$2-1; c++ ))
do  
   num1=$RANDOM
   num2=$RANDOM
   str=$(cat /dev/urandom | tr -cd 'a-f0-9' | head -c100)
   echo "$num1$spc$num2$spc$str" >> $1.txt
done





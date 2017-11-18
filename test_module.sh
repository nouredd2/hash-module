#!/bin/sh

MODULE_FILE=challenge.c
MODULE_BACK=challenge.bak
DATA_FILE=hps.data

cp $MODULE_FILE $MODULE_BACK
for ((usec=10000; usec <= 1000000 ; usec+=10000))
do
  echo "Performing test for usec = $usec"
  sed "s/u64 usec =.*;/u64 usec = $usec;/" $MODULE_BACK > $MODULE_FILE
  make > /dev/null 2>&1
  sudo insmod challenge.ko
  dmesg | tail -n1 >> $DATA_FILE
  sudo rmmod challenge
done

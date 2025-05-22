#!/bin/bash

N=64
NSTEPS=50

echo "N,NSTEPS,Threads,Time(s)" > escalabilidade_forte.csv

for threads in 1 2 4 8 16 32
do
    export OMP_NUM_THREADS=$threads
    ./simulacao $N $NSTEPS $threads >> escalabilidade_forte.csv
done

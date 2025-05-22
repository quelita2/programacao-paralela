#!/bin/bash

NSTEPS=50

echo "N,NSTEPS,Threads,Time(s)" > escalabilidade_fraca.csv

for threads in 1 2 4 8 16 32
do
    export OMP_NUM_THREADS=$threads
    N=$((32 * threads))  # aumenta o problema proporcionalmente
    ./simulacao $N $NSTEPS $threads >> escalabilidade_fraca.csv
done

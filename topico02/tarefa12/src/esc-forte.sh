#!/bin/bash
#SBATCH --job-name=pp-task12
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=esc-forte-%j.out

N=64
NSTEPS=50

echo "N,NSTEPS,Threads,Time(s)" > escalabilidade.csv

gcc -fopenmp -o main main.c -lm

for threads in 1 2 4 8 16 32
do
    export OMP_NUM_THREADS=$threads
    ./main $N $NSTEPS $threads >> escalabilidade.csv
done

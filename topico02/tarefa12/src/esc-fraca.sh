#!/bin/bash
#SBATCH --job-name=pp-task12
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --output=esc-fraca-%j.out

NSTEPS=50

echo "N,NSTEPS,Threads,Time(s)" > escalabilidade_fraca.csv

gcc -fopenmp -o main main.c -lm

for threads in 1 2 4 8 16 32
do
    export OMP_NUM_THREADS=$threads
    N=$((32 * threads))
    ./main $N $NSTEPS $threads >> escalabilidade_fraca.csv
done

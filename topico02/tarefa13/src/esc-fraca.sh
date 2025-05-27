#!/bin/bash
#SBATCH --job-name=esc-fraca-afinidade
#SBATCH --time=0-0:30
#SBATCH --partition=intel-128
#SBATCH --output=esc-fraca-%j.out

NSTEPS=50

gcc -fopenmp -o main main.c -lm

echo "N,NSTEPS,Threads,Time(s),OMP_PROC_BIND,OMP_PLACES" > escalabilidade.csv

# Afinidades a testar
BIND_OPTIONS=("false" "true" "close" "spread" "master")
PLACES_OPTIONS=("cores" "threads" "sockets")

for threads in 1 2 4 8 16 32; do
    export OMP_NUM_THREADS=$threads
    N=$((4 * threads))
    for bind in "${BIND_OPTIONS[@]}"; do
        for places in "${PLACES_OPTIONS[@]}"; do
            export OMP_PROC_BIND=$bind
            export OMP_PLACES=$places
            ./main $N $NSTEPS
        done
    done
done

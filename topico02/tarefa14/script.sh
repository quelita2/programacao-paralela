#!/bin/bash
#SBATCH --job-name=task14
#SBATCH --output=task14-%j.out
#SBATCH --time=0-0:20
#SBATCH --partition=intel-128
#SBATCH --exclusive
#SBATCH --nodes=1
#SBATCH --ntasks=2

mpicc -o main main.c
mpirun -np 2 ./main
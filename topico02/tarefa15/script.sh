#!/bin/bash
#SBATCH --job-name=task15
#SBATCH --output=task15-%j.out
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=1
#SBATCH --time=1-00:00:00
#SBATCH --partition=intel-512

mpicc -O3 -o main main.c
EXEC=./main

echo "Iniciando..."

# Testa com múltiplos valores de processos
for P in 2 4 8 16 32; do
  mpirun -np $P $EXEC
done

echo "Finalizado!"
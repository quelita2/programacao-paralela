#!/bin/bash
#SBATCH --job-name=task16
#SBATCH --output=results-%j.out
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=1
#SBATCH --time=1-00:00:00
#SBATCH --partition=intel-512

set -euo pipefail

mpicc main.c -o main

output_file="resultados.csv"
echo "processos,M,N,tempo" > $output_file

tamanhos=(512 1024 2048 4096)
processos=(2 4 8 16)

for np in "${processos[@]}"; do
  for tam in "${tamanhos[@]}"; do
    if (( tam % np == 0 )); then
      echo "Executando: srun -n $np ./main $tam $tam"
      tempo=$(srun -n $np ./main $tam $tam | grep "Tempo de execução" | awk '{print $4}')
      if [ -n "$tempo" ]; then
        echo "$np,$tam,$tam,$tempo" >> $output_file
      else
        echo "$np,$tam,$tam,NA" >> $output_file
      fi
    else
      echo "Ignorando: $tam x $tam não é divisível por $np processos"
    fi
  done
done

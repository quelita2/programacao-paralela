#!/bin/bash
#SBATCH --job-name=task16
#SBATCH --output=results-%j.out
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=1
#SBATCH --time=1-00:00:00
#SBATCH --partition=intel-512

mpicc -o matvec_mpi matvec_mpi.c

sizes=(512 1024 2048 4096 8192)
procs=(2 4 8 16)

# Loop sobre processos e tamanhos
for np in "${procs[@]}"; do
  for size in "${sizes[@]}"; do
    # Verifica se o tamanho da matriz é divisível pelo número de processos
    if (( size % np == 0 )); then
      # Executa o programa
      output=$(mpirun -np "$np" ./matvec_mpi "$size" "$size")
      # Extrai tempo da saída
      tempo=$(echo "$output" | grep "Tempo de execução" | awk '{print $4}')
      # Mostra os resultados formatados
      printf "%-10d %-10d %-10s\n" "$np" "$size" "$tempo"
    fi
  done
done

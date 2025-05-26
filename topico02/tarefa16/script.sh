#!/bin/bash
#SBATCH --job-name=task16
#SBATCH --output=results-%j.out
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=1
#SBATCH --time=1-00:00:00
#SBATCH --partition=intel-512

mpicc matvec_mpi.c -o matvec_mpi

# Arquivo de saída
output_file="resultados.csv"
echo "processos,M,N,tempo" > $output_file

tamanhos=(512 1024 2048 4096)
processos=(2 4 8 16)

for np in "${processos[@]}"; do
  for tam in "${tamanhos[@]}"; do
    # Verifica se o tamanho é divisível pelo número de processos
    if (( tam % np == 0 )); then
      echo "Executando: mpirun -np $np ./matvec_mpi $tam $tam"
      tempo=$(mpirun -np $np ./matvec_mpi $tam $tam | grep "Tempo de execução" | awk '{print $4}')
      echo "$np,$tam,$tam,$tempo" >> $output_file
    else
      echo "Ignorando: $tam x $tam não é divisível por $np processos"
    fi
  done
done

echo "Todos os testes concluídos. Resultados salvos em $output_file."

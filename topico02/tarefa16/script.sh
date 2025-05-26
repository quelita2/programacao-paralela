#!/bin/bash
#SBATCH --job-name=task16
#SBATCH --output=results-%j.out
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=1
#SBATCH --time=1-00:00:00
#SBATCH --partition=intel-512

# Compila o programa
mpicc main.c -o main

# Define os tamanhos das matrizes e os números de processos
mat_sizes=(500 1000 2000 4000)
num_procs=(2 4 8 16 32)

# Arquivo para registrar os resultados
output_file="resultados.csv"
echo "Processos,Tamanho_Matriz,Tempo(s)" > $output_file

# Loop pelos tamanhos de matriz e números de processos
for size in "${mat_sizes[@]}"; do
  for np in "${num_procs[@]}"; do
    # Verifica se o tamanho da matriz é divisível pelo número de processos
    if (( $size % $np == 0 )); then
      echo "Executando: mpirun -np $np ./main $size $size"
      tempo=$(mpirun -np $np ./main $size $size | grep "Tempo de execução" | awk '{print $4}')
      echo "$np,$size,$tempo" >> $output_file
    else
      echo "Pulando: Tamanho $size não é divisível por $np processos."
    fi
  done
done

echo "Testes concluídos. Resultados salvos em $output_file."

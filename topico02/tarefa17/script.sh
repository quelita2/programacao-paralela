#!/bin/bash
#SBATCH --job-name=tarefa17
#SBATCH --output=saida-%j.out
#SBATCH --ntasks=32
#SBATCH --cpus-per-task=1
#SBATCH --time=1-00:00:00
#SBATCH --partition=intel-512

# Compilar o código
mpicc -O3 -o main main.c

# Arquivo de saída CSV
CSV_FILE="resultados.csv"
echo "M,N,processos,tempo" > $CSV_FILE

# Tamanhos de matriz a testar
sizes=(512 1024 2048 4096)

# Números de processos a testar
procs=(2 4 8 16 32)

echo "Produto matriz-vetor (colunas)..."
echo "---------------------------------------------"

for sz in "${sizes[@]}"; do
  for p in "${procs[@]}"; do
    if (( sz % p == 0 )); then
      echo -n "Executando: M=N=$sz, P=$p ... "
      
      # Captura da saída da execução
      result=$(mpirun -np $p ./main $sz $sz | grep "Tempo de execução")
      
      # Extração do tempo em segundos
      tempo=$(echo $result | awk '{print $5}')
      
      # Salvar no CSV
      echo "$sz,$sz,$p,$tempo" >> $CSV_FILE
      
      # Mostrar resultado no terminal
      echo "$result"
    fi
  done
done

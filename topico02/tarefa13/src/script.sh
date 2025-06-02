#!/bin/bash
#SBATCH --job-name=task13
#SBATCH --cpus-per-task=32
#SBATCH --time=1-00:00:00
#SBATCH --partition=intel-512
#SBATCH --output=results-%j.out

gcc -O3 -fopenmp main.c -o main

EXEC=./main

THREADS_LIST=(1 2 4 8 16 32)
BIND_POLICIES=("false" "true" "spread" "close" "master")
PLACES_LIST=("cores" "threads" "sockets")

RESULTS="resultados_afinidade.csv"
echo "Threads,OMP_PROC_BIND,OMP_PLACES,Tempo(s)" > "$RESULTS"

for threads in "${THREADS_LIST[@]}"; do
  export OMP_NUM_THREADS=$threads

  for bind in "${BIND_POLICIES[@]}"; do
    export OMP_PROC_BIND=$bind

    for place in "${PLACES_LIST[@]}"; do
      export OMP_PLACES=$place

      echo "Rodando com $threads threads | BIND=$bind | PLACE=$place"

      START=$(date +%s.%N)
      $EXEC > /dev/null 2>&1
      END=$(date +%s.%N)
      DURATION=$(awk "BEGIN {print $END - $START}")

      echo "$threads,$bind,$place,$DURATION" >> "$RESULTS"
    done
  done
done

echo "Testes concluídos! Resultados em $RESULTS"

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_ITER 10000

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) {
            fprintf(stderr, "Este programa requer exatamente 2 processos.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int message_sizes[] = {8, 64, 512, 4096, 32768, 262144, 1048576};
    int num_sizes = sizeof(message_sizes) / sizeof(message_sizes[0]);

    if (rank == 0) {
        printf("Tamanho (bytes)\tTempo medio (us)\n");
    }

    for (int s = 0; s < num_sizes; s++) {
        int size_bytes = message_sizes[s];
        char* buffer = (char*) malloc(size_bytes);
        memset(buffer, 'a', size_bytes);

        MPI_Barrier(MPI_COMM_WORLD); // sincroniza antes da medicao
        double start = MPI_Wtime();

        for (int i = 0; i < NUM_ITER; i++) {
            if (rank == 0) {
                MPI_Send(buffer, size_bytes, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(buffer, size_bytes, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (rank == 1) {
                MPI_Recv(buffer, size_bytes, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(buffer, size_bytes, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }

        double end = MPI_Wtime();
        if (rank == 0) {
            double avg_time_us = (end - start) * 1e6 / NUM_ITER;
            printf("%d\t\t%.2f\n", size_bytes, avg_time_us);
        }

        free(buffer);
    }

    MPI_Finalize();
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void fill_matrix(double *A, int M, int N) {
    for (int i = 0; i < M * N; i++) {
        A[i] = 1.0;  // valor simples para teste
    }
}

void fill_vector(double *x, int N) {
    for (int i = 0; i < N; i++) {
        x[i] = 1.0;
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int M = 8; // Número de linhas da matriz A
    int N = 4; // Número de colunas da matriz A e tamanho do vetor x
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Permitir entrada de M e N via linha de comando
    if (argc >= 3) {
        M = atoi(argv[1]);
        N = atoi(argv[2]);
    }

    if (M % size != 0) {
        if (rank == 0) fprintf(stderr, "M (%d) deve ser múltiplo do número de processos (%d).\n", M, size);
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int local_M = M / size;
    double *A = NULL;
    double *x = (double *)malloc(N * sizeof(double));
    double *y = NULL;
    double *local_A = (double *)malloc(local_M * N * sizeof(double));
    double *local_y = (double *)malloc(local_M * sizeof(double));

    if (rank == 0) {
        A = (double *)malloc(M * N * sizeof(double));
        y = (double *)malloc(M * sizeof(double));
        fill_matrix(A, M, N);
        fill_vector(x, N);
    }

    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, local_M * N, MPI_DOUBLE, local_A, local_M * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double start = MPI_Wtime();
    for (int i = 0; i < local_M; i++) {
        local_y[i] = 0.0;
        for (int j = 0; j < N; j++) {
            local_y[i] += local_A[i * N + j] * x[j];
        }
    }
    double end = MPI_Wtime();

    MPI_Gather(local_y, local_M, MPI_DOUBLE, y, local_M, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Tempo de execução: %.6f segundos\n", end - start);
        // printf("Resultado y:\n");
        // for (int i = 0; i < M; i++) printf("%.2f ", y[i]);
        // printf("\n");

        free(A);
        free(y);
    }

    free(x);
    free(local_A);
    free(local_y);
    MPI_Finalize();
    return 0;
}

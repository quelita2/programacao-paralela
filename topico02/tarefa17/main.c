#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, size;
    int M = 1000, N = 1000; // Valores padrão

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Argumentos de linha de comando para M e N
    if (argc == 3) {
        M = atoi(argv[1]);
        N = atoi(argv[2]);
    }

    if (N % size != 0) {
        if (rank == 0)
            fprintf(stderr, "Erro: N (%d) deve ser divisível pelo número de processos (%d).\n", N, size);
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int local_cols = N / size;
    double *A = NULL, *x = NULL, *y = NULL;
    double *A_local = (double*) malloc(M * local_cols * sizeof(double));
    double *x_local = (double*) malloc(local_cols * sizeof(double));
    double *y_partial = (double*) calloc(M, sizeof(double));

    if (rank == 0) {
        A = (double*) malloc(M * N * sizeof(double));
        x = (double*) malloc(N * sizeof(double));
        y = (double*) malloc(M * sizeof(double));
        for (int i = 0; i < M * N; i++)
            A[i] = (double)(i % 100) / 10.0;
        for (int i = 0; i < N; i++)
            x[i] = (double)(i % 10);
    }

    // Tipo derivado para coluna
    MPI_Datatype col_type, col_type_resized;
    MPI_Type_vector(M, local_cols, N, MPI_DOUBLE, &col_type);
    MPI_Type_create_resized(col_type, 0, local_cols * sizeof(double), &col_type_resized);
    MPI_Type_commit(&col_type_resized);

    MPI_Barrier(MPI_COMM_WORLD); // Sincroniza antes da cronometragem
    double start = MPI_Wtime();

    // Distribuir colunas de A e partes de x
    MPI_Scatter(A, 1, col_type_resized, A_local, M * local_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(x, local_cols, MPI_DOUBLE, x_local, local_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Cálculo parcial
    for (int i = 0; i < M; i++)
        for (int j = 0; j < local_cols; j++)
            y_partial[i] += A_local[i * local_cols + j] * x_local[j];

    MPI_Reduce(y_partial, y, M, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if (rank == 0)
        printf("Tempo de execução: %f segundos (M=%d, N=%d, P=%d)\n", end - start, M, N, size);

    MPI_Type_free(&col_type_resized);
    free(A_local);
    free(x_local);
    free(y_partial);
    if (rank == 0) {
        free(A);
        free(x);
        free(y);
    }

    MPI_Finalize();
    return 0;
}

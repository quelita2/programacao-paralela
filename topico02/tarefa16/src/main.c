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

    /*Envia o vetor x completo a todos os processos:
    
     *x: ponteiro para o vetor que será enviado (no processo mestre) ou que receberá os dados (nos outros processos)
     *N: número de elementos no vetor x
     *MPI_DOUBLE: tipo dos elementos
     *0: rank do processo que contém o vetor original
     *MPI_COMM_WORLD: comunicador com todos os processos
    */
    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /*Distribui partes da matriz A entre os processos:
    
     *A: ponteiro para a matriz no processo 0 (origem dos dados)
     local_M * N: M - número de linhas que cada processo recebe, e N - número total de colunas da matriz
     *MPI_DOUBLE: tipo dos dados enviados
     *local_A: vetor de destino onde cada processo irá receber sua parte da matriz.
     *local_M * N: quantidade de dados que cada processo espera receber
     *MPI_DOUBLE: tipo dos dados recebidos
     *0: rank do processo que envia os dados (mestre)
     *MPI_COMM_WORLD: comunicador que engloba todos os processos
    */
    MPI_Scatter(A, local_M * N, MPI_DOUBLE, local_A, local_M * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double start = MPI_Wtime();
    //ele multiplica as linhas da matriz pelo vetor x, acumulando os resultados em local_y
    for (int i = 0; i < local_M; i++) {
        local_y[i] = 0.0;
        for (int j = 0; j < N; j++) {
            local_y[i] += local_A[i * N + j] * x[j];
        }
    }
    double end = MPI_Wtime();

    /*Recolhe os resultados parciais de y de todos os processos no processo mestre:

     *local_y: vetor local com os resultados parciais de cada processo
     *local_n: quantidade de elementos que cada processo envia (uma linha do vetor y por linha da matriz A que possui)
     *MPI_DOUBLE: tipo dos dados enviados
     *y: vetor no processo mestre onde os dados serão reunidos
     *local_n: quantidade de elementos que o processo mestre espera receber de cada processo
     *MPI_DOUBLE: tipo dos dados recebidos
     *0: rank do processo que irá receber os dados
     *MPI_COMM_WORLD: comunicador
    */
    MPI_Gather(local_y, local_M, MPI_DOUBLE, y, local_M, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Tempo de execução: %.6f segundos\n", end - start);

        free(A);
        free(y);
    }

    free(x);
    free(local_A);
    free(local_y);
    MPI_Finalize();
    return 0;
}

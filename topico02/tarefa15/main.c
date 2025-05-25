#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

#define N 1000
#define STEPS 1000
#define ALPHA 0.01
#define DX 0.01
#define DT 0.0005

void initialize(double* u_local, int local_n, int rank) {
    for (int i = 1; i <= local_n; i++)
        u_local[i] = 0.0;
    if (rank == 0)
        u_local[1] = 100.0;
}

void update(double* u_new, double* u_old, int start, int end) {
    for (int i = start; i <= end; i++) {
        u_new[i] = u_old[i] + ALPHA * DT / (DX * DX) * (u_old[i+1] - 2*u_old[i] + u_old[i-1]);
    }
}

// 1. Bloqueante (Send/Recv)
double run_blocking(int local_n, int rank, int size, MPI_Comm comm) {
    double* u_old = malloc((local_n + 2) * sizeof(double));
    double* u_new = malloc((local_n + 2) * sizeof(double));
    initialize(u_old, local_n, rank);

    double t_start = MPI_Wtime();
    for (int step = 0; step < STEPS; step++) {
        if (rank > 0) {
            MPI_Send(&u_old[1], 1, MPI_DOUBLE, rank - 1, 0, comm);
            MPI_Recv(&u_old[0], 1, MPI_DOUBLE, rank - 1, 0, comm, MPI_STATUS_IGNORE);
        }
        if (rank < size - 1) {
            MPI_Send(&u_old[local_n], 1, MPI_DOUBLE, rank + 1, 0, comm);
            MPI_Recv(&u_old[local_n + 1], 1, MPI_DOUBLE, rank + 1, 0, comm, MPI_STATUS_IGNORE);
        }
        update(u_new, u_old, 1, local_n);
        double* temp = u_old; u_old = u_new; u_new = temp;
    }
    double t_end = MPI_Wtime();
    free(u_old); free(u_new);
    return t_end - t_start;
}

// 2. Não bloqueante com Wait
double run_nonblocking_wait(int local_n, int rank, int size, MPI_Comm comm) {
    double* u_old = malloc((local_n + 2) * sizeof(double));
    double* u_new = malloc((local_n + 2) * sizeof(double));
    initialize(u_old, local_n, rank);

    double t_start = MPI_Wtime();
    for (int step = 0; step < STEPS; step++) {
        MPI_Request reqs[4];
        int count = 0;
        if (rank > 0) {
            MPI_Isend(&u_old[1], 1, MPI_DOUBLE, rank - 1, 0, comm, &reqs[count++]);
            MPI_Irecv(&u_old[0], 1, MPI_DOUBLE, rank - 1, 0, comm, &reqs[count++]);
        }
        if (rank < size - 1) {
            MPI_Isend(&u_old[local_n], 1, MPI_DOUBLE, rank + 1, 0, comm, &reqs[count++]);
            MPI_Irecv(&u_old[local_n + 1], 1, MPI_DOUBLE, rank + 1, 0, comm, &reqs[count++]);
        }
        MPI_Waitall(count, reqs, MPI_STATUSES_IGNORE);
        update(u_new, u_old, 1, local_n);
        double* temp = u_old; u_old = u_new; u_new = temp;
    }
    double t_end = MPI_Wtime();
    free(u_old); free(u_new);
    return t_end - t_start;
}

// 3. Não bloqueante com Test (sobreposição)
double run_nonblocking_test(int local_n, int rank, int size, MPI_Comm comm) {
    double* u_old = malloc((local_n + 2) * sizeof(double));
    double* u_new = malloc((local_n + 2) * sizeof(double));
    initialize(u_old, local_n, rank);

    double t_start = MPI_Wtime();
    for (int step = 0; step < STEPS; step++) {
        MPI_Request reqs[4];
        int count = 0;
        if (rank > 0) {
            MPI_Isend(&u_old[1], 1, MPI_DOUBLE, rank - 1, 0, comm, &reqs[count++]);
            MPI_Irecv(&u_old[0], 1, MPI_DOUBLE, rank - 1, 0, comm, &reqs[count++]);
        }
        if (rank < size - 1) {
            MPI_Isend(&u_old[local_n], 1, MPI_DOUBLE, rank + 1, 0, comm, &reqs[count++]);
            MPI_Irecv(&u_old[local_n + 1], 1, MPI_DOUBLE, rank + 1, 0, comm, &reqs[count++]);
        }

        // computa o núcleo interno (sem bordas)
        update(u_new, u_old, 2, local_n - 1);

        int completed = 0;
        if (count > 0) {
            while (!completed)
                MPI_Testall(count, reqs, &completed, MPI_STATUSES_IGNORE);
        }

        // computa bordas
        update(u_new, u_old, 1, 1);
        update(u_new, u_old, local_n, local_n);

        double* temp = u_old; u_old = u_new; u_new = temp;
    }
    double t_end = MPI_Wtime();
    free(u_old); free(u_new);
    return t_end - t_start;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int local_n = N / size;

    double t_block = run_blocking(local_n, rank, size, comm);
    double t_wait = run_nonblocking_wait(local_n, rank, size, comm);
    double t_test = run_nonblocking_test(local_n, rank, size, comm);

    double t_block_all, t_wait_all, t_test_all;
    MPI_Reduce(&t_block, &t_block_all, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
    MPI_Reduce(&t_wait, &t_wait_all, 1, MPI_DOUBLE, MPI_MAX, 0, comm);
    MPI_Reduce(&t_test, &t_test_all, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

    if (rank == 0) {
        printf("\n| Nº de Processos | Send/Recv (s) | Isend/Irecv+Wait (s) | Isend/Irecv+Test (s) |\n");
        printf("|-----------------|---------------|---------------------|----------------------|\n");
        printf("| %15d | %13f | %20f | %20f |\n", size, t_block_all, t_wait_all, t_test_all);
    }

    MPI_Finalize();
    return 0;
}

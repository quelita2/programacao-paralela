#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Tamanho do domínio
#define NX 128
#define NY 128
#define NZ 128
#define NSTEPS 50
#define DT 0.01
#define VISC 0.1

// Indexador para vetor 3D linearizado
#define IDX(i, j, k) ((i) * NY * NZ + (j) * NZ + (k))

void initialize(double *u) {
    for (int i = 0; i < NX; i++)
        for (int j = 0; j < NY; j++)
            for (int k = 0; k < NZ; k++)
                u[IDX(i,j,k)] = (i > 40 && i < 90 && j > 40 && j < 90 && k > 40 && k < 90) ? 1.0 : 0.0;
}

void diffuse(double *u, double *u_new) {
    #pragma omp parallel for collapse(3)
    for (int i = 1; i < NX-1; i++) {
        for (int j = 1; j < NY-1; j++) {
            for (int k = 1; k < NZ-1; k++) {
                u_new[IDX(i,j,k)] = u[IDX(i,j,k)] + VISC * DT * (
                    (u[IDX(i+1,j,k)] - 2*u[IDX(i,j,k)] + u[IDX(i-1,j,k)]) +
                    (u[IDX(i,j+1,k)] - 2*u[IDX(i,j,k)] + u[IDX(i,j-1,k)]) +
                    (u[IDX(i,j,k+1)] - 2*u[IDX(i,j,k)] + u[IDX(i,j,k-1)])
                );
            }
        }
    }
}

int main() {
    size_t size = NX * NY * NZ * sizeof(double);
    double *u = (double*) malloc(size);
    double *u_new = (double*) malloc(size);

    initialize(u);

    double start = omp_get_wtime();
    for (int step = 0; step < NSTEPS; step++) {
        diffuse(u, u_new);

        // Troca os ponteiros
        double *tmp = u;
        u = u_new;
        u_new = tmp;
    }
    double end = omp_get_wtime();

    printf("Tempo de simulação: %.6f segundos\n", end - start);

    free(u);
    free(u_new);
    return 0;
}

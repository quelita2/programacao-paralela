#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>

#define DX 1.0
#define DT 0.01
#define VISC 0.1

double ***u, ***u_new;
int N, STEPS;

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Aloca memória dinamicamente
double ***allocate_3d_array(int n) {
    double ***array = malloc(n * sizeof(double **));
    for (int i = 0; i < n; i++) {
        array[i] = malloc(n * sizeof(double *));
        for (int j = 0; j < n; j++) {
            array[i][j] = calloc(n, sizeof(double));
        }
    }
    return array;
}

void free_3d_array(double ***array, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            free(array[i][j]);
        }
        free(array[i]);
    }
    free(array);
}

void initialize(int perturb) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++)
                u[i][j][k] = (perturb && i == N/2 && j == N/2 && k == N/2) ? 1.0 : 0.0;
}

void step() {
    #pragma omp parallel for collapse(3) schedule(static)
    for (int i = 1; i < N-1; i++)
        for (int j = 1; j < N-1; j++)
            for (int k = 1; k < N-1; k++) {
                u_new[i][j][k] = u[i][j][k] + VISC * DT / (DX * DX) * (
                    u[i+1][j][k] + u[i-1][j][k] +
                    u[i][j+1][k] + u[i][j-1][k] +
                    u[i][j][k+1] + u[i][j][k-1] -
                    6 * u[i][j][k]
                );
            }

    for (int i = 1; i < N-1; i++)
        for (int j = 1; j < N-1; j++)
            for (int k = 1; k < N-1; k++)
                u[i][j][k] = u_new[i][j][k];
}

void save_slice(char* filename, int z) {
    FILE *fp = fopen(filename, "w");
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            fprintf(fp, "%d %d %.5f\n", i, j, u[i][j][z]);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <N> <STEPS>\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    STEPS = atoi(argv[2]);

    u = allocate_3d_array(N);
    u_new = allocate_3d_array(N);

    int num_threads = omp_get_max_threads();

    // PARTE 1: Campo parado
    initialize(0);
    double start = get_time();
    for (int t = 0; t < STEPS; t++) step();
    double end = get_time();
    double tempo_parado = end - start;
    save_slice("velocidade_parado.dat", N/2);

    // PARTE 2: Campo perturbado
    initialize(1);
    start = get_time();
    for (int t = 0; t < STEPS; t++) step();
    end = get_time();
    double tempo_perturbado = end - start;
    save_slice("velocidade_perturbado.dat", N/2);

    // SALVAR RESULTADOS EM CSV
    FILE *f = fopen("escalabilidade_fraca.csv", "a");
    if (f) {
        fprintf(f, "%d,%d,%d,%.6f\n", N, STEPS, num_threads, tempo_perturbado);
        fclose(f);
    } else {
        perror("Erro ao salvar CSV");
    }

    free_3d_array(u, N);
    free_3d_array(u_new, N);

    return 0;
}
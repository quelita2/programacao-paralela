#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>
#include <string.h>

#define N 32
#define STEPS 100
#define DX 1.0
#define DT 0.01
#define VISC 0.1

double u[N][N][N], u_new[N][N][N];

// Medição de tempo
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Inicializa campo
void initialize(int perturb) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++)
                u[i][j][k] = (perturb && i == N/2 && j == N/2 && k == N/2) ? 1.0 : 0.0;
}

// Executa um passo da simulação com diferentes opções
void step(const char* schedule, int collapse, int chunk_size) {
    if (strcmp(schedule, "static") == 0) {
        if (collapse)
            #pragma omp parallel for collapse(3) schedule(static,chunk_size)
            for (int i = 1; i < N-1; i++)
                for (int j = 1; j < N-1; j++)
                    for (int k = 1; k < N-1; k++)
                        u_new[i][j][k] = u[i][j][k] + VISC * DT / (DX * DX) *
                            (u[i+1][j][k] + u[i-1][j][k] + u[i][j+1][k] + u[i][j-1][k] + u[i][j][k+1] + u[i][j][k-1] - 6 * u[i][j][k]);
        else
            #pragma omp parallel for schedule(static,chunk_size)
            for (int i = 1; i < N-1; i++)
                for (int j = 1; j < N-1; j++)
                    for (int k = 1; k < N-1; k++)
                        u_new[i][j][k] = u[i][j][k] + VISC * DT / (DX * DX) *
                            (u[i+1][j][k] + u[i-1][j][k] + u[i][j+1][k] + u[i][j-1][k] + u[i][j][k+1] + u[i][j][k-1] - 6 * u[i][j][k]);
    } else if (strcmp(schedule, "dynamic") == 0) {
        if (collapse)
            #pragma omp parallel for collapse(3) schedule(dynamic,chunk_size)
            for (int i = 1; i < N-1; i++)
                for (int j = 1; j < N-1; j++)
                    for (int k = 1; k < N-1; k++)
                        u_new[i][j][k] = u[i][j][k] + VISC * DT / (DX * DX) *
                            (u[i+1][j][k] + u[i-1][j][k] + u[i][j+1][k] + u[i][j-1][k] + u[i][j][k+1] + u[i][j][k-1] - 6 * u[i][j][k]);
        else
            #pragma omp parallel for schedule(dynamic,chunk_size)
            for (int i = 1; i < N-1; i++)
                for (int j = 1; j < N-1; j++)
                    for (int k = 1; k < N-1; k++)
                        u_new[i][j][k] = u[i][j][k] + VISC * DT / (DX * DX) *
                            (u[i+1][j][k] + u[i-1][j][k] + u[i][j+1][k] + u[i][j-1][k] + u[i][j][k+1] + u[i][j][k-1] - 6 * u[i][j][k]);
    } else if (strcmp(schedule, "guided") == 0) {
        if (collapse)
            #pragma omp parallel for collapse(3) schedule(guided,chunk_size)
            for (int i = 1; i < N-1; i++)
                for (int j = 1; j < N-1; j++)
                    for (int k = 1; k < N-1; k++)
                        u_new[i][j][k] = u[i][j][k] + VISC * DT / (DX * DX) *
                            (u[i+1][j][k] + u[i-1][j][k] + u[i][j+1][k] + u[i][j-1][k] + u[i][j][k+1] + u[i][j][k-1] - 6 * u[i][j][k]);
        else
            #pragma omp parallel for schedule(guided,chunk_size)
            for (int i = 1; i < N-1; i++)
                for (int j = 1; j < N-1; j++)
                    for (int k = 1; k < N-1; k++)
                        u_new[i][j][k] = u[i][j][k] + VISC * DT / (DX * DX) *
                            (u[i+1][j][k] + u[i-1][j][k] + u[i][j+1][k] + u[i][j-1][k] + u[i][j][k+1] + u[i][j][k-1] - 6 * u[i][j][k]);
    }

    // Copia de volta
    for (int i = 1; i < N-1; i++)
        for (int j = 1; j < N-1; j++)
            for (int k = 1; k < N-1; k++)
                u[i][j][k] = u_new[i][j][k];
}

// Executa uma simulação completa
double run_simulation(const char* schedule, int collapse, int chunk_size, int perturb) {
    initialize(perturb);
    double start = get_time();
    for (int t = 0; t < STEPS; t++) step(schedule, collapse, chunk_size);
    double end = get_time();
    return end - start;
}

int main() {
    FILE *fp = fopen("resultados.csv", "w");
    fprintf(fp, "schedule,collapse,chunk_size,tempo,estado_inicial\n");

    const char* schedules[] = {"static", "dynamic", "guided"};
    int chunk_sizes[] = {1, 2, 4, 8, 16};
    int collapses[] = {0, 1};

    for (int s = 0; s < 3; s++) {
        for (int c = 0; c < 2; c++) {
            for (int cs = 0; cs < 5; cs++) {
                for (int pert = 0; pert < 2; pert++) {
                    double tempo = run_simulation(schedules[s], collapses[c], chunk_sizes[cs], pert);
                    fprintf(fp, "%s,%d,%d,%.6f,%s\n", schedules[s], collapses[c], chunk_sizes[cs], tempo, pert ? "perturbado" : "parado");
                    printf("%s collapse=%d chunk=%d tempo=%.6f (%s)\n", schedules[s], collapses[c], chunk_sizes[cs], tempo, pert ? "perturbado" : "parado");
                }
            }
        }
    }

    fclose(fp);
    return 0;
}

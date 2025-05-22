#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>

#define N 32
#define STEPS 100
#define DX 1.0
#define DT 0.01
#define VISC 0.1

// Preeencher um csv com os valores de tempo de cada schedule com collapse ou não

double u[N][N][N], u_new[N][N][N];

// Função para medir tempo
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Inicializa o campo de velocidade
void initialize(int perturb) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++)
                u[i][j][k] = (perturb && i == N/2 && j == N/2 && k == N/2) ? 1.0 : 0.0;
}

// Atualiza o campo de velocidade com diferenças finitas
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
        

    // Copia u_new para u
    for (int i = 1; i < N-1; i++)
        for (int j = 1; j < N-1; j++)
            for (int k = 1; k < N-1; k++)
                u[i][j][k] = u_new[i][j][k];
}

// Salva uma fatia para visualização
void save_slice(char* filename, int z) {
    FILE *fp = fopen(filename, "w");
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            fprintf(fp, "%d %d %.5f\n", i, j, u[i][j][z]);
    fclose(fp);
}

int main() {
    double start, end;

    printf("Dynamic e Collapse\n");
    initialize(0);
    start = get_time();
    for (int t = 0; t < STEPS; t++) step();
    end = get_time();
    printf("Tempo execução (parado): %.6f s\n", end - start);
    save_slice("velocidade_parado.dat", N/2);

    initialize(1);
    start = get_time();
    for (int t = 0; t < STEPS; t++) step();
    end = get_time();
    printf("Tempo execução (perturbado): %.6f s\n", end - start);
    save_slice("velocidade_perturbado.dat", N/2);

    return 0;
}

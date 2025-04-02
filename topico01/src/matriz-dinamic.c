#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Função para obter o tempo em microssegundos
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Multiplicação matriz x vetor (acesso por linhas)
void mat_vec_row_major(double *A, double *x, double *y, int n) {
    for (int i = 0; i < n; i++) {
        y[i] = 0;
        for (int j = 0; j < n; j++) {
            y[i] += A[i * n + j] * x[j];
        }
    }
}

// Multiplicação matriz x vetor (acesso por colunas)
void mat_vec_col_major(double *A, double *x, double *y, int n) {
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            y[i] += A[i * n + j] * x[j];
        }
    }
}

int main() {
    int sizes[] = {1000, 2000, 5000, 10000}; // Tamanhos de matriz para teste
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        double *A = (double *)malloc(n * n * sizeof(double));
        double *x = (double *)malloc(n * sizeof(double));
        double *y = (double *)malloc(n * sizeof(double));
        
        // Inicialização dos valores
        for (int i = 0; i < n * n; i++) A[i] = (double)(rand()) / RAND_MAX;
        for (int i = 0; i < n; i++) x[i] = (double)(rand()) / RAND_MAX;
        
        // Medir tempo de execução - acesso por linhas
        double start = get_time();
        mat_vec_row_major(A, x, y, n);
        double row_time = get_time() - start;
        
        // Reinicializa vetor resultado
        for (int i = 0; i < n; i++) y[i] = 0;
        
        // Medir tempo de execução - acesso por colunas
        start = get_time();
        mat_vec_col_major(A, x, y, n);
        double col_time = get_time() - start;
        
        printf("Tamanho %d: Linha %.6f s, Coluna %.6f s\n", n, row_time, col_time);
        
        free(A);
        free(x);
        free(y);
    }
    return 0;
}

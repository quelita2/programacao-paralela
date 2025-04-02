#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_N 10000  

double A[MAX_N][MAX_N];
double x[MAX_N];
double y[MAX_N];

// Tempo em microssegundos
double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Multiplicação matriz x vetor (acesso por linhas)
void mat_vec_row_major(int n) {
    for (int i = 0; i < n; i++) {
        y[i] = 0;
        for (int j = 0; j < n; j++) {
            y[i] += A[i][j] * x[j];
        }
    }
}

// Multiplicação matriz x vetor (acesso por colunas)
void mat_vec_col_major(int n) {
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            y[i] += A[i][j] * x[j];
        }
    }
}

int main() {
    int sizes[] = {1000, 2000, 5000, 10000}; 
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        
        // Inicialização dos valores
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A[i][j] = (double)(rand()) / RAND_MAX;
            }
            x[i] = (double)(rand()) / RAND_MAX;
        }
        
        // Medir tempo de execução - acesso por linhas
        double start = get_time();
        mat_vec_row_major(n);
        double row_time = get_time() - start;
        
        // Reinicializa vetor resultado
        for (int i = 0; i < n; i++) y[i] = 0;
        
        // Medir tempo de execução - acesso por colunas
        start = get_time();
        mat_vec_col_major(n);
        double col_time = get_time() - start;
        
        printf("Tamanho %d: Row-major %.6f s, Column-major %.6f s\n", n, row_time, col_time);
    }
    return 0;
}

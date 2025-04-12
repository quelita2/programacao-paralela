#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <sys/time.h>

#define N 10000000
#define ITERATIONS 10000000

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

void memory_bound(int num_threads) {
    double *a = (double *)malloc(N * sizeof(double));
    double *b = (double *)malloc(N * sizeof(double));
    double *c = (double *)malloc(N * sizeof(double));
    
    for (int i = 0; i < N; i++) {
        a[i] = i * 1.0;
        b[i] = (i + 1) * 1.0;
    }
    
    double start = get_time();
    
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }
    
    double end = get_time();
    printf("Memory-bound (Threads: %d) -> Tempo: %f s\n", num_threads, end - start);
    
    free(a);
    free(b);
    free(c);
}

void compute_bound(int num_threads) {
    double result = 0.0;
    double start = get_time();
    
    #pragma omp parallel for
    for (int i = 0; i < ITERATIONS; i++) {
        result += sin(i) * cos(i) / (sqrt(i + 1.0));
    }
    
    double end = get_time();
    printf("Compute-bound (Threads: %d) -> Tempo: %f s\n", num_threads, end - start);
  }

int main() {
    int thread_counts[] = {1, 2, 4, 8, 12, 16, 32};
    int num_tests = sizeof(thread_counts) / sizeof(thread_counts[0]);

    for (int i = 0; i < num_tests; i++) {
        omp_set_num_threads(thread_counts[i]);
        
        memory_bound(thread_counts[i]);
        compute_bound(thread_counts[i]);

        printf("-------------------------------------------\n");
    }
    
    return 0;
}
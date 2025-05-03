#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>

#define NUM_TENTATIVAS 1000000000

double get_time() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1e6;
}

// 1. Versão com #pragma omp critical
double estima_pi_critical() {
    int total_dentro = 0;

    #pragma omp parallel
    {
        unsigned int seed = time(NULL) + omp_get_thread_num();
        int local_count = 0;

        #pragma omp for
        for (int i = 0; i < NUM_TENTATIVAS; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0)
                local_count++;
        }

        #pragma omp critical
        total_dentro += local_count;
    }

    return 4.0 * total_dentro / NUM_TENTATIVAS;
}

// 2. Versão com #pragma omp atomic
double estima_pi_atomic() {
    int total_dentro = 0;

    #pragma omp parallel
    {
        unsigned int seed = time(NULL) + omp_get_thread_num();
        int local_count = 0;

        #pragma omp for
        for (int i = 0; i < NUM_TENTATIVAS; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0)
                local_count++;
        }

        #pragma omp atomic
        total_dentro += local_count;
    }

    return 4.0 * total_dentro / NUM_TENTATIVAS;
}

// 3. Versão com reduction
double estima_pi_reduction() {
    int total_dentro = 0;

    #pragma omp parallel reduction(+:total_dentro)
    {
        unsigned int seed = time(NULL) + omp_get_thread_num();

        #pragma omp for
        for (int i = 0; i < NUM_TENTATIVAS; i++){
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0)
                total_dentro++;
        }
    }

    return 4.0 * total_dentro / NUM_TENTATIVAS;
}

int main() {
    int threads;
    #pragma omp parallel
    {
        #pragma omp master
        threads = omp_get_num_threads();
    }

    double start, end;

    printf("Estimativa de π com %d threads e %d tentativas:\n\n", threads, NUM_TENTATIVAS);

    start = get_time();
    double pi_crit = estima_pi_critical();
    end = get_time();
    printf("Critical:   π ≈ %.6f | Tempo: %.3f s\n", pi_crit, end - start);

    start = get_time();
    double pi_atomic = estima_pi_atomic();
    end = get_time();
    printf("Atomic:     π ≈ %.6f | Tempo: %.3f s\n", pi_atomic, end - start);

    start = get_time();
    double pi_red = estima_pi_reduction();
    end = get_time();
    printf("Reduction:  π ≈ %.6f | Tempo: %.3f s\n", pi_red, end - start);

    return 0;
}
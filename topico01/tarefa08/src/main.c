#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

#define NUM_TENTATIVAS 10000000

double calcula_tempo(struct timeval inicio, struct timeval fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1e6;
}

double pi_critical_rand(int n, double *tempo_execucao) {
    int total_acertos = 0;
    struct timeval inicio, fim;
    gettimeofday(&inicio, NULL);

    #pragma omp parallel
    {
        int acertos_thread = 0;
        for (int i = 0; i < n / omp_get_num_threads(); i++) {
            double x = (double)rand() / RAND_MAX;
            double y = (double)rand() / RAND_MAX;
            if (x * x + y * y <= 1.0)
                acertos_thread++;
        }

        #pragma omp critical
        total_acertos += acertos_thread;
    }

    gettimeofday(&fim, NULL);
    *tempo_execucao = calcula_tempo(inicio, fim);
    return 4.0 * total_acertos / n;
}

double pi_vetor_rand(int n, double *tempo_execucao) {
    int *acertos = NULL, nthreads;
    struct timeval inicio, fim;
    gettimeofday(&inicio, NULL);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        #pragma omp single
        {
            nthreads = omp_get_num_threads();
            acertos = (int*)calloc(nthreads, sizeof(int));
        }

        for (int i = 0; i < n / nthreads; i++) {
            double x = (double)rand() / RAND_MAX;
            double y = (double)rand() / RAND_MAX;
            if (x * x + y * y <= 1.0)
                acertos[id]++;
        }
    }

    int total_acertos = 0;
    for (int i = 0; i < nthreads; i++) total_acertos += acertos[i];
    free(acertos);

    gettimeofday(&fim, NULL);
    *tempo_execucao = calcula_tempo(inicio, fim);
    return 4.0 * total_acertos / n;
}

double pi_critical_rand_r(int n, double *tempo_execucao) {
    int total_acertos = 0;
    struct timeval inicio, fim;
    gettimeofday(&inicio, NULL);

    #pragma omp parallel
    {
        int acertos_thread = 0;
        unsigned int seed = 1234 + omp_get_thread_num();
        for (int i = 0; i < n / omp_get_num_threads(); i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0)
                acertos_thread++;
        }

        #pragma omp critical
        total_acertos += acertos_thread;
    }

    gettimeofday(&fim, NULL);
    *tempo_execucao = calcula_tempo(inicio, fim);
    return 4.0 * total_acertos / n;
}

double pi_vetor_rand_r(int n, double *tempo_execucao) {
    int *acertos = NULL, nthreads;
    struct timeval inicio, fim;
    gettimeofday(&inicio, NULL);

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        unsigned int seed = 5678 + id;
        #pragma omp single
        {
            nthreads = omp_get_num_threads();
            acertos = (int*)calloc(nthreads, sizeof(int));
        }

        for (int i = 0; i < n / nthreads; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0)
                acertos[id]++;
        }
    }

    int total_acertos = 0;
    for (int i = 0; i < nthreads; i++) total_acertos += acertos[i];
    free(acertos);

    gettimeofday(&fim, NULL);
    *tempo_execucao = calcula_tempo(inicio, fim);
    return 4.0 * total_acertos / n;
}

int main() {
    double tempo1, tempo2, tempo3, tempo4;
    double pi1 = pi_critical_rand(NUM_TENTATIVAS, &tempo1);
    double pi2 = pi_vetor_rand(NUM_TENTATIVAS, &tempo2);
    double pi3 = pi_critical_rand_r(NUM_TENTATIVAS, &tempo3);
    double pi4 = pi_vetor_rand_r(NUM_TENTATIVAS, &tempo4);

    printf("\n-------------------------------------------------------------\n");
    printf("| %-31s | %-10s | %-13s |\n", "Versão", "Tempo(s)", "Valor de pi");
    printf("-------------------------------------------------------------\n");
    printf("| %-30s | %-10.6f | %-13.10f |\n", "rand() + critical", tempo1, pi1);
    printf("| %-30s | %-10.6f | %-13.10f |\n", "rand() + vetor", tempo2, pi2);
    printf("| %-30s | %-10.6f | %-13.10f |\n", "rand_r() + critical", tempo3, pi3);
    printf("| %-30s | %-10.6f | %-13.10f |\n", "rand_r() + vetor", tempo4, pi4);
    printf("-------------------------------------------------------------\n");

    return 0;
}

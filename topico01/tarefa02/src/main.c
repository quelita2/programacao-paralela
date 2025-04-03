#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define SIZE 100000000

void initialize_array(int *arr) {
    for (int i = 0; i < SIZE; i++) {
        arr[i] = (i % 10) + 2;
    }
}

long long sum_sequential(int *arr) {
    long long sum = 0;
    for (int i = 0; i < SIZE; i++) {
        sum += arr[i];
    }
    return sum;
}

long long sum_parallel(int *arr) {
    long long sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
    for (int i = 0; i < SIZE; i += 4) {
        sum1 += arr[i];
        sum2 += arr[i + 1];
        sum3 += arr[i + 2];
        sum3 += arr[i + 3];
    }
    return sum1 + sum2 + sum3 + sum4;
}

void measure_execution_time(void (*func)(int *), int *arr, const char *label) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    func(arr);
    gettimeofday(&end, NULL);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("%s: Tempo = %f segundos\n", label, elapsed_time);
}

long long measure_sum_execution_time(long long (*func)(int *), int *arr, const char *label) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    long long result = func(arr);
    gettimeofday(&end, NULL);
    double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("%s: Resultado = %lld, Tempo = %f segundos\n", label, result, elapsed_time);
    return result;
}

int main() {
    int *arr = (int *)malloc(SIZE * sizeof(int));
    if (!arr) {
        printf("Erro ao alocar memória!\n");
        return 1;
    }
    
    measure_execution_time(initialize_array, arr, "\nInicialização do array");
    measure_sum_execution_time(sum_sequential, arr, "Soma sequencial");
    measure_sum_execution_time(sum_parallel, arr, "Soma paralelizada");
    
    free(arr);
    return 0;
}
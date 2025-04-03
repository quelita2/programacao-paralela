/* 
- Compilado com: gcc -O3 main.c -o teste03 && ./teste03
(descrevendo o tipo de otimização O0, O2 e O3)
(colocar no relatorio os comandos de compilação)

- Exercicio de Paralelismo em nivel:
Por que na compilação O0 a soma com multiplas variaveis é mais rápido do que a soma acumulativa, enquanto na O3 é o oposto?
R:  Na compilação -O0 (sem otimizações), o código é traduzido quase diretamente, sem otimizações de registradores e sem eliminação de dependências. Assim, a soma com múltiplas variáveis (sum_parallel) é mais rápida porque reduz dependências entre iterações, permitindo mais paralelismo ao nível de instrução.  Já na -O3, o compilador vetoriza e desenrola loops, otimizando automaticamente a soma sequencial (sum_sequential). Isso faz com que a otimização torne o loop acumulativo mais eficiente que a abordagem manual de paralelismo.

*/

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
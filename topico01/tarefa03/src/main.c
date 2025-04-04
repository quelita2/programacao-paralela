#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>

#define REAL_PI 3.141592653589793

// Função para calcular pi usando a série de Leibniz
double calculate_pi(long iterations) {
    double pi = 0.0;
    for (long i = 0; i < iterations; i++) {
        double term = (i % 2 == 0 ? 1.0 : -1.0) / (2.0 * i + 1.0);
        pi += term;
    }
    return pi * 4.0;
}

// Função para contar quantos dígitos corretos a aproximação tem
int count_correct_digits(double approx_pi) {
    char real_pi_str[50], approx_pi_str[50];
    snprintf(real_pi_str, sizeof(real_pi_str), "%.20f", REAL_PI);
    snprintf(approx_pi_str, sizeof(approx_pi_str), "%.20f", approx_pi);
    
    int correct_digits = 0;
    for (int i = 0; i < strlen(real_pi_str); i++) {
        if (real_pi_str[i] == approx_pi_str[i]) {
            correct_digits++;
        } else {
            break;
        }
    }
    return correct_digits - 2; // Subtrai "3."
}

int main() {
    long iterations[] = {100, 1000, 10000, 100000, 1000000, 10000000};
    int num_cases = sizeof(iterations) / sizeof(iterations[0]);

    // Abrir arquivo CSV para escrita
    FILE *fp = fopen("data.csv", "w");
    if (fp == NULL) {
        printf("Erro ao criar o arquivo CSV.\n");
        return 1;
    }
    fprintf(fp, "Iterações,Dígitos Corretos,Aproximação de Pi,Erro Absoluto,Tempo (ms)\n");

    printf("Iterações | Dígitos Corretos | Aproximação de Pi | Erro Absoluto | Tempo (ms)\n");
    printf("------------------------------------------------------------------------------------\n");

    for (int i = 0; i < num_cases; i++) {
        struct timeval start, end;
        gettimeofday(&start, NULL);

        double approx_pi = calculate_pi(iterations[i]);

        gettimeofday(&end, NULL);
        double elapsed_time = (end.tv_sec - start.tv_sec) * 1000.0;
        elapsed_time += (end.tv_usec - start.tv_usec) / 1000.0;

        int correct_digits = count_correct_digits(approx_pi);

        printf("%9ld | %16d | %17.15f | %13.10f | %8.3f ms\n", 
               iterations[i], correct_digits, approx_pi, fabs(REAL_PI - approx_pi), elapsed_time);

        fprintf(fp, "%ld,%d,%.15f,%.10f,%.3f\n", 
                iterations[i], correct_digits, approx_pi, fabs(REAL_PI - approx_pi), elapsed_time);
    }

    fclose(fp); // Fechar o arquivo

    return 0;
}

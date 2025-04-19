/*
Devemos nos preocupar da ineficiencia quando programamos em paralelo!
- Deve ter equilibrio, reduzindo desequilibrio de carga, 
- minimizar sincronização para evitar condição de corrida 
- Diminuir adições de coisas na comunicação visando diminuir a latencia e tempo ocioso (utilizando tecnicas para esconder a latencia).


-> Exercicio: calcular sem proteção a zona critica da variavel utilizada. Não é possível obter o mesmo resultado sem a proteção à zona critica, pelo problema da condição de corrida! A contagem se perde e não se torna um código confiável. Entretanto com a zona critica utilizada a contagem é confiável e a versão paralela é mais otimizada. Qual a causa para essa otimização não funcionar? 
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

int eh_primo(int num) {
    if (num < 2) return 0;
    for (int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

double tempo_execucao(struct timeval inicio, struct timeval fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1e6;
}

int main() {
    int valores_n[] = {10, 100, 1000, 10000, 100000, 1000000, 10000000};
    int total_testes = sizeof(valores_n) / sizeof(valores_n[0]);

    struct timeval inicio, fim;

    // CSV header
    printf("n,Threads,Tempo(s),Qtd_Primos\n");

    // --- VERSÃO SEQUENCIAL (simulando Threads = 1) ---
    for (int t = 0; t < total_testes; t++) {
        int n = valores_n[t];
        int count_seq = 0;

        gettimeofday(&inicio, NULL);
        for (int i = 2; i <= n; i++) {
            if (eh_primo(i)) {
                count_seq++;
            }
        }
        gettimeofday(&fim, NULL);
        double tempo_seq = tempo_execucao(inicio, fim);

        printf("%d,1,%.8f,%d\n", n, tempo_seq, count_seq);
    }

    // --- VERSÃO PARALELA COM THREADS DE 2 A 8 ---
    for (int num_threads = 2; num_threads <= 8; num_threads++) {
        omp_set_num_threads(num_threads);

        for (int t = 0; t < total_testes; t++) {
            int n = valores_n[t];
            int count_par = 0;

            gettimeofday(&inicio, NULL);
            #pragma omp parallel for
            for (int i = 2; i <= n; i++) {
                if (eh_primo(i)) {
                    #pragma omp critical
                    count_par++;
                }
            }
            gettimeofday(&fim, NULL);
            double tempo_par = tempo_execucao(inicio, fim);

            printf("%d,%d,%.8f,%d\n", n, num_threads, tempo_par, count_par);
        }
    }

    return 0;
}

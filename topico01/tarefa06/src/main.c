#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

#define PI_REAL 3.141592

/*Versão Sequencial*/
double pi_sequencial(int N) {
    int count = 0;
    for (int i = 0; i < N; i++) {
        double x = (double) rand() / RAND_MAX;
        double y = (double) rand() / RAND_MAX;
        if (x * x + y * y <= 1.0)
            count++;
    }
    return 4.0 * count / N;
}

/*Versão Paralela com Erro (condição de corrida)*/
/*Tentando paralelizar com #pragma omp parallel for — resultado incorreto*/
double pi_com_erro(int N) {
    int count = 0;
    
    #pragma omp parallel for
    for (int i = 0; i < N; i++){
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1.0)
            count++;
    }    

    return 4.0 * count / N;
}

/*Correção com #pragma omp critical*/
double pi_com_critical(int N) {
    int count = 0;

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        double x = (double) rand() / RAND_MAX;
        double y = (double) rand() / RAND_MAX;
        if (x * x + y * y <= 1.0) {
            #pragma omp critical
            count++;
        }
    }

    return 4.0 * count / N;
}

/*Versão Reestruturada*/
double pi_reestruturada(int N) {
    int count = 0;

    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < N; i++) {
            double x = (double) rand() / RAND_MAX;
            double y = (double) rand() / RAND_MAX;

            if (x * x + y * y <= 1.0) {
                #pragma omp critical
                count++;
            }
        }
    }

    return 4.0 * count / N;
}

/*Versão com a cláusula Private*/
double pi_com_private(int N) {
    int count = 0;
    int local_count;  // Declarada fora da região paralela

    #pragma omp parallel private(local_count)
    {
        local_count = 0;

        #pragma omp for
        for (int i = 0; i < N; i++) {
            double x = (double) rand() / RAND_MAX;
            double y = (double) rand() / RAND_MAX;

            if (x * x + y * y <= 1.0) {
                local_count++;
            }
        }

        #pragma omp critical
        {
            count += local_count;
        }
    }

    return 4.0 * (double) count / N;
}

/*Versão com a cláusula FirstPrivate*/
double pi_com_firstprivate(int N) {
    int count = 0;
    unsigned int seed = time(NULL);

    #pragma omp parallel firstprivate(seed)
    {
        int local_count = 0;
        seed += omp_get_thread_num();

        #pragma omp for
        for (int i = 0; i < N; i++)
        {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0)
                local_count++;
        }

        #pragma omp critical
        count += local_count;
    }

    return 4.0 * count / N;
}

/*Versão com a cláusula LastPrivate*/
double pi_com_lastprivate(int N) {
    int count = 0;
    int last_i = -1;

    #pragma omp parallel
    {
        int local_count = 0;

        #pragma omp for lastprivate(last_i)
        for (int i = 0; i < N; i++)
        {
            double x = (double)rand() / RAND_MAX;
            double y = (double)rand() / RAND_MAX;
            if (x * x + y * y <= 1.0)
                local_count++;
            last_i = i;
        }

        #pragma omp critical
        count += local_count;
    }

    return 4.0 * count / N;
}

/*Versão com a cláusula Share + diretiva Default*/
double pi_com_default_share(int N) {
    int count = 0;

    #pragma omp parallel default(none) shared(count, N)
    {
        int local_count = 0;
        unsigned int seed = time(NULL) ^ omp_get_thread_num();

        #pragma omp for
        for (int i = 0; i < N; i++)
        {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0)
                local_count++;
        }

        #pragma omp critical
        count += local_count;
    }

    return 4.0 * count / N;
}

// Função para calcular erro percentual
double erro_percentual(double estimado) {
    return fabs((estimado - PI_REAL) / PI_REAL) * 100;
}

int main() {
    int N = 10000000;

    srand(time(NULL)); // Inicializa rand() para versão sequencial e com erro

    double pi_seq = pi_sequencial(N);
    double pi_erro = pi_com_erro(N);
    double pi_crit = pi_com_critical(N);
    double pi_final = pi_reestruturada(N);
    double pi_private = pi_com_private(N);
    double pi_firstprivate = pi_com_firstprivate(N);
    double pi_lastprivate = pi_com_lastprivate(N);
    double pi_default_share = pi_com_default_share(N);

    printf("Valor real de pi                      : %.6f\n\n", PI_REAL);

    printf("Estimativa de pi (sequencial)         : %.6f ✅ | Erro: %.4f%%\n", pi_seq, erro_percentual(pi_seq));
    printf("Estimativa de pi (com erro)           : %.6f ❌ | Erro: %.4f%%\n", pi_erro, erro_percentual(pi_erro));
    printf("Estimativa de pi (com critical)       : %.6f ✅ | Erro: %.4f%%\n", pi_crit, erro_percentual(pi_crit));
    printf("Estimativa de pi (reestruturado)      : %.6f ✅ | Erro: %.4f%%\n", pi_final, erro_percentual(pi_final));
    printf("Estimativa de pi (com private)        : %.6f ✅ | Erro: %.4f%%\n", pi_private, erro_percentual(pi_private));
    printf("Estimativa de pi (com firstprivate)   : %.6f ✅ | Erro: %.4f%%\n", pi_firstprivate, erro_percentual(pi_firstprivate));
    printf("Estimativa de pi (com lastprivate)    : %.6f ✅ | Erro: %.4f%%\n", pi_lastprivate, erro_percentual(pi_lastprivate));
    printf("Estimativa de pi (com default e share): %.6f ✅ | Erro: %.4f%%\n", pi_default_share, erro_percentual(pi_default_share));
    
    return 0;
}
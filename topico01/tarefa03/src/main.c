/*
Por que algumas aplicações requerem maior poder computacional continuamente?
R:
Algumas aplicações requerem maior poder computacional continuamente porque lidam com grandes volumes de dados, processamento intenso e execução em tempo real.

Por meio deste exercicio reconhecemos que precisamos de programação paralela para alcançar alto desempenho?

- Implemente um programa em c que calcule uma aproximação de pi usando uma serie matemaatica, variando o numero de iterações e medindo o tempo de execução com a função gettimeofday(). Compare os valores obtidos com o valor real de pi e analise como a acuracia melhora com mais processamento. Reflita sobre como esse comportamento se repete em aplicações reais que demandam resultados cada vez mais precisos, como simulações fisicas e inteligencia artificial.
R: 
À medida que aumentamos o número de iterações no cálculo da série de Leibniz, 'lsa aproximação de π se torna mais precisa, reduzindo gradativamente o erro absoluto. No entanto, essa melhoria na precisão ocorre de forma não linear. No início, pequenos aumentos no número de iterações trazem ganhos significativos na acurácia, como observado ao passar de 100 para 1.000 iterações, onde os dígitos corretos dobram de 1 para 2. Porém, conforme a aproximação se torna mais precisa, cada novo dígito correto exige um número de iterações exponencialmente maior, como evidenciado pelo salto de 1 milhão para 10 milhões de iterações, que resulta apenas em um dígito correto adicional.

Esse comportamento reflete um princípio fundamental em aplicações reais que exigem alta precisão computacional. Em **simulações físicas**, por exemplo, modelos numéricos para previsão do clima ou dinâmica de fluidos requerem refinamentos sucessivos para reduzir erros, mas cada aumento na precisão demanda mais poder computacional e tempo de processamento. Da mesma forma, em **inteligência artificial**, especialmente no treinamento de redes neurais profundas, ganhos marginais de acurácia geralmente requerem ordens de grandeza mais recursos computacionais, evidenciando um ponto de **diminuição de retornos**.

Assim, em muitas aplicações, existe um balanço entre precisão e eficiência computacional. Em certos cenários, pode não ser viável buscar precisão extrema devido às limitações de tempo e custo computacional. Estratégias como paralelização, aproximações otimizadas e modelos híbridos são frequentemente utilizadas para obter um bom compromisso entre desempenho e exatidão. Esse desafio é central na computação científica e no avanço da inteligência artificial, destacando a importância de algoritmos eficientes que maximizem a precisão sem custos computacionais proibitivos.

- Plotar um gráfico de digitos corretos em decorrencia da iteração.

*/
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

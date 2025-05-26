#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_ITER 1000

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0)
            printf("Este programa requer exatamente 2 processos.\n");
        MPI_Finalize();
        return 1;
    }

    int sizes[] = {8, 64, 512, 1024, 8192, 65536, 131072, 262144, 524288, 1048576};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    FILE* fp = NULL;
    if (rank == 0) {
        fp = fopen("tempo_mensagens.csv", "w");
        if (fp == NULL) {
            perror("Erro ao criar o arquivo CSV");
            MPI_Abort(MPI_COMM_WORLD, 1); // é ativado e finaliza tudo pq deu erro
        }
        fprintf(fp, "Tamanho (bytes),Tempo medio (ms)\n");
    }

    for (int s = 0; s < num_sizes; s++) {
        int size_bytes = sizes[s];
        char* message = malloc(size_bytes);
        memset(message, 'a', size_bytes);

        // para que todos os processos comecem a troca de mensagens no mesmo momento
        MPI_Barrier(MPI_COMM_WORLD);
        double start = MPI_Wtime();

        for (int i = 0; i < NUM_ITER; i++) {
            if (rank == 0) {
                /* Envia uma mensagem para outro processo
                 *message: ponteiro para o buffer que contém a mensagem a ser enviada.
                 *size_bytes: número de elementos do tipo indicado (aqui, char).
                 *MPI_CHAR: tipo dos dados a serem enviados.
                 *dest: rank do processo de destino.
                 *tag: identificador da mensagem (usado para distinguir mensagens diferentes).
                 *comm: comunicador (geralmente MPI_COMM_WORLD)
                */
                MPI_Send(message, size_bytes, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

                /* Recebe uma mensagem de outro processo e bloqueia até que a mensagem correta seja recebida
                 *source: rank do processo remetente
                 *tag: identificador da mensagem (deve bater com o da Send)
                 *MPI_STATUS_IGNORE: ignora informações detalhadas da mensagem recebida (como número real de bytes ou origem)
                */
                MPI_Recv(message, size_bytes, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (rank == 1) {
                MPI_Recv(message, size_bytes, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(message, size_bytes, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }

        double end = MPI_Wtime();
        if (rank == 0) {
            double elapsed = (end - start) * 1000.0;
            double avg_time = elapsed / NUM_ITER;
            fprintf(fp, "%d,%.6f\n", size_bytes, avg_time);
        }

        free(message);
    }

    if (fp != NULL) fclose(fp);
    MPI_Finalize();
    return 0;
}

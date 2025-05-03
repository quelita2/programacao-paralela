#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

typedef struct Node {
    int value;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
} List;

// Função para inserir no início da lista
void insert(List* list, int value) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->value = value;
    new_node->next = list->head;
    list->head = new_node;
}

// Função para imprimir a lista
void print_list(List* list, int id) {
    printf("Lista %d: ", id);
    Node* curr = list->head;
    while (curr) {
        printf("%d -> ", curr->value);
        curr = curr->next;
    }
    printf("NULL\n");
}

// Versão 1: 2 listas com regiões críticas nomeadas
void versao_1(int N) {
    printf("\n=== VERSÃO 1: 2 listas com regiões críticas nomeadas ===\n");

    List lista1 = {NULL};
    List lista2 = {NULL};

    #pragma omp parallel
    {
        #pragma omp single
        for (int i = 0; i < N; i++) {
            #pragma omp task
            {
                int num = rand() % 1000;
                int destino = rand() % 2;

                if (destino == 0) {
                    #pragma omp critical(lista1)
                    insert(&lista1, num);
                } else {
                    #pragma omp critical(lista2)
                    insert(&lista2, num);
                }
            }
        }
    }

    print_list(&lista1, 1);
    print_list(&lista2, 2);
}

// Versão 2: M listas com locks explícitos
void versao_2(int N, int M) {
    printf("\n=== VERSÃO 2: %d listas com locks explícitos ===\n", M);

    List* listas = (List*)malloc(M * sizeof(List));
    omp_lock_t* locks = (omp_lock_t*)malloc(M * sizeof(omp_lock_t));

    for (int i = 0; i < M; i++) {
        listas[i].head = NULL;
        omp_init_lock(&locks[i]);
    }

    #pragma omp parallel
    {
        #pragma omp single
        for (int i = 0; i < N; i++) {
            #pragma omp task
            {
                int num = rand() % 1000;
                int destino = rand() % M;

                omp_set_lock(&locks[destino]);
                insert(&listas[destino], num);
                omp_unset_lock(&locks[destino]);
            }
        }
    }

    for (int i = 0; i < M; i++) {
        print_list(&listas[i], i + 1);
        omp_destroy_lock(&locks[i]);
    }

    free(listas);
    free(locks);
}

int main() {
    srand(time(NULL));
    int N = 50; // Número de inserções

    versao_1(N);

    int M = 4;  // Número de listas na versão 2
    versao_2(N, M);

    return 0;
}

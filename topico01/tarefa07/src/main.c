#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Definindo o nó da lista
typedef struct Node {
    char filename[100];
    struct Node* next;
} Node;

// Função para criar um novo nó
Node* create_node(const char* filename) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    strcpy(new_node->filename, filename);
    new_node->next = NULL;
    return new_node;
}

// Função para adicionar um nó ao final da lista
void append_node(Node** head, const char* filename) {
    Node* new_node = create_node(filename);
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    Node* temp = *head;
    while (temp->next)
        temp = temp->next;
    temp->next = new_node;
}

int main() {
    Node* head = NULL;

    // Criando lista de arquivos fictícios
    append_node(&head, "arquivo1.txt");
    append_node(&head, "arquivo2.txt");
    append_node(&head, "arquivo3.txt");
    append_node(&head, "arquivo4.txt");

    Node* current = head;
    
    // Região paralela com tasks
    #pragma omp parallel
    {
        #pragma omp master
        {
            while (current) {
                Node* node = current;
                current = current->next;
 
                #pragma omp task firstprivate(node)
                {
                    int thread_id = omp_get_thread_num();
                    printf("Processando %s na thread %d\n", node->filename, thread_id);
                }
            }
        } // master não tem barreira implicita
    } // tem barreira implicita

    // Liberar memória
    current = head;
    while (current) {
        Node* tmp = current;
        current = current->next;
        free(tmp);
    }

    return 0;
}

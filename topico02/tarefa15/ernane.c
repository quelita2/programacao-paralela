#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define N 1000     // Tamanho total da barra
#define STEPS 1000 // Passos de tempo
#define ALPHA 0.01 // Coeficiente de difusão
#define MASTER 0

void init_bar(double *u_local, int local_n, int rank, int size)
{
  for (int i = 1; i <= local_n; i++)
  {
    u_local[i] = 0.0;
  }
  
  if (rank == size / 2) // Apenas o processo que contém o meio inicializa com calor
  {
    int mid = local_n / 2;
    u_local[mid] = 100.0;
  }
}

// eu too ficando maluco
//      Proc 0           Proc 1          Proc 2          Proc 3
// |---------------||---------------||---------------||---------------|
//  [000] ... [250]  [251] ... [500]  [501] ... [750]  [751] ... [999]

void simulate_blocking(int rank, int size, int local_n, double *u_local, double *u_next)
{
  for (int t = 0; t < STEPS; t++)
  {
    if (rank > 0)
      // se eu não sou o primeiro processo (rank > 0), envio meu valor mais à esquerda (u_local[1])
      // para o processo à minha esquerda (rank - 1)
      // MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
      MPI_Send(&u_local[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD);
    if (rank < size - 1)
      // se eu não sou o último processo, envio o meu valor mais à direita para o processo à minha 
      // direita (rank + 1)
      // MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm);
      MPI_Send(&u_local[local_n], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD);

    if (rank < size - 1)
      // Se eu não sou o último processo, recebo da direita e coloco na ghost cell da direita 
      // (u_local[local_n + 1])
      // MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status);
      MPI_Recv(&u_local[local_n + 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (rank > 0)
      // Se eu não sou o primeiro processo, recebo da esquerda e coloco na ghost cell da esquerda
      // (u_local[0])
      MPI_Recv(&u_local[0], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int i = 1; i <= local_n; i++)
    {
      u_next[i] = u_local[i] + ALPHA * (u_local[i - 1] - 2 * u_local[i] + u_local[i + 1]);
    }

    memcpy(u_local, u_next, (local_n + 2) * sizeof(double));
  }
}

//      Proc 0           Proc 1          Proc 2          Proc 3
// |---------------||---------------||---------------||---------------|
//  [000] ... [250]  [251] ... [500]  [501] ... [750]  [751] ... [999]

void simulate_nonblocking_wait(int rank, int size, int local_n, double *u_local, double *u_next)
{
  for (int t = 0; t < STEPS; t++)
  {
    MPI_Request reqs[4]; // 0 - recv left, 1 - recv right, 2 - send left, 3 - send right

    if (rank > 0)
      // Se tenho um vizinho à esquerda, inicio uma recepção do valor da borda dele para minha ghost cell esquerda
      // MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
      MPI_Irecv(&u_local[0], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &reqs[0]);
    else
      // Caso contrário, marco como MPI_REQUEST_NULL - indicando que n faz nd
      reqs[0] = MPI_REQUEST_NULL;

    if (rank < size - 1)
      // Se tenho um vizinho à direita, recebo a borda dele na minha ghost cell direita (u_local[local_n + 1]).
      // MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
      MPI_Irecv(&u_local[local_n + 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[1]);
    else
      // Caso contrário, marco como MPI_REQUEST_NULL - indicando que n faz nd
      reqs[1] = MPI_REQUEST_NULL;

    if (rank > 0)
      // Se tenho vizinho à esquerda, envio minha borda esquerda (u_local[1]) para ele.
      // MPI_Isend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
      MPI_Isend(&u_local[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[2]);
    else
      // Caso contrário, marco como MPI_REQUEST_NULL - indicando que n faz nd
      reqs[2] = MPI_REQUEST_NULL;

    if (rank < size - 1)
      // Se tenho vizinho à direita, envio minha borda direita (u_local[local_n]) para ele
      // MPI_Isend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
      MPI_Isend(&u_local[local_n], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &reqs[3]);
    else
      // Caso contrário, marco como MPI_REQUEST_NULL - indicando que n faz nd
      reqs[3] = MPI_REQUEST_NULL;

    // MPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[]);
    MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE); // só continua quando todas as requisições finalizarem

    for (int i = 1; i <= local_n; i++)
    {
      u_next[i] = u_local[i] + ALPHA * (u_local[i - 1] - 2 * u_local[i] + u_local[i + 1]);
    }

    memcpy(u_local, u_next, (local_n + 2) * sizeof(double));
  }
}

void simulate_nonblocking_test(int rank, int size, int local_n, double *u_local, double *u_next) // overlap
{
  for (int t = 0; t < STEPS; t++)
  {
    MPI_Request reqs[4]; // 0 - recv left, 1 - recv right, 2 - send left, 3 - send right
    int completed[4] = {0, 0, 0, 0}; // 0 - não completou, 1 - completou
    
    if (rank > 0)
      // Se houver vizinho à esquerda, receba o valor da borda esquerda dele (salva em u_local[0], ghost cell).
      // MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
      MPI_Irecv(&u_local[0], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &reqs[0]);
    else
      // Caso contrário, marco como MPI_REQUEST_NULL - indicando que n faz nd
      reqs[0] = MPI_REQUEST_NULL;

    if (rank < size - 1)
      // Se houver vizinho à direita, receba o valor da borda direita dele (salva em u_local[local_n + 1], ghost cell).
      // MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request);
      MPI_Irecv(&u_local[local_n + 1], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[1]);
    else
      // Caso contrário, marco como MPI_REQUEST_NULL - indicando que n faz nd
      reqs[1] = MPI_REQUEST_NULL;

    if (rank > 0)
      // Se houver vizinho à esquerda, envie meu valor mais à esquerda (u_local[1]) para ele.
      // MPI_Isend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
      MPI_Isend(&u_local[1], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[2]);
    else
      // Caso contrário, marco como MPI_REQUEST_NULL - indicando que n faz nd
      reqs[2] = MPI_REQUEST_NULL;

    if (rank < size - 1)
      // Se houver vizinho à direita, envie meu valor mais à direita (u_local[local_n]) para ele.
      // MPI_Isend(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request);
      MPI_Isend(&u_local[local_n], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &reqs[3]);
    else
      // Caso contrário, marco como MPI_REQUEST_NULL - indicando que n faz nd
      reqs[3] = MPI_REQUEST_NULL;

    // SEM WAIT => não bloqueante
    // MPI_Waitall.....

    // Atualiza apenas os pontos internos enquanto aguarda comunicação pois não dependem das bordas
    for (int i = 2; i <= local_n - 1; i++)
    {
      u_next[i] = u_local[i] + ALPHA * (u_local[i - 1] - 2 * u_local[i] + u_local[i + 1]);
    }


    while (!(completed[0] && completed[1])) // Espera chegar as bordas esq e dir
    {
      if (!completed[0]) // Se não chegou a borda esquerda, espera
      // MPI_Test(MPI_Request *request, int *flag, MPI_Status *status);
      MPI_Test(&reqs[0], &completed[0], MPI_STATUS_IGNORE); // Testa se chegou e atualiza
      if (!completed[1]) // Se não chegou a borda direita, espera
      MPI_Test(&reqs[1], &completed[1], MPI_STATUS_IGNORE); // Testa se chegou e atualiza
    } // passou, significa que as bordas chegaram e podem ser atualizadas assim como os pontos internos foram

    u_next[1] = u_local[1] + ALPHA * (u_local[0] - 2 * u_local[1] + u_local[2]);
    u_next[local_n] = u_local[local_n] + ALPHA * (u_local[local_n - 1] - 2 * u_local[local_n] + u_local[local_n + 1]);

    // Aguarda envio finalizar - garante que os dados foram realmente enviados
    // MPI_Wait(MPI_Request *request, MPI_Status *status);
    MPI_Wait(&reqs[2], MPI_STATUS_IGNORE);
    MPI_Wait(&reqs[3], MPI_STATUS_IGNORE);

    memcpy(u_local, u_next, (local_n + 2) * sizeof(double));
  }
}

int main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  int rank, size;
  double start, end;
  // MPI_Comm_rank(MPI_Comm comm, int *rank);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  // MPI_Comm_size(MPI_Comm comm, int *size);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (N % size != 0)
  {
    if (rank == MASTER)
      printf("Erro: o tamanho da barra (%d) deve ser divisível pelo número de processos (%d).\n", N, size);
    MPI_Finalize();
    return EXIT_FAILURE;
  }

  int local_n = N / size;
  double *u_local = malloc((local_n + 2) * sizeof(double));
  double *u_next = malloc((local_n + 2) * sizeof(double));

  init_bar(u_local, local_n, rank, size);
  // MPI_Barrier(MPI_Comm comm);
  MPI_Barrier(MPI_COMM_WORLD);
  // double MPI_Wtime();
  start = MPI_Wtime();
  simulate_blocking(rank, size, local_n, u_local, u_next);
  // MPI_Barrier(MPI_Comm comm);
  MPI_Barrier(MPI_COMM_WORLD);
  // double MPI_Wtime();
  end = MPI_Wtime();
  if (rank == MASTER)
    printf("Versão 1 (bloqueante): %f s\n", end - start);

  init_bar(u_local, local_n, rank, size);
  // MPI_Barrier(MPI_Comm comm);
  MPI_Barrier(MPI_COMM_WORLD);
  // double MPI_Wtime();
  start = MPI_Wtime();
  simulate_nonblocking_wait(rank, size, local_n, u_local, u_next);
  // MPI_Barrier(MPI_Comm comm);
  MPI_Barrier(MPI_COMM_WORLD);
  // double MPI_Wtime();
  end = MPI_Wtime();
  if (rank == MASTER)
    printf("Versão 2 (não bloqueante + wait): %f s\n", end - start);

  init_bar(u_local, local_n, rank, size);
  // MPI_Barrier(MPI_Comm comm);
  MPI_Barrier(MPI_COMM_WORLD);
  // double MPI_Wtime();
  start = MPI_Wtime();
  simulate_nonblocking_test(rank, size, local_n, u_local, u_next);
  // MPI_Barrier(MPI_Comm comm);
  MPI_Barrier(MPI_COMM_WORLD);
  // double MPI_Wtime();
  end = MPI_Wtime();
  if (rank == MASTER)
    printf("Versão 3 (não bloqueante + test): %f s\n", end - start);

  free(u_local);
  free(u_next);

  MPI_Finalize();
  return 0;
}
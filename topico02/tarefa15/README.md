
## Comunicação MPI
| Função    | Tipo        | Bloqueia? | Quando usar?                          |
| --------- | ----------- | --------- | ------------------------------------- |
| MPI_Send  | Envio       | Sim       | Comunicação simples e direta          |
| MPI_Recv  | Recebimento | Sim       | Comunicação simples e direta          |
| MPI_Isend | Envio       | Não       | Quando quiser continuar computando    |
| MPI_Irecv | Recebimento | Não       | Quando quiser continuar computando    |
| MPI_Wait  | Finalização | Sim       | Para garantir que a operação terminou |
| MPI_Test  | Verificação | Não       | Para verificar se a operação terminou |


## Resultados de tempo:

| Nº de Processos | Send/Recv (s) | Isend/Irecv+Wait (s) | Isend/Irecv+Test (s) |
|-----------------|---------------|----------------------|----------------------|
|               2 |      0.000946 |             0.000907 |             0.000904 |
|               4 |      0.001183 |             0.001010 |             0.001002 |
|               8 |      0.001137 |             0.000975 |             0.000958 |
|              16 |      0.002430 |             0.002184 |             0.002126 |
|              32 |      0.015316 |             0.001869 |             0.001899 |

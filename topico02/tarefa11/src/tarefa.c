#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>
#include <string.h>

#define N 50
#define T 100
#define DX 1.0
#define DT 0.01
#define NU 0.1

double u[N][N][N], u_next[N][N][N];

void initialize() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; k++)
                u[i][j][k] = 0.0;

    int c = N / 2;
    u[c][c][c] = 1.0;
}

void step(const char* sched_type, int chunk_size, int use_collapse) {
    if (strcmp(sched_type, "static") == 0)
        omp_set_schedule(omp_sched_static, chunk_size);
    else if (strcmp(sched_type, "dynamic") == 0)
        omp_set_schedule(omp_sched_dynamic, chunk_size);
    else if (strcmp(sched_type, "guided") == 0)
        omp_set_schedule(omp_sched_guided, chunk_size);

    #pragma omp parallel
    {
        if (use_collapse) {
            #pragma omp for collapse(2) schedule(runtime)
            for (int i = 1; i < N - 1; i++)
                for (int j = 1; j < N - 1; j++)
                    for (int k = 1; k < N - 1; k++) {
                        double laplacian =
                            (u[i+1][j][k] + u[i-1][j][k] +
                             u[i][j+1][k] + u[i][j-1][k] +
                             u[i][j][k+1] + u[i][j][k-1] -
                             6*u[i][j][k]) / (DX * DX);
                        u_next[i][j][k] = u[i][j][k] + NU * DT * laplacian;
                    }
        } else {
            #pragma omp for schedule(runtime)
            for (int i = 1; i < N - 1; i++)
                for (int j = 1; j < N - 1; j++)
                    for (int k = 1; k < N - 1; k++) {
                        double laplacian =
                            (u[i+1][j][k] + u[i-1][j][k] +
                             u[i][j+1][k] + u[i][j-1][k] +
                             u[i][j][k+1] + u[i][j][k-1] -
                             6*u[i][j][k]) / (DX * DX);
                        u_next[i][j][k] = u[i][j][k] + NU * DT * laplacian;
                    }
        }
    }

    for (int i = 1; i < N - 1; i++)
        for (int j = 1; j < N - 1; j++)
            for (int k = 1; k < N - 1; k++)
                u[i][j][k] = u_next[i][j][k];
}

void run_simulation(const char* sched_type, int chunk_size, int use_collapse) {
    struct timeval start, end;
    initialize();
    gettimeofday(&start, NULL);

    for (int t = 0; t < T; t++) {
        step(sched_type, chunk_size, use_collapse);
    }

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_usec - start.tv_usec) / 1e6;

    printf("Schedule: %-7s | Chunk: %d | Collapse: %d | Tempo: %.3f s\n",
           sched_type, chunk_size, use_collapse, elapsed);
}

int main() {
    const char* scheds[] = {"static", "dynamic", "guided"};
    int chunks[] = {1, 2, 4, 8};

    for (int collapse = 0; collapse <= 1; collapse++) {
        for (int s = 0; s < 3; s++) {
            for (int c = 0; c < 4; c++) {
                run_simulation(scheds[s], chunks[c], collapse);
            }
        }
    }
    return 0;
}

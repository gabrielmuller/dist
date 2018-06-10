/*
 * ring.c
 * Ilustra um protocolo de anel com token.
 * Note na saída que a ordem dos processos trabalhando na região
 * crítica é sempre 01230123..., na ordem em que circula o token,
 * enquanto o trabalho local é feito em qualquer ordem.
 */
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

#define next(r) (r + 1) % proc_n
#define prev(r) (r - 1) % proc_n

int proc_n;
int rank;

/*
 * Trabalha numa região crítica.
 */
void work_critical (int val) {
    printf("Trabalhando em região crítica proc %d: %d\n", rank, val);
    usleep(1000000);
}

/*
 * Trabalha localmente enquanto não tem o token.
 */
void work_local (MPI_Request* req) {
    int flag;
    MPI_Test(req, &flag, MPI_STATUS_IGNORE);

    while (!flag) {
        MPI_Test(req, &flag, MPI_STATUS_IGNORE);
        printf("Processo %d trabalhando em região não-crítica...\n", rank);
        usleep(500000);
    }
}
        
/*
 * Passa o token para o próximo processo.
 */
void yield_token (int token) {
    // Manda o token
    MPI_Send(&token, 1, MPI_INT, next(rank), 0, MPI_COMM_WORLD);
}

/*
 * Recebe o token
 */
int recv_token () {
    int token;
    MPI_Request req;
    MPI_Irecv(&token, 1, MPI_INT, prev(rank), 0, MPI_COMM_WORLD, &req);
    work_local(&req);
    return token;
}

int main (int argc, char** argv) {
    // Inicia MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Primeiro token é gerado.
    if (rank == 0) {
        yield_token(0);
    } 

    for(;;) {
        int token = recv_token();
        work_critical(token);
        yield_token(++token);
    }

    // Finaliza
    MPI_Finalize();
    return 0;
}

#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

#define next(r) (r + 1) % proc_n
#define prev(r) (r - 1) % proc_n

int proc_n;
int rank;

/*
 * Trabalha numa região crítica.
 * Ex: escrever num buffer de saída
 */
void work (int val) {
    sleep(1);
    printf("Trabalhando @ proc %d: %d\n", rank, val);
    fflush(stdout);
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
    MPI_Recv(&token, 1, MPI_INT, prev(rank), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
        work(token);
        yield_token(++token);
    }

    // Finaliza
    MPI_Finalize();
    return 0;
}

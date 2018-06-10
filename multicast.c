#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define next(r) (r + 1) % proc_n

enum tag {REQUEST, ALLOW};

int tick = 0;
int proc_n;
int rank;

void work_local();

void update_tick(int new_tick) {
    tick = new_tick > tick ? new_tick : tick;
    tick++;
}
    
void print_tick() {
    printf("tick %d: ", tick);
}
void work_critical () {
    // pede acesso
    print_tick();
    printf("Proc %d pede acesso à RC.\n", rank);
    for (int i = 1; i < proc_n; i++) {
    }

    // espera resposta de todos
    MPI_Request req[proc_n-1];
    int msg[proc_n-1];
    int inc = 0;
    for (int i = 0; i < proc_n; i++) {
        // não mandar mensagem para si
        if (i == rank) {
            inc = -1;
            continue;
        }
        tick++;
        MPI_Request r;
        MPI_Isend(&tick, 1, MPI_INT, i, REQUEST, MPI_COMM_WORLD, &r);
        MPI_Request_free(&r);
        MPI_Irecv(&msg[i+inc], 1, MPI_INT, i, ALLOW, MPI_COMM_WORLD, &req[i+inc]);
    }
    MPI_Waitall(proc_n-1, req, MPI_STATUSES_IGNORE);

    for (int i = 0; i < proc_n-1; i++) {
        update_tick(msg[i]);
    }
    
    // trabalha na região crítica
    print_tick();
    printf("Trabalhando em RC proc %d\n", rank);
    usleep(1000000);

    // acaba e sai da RC
    work_local();
}

void work_local () {
    print_tick();
    printf("Proc %d trabalhando localmente\n", rank);

    int recv_tick;
    MPI_Status status;
    MPI_Recv(&recv_tick, 1, MPI_INT, MPI_ANY_SOURCE,
        REQUEST, MPI_COMM_WORLD, &status);
    int src = status.MPI_SOURCE;

    update_tick(recv_tick);

    tick++;
    MPI_Send(&tick, 1, MPI_INT, src, ALLOW, MPI_COMM_WORLD);

    print_tick();
    printf("Proc %d permite %d trabalhar na RC.\n", rank, src);

    usleep(1500000);
    if (next(src) == rank) {
        work_critical();
    } else {
        work_local();
    }
}

int main (int argc, char** argv) {
    // Inicia MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // proc 0 começa pedindo entrada na RC.
    if (!rank) {
        work_critical();
    } else {
        work_local();
    }

    // Finaliza
    MPI_Finalize();
    return 0;
}


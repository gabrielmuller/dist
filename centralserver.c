#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "queue.c"

#define REQUEST_ACCESS 1 
#define RETURN_TOKEN 2 

int proc_n;
int rank;

/*
 * Trabalha numa região crítica.
 * Ex: escrever num buffer de saída
 */
void work_critical (int val) {
    usleep(1000000);
    printf("Trabalhando em região crítica proc %d: %d\n", rank, val);
    fflush(stdout);
}

/*
 * Trabalha localmente.
 */
void work_local (int time) {
    sleep(time);
    #ifdef DEBUG
    printf("Processo %d trabalhando em região não-crítica...\n", rank);
    fflush(stdout);
    #endif
}

/*
 * Faz requisição de acesso em região crítica.
 */
void request_access () {
    #ifdef DEBUG
    printf("Worker %d requesting... \n", rank);
    fflush(stdout);
    #endif

    MPI_Send(NULL, 0, MPI_INT, 0, REQUEST_ACCESS, MPI_COMM_WORLD);
}

/*
 * Recebe o token do processo Líder.
 */
int recv_token () {
    int token;
    MPI_Recv(&token, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    #ifdef DEBUG
    printf("Worker %d received token... \n", rank);
    fflush(stdout);
    #endif

    return token;
}

/*
 * Devolve o token para o processo Líder.
 */
void yield_token (int token) {
    MPI_Send(&token, 1, MPI_INT, 0, RETURN_TOKEN, MPI_COMM_WORLD);

    #ifdef DEBUG
    printf("Worker %d returned token... \n", rank);
    fflush(stdout);
    #endif
}

/*
 * Processo Líder
 * Recebe pedidos de acesso, envia token e recebe token.
 */
void leader () {
    struct Queue* queue = createQueue(proc_n);

    int accessing = 0;
    int token = 0;
    int has_token = 1;

    MPI_Status status;
    MPI_Request req;
    int flag;

    while (1) 
    {
        // Receber request de qualquer processo (REQUEST_ACCESS)
        // ou receber token de volta (RETURN_TOKEN)
        MPI_Irecv(&token, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &req);
        MPI_Test(&req, &flag, &status);

        while(!flag) {
            MPI_Test(&req, &flag, &status);
        }

        if (flag && has_token && status.MPI_TAG == REQUEST_ACCESS && isEmpty(queue)) 
        {   
            // Recebeu request e possui o token e fila vazia
            // Enviar token para o processo
            MPI_Send(&token, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            
            accessing = status.MPI_SOURCE;
            has_token = 0;
            
            #ifdef DEBUG
            printf("Leader sent token to %d...\n", accessing);
            fflush(stdout);
            #endif
        } 
        else if (flag && !has_token && status.MPI_TAG == REQUEST_ACCESS)
        {
            // Recebeu request e não possui o token
            // Colocar na fila
            //push(queue, &tail, status.MPI_SOURCE);
            push(queue, status.MPI_SOURCE);

            #ifdef DEBUG
            printf("Leader put %d on queue...\n", status.MPI_SOURCE);
            fflush(stdout);
            #endif
        }
        else if (status.MPI_TAG == RETURN_TOKEN) 
        {   
            // Recebeu token de volta
            has_token = 1;

            #ifdef DEBUG
            printf("Leader received token from %d...\n", status.MPI_SOURCE);
            fflush(stdout);
            #endif

            // Enviar token para o primeiro processo da fila (se houver)
            if (!isEmpty(queue)) 
            {   
                accessing = pop(queue);
                MPI_Send(&token, 1, MPI_INT, accessing, 0, MPI_COMM_WORLD);

                #ifdef DEBUG
                printf("Leader sent token to %d (from queue)...\n", accessing);
                fflush(stdout);
                #endif

                has_token = 0;
            }
        }
    }
}

int main (int argc, char** argv) {
    srand(time(NULL));

    // Inicia MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Processo Líder/Coordenador
    if (rank == 0) {
        #ifdef DEBUG
        printf("Leader %d starting... \n", rank);
        fflush(stdout);
        #endif

        leader();
    } 

    #ifdef DEBUG
    printf("Worker %d starting... \n", rank);
    fflush(stdout);
    #endif

    for(;;) {
        if (rank != 0) {
            work_local(rand() % 10);
            request_access();
            int token = recv_token();
            work_critical(token);
            yield_token(++token); 
        }
    }

    // Finaliza
    MPI_Finalize();
    return 0;
}
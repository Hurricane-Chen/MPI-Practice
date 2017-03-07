#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argv, char* argc[]){
    int rank, tot, i;
    char msg[50], rev[50];
    MPI_Status state;
    MPI_Init(&argv, &argc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tot);
    if (rank == 0){
        for(i = 1; i < tot; i++){
            sprintf(msg,"rank %d sent, I'm master process.", i);
            MPI_Send(msg, 50, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
        for(i = 1; i < tot; i++){
            MPI_Recv(rev, 50, MPI_CHAR, i, 0, MPI_COMM_WORLD, &state);
            printf("P%d got: %s\n", rank, rev);
        }
    }else{
        MPI_Recv(rev, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &state);
        printf("P%d got: %s\n", rank, rev);
        sprintf(msg, "hello, master process, this is rank %d.", rank);
        MPI_Send(msg, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}

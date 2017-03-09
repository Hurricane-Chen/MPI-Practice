#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define  ARRAYSIZE	16000000
#define  MASTER		0

float  data[ARRAYSIZE];

int main (int argc, char *argv[])
{
int   numtasks, taskid, rc, dest, offset, i, j, tag1,
      tag2, source, chunksize; 
float mysum, sum;
float update(int myoffset, int chunk, int myid);
MPI_Status status;

/***** Initializations *****/
MPI_Init(&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
if (numtasks % 4 != 0) {
   printf("Quitting. Number of MPI tasks must be divisible by 4.\n");
   MPI_Abort(MPI_COMM_WORLD, rc);
   exit(0);
   }
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
printf ("MPI task %d has started...\n", taskid);
chunksize = (ARRAYSIZE / numtasks);
tag2 = 1;
tag1 = 2;

if (taskid == MASTER){

  /* 初始化数组 */
  sum = 0;
  for(i=0; i<ARRAYSIZE; i++) {
    data[i] =  i * 1.0;
    sum = sum + data[i];
    }
  printf("Initialized array sum = %e\n",sum);

  /* 向线程发送数据 */
  offset = chunksize;
  for (dest=1; dest<numtasks; dest++) {
    MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
    MPI_Send(&data[offset], chunksize, MPI_FLOAT, dest, tag2, MPI_COMM_WORLD);
    printf("Sent %d elements to task %d offset= %d\n",chunksize,dest,offset);
    offset = offset + chunksize;
    }

  /* 主线程工作 */
  offset = 0;
  mysum = update(offset, chunksize, taskid);

  /* 接收数据 */
  for (i=1; i<numtasks; i++) {
    source = i;
    MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
    MPI_Recv(&data[offset], chunksize, MPI_FLOAT, source, tag2,
      MPI_COMM_WORLD, &status);
    }

  /* 进行计算 */  
  MPI_Reduce(&mysum, &sum, 1, MPI_FLOAT, MPI_SUM, MASTER, MPI_COMM_WORLD);
  printf("Sample results: \n");
  offset = 0;
  for (i=0; i<numtasks; i++) {
    for (j=0; j<5; j++) 
      printf("  %e",data[offset+j]);
    printf("\n");
    offset = offset + chunksize;
    }
  printf("*** Final sum= %e ***\n",sum);

  }  


 /* 其它线程 */
if (taskid > MASTER) {

  source = MASTER;
  MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
  MPI_Recv(&data[offset], chunksize, MPI_FLOAT, source, tag2, 
    MPI_COMM_WORLD, &status);

  mysum = update(offset, chunksize, taskid);

 
  dest = MASTER;
  MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
  MPI_Send(&data[offset], chunksize, MPI_FLOAT, MASTER, tag2, MPI_COMM_WORLD);

  MPI_Reduce(&mysum, &sum, 1, MPI_FLOAT, MPI_SUM, MASTER, MPI_COMM_WORLD);

  } 


MPI_Finalize();

}   


float update(int myoffset, int chunk, int myid) {
  int i; 
  float mysum;
  /* 保存数据 */
  mysum = 0;
  for(i=myoffset; i < myoffset + chunk; i++) {
    data[i] = data[i] + i * 1.0;
    mysum = mysum + data[i];
    }
  printf("Task %d mysum = %e\n",myid,mysum);
  return(mysum);
}

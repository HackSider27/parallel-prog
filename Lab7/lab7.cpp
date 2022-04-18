#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <fstream>
#include <omp.h>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char** argv)
{
  int ret = -1; ///< For return values
  int size = -1; ///< Total number of processors
  int rank = -1; ///< This processor's number
  const int random_seed = time(NULL); ///< RNG seed

  std::ofstream file("out.txt");

  int N[2]={(int)4e6, (int)1e8};
  vector<int>numbers;

  int* array=0; ///< The array we need to find the max in
  int* pnumbers=0;
  double startwtime;
  double endwtime;

  /* Initialize the MPI */
  ret = MPI_Init(&argc, &argv);
  if (!rank) { printf("MPI Init returned (%d);\n", ret); }

  /* Determine our rankand processor count */
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  printf("MPI Comm Size: %d;\n", size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("MPI Comm Rank: %d;\n", rank);

  startwtime = MPI_Wtime();

  array=(int*)malloc(2*sizeof(int));
  MPI_Bcast ((&N), 2, MPI_INT, 0, MPI_COMM_WORLD);

  if(!rank){
    int length = N[1]-N[0] +1;
    int amount = length % size;
    array[0] = N[0];
    array[1] = amount ? length/size + 1 : length/size;
    amount--;

    for(int i=1; i<size;i++){
      if (amount>0){
        array[0]+= array[1];
        array[1]= length/size + 1;
        amount--;

        MPI_Send(array, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
      else{
        array[0]+= array[1];
        array[1]= length/size;
        MPI_Send(array, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
    }
    amount = length % size;
    array[0] = N[0];
    array[1] = amount ? length/size + 1 : length/size;
  }
  else
    MPI_Recv(array, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  pnumbers=(int*)malloc(array[1]*sizeof(int));
  int count=0;

  #pragma omp parallel for num_threads(1) shared(array, pnumbers, count) default(none)
    for(int i=0; i<array[1];i++){
      pnumbers[i]=0;
      int sq=sqrt(array[0]+i);
      int flag=0;
      for(int j=2; j<=sq;j++)   // check for simple numbers
        if((array[0]+i)%j==0){
          flag=1;
          break;
        }
      if(!flag){
        pnumbers[count]=array[0]+i;
        count++;
      }
    }

  if(!rank){
    printf("Hello world\n");
    for(int i=0; i<count;i++)
      numbers.push_back(pnumbers[i]);
    for(int i=1; i<size;i++){
      MPI_Status status;
      int k1=0;
      int* y=0;
      MPI_Recv (&k1, 1, MPI_INTEGER,MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
      y=(int*)malloc(k1*sizeof(int));
      printf("\nSource: %d", status.MPI_SOURCE);
      MPI_Recv(y, k1, MPI_INTEGER,status.MPI_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for(int j=0;j<k1&&y[j]!=0;j++)
      numbers.push_back(y[j]);
      free(y);
    }
  }
  else {
    MPI_Send(&array[1], 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD);
    MPI_Send(pnumbers, array[1], MPI_INTEGER, 0, 0, MPI_COMM_WORLD);
  }

  endwtime = MPI_Wtime();
  double timer = endwtime-startwtime;
  printf("\nTime: %f %d\n", timer, rank);
  MPI_Barrier(MPI_COMM_WORLD);

  if (!rank){
    timer = (endwtime-startwtime);
    printf("\nTime: %f\n", timer);
    file << timer<<endl<<0<<endl;
    for(int i=1; i<4;i++){
      MPI_Status status;
      MPI_Recv(&timer, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      file << timer<<endl<<i<<endl;
    }
  }
  else
    MPI_Send(&timer, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

  ret = MPI_Finalize();

  free(array);
  file.close();
  printf("MPI Finalize returned (%d);\n", ret);

  return(0);
}

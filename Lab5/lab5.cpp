#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <fstream>

int main(int argc, char** argv)

{
  int ret = -1; ///< For return values
  int size = -1; ///< Total number of processors
  int rank = -1; ///< This processor's number

  const int count = 1e7; ///< Number of array elements
  const int random_seed = time(NULL); ///< RNG seed

  std::ofstream file("data.txt", std::ios::app);

  int* array = 0; ///< The array we need to find the max in
  int lmax = -1; ///< Local maximums
  int max = -1; ///< The maximal element
  double start_time = 0.0;
  double end_time;

  /* Initialize the MPI */
  ret = MPI_Init(&argc, &argv);
  if (!rank) { printf("MPI Init returned (%d);\n", ret); }

  /* Determine our rankand processor count */
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  printf("MPI Comm Size: %d;\n", size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("MPI Comm Rank: %d;\n", rank);

  /* Allocate the array */
  array = (int*)malloc(count * sizeof(int));

  /* Master generates the array */
  if (!rank) {
    /* Initialize the RNG */
    srand(random_seed);
    /* Generate the random array */
    for (int i = 0; i < count; i++) { array[i] = rand(); }

  }

  /* Send the array to all other processors */
  MPI_Bcast(array, count, MPI_INTEGER, 0, MPI_COMM_WORLD);

  if(!rank)
    start_time = MPI_Wtime();
  const int wstart = (rank ) * count / size;
  const int wend = (rank + 1) * count / size;

  for (int i = wstart;  i < wend;  i++)
    if (array[i] > lmax) { lmax = array[i]; }

  MPI_Reduce(&lmax, &max, 1, MPI_INTEGER, MPI_MAX, 0, MPI_COMM_WORLD);

  if(!rank)
    end_time = MPI_Wtime();
  ret = MPI_Finalize();

  if (!rank) {
  double time = end_time - start_time;
  printf("\n*** Global Maximum is %d;\nTime: %f\n\n", max, time);
  file << time << "\n";
  }

  file.close();
  free(array);
  printf("MPI Finalize returned (%d);\n", ret);

  return(0);

}

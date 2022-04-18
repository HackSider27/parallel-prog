#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <fstream>

void insertionsort(int a[], int n, int stride) {
  for (int j=stride; j<n; j+=stride) {
      int key = a[j];
      int i = j - stride;
      while (i >= 0 && a[i] > key) {
        a[i+stride] = a[i];
        i-=stride;
      }
    a[i+stride] = key;
    }
}

int main(int argc, char** argv)
{
    int count = 10000;     ///< Number of array elements
    const int random_seed = 924114; ///< RNG seed
    int m, y;

    std::ofstream file("data.txt", std::ios::app);
    int* array = 0;                 ///< The array we need to sort the value in
    double  start = 0, end = 0, average;
    int amount = 20;                // Number of arrays

    /* Initialize the MPI */
    ret = MPI_Init(&argc, &argv);
    if (!rank) { printf("MPI Init returned (%d);\n", ret); }

    /* Determine our rankand processor count */
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    //printf("MPI Comm Size: %d;\n", size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //rintf("MPI Comm Rank: %d;\n", rank);

    /* Generate the random array */
    array = (int*)malloc(count*sizeof(int));

    /* Initialize the RNG */
    srand(random_seed);

	for (int threads = 1; threads <= 16; threads++) {
    omp_set_num_threads(threads);
    printf("%d\n", threads);

		srand(random_seed + threads);

		average = 0;

		for (int j = 0; j < amount; j++) {

			for (int i = 0; i < count; i++) { array[i] = rand() % (count * 100); }

			start = omp_get_wtime();
                /* Sort array by shell_sort */
                for(int s=count/(2); s>0; s/=(2)){
                #pragma omp parallel for shared(array, count, s) default(none)
                for(int i = 0; i < s; i++)
                insertionsort(&(array[i]), count-i, s);
                }

                end = omp_get_wtime();
                average+=end-start;
    }


    average = average / amount;
		printf("Time:%f\n", average);
		//average = average / amount;
		fprintf(stream, "%d\n%f\n", threads, average*100);

  /*  for(int i = 0; i < count; i++){
      printf("%d ", array[i]);
    }*/

	}
    fclose(stream);
    return(0);
}

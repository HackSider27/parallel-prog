#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

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
    //int target = 16;          ///< Number to look for

    FILE* stream;
    stream = fopen("data.txt", "w+");

    int* array = 0;                 ///< The array we need to find the value in
    double  start = 0, end = 0, average;
    int amount = 20;                // Number of arrays

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

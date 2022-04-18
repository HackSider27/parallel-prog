#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char** argv)
{
    FILE *stream;
    stream = fopen("data","w+");
    int count = 10000000;     ///< Number of array elements
    int threads = 16;         ///< Number of parallel threads to use
    const int random_seed = 920215; ///< RNG seed
    double start_time, end_time, average;
    int* array = 0;                 ///< The array we need to find the max in
    int  max = -1;                ///< The maximal element
    int  sum=0;
    /* Initialize the RNG */
//    srand(random_seed);

    /* Determine the OpenMP support */
    printf("OpenMP: %d;\n======\n", _OPENMP);

    /* Generate the random array */
    array = (int*)malloc(count*sizeof(int));
    for(threads=1; threads<=32; threads++)
    {
	    average=0;
	    for(int j=0; j<40; j++)
	    {
	    srand(random_seed-j);
	    max=-1;
	    for(int i=0; i<count; i++) { array[i] = rand(); }

	    start_time = omp_get_wtime();
	    /* Find the maximal element */
	    #pragma omp parallel num_threads(threads) shared(array, count) reduction(max: max) firstprivate(sum) default(none)
	    {
	        #pragma omp for
	        for(int i=0; i<count; i++)
	        {
		  //  sum++;
	            if(array[i] > max) { max = array[i]; };
	        }
	        //printf("-- My lmax and amount cmp is: %d and %d;\n", max, sum);
    }
    end_time= omp_get_wtime();
    average+=end_time-start_time;
    printf("======\nMax is: %d %f;\n", max, end_time-start_time);
    }

    average= average/40;
    fprintf(stream,"%d\n%f\n",threads,average);
    }

    fclose(stream);
    return(0);
}

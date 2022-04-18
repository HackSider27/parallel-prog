#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char** argv)
{
    int count = 1000000;     ///< Number of array elements
    const int random_seed = 924114; ///< RNG seed
    //int target = 16;          ///< Number to look for

    FILE* stream;
    stream = fopen("data.txt", "w+");

    int* array = 0;                 ///< The array we need to find the value in
    int  index = -1;                ///< The index of the element we need
    double  start, end, average;
    int amount = 250;
    int summ=0;

    /* Generate the random array */
    array = (int*)malloc(count*sizeof(int));

    int flag;

    /* Initialize the RNG */
    srand(random_seed);

    int target=rand()%(count*10);

	for (int threads = 1; threads <= 16; threads++) {
    printf("%d\n", threads);

		srand(random_seed + threads);

		average = 0;

		index = count + 1;

		/* Generate the random array */
		// array = (int*)malloc(count*sizeof(int));

		for (int j = 0; j < amount; j++) {

			for (int i = 0; i < count; i++) { array[i] = rand() % (count * 10); }

			flag = 0;
			start = omp_get_wtime();

#pragma omp parallel num_threads(threads) shared(stdin,array,flag,count, summ, target, index, threads)  default(shared)
			{
				summ = 0;
				flag = 0;
				/* Find the index of the element */
#pragma omp for
				for (int k = 0; k < threads; k++) {
					//if(flag) break;
					//#pragma omp for
					for (int i = 0; i < count / threads; i++)
					{
						if (flag == 1) break;
						if (array[k * count / threads + i] == target)
						{
							index = k * count / threads + i;
							flag = 1;
						}
						//printf("Number:%d\n", array[k*count/threads+i]);
					}
				}

#pragma omp single
				{
					if (flag != 1)
						for (int i = 0; i < count % threads; i++) {
							if (array[count / threads + i] == target)
							{
								index = threads * (count / threads) + i;
								flag = 1;
							}
							//printf("Number: %d\n",array[threads*(count/threads)+i]);
						}
				}
			}
		}
		end = omp_get_wtime();
		average += end - start;
		printf("#####################################################\nIndex: %d\n", index);
		printf("Time:%f\n", average / amount);
		average = average / amount;
		fprintf(stream, "%d\n%f\n", threads, average*1000);
	}
    fclose(stream);
    return(0);
}

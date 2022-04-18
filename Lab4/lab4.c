#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char** argv)
{
  FILE *stream1;
  FILE *stream2;
  FILE *stream3;
  FILE *stream4;
  FILE *stream5;
  FILE *stream6;
  FILE *stream7;
  FILE *stream8;

  stream1 = fopen("static_chunk.txt","w+");
  stream2 = fopen("static.txt","w+");
  stream3 = fopen("dynamic_chunk.txt","w+");
  stream4 = fopen("dynamic.txt","w+");
  stream5 = fopen("guided_chunk.txt","w+");
  stream6 = fopen("guided.txt","w+");
  stream7 = fopen("auto.txt","w+");
  stream8 = fopen("default.txt","w+");

  int count = 1000000; ///< Number of array elements
  int threads = 16; ///< Number of parallel threads to use
  const int random_seed = 920215; ///< RNG seed
  double start_time1, end_time1, average1;
  double start_time2, end_time2, average2;
  double start_time3, end_time3, average3;
  double start_time4, end_time4, average4;
  double start_time5, end_time5, average5;
  double start_time6, end_time6, average6;
  double start_time7, end_time7, average7;
  double start_time8, end_time8, average8;
  int* array = 0; ///< The array we need to find the max in
  int max = -1; ///< The maximal element

//omp_set_dynamic(1);

  /* Determine the OpenMP support */
  printf("OpenMP: %d;\n======\n", _OPENMP); //get standart
  printf("Max Procs: %d\n",omp_get_num_procs());
  printf("Max Threads: %d\n=======\n",omp_get_max_threads());
  printf("OMP_DYNAMIC: %d\n=======\n", omp_get_dynamic());
  printf("OMP_WTICK: %.16g second\n=======\n",omp_get_wtick());
  printf("OMP_GET_NESTED: %d\n",omp_get_nested());
  printf("OMP_GET_MAX_ACTIVE_LEVELS: %d\n=======\n",omp_get_max_active_levels());

  omp_sched_t kind;
  int chunk_size;

  //omp_get_schedule(&kind, &chunk_size);

  printf("In case a runtime schedule is encountered, the ");

  switch(kind)
  {
  case omp_sched_static:
    printf("static");
    break;

  case omp_sched_dynamic:
    printf("dynamic");
    break;

  case omp_sched_guided:
    printf("guided");
    break;

  case omp_sched_auto:
    printf("auto");
    break;

  default:
    printf("other (implementation specific)");
    break;
  }

  printf(" schedule is applied, with chunks made of %d iteration%s.\n", chunk_size, (chunk_size > 1) ? "s" : "");

  /* Generate the random array */

  array = (int*)malloc(count*sizeof(int));

      /*Work with lock*/
  int array_average = 0;
  omp_lock_t lock;
  omp_init_lock(&lock);
  #pragma omp parallel num_threads(omp_get_max_threads())
  {
    #pragma omp parallel for shared(array_average)
      for (int i = 0; i < count; i++ )
      {
         array_average += array[i];
      }
    omp_set_lock(&lock);
    array_average /= count;
    omp_unset_lock(&lock);
  }
  omp_destroy_lock(&lock);
    /*End of working with lock*/

    /* A big cycle of testing threads*/
  for(threads=1; threads<=16; threads++){

    //omp_set_num_threads(threads);

    //average=0;
      /* The number of array*/
    for(int j=0; j<25; j++)
    {
      srand(random_seed+j);
      max=-1;

      for(int i=0; i<count; i++) { array[i] = rand(); }

        /* */
      start_time1 = omp_get_wtime();
      #pragma omp parallel num_threads(j) shared(array, j, count) reduction(max: max) default(none)
      {
        #pragma omp for schedule(static)//it's equavalent of (static, SIZE/j)
          for(int i=0; i<count; i++)
            if(array[i] > max) { max = array[i]; };
      }
      end_time1 = omp_get_wtime();
      max = -1;
      average1+=end_time1-start_time1;
      //fprintf(stream1, "Time: %lf\n",  end_time - start_time);


      start_time2 = omp_get_wtime();
      #pragma omp parallel num_threads(j) shared(array, j, count) reduction(max: max) default(none)
      {
        #pragma omp for schedule(static, 1000)
          for(int i=0; i<count; i++)
            if(array[i] > max) { max = array[i]; };
      }
      end_time2 = omp_get_wtime();
      max = -1;
      average2+=end_time2-start_time2;
      //fprintf(stream2, "Time: %lf\n", end_time - start_time);


      start_time3 = omp_get_wtime();
      #pragma omp parallel num_threads(j) shared(array, j, count) reduction(max: max) default(none)
      {
        #pragma omp for schedule(dynamic)//equavalent of (dynamic, 1)
          for(int i=0; i<count; i++)
          if(array[i] > max) { max = array[i]; };
      }
      end_time3 = omp_get_wtime();
      max = -1;
      average3+=end_time3-start_time3;
      //fprintf(stream3, "Time: %lf\n", end_time - start_time);


      start_time4 = omp_get_wtime();
      #pragma omp parallel num_threads(j) shared(array, j, count) reduction(max: max) default(none)
      {
        #pragma omp for schedule(dynamic, 1000)
          for(int i=0; i<count; i++)
            if(array[i] > max) { max = array[i]; };
      }
      end_time4 = omp_get_wtime();
      max = -1;
      average4+=end_time4-start_time4;
      //fprintf(stream4, "Time: %lf\n", end_time - start_time);


      start_time5 = omp_get_wtime();
      #pragma omp parallel num_threads(j) shared(array, j, count) reduction(max: max) default(none)
      {
        #pragma omp for schedule(guided)
          for(int i=0; i<count; i++)
            if(array[i] > max) { max = array[i]; };
      }
      end_time5 = omp_get_wtime();
      max = -1;
      average5+=end_time5-start_time5;
      //fprintf(stream5, "Time: %lf\n", end_time - start_time);


      start_time6 = omp_get_wtime();
      #pragma omp parallel num_threads(j) shared(array, j, count) reduction(max: max) default(none)
      {
        #pragma omp for schedule(guided, 1000)
          for(int i=0; i<count; i++)
            if(array[i] > max) { max = array[i]; };
      }
      end_time6 = omp_get_wtime();
      max = -1;
      average6+=end_time6-start_time6;
      //fprintf(stream6, "Time: %lf\n", end_time - start_time);


      start_time7 = omp_get_wtime();
      #pragma omp parallel num_threads(j) shared(array, j, count) reduction(max: max) default(none)
      {
        #pragma omp for schedule(auto)
          for(int i=0; i<count; i++)
            if(array[i] > max) { max = array[i]; };
      }
      end_time7 = omp_get_wtime();
      average7+=end_time7-start_time7;
      //fprintf(stream7, "Time: %lf\n", end_time - start_time);
      max = -1;

      start_time8 = omp_get_wtime();
      #pragma omp parallel num_threads(j) shared(array, j, count) reduction(max: max) default(none)
      {
        #pragma omp for //default
          for(int i=0; i<count; i++)
            if(array[i] > max) { max = array[i]; };
      }
      end_time8 = omp_get_wtime();
      average8+=end_time8-start_time8;
      //fprintf(stream8, "Time: %lf\n", end_time - start_time);
    }

    average1 = average1/25;
    average2 = average2/25;
    average3 = average3/25;
    average4 = average4/25;
    average5 = average5/25;
    average6 = average6/25;
    average7 = average7/25;
    average8 = average8/25;

    fprintf(stream1,"%d\n%f\n",threads,average1);
    fprintf(stream2,"%d\n%f\n",threads,average2);
    fprintf(stream3,"%d\n%f\n",threads,average3);
    fprintf(stream4,"%d\n%f\n",threads,average4);
    fprintf(stream5,"%d\n%f\n",threads,average5);
    fprintf(stream6,"%d\n%f\n",threads,average6);
    fprintf(stream7,"%d\n%f\n",threads,average7);
    fprintf(stream8,"%d\n%f\n",threads,average8);

    //printf("%d\n%f\n", threads, average);
  }

  fclose(stream1);
  fclose(stream2);
  fclose(stream3);
  fclose(stream4);
  fclose(stream5);
  fclose(stream6);
  fclose(stream7);
  fclose(stream8);

  return(0);

}

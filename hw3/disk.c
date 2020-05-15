
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

// Operation hnadler to keep track of various parameters for each experiment.
typedef struct
{
  int b_size;	//Block Size
  pthread_mutex_t * mutex;
  int fh;	//File Handler
  int no_of_threads;
}
op_handle;

//This is used to read the file in sequential order
//An operation handler is passed as an input argument
void *read_Seq(void *arg)
{

  op_handle *hndl = (op_handle*) arg;
  pthread_mutex_lock(hndl->mutex);

  long no_of_blocks = 10737418240 / (hndl->b_size *hndl->no_of_threads);

  char *read_c = (char*) aligned_alloc(4096, hndl->b_size* sizeof(char));

  lseek(hndl->fh, 0, SEEK_SET);

  for (long i = 0; i < no_of_blocks; i++)
  {
    read(hndl->fh, read_c, hndl->b_size* sizeof(char));
  }

  pthread_mutex_unlock(hndl->mutex);
  pthread_exit(NULL);
  free(read_c);
}

//This is used to write the file in sequential order
//An operation handler is passed as an input argument
void *write_Seq(void *arg)
{
  op_handle *hndl = (op_handle*) arg;
  pthread_mutex_lock(hndl->mutex);

  long no_of_blocks = 10737418240 / (hndl->b_size *hndl->no_of_threads);

  char *read_c = (char*) aligned_alloc(4096, hndl->b_size* sizeof(char));
  lseek(hndl->fh, 0, SEEK_SET);
  memset(read_c, 'Z', hndl->b_size* sizeof(char));
  for (long i = 0; i < no_of_blocks; i++)
  {
    write(hndl->fh, read_c, hndl->b_size* sizeof(char));
  }
  fsync(hndl->fh);
  pthread_mutex_unlock(hndl->mutex);
  pthread_exit(NULL);
  free(read_c);
}

//This is used to read the file in random order
//An operation handler is passed as an input argument
void *read_Rand(void *arg)
{
  op_handle *hndl = (op_handle*) arg;
  pthread_mutex_lock(hndl->mutex);

  long no_of_blocks = 10737418240 / (hndl->b_size *hndl->no_of_threads);

  char *read_c = (char*) aligned_alloc(4096, hndl->b_size* sizeof(char));

  for (long i = 0; i < no_of_blocks; i++)
  {
    lseek(hndl->fh, (rand() % no_of_blocks - 1) *hndl->b_size, SEEK_SET);
    read(hndl->fh, read_c, hndl->b_size* sizeof(char));
  }

  pthread_mutex_unlock(hndl->mutex);
  pthread_exit(NULL);
  free(read_c);
}

//This is used to write the file in random order
//An operation handler is passed as an input argument
void *write_Rand(void *arg)
{
  op_handle *hndl = (op_handle*) arg;
  pthread_mutex_lock(hndl->mutex);

  long no_of_blocks = 10737418240 / (hndl->b_size *hndl->no_of_threads);

  char *read_c = (char*) aligned_alloc(4096, hndl->b_size* sizeof(char));
  lseek(hndl->fh, 0, SEEK_SET);
  memset(read_c, 'Z', hndl->b_size* sizeof(char));
  for (long i = 0; i < no_of_blocks; i++)
  {
    lseek(hndl->fh, (rand() % no_of_blocks - 1) *hndl->b_size, SEEK_SET);
    write(hndl->fh, read_c, hndl->b_size* sizeof(char));
  }
  fsync(hndl->fh);
  pthread_mutex_unlock(hndl->mutex);
  pthread_exit(NULL);
  free(read_c);

}

int main(int argc, char **argv)
{

  int b_size[] = { 
    65536,	//64 KB
    1048576,	//1 MB
    16777216	//16 MB
  };
  int thread_count[] = { 
    1,
    2,
    4,
    8,
    12,
    24,
    48
  };
  int op_type[] = {
    1,	//1 - Read Sequential
    2,	//2 - Write Sequential
    3,	//3 - Read Random
    4	//4 - Write Random
  };

  struct timeval start, end;	//Variable to store the time information to measure the execution time
  char *c2 = (char*) malloc(1* sizeof(int));
  char *c1 = "./Data/";
  char *c3 = (char*) malloc(strlen(c1) + sizeof(int) + 1);

  double time_taken;
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      for (int k = 0; k < 7; k++)
      {

        pthread_t *threads = (pthread_t*) malloc(thread_count[k] *sizeof(pthread_t));
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

        sprintf(c2, "%d", thread_count[k]);
        strcpy(c3, c1);
        strcat(c3, c2);
        strcat(c3, "/");

       	//Open the directory containing the files
        DIR *dr = opendir(c3);

       	//variable to hold the file information from the directory
        struct dirent * in_file;

        op_handle *hndl = (op_handle*) malloc(thread_count[k] *sizeof(op_handle));

        int ind = 0;

       	//Open the files in the directory one by one and store their handler in the operation handler
        while ((in_file = readdir(dr)))
        {
          if (in_file->d_type != DT_REG)
          {
            continue;
          }

          char *fname = (char*) malloc(strlen(c3) + strlen(in_file->d_name) + 1);

          strcpy(fname, c3);
          strcat(fname, in_file->d_name);

          hndl[ind++].fh = open(fname, O_RDWR | O_DIRECT);
        }
        switch (op_type[i])
        {
         	//Sequential Read
          case 1:
            gettimeofday(&start, NULL);
            pthread_mutex_init(&mutex, NULL);

            for (int a = 0; a < thread_count[k]; a++)
            {
              hndl[a].b_size = b_size[j];
              hndl[a].mutex = &mutex;
              hndl[a].no_of_threads = thread_count[k];
              pthread_create(&threads[a], NULL, read_Seq, (void*) &hndl[a]);
            }
            for (int a = 0; a < thread_count[k]; a++)
            {
              pthread_join(threads[a], NULL);
            }
            pthread_mutex_destroy(&mutex);
            gettimeofday(&end, NULL);

            time_taken = (end.tv_sec - start.tv_sec) *1e6;
            time_taken = (time_taken + (end.tv_usec - start.tv_usec)) *1e-6;
            printf("RS: Threads=%d, Block_Size(in Bytes)=%d, Time=%f\n", thread_count[k], b_size[j], time_taken);
            break;
           	//Sequential Write
          case 2:
            gettimeofday(&start, NULL);
            pthread_mutex_init(&mutex, NULL);
            for (int a = 0; a < thread_count[k]; a++)
            {
              hndl[a].b_size = b_size[j];
              hndl[a].mutex = &mutex;
              hndl[a].no_of_threads = thread_count[k];
              pthread_create(&threads[a], NULL, write_Seq, (void*) &hndl[a]);
            }
            for (int a = 0; a < thread_count[k]; a++)
            {
              pthread_join(threads[a], NULL);
            }
            pthread_mutex_destroy(&mutex);
            gettimeofday(&end, NULL);

            time_taken = (end.tv_sec - start.tv_sec) *1e6;
            time_taken = (time_taken + (end.tv_usec - start.tv_usec)) *1e-6;
            printf("WS: Threads=%d, Block_Size(in Bytes)=%d, Time=%f\n", thread_count[k], b_size[j], time_taken);
            break;
           	//Random Read
          case 3:
            gettimeofday(&start, NULL);
            pthread_mutex_init(&mutex, NULL);
            for (int a = 0; a < thread_count[k]; a++)
            {
              hndl[a].b_size = b_size[j];
              hndl[a].mutex = &mutex;
              hndl[a].no_of_threads = thread_count[k];
              pthread_create(&threads[a], NULL, read_Rand, (void*) &hndl[a]);
            }
            for (int a = 0; a < thread_count[k]; a++)
            {
              pthread_join(threads[a], NULL);
            }
            pthread_mutex_destroy(&mutex);
            gettimeofday(&end, NULL);

            time_taken = (end.tv_sec - start.tv_sec) *1e6;
            time_taken = (time_taken + (end.tv_usec - start.tv_usec)) *1e-6;
            printf("RR: Threads=%d, Block_Size(in Bytes)=%d, Time=%f\n", thread_count[k], b_size[j], time_taken);
            break;
           	//Random Write
          case 4:
            gettimeofday(&start, NULL);
            pthread_mutex_init(&mutex, NULL);
            for (int a = 0; a < thread_count[k]; a++)
            {
              hndl[a].b_size = b_size[j];
              hndl[a].mutex = &mutex;
              hndl[a].no_of_threads = thread_count[k];
              pthread_create(&threads[a], NULL, write_Rand, (void*) &hndl[a]);
            }
            for (int a = 0; a < thread_count[k]; a++)
            {
              pthread_join(threads[a], NULL);
            }
            pthread_mutex_destroy(&mutex);
            gettimeofday(&end, NULL);

            time_taken = (end.tv_sec - start.tv_sec) *1e6;
            time_taken = (time_taken + (end.tv_usec - start.tv_usec)) *1e-6;
            printf("WR: Threads=%d, Block_Size(in Bytes)=%d, Time=%f\n", thread_count[k], b_size[j], time_taken);
            break;
        }
        while (ind >= 0)
        {
          close(hndl[ind--].fh);
        }
        free(threads);
        free(hndl);
        free(in_file);
        closedir(dr);
      }
    }
  }


//These loops are used to calculate the Random Read &Random Write for 4KB blocks
  for (int i = 0; i < 2; i++)
  {
    for (int k = 0; k < 7; k++)
    {

      pthread_t *threads = (pthread_t*) malloc(thread_count[k] *sizeof(pthread_t));
      pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

      sprintf(c2, "%d", thread_count[k]);
      strcpy(c3, c1);
      strcat(c3, c2);
      strcat(c3, "/");

     	//Open the directory containing the files
      DIR *dr = opendir(c3);

     	//variable to hold the file information from the directory
      struct dirent * in_file;

      op_handle *hndl = (op_handle*) malloc(thread_count[k] *sizeof(op_handle));

      int ind = 0;

     	//Open the files in the directory one by one and store their handler in the operation handler
      while ((in_file = readdir(dr)))
      {
        if (in_file->d_type != DT_REG)
        {
          continue;
        }

        char *fname = (char*) malloc(strlen(c3) + strlen(in_file->d_name) + 1);

        strcpy(fname, c3);
        strcat(fname, in_file->d_name);

        hndl[ind++].fh = open(fname, O_RDWR | O_DIRECT);
      }
      switch (op_type[i])
      {
       	//Random Read
        case 1:
          gettimeofday(&start, NULL);
          pthread_mutex_init(&mutex, NULL);
          for (int a = 0; a < thread_count[k]; a++)
          {
            hndl[a].b_size = 4096;
            hndl[a].mutex = &mutex;
            hndl[a].no_of_threads = thread_count[k];
            pthread_create(&threads[a], NULL, read_Rand, (void*) &hndl[a]);
          }
          for (int a = 0; a < thread_count[k]; a++)
          {
            pthread_join(threads[a], NULL);
          }
          pthread_mutex_destroy(&mutex);
          gettimeofday(&end, NULL);

          time_taken = (end.tv_sec - start.tv_sec) *1e6;
          time_taken = (time_taken + (end.tv_usec - start.tv_usec)) *1e-6;
          printf("RR: Threads=%d, Block_Size(in Bytes)=4096, Time=%f\n", thread_count[k], time_taken);
          break;
         	//Random Write
        case 2:
          gettimeofday(&start, NULL);
          pthread_mutex_init(&mutex, NULL);
          for (int a = 0; a < thread_count[k]; a++)
          {
            hndl[a].b_size = 4096;
            hndl[a].mutex = &mutex;
            hndl[a].no_of_threads = thread_count[k];
            pthread_create(&threads[a], NULL, write_Rand, (void*) &hndl[a]);
          }
          for (int a = 0; a < thread_count[k]; a++)
          {
            pthread_join(threads[a], NULL);
          }
          pthread_mutex_destroy(&mutex);
          gettimeofday(&end, NULL);

          time_taken = (end.tv_sec - start.tv_sec) *1e6;
          time_taken = (time_taken + (end.tv_usec - start.tv_usec)) *1e-6;
          printf("WR: Threads=%d, Block_Size(in Bytes)=4096, Time=%f\n", thread_count[k], time_taken);
          break;
      }
      while (ind >= 0)
      {
        close(hndl[ind--].fh);
      }
      free(threads);
      free(hndl);
      free(in_file);
      closedir(dr);
    }
  }

  return 0;
}

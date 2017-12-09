#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
long g_count = 0;

#define TIME 1000000


#ifdef USE_SPINLOCK
pthread_spinlock_t spinlock;
#else
pthread_mutex_t g_mutex;
#endif

void *thread_func(void *arg)
{
  long i, count = (long)arg;

  	/*
  	 * Increase the global variable, g_count.
  	 * This code should be protected by
  	 * some locks, e.g. spin lock, and the lock that
  	 * you implemented for assignment,
  	 * because g_count is shared by other threads.
  	 */
 
#ifdef USE_SPINLOCK
pthread_spin_lock(&spinlock);
#else
pthread_mutex_lock(&g_mutex);
#endif

  for (i = 0; i<count; i++){
    /************Critical Section**************/
    g_count++;
    /******************************************/
  }

  #ifdef USE_SPINLOCK
  pthread_spin_unlock(&spinlock);
  #else
  pthread_mutex_unlock(&g_mutex);
  #endif



}
int main(int argc, char *argv[])
{
  //struct timeval Pstart, Pend;
  struct timespec tstart={0,0}, tend={0,0};
  pthread_t *tid;
  long i, thread_count, value;
  uint64_t Pdiff =0;
  int rc;

  #ifdef USE_SPINLOCK
  pthread_spin_init(&spinlock, 0);
  #else
  pthread_mutex_init(&g_mutex,NULL);
  #endif
  /*
	 * Check that the program has three arguments
	 * If the number of arguments is not 3, terminate the process.
	 */
   if (3 != argc){
     fprintf(stderr, "usage: %s <thread count> <value>\n",argv[0]);
     exit(0);
   }

   /*
	 * Get the values of the each arguments as a long type.
	 * It is better that use long type instead of int type,
	 * because sizeof(long) is same with sizeof(void*).
	 * It will be better when we pass argument to thread
	 * that will be created by this thread.
	 */

   thread_count = atol(argv[1]);
   value = atol(argv[2]);

   /*
  * Create array to get tids of each threads that will
  * be created by this thread.
  */
  tid = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
  if(!tid){
    fprintf(stderr, "malloc() error\n");
    exit(0);
  }

  /*
	 * Create a threads by the thread_count value received as
	 * an argument. Each threads will increase g_count for
	 * value times.
	 */



  // gettimeofday(&Pstart,NULL);
	clock_gettime(CLOCK_MONOTONIC, &tstart);
//	some_long_computation();

   for(i=0; i<thread_count; i++){
     rc = pthread_create(&tid[i], NULL, thread_func, (void*)value);
     if(rc){
       fprintf(stderr, "pthread_create() error\n");
       free(tid);
	#ifdef USE_SPINLOCK
  	pthread_spin_destroy(&spinlock);
  	#else
  	pthread_mutex_destroy(&g_mutex);
  	#endif
       exit(0);
     }
  
   }




   /*
	 * Wait until all the threads you created above are finished.
	 */
   for(i=0; i<thread_count; i++){
     rc=pthread_join(tid[i], NULL);
     if(rc){
       fprintf(stderr, "pthread_join() error\n");
       free(tid);
	#ifdef USE_SPINLOCK
  	pthread_spin_destroy(&spinlock);
 	#else
 	pthread_mutex_destroy(&g_mutex);
  	#endif
 
       exit(0);
     }
   }
   //gettimeofday(&Pend,NULL);
	clock_gettime(CLOCK_MONOTONIC, &tend);
	

   /*
	 * Print the value of g_count.
	 * It must be (thread_count * value)
	 */
	printf("value: %ld\n", g_count);
  	//Pdiff = TIME*(Pend.tv_sec-Pstart.tv_sec)+Pend.tv_usec-Pstart.tv_usec;
  	//printf("**Process time = %llu.%llu sec\n",(long long unsigned int)Pdiff /1000000, ((long long unsigned int) Pdiff/100000)%100000);
	printf("time : %.5f seconds\n",((double)tend.tv_sec+1.0e-9*tend.tv_nsec)-((double)tstart.tv_sec+1.0e-9*tstart.tv_nsec));
	free(tid);


}

#include "hybrid_lock.h"
#include <sys/time.h>
#include <time.h>

long g_count = 0;

#define TIME 1000000

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
    hybrid_lock_lock();
	for (i = 0; i<count; i++) {
		/************ Critical Section ************/
		g_count++;
		/******************************************/
	}
    hybrid_lock_unlock();
}

int main(int argc, char *argv[])
{


	pthread_t *tid;
	//struct timespec tstart={0,0}, tend ={0,0};
	struct timeval Pstart, Pend;
	long i, thread_count, value;
	int rc;

	/*
	 * Check that the program has three arguments
	 * If the number of arguments is not 3, terminate the process.
	 */
	if (3 != argc) {
		fprintf(stderr, "usage: %s <thread count> <value>\n", argv[0]);
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


	
    hybrid_lock_init();

	/*
	 * Create array to get tids of each threads that will
	 * be created by this thread.
	 */
	tid = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
	if (!tid) {
		fprintf(stderr, "malloc() error\n");
		exit(0);
	}

	/*
	 * Create a threads by the thread_count value received as
	 * an argument. Each threads will increase g_count for
	 * value times.
	 */
	//clock_gettime(CLOCK_MONOTONIC, &tstart);
	gettimeofday(&Pstart,NULL);
	
	for (i = 0; i<thread_count; i++) {
		rc = pthread_create(&tid[i], NULL, thread_func, (void*)value);
		if (rc) {
			fprintf(stderr, "pthread_create() error\n");
			free(tid);
	//		pthread_mutex_destroy(&g_mutex);
			exit(0);
		}
	}

	/*
	 * Wait until all the threads you created above are finished.
	 */
	for (i = 0; i<thread_count; i++) {
		rc = pthread_join(tid[i], NULL);
		if (rc) {
			fprintf(stderr, "pthread_join() error\n");
			free(tid);
//			pthread_mutex_destroy(&g_mutex);
			exit(0);
		}
	}

	//clock_gettime(CLOCK_MONOTONIC, &tend);
	gettimeofday(&Pend, NULL);

	/*
	 * Print the value of g_count.
	 * It must be (thread_count * value)
	 */ 
	printf("value: %ld\n", g_count);
	Pdiff = TIME*(Pend.tv_sec-Pstart.tv_sec)+Pend.tv_usec-Pstart.tv_usec;
	//printf("time : %.5f seconds\n",((double)tend.tv_sec+1.0e-9*tend.tv_nsec)-((double)tstart.tv_sec+1.0e-9*tstart.tv_nsec));
	Printf("**Process time = %llu.%llu sec \n", (long long unsigned int)Pdiff / 1000000, (long long unsigned int) Pdiff/1000)%1000);
    hybrid_lock_destroy();
	free(tid);
}

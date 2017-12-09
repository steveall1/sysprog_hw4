#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
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
	long i, j, k, count = (long)arg;
	long long l;

#ifdef USE_SPINLOCK
pthread_spin_lock(&spinlock);
#else
pthread_mutex_lock(&g_mutex);
#endif
	

	for(i=0; i<count; i++){
	
		for(j = 0; j<100000; j++)
			for(k=0; k<3000; k++)
				l += j*k;

		g_count++;		
	}

#ifdef USE_SPINLOCK
pthread_spin_unlock(&spinlock);
#else
pthread_mutex_unlock(&g_mutex);
#endif

}
int main(int argc, char *argv[])
{
	struct timeval start, end;
	pthread_t *tid;
	uint64_t diff = 0;
	long i, thread_count, value;
	int rc;

	#ifdef USE_SPINLOCK
	pthread_spin_init(&spinlock, 0);
	#else
	pthread_mutex_init(&g_mutex, NULL);
	#endif


	if(3!=argc){
		fprintf(stderr, "usage: %s <thread count> <value>\n",argv[0]);
		exit(0);
	}


	thread_count = atol(argv[1]);
	value = atol(argv[2]);

	tid = (pthread_t*)malloc(sizeof(pthread_t)*thread_count);
	if(!tid){
		fprintf(stderr, "malloc() error\n");
		exit(0);
	}

	gettimeofday(&start,NULL);

	for(i=0; i<thread_count; i++){
		rc=pthread_create(&tid[i], NULL, thread_func, (void*)value);
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


	for(i=0; i<thread_count; i++){
		rc=pthread_join(tid[i], NULL);
		if(rc){
			fprintf(stderr,"pthread_join() error\n");
			free(tid);
			#ifdef USE_SPINLOCK
			pthread_spin_destroy(&spinlock);
			#else
			pthread_mutex_destroy(&g_mutex);
			#endif
			exit(0);
		}
	}

	gettimeofday(&end, NULL);

	printf("value: %ld\n", g_count);
	diff = TIME*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
	printf("Process time = %llu.%llu sec\n",(long long unsigned int)diff/1000000, ((long long unsigned int) diff/1000)%1000);
	free(tid);
	
}

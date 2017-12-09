#include "hybrid_lock.h"

void hybrid_lock_init()
{
    hlock.wait_thr = 0;
    pthread_mutex_init(&hlock.mutex, NULL);
    pthread_spin_init(&hlock.spin, NULL);
}
 
void hybrid_lock_destroy()
{
    pthread_mutex_destroy(&hlock.mutex);
    pthread_spin_destroy(&hlock.spin);
}
 
void hybrid_lock_lock()
{
    //struct timeval START, END;
	struct timespec tstart={0,0}, tend={0,0};
    int time;

    //gettimeofday(&START,NULL);
	clock_gettime(CLOCK_MONOTONIC,&tstart);
    while(pthread_spin_trylock(&hlock.spin) != 0){
      //   printf("I'm spinning!\n");
       //  gettimeofday(&END,NULL);
        // if(time=(END.tv_sec -START.tv_sec)>=1){
          //printf("**Process time = %llu.%llu sec\n",(long long unsigned int)time/1000000,((long long unsigned int)(END.tv_usec-START.tv_usec)/1000)%1000);
		//break;
        // }
    }
	clock_gettime(CLOCK_MONOTONIC,&tend);
	printf("time : %.8f seconds\n",((double)tend.tv_sec+1.0e-9*tend.tv_nsec)-((double)tstart.tv_sec+1.0e-9*tstart.tv_nsec));
    hlock.wait_thr += 1;
    pthread_mutex_lock(&hlock.mutex);
    hlock.wait_thr -= 1;
}
    
void hybrid_lock_unlock()
{
    if(hlock.wait_thr > 0){        
        pthread_mutex_unlock(&hlock.mutex);
    }else{
        pthread_spin_unlock(&hlock.spin);
        pthread_mutex_unlock(&hlock.mutex);
    }
}


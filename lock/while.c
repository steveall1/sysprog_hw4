
#include <stdio.h>
#include <time.h>
#include <pthread.h>

int main(){

struct timespec tstart={0,0}, tend={0,0};
int i=0;

clock_gettime(CLOCK_MONOTONIC, &tstart);
while(pthread_spin_trylock()){
}
clock_gettime(CLOCK_MONOTONIC, &tend);

printf("time: %5f seconds\n",((double)tend.tv_sec+1.0e-9*tend.tv_nsec)-((double)tstart.tv_sec+1.0e-9*tstart.tv_nsec));



}


#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define THREADS 1

pthread_mutex_t threadsDisponiblesMutex;
int threadsDisponibles = THREADS-1;

struct MergeStruct{
    int* tab;
    int start;
    int end;
};

void* ThdMergeSortIter(void* pointer){
}

void MergeSortIter(int* tab, int start, int end){
    if(end-start < 3){
        if(tab[start] > tab[end]){
            int val = tab[start];
            tab[start] = tab[end];
            tab[end] = val;
        }
    }else{
        while(pthread_mutex_lock(&threadsDisponiblesMutex) != 0){}
        if(threadsDisponibles > 0){
            threadsDisponibles-=1;
        }else{
            MergeSortIter(tab, start, end/2);
        }
        MergeSortIter(tab, (end/2)+1, end);
        pthread_mutex_unlock(&threadsDisponiblesMutex);
    }
}
void MergeSort(int* tab, int size){
    MergeSortIter(tab, 0, size-1);
}

int main(int argc, char** argv){
    pthread_mutex_init(&threadsDisponiblesMutex, NULL);
    return 0;
}
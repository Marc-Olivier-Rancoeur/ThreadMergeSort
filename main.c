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
    struct MergeStruct* item = (struct MergeStruct*)pointer;

}

void MergeSortIter(int* tab, int start, int end){
    if(end-start < 3){ // si on a un tableau de 1 ou 2, on fait le trie.
        if(tab[start] > tab[end]){
            int val = tab[start]; // échange des deux valeurs dans le cas ou la valeur en premier est supérieur à la valeur en deuxième
            tab[start] = tab[end];
            tab[end] = val;
        }
    }else{
        bool threaded = 0;
        pthread_t thd;
        while(pthread_mutex_lock(&threadsDisponiblesMutex) != 0){}
        if(threadsDisponibles > 0){
            threadsDisponibles-=1;
            pthread_mutex_unlock(&threadsDisponiblesMutex);
            threaded = 1;
            struct MergeStruct item;
            item.tab = tab;
            item.start = start;
            item.end = end/2;
            pthread_create(&thd, NULL, ThdMergeSortIter, (void*)&item);
        }else{
            pthread_mutex_unlock(&threadsDisponiblesMutex);
            MergeSortIter(tab, start, end/2);
        }
        MergeSortIter(tab, (end/2)+1, end);
        if(threaded){
            pthread_join(thd, NULL);
        }
    }
}
void MergeSort(int* tab, int size){
    MergeSortIter(tab, 0, size-1);
}

int main(int argc, char** argv){
    pthread_mutex_init(&threadsDisponiblesMutex, NULL);
    return 0;
}
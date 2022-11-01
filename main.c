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
    }else{ // si le tableau est plus grand que 2
        bool threaded = 0; // variable utilisée pour savoir si on a utilisé un thread ou pas
        pthread_t thd;
        while(pthread_mutex_lock(&threadsDisponiblesMutex) != 0){} // on attend d'avoir le verrou sur le mutex pour avoir accès au nombre de threads disponibles
        if(threadsDisponibles > 0){ // si il nous reste des threads disponibles
            threadsDisponibles-=1;
            pthread_mutex_unlock(&threadsDisponiblesMutex); // on rend immédiatement après le mutex.
            threaded = 1;
            struct MergeStruct item; // création de la scructure qui sera envoyée à la fonction threadée
            item.tab = tab;
            item.start = start;
            item.end = end/2;
            pthread_create(&thd, NULL, ThdMergeSortIter, (void*)&item); //on crée le thread sur la partie gauche du tableau
        }else{
            pthread_mutex_unlock(&threadsDisponiblesMutex);// on rend le mutex dans le cas ou il n'y a plus de threads disponibles
            MergeSortIter(tab, start, end/2); // on lance le merge sort non parallèle sur la partie gauche
        }
        MergeSortIter(tab, (end/2)+1, end); // on reste sur ce thread pour la partie droite.
        if(threaded){
            pthread_join(thd, NULL); // dans le cas ou le thread a été créé, on attend qu'il termine.
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
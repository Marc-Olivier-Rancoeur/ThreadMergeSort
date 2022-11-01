#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

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
    if(end-start < 2){ // si on a un tableau de 1 ou 2, on fait le trie.
        if(tab[start] > tab[end]){
            int val = tab[start]; // échange des deux valeurs dans le cas ou la valeur en premier est supérieur à la valeur en deuxième
            tab[start] = tab[end];
            tab[end] = val;
        }
    }else{ // si le tableau est plus grand que 2
        bool threaded = 0; // variable utilisée pour savoir si on a utilisé un thread ou pas
        pthread_t thd;
        while(pthread_mutex_lock(&threadsDisponiblesMutex) != 0){} // on attend d'avoir le verrou sur le mutex pour avoir accès au nombre de threads disponibles
        if(threadsDisponibles > 0){ // s'il nous reste des threads disponibles
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
            MergeSortIter(tab, start, start+((end-start)/2)); // on lance le merge sort non parallèle sur la partie gauche
        }
        MergeSortIter(tab, start+((end-start)/2)+1, end); // on reste sur ce thread pour la partie droite.
        if(threaded){
            pthread_join(thd, NULL); // dans le cas où le thread a été créé, on attend qu'il termine.
        }
        int taille = end-start+1;
        int iter1 = 0;
        int iter2 = ((end-start)/2)+1;
        int itertab = start;
        int* tab2 = malloc(taille*sizeof(int));
        for(int i = 0 ; i < taille ; i++){
            tab2[i] = tab[start+i];
        }
        while(iter1 < ((end-start)/2)+1 && iter2 < taille){
            if(tab2[iter1] < tab2[iter2]){
                tab[itertab] = tab2[iter1];
                iter1++;
            }else{
                tab[itertab] = tab2[iter2];
                iter2++;
            }
            itertab++;
        }
        for (int i = iter1 ; i < ((end-start)/2)+1 ; ++i){
            tab[itertab] = tab2[i];
            itertab++;
        }
        for (int i = iter2 ; i < taille ; ++i){
            tab[itertab] = tab2[i];
            itertab++;
        }
        printf("tab %d, %d : ", start, end);
        for (int i = start ; i <= end ; ++i){
            printf(" %d", tab[i]);
        }
        printf("\n");
        free(tab2);
    }
}
void MergeSort(int* tab, int size){
    MergeSortIter(tab, 0, size-1);
}

#define TABTAILLE 871

int main(int argc, char** argv){
    pthread_mutex_init(&threadsDisponiblesMutex, NULL);
    int* tab = malloc(TABTAILLE*sizeof(int));
    for (int i = 0 ; i < TABTAILLE ; ++i){
        tab[i] = TABTAILLE-i;
    }
    printf("tab : ");
    for (int i = 0 ; i < TABTAILLE ; ++i){
        printf(" %d", tab[i]);
    }
    printf("\n");
    MergeSort(tab, TABTAILLE);
    printf("tab : ");
    for (int i = 0 ; i < TABTAILLE ; ++i){
        printf(" %d", tab[i]);
    }
    printf("\n");
    return 0;
}
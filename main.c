#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#define THREADS 1


// - Variables globales - //
pthread_mutex_t threadsDisponiblesMutex; // mutex d'accès à la variable partagée threadsDisponibles
int threadsDisponibles = THREADS-1; // définit le nombre de threads disponibles

struct MergeStruct{ // structure de passe des données au thread
    int* tab;
    int start;
    int end;
};

void MergeSortIter(int* tab, int start, int end); // déclaration de la fonction de merge pour être réutilisée dans la version thread.

void* ThdMergeSortIter(void* pointer){ // version thread qui rappel la fonction normale
    struct MergeStruct* item = (struct MergeStruct*)pointer;
    MergeSortIter(item->tab, item->start, item->end);
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
            item.end = start+((end-start)/2);
            pthread_create(&thd, NULL, ThdMergeSortIter, (void*)&item); //on crée le thread sur la partie gauche du tableau
        }else{
            pthread_mutex_unlock(&threadsDisponiblesMutex);// on rend le mutex dans le cas ou il n'y a plus de threads disponibles
            MergeSortIter(tab, start, start+((end-start)/2)); // on lance le merge sort non parallèle sur la partie gauche
        }
        MergeSortIter(tab, start+((end-start)/2)+1, end); // on reste sur ce thread pour la partie droite.
        if(threaded){
            pthread_join(thd, NULL); // dans le cas où le thread a été créé, on attend qu'il termine.
        }
        // - initialisation des variables nécessaires au réarrangement du tableau. - //
        int taille = end-start+1;
        int iter1 = 0; // itérateur pour la partie gauche du tableau déjà trillée
        int iter2 = ((end-start)/2)+1; // itérateur pour la partie droite du tableau déjà trillée
        int itertab = start; // point de départ de remise des valeurs dans le tableau principal
        int* tab2 = malloc(taille*sizeof(int)); // allocation du tableau temporaire
        for(int i = 0 ; i < taille ; i++){ // remplissage du tableau temporaire
            tab2[i] = tab[start+i];
        }
        while(iter1 < ((end-start)/2)+1 && iter2 < taille){ // tant que l'on n'arrive pas au bout d'un tableau
            if(tab2[iter1] < tab2[iter2]){ // test pour placer les valeurs une a une à la bonne place.
                tab[itertab] = tab2[iter1];
                iter1++;
            }else{
                tab[itertab] = tab2[iter2];
                iter2++;
            }
            itertab++;
        }
        // - pour chacun des tableaux, même si un seul n'est pas fini, on termine le remplissage - //
        for (int i = iter1 ; i < ((end-start)/2)+1 ; ++i){
            tab[itertab] = tab2[i];
            itertab++;
        }
        for (int i = iter2 ; i < taille ; ++i){
            tab[itertab] = tab2[i];
            itertab++;
        }
        free(tab2); // on libère la mémoire du tableau temporaire.
    }
}
void MergeSort(int* tab, int size){ // initialisation du mergesort
    MergeSortIter(tab, 0, size-1);
}

int main(void){
    pthread_mutex_init(&threadsDisponiblesMutex, NULL);

    size_t len = 0;
    ssize_t length;
    char* line;
    char tempTab[11];
    int iteratorTemp = 0;
    int iteratorLine = 0;
    length = getline(&line, &len, stdin);
    while(line[iteratorLine] != ' ' && iteratorLine < length) {
        tempTab[iteratorTemp] = line[iteratorLine];
        iteratorLine++;
        iteratorTemp++;
    }

    tempTab[iteratorTemp] = '\0';
    int tabSize = atoi(tempTab);
    printf("taille tab : %d", tabSize);
    int* tab = malloc(tabSize*sizeof(int));
    int iteratorTab = 0;



    while(iteratorLine < length && iteratorTab < tabSize){
        iteratorTemp = 0;
        iteratorLine++;
        while(line[iteratorLine] != ' ' && iteratorLine < length){
            tempTab[iteratorTemp] = line[iteratorLine];
            iteratorLine++;
            iteratorTemp++;
        }
        tempTab[iteratorTemp] = '\0';
        tab[iteratorTab] = atoi(tempTab);
        iteratorTab++;
    }

    MergeSort(tab, tabSize);

    printf("tab : ");
    for (int i = 0 ; i < tabSize ; ++i){
        printf(" %d", tab[i]);
    }
    printf("\n");
    free(tab);
    return 0;
}
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

//#define TEST
#ifdef TEST
#include <omp.h>
#include <time.h>
#define ITERATIONS 3
double moyenne(const double tab[ITERATIONS]){
    double result = 0;
    for (int i = 0 ; i < ITERATIONS ; ++i){
        result+=tab[i];
    }
    return result/ITERATIONS;
}

#endif

int THREADS = 2;

struct MergeStruct{ // structure de passe des données au thread
    int* tab;
    int start;
    int end;
    int thditer;
};

void MergeSortIter(int* tab, int start, int end, int thditer); // déclaration de la fonction de merge pour être réutilisée dans la version thread.

void* ThdMergeSortIter(void* pointer){ // version thread qui rappel la fonction normale
    struct MergeStruct* item = (struct MergeStruct*)pointer;
    MergeSortIter(item->tab, item->start, item->end, item->thditer);
}

void MergeSortIter(int* tab, int start, int end, int thditer){
    if(end-start < 2){ // si on a un tableau de 1 ou 2, on fait le trie.
        if(tab[start] > tab[end]){
            int val = tab[start]; // échange des deux valeurs dans le cas ou la valeur en premier est supérieur à la valeur en deuxième
            tab[start] = tab[end];
            tab[end] = val;
        }
    }else{ // si le tableau est plus grand que 2
        bool threaded = 0; // variable utilisée pour savoir si on a utilisé un thread ou pas
        pthread_t thd;
        if(thditer*2 < THREADS*2){ // s'il nous reste des threads disponibles
            threaded = 1;
            struct MergeStruct item; // création de la structure qui sera envoyée à la fonction threadée
            item.tab = tab;
            item.start = start;
            item.end = start+((end-start)/2);
            item.thditer = thditer*2;
            pthread_create(&thd, NULL, ThdMergeSortIter, (void*)&item); //on crée le thread sur la partie gauche du tableau
        }else{
            MergeSortIter(tab, start, start+((end-start)/2), thditer*2); // on lance le merge sort non parallèle sur la partie gauche
        }
        MergeSortIter(tab, start+((end-start)/2)+1, end, thditer*2+1); // on reste sur ce thread pour la partie droite.
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
            if(tab2[iter1] < tab2[iter2]){ // test pour placer les valeurs unes à une à la bonne place.
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
    MergeSortIter(tab, 0, size-1, 1);
}

#ifdef TEST
#define MIN 0
#define MAX 1000000
int main(void){
    double start, end;
    int* tab;
    double resultats[ITERATIONS];
    int thds[7] = {4,8,16,24,48};
    printf(";4;8;16;24;48\n");
    for (int i = 25000 ; i <= 1000000 ; i+=25000){
        printf("%d", i);
        for (int l = 0; l < 5; ++l) {
            THREADS = thds[l];
            for (int j = 0; j < ITERATIONS; ++j) {
                tab = malloc(i * sizeof(int));
                for (int k = 0; k < i; ++k) {
                    srand(time(NULL) + rand());
                    tab[k] = (rand() % (MAX - MIN)) + MIN;
                }
                start = omp_get_wtime();
                MergeSort(tab, i);
                end = omp_get_wtime();
                free(tab);
                resultats[j] = end - start;
            }
            printf("; %f", moyenne(resultats));
        }
        printf("\n");
    }
    return 0;
}

#else
int main(void){
    // - variables utiles - //
    size_t len = 0;
    ssize_t length;
    char* line;
    char tempTab[11];
    int iteratorTemp = 0;
    int iteratorLine = 0;

    length = getline(&line, &len, stdin); // récupération de la ligne d'entiers

    while(line[iteratorLine] != ' ' && iteratorLine < length) { // boucle de récupération de la taille du tableau
        tempTab[iteratorTemp] = line[iteratorLine];
        iteratorLine++;
        iteratorTemp++;
    }

    tempTab[iteratorTemp] = '\0'; // finalisation du string pour temporaire pour la conversion.

    // - initialisation du tableau - //
    int tabSize = atoi(tempTab);
    int* tab = malloc(tabSize*sizeof(int));
    int iteratorTab = 0;

    while(iteratorLine < length && iteratorTab < tabSize){ // boucle tant qu'il y a des informations à récupérer
        iteratorTemp = 0;
        iteratorLine++;
        while(line[iteratorLine] != ' ' && iteratorLine < length){ // boucle pour chaque nombre
            tempTab[iteratorTemp] = line[iteratorLine];
            iteratorLine++;
            iteratorTemp++;
        }
        tempTab[iteratorTemp] = '\0';
        tab[iteratorTab] = atoi(tempTab);
        iteratorTab++;
    }

    free(line); // libération de la mémoire le la ligne dont on n'a plus besoin

    MergeSort(tab, tabSize); // application de mergesort sur le tableau initialisé

    // - affichage du tableau - //
    if(tabSize <= 1000) {
        for (int i = 0; i < tabSize; ++i) {
            fprintf(stdout, "%d ", tab[i]);
        }
    }else{
        for (int i = 0; i < 100; ++i) {
            fprintf(stdout, "%d ", tab[i]);
        }
        fprintf(stdout, "...");
        for (int i = tabSize-101; i < tabSize; ++i) {
            fprintf(stdout, " %d", tab[i]);
        }
    }
    fprintf(stdout, "\n");
    free(tab);
    return 0;
}
#endif
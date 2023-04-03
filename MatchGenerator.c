#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <math.h>
#include <sys/sem.h>
#include <ctype.h>
#include <stdbool.h>

#include "ipcTools.h"

// Var partagé
typedef struct 
{
    char name[NAMEZIZE+1];
    int status; // 1:qualifié, 0:non-qualifié
}Team;

typedef struct
{
    Team tab[TEAMNAMESIZE];
}Shared;
Shared *shared;

// sémaphores
int **tabSem;
int mutMatch;

void simule(int id, int t, int fd){

    srandom(getpid());
    int k = id-1;

    P(tabSem[t-1][2*k]);
    P(tabSem[t-1][(2*k)+1]);

    P(mutMatch);

    int i1=((int)pow(2, t))*k;
    while (shared->tab[i1].status!=1)
        i1++;
    
    int i2=i1+1;
    while (shared->tab[i2].status!=1)
        i2++;

    V(mutMatch);
    //------------------------------------------------------------------------------
    int nbGoal1=random()%5;
    int nbGoal2=random()%7;
    usleep(random()%MAXTIME);
    //------------------------------------------------------------------------------

    P(mutMatch);
    if (nbGoal2<nbGoal1)
        shared->tab[i2].status=0;
    else
        shared->tab[i1].status=0;
        
    printf("%s : %d - %d : %s \t (idMatch : %d \t tour : %d)\n", shared->tab[i1].name, nbGoal1, nbGoal2, shared->tab[i2].name, id, t);
    saveResult(fd, shared->tab[i1].name, nbGoal1, shared->tab[i2].name, nbGoal2, id, t);

    V(mutMatch);
    V(tabSem[t][k]);
}

void simule_man(int id, int t){
    srandom(getpid());
    int k = id-1;

    P(tabSem[t-1][2*k]);
    P(tabSem[t-1][(2*k)+1]);

    P(mutMatch);

    int i1=((int)pow(2, t))*k;
    while (shared->tab[i1].status!=1)
        i1++;
    
    int i2=i1+1;
    while (shared->tab[i2].status!=1)
        i2++;
        

    //------------------------------------------------------------------------------

    int nbGoal1, nbGoal2;
    printf("%s VS %s \n Entrez le score (séparé par un espace) : \n", shared->tab[i1].name, shared->tab[i2].name);
    scanf("%d %d", &nbGoal1, &nbGoal2);
    usleep(random()%MAXTIME);

    //------------------------------------------------------------------------------

    if (nbGoal2<nbGoal1)
        shared->tab[i2].status=0;
    else
        shared->tab[i1].status=0;
        
    printf("%s : %d - %d : %s \t (idMatch : %d \t tour : %d)\n", shared->tab[i1].name, nbGoal1, nbGoal2, shared->tab[i2].name, id, t);
    V(mutMatch);
    V(tabSem[t][k]);
}

void cleanup(int status, key_t key, int n){
    int m=n;
    int nbtour = (int) (log2(n) + 1);
    for (int i = 0; i < nbtour; i++)
    {
        for (int j = 0; j < m; j++){
                semfree(tabSem[i][j]);
            }
            m/=2;
    }   
    semfree(mutMatch);
    shmfree(key);
    msgfree(key+1);
    exit(status);
}

    //------------------------------------------------------------------------------

int main(int argc, char *argv[]){   

    int fd2;                     /* le descripteur pour le fichier de sortie */

    clock_t start_time, end_time;
    double elapsed_time;

    start_time = clock();

    int fd1;                     /* le descripteur pour le fichier d'entrée */
    ssize_t desc;
    char buffer[BUFFER_SIZE];
    char line_buffer[BUFFER_SIZE];
    int line_pos = 0;
    char** teams=NULL;
    int max_teams = TEAMS; // maximum number of teams that can be stored in the array
    int numTeams=0;
    int dur=MAXTIME;

    if(argc<2){printf("Veuillez saisi le nombre d'equipes\n");}
    int nbEquipes=atoi(argv[1]);
    //printf("%s",argv[1]);
    if (ceil(log2(nbEquipes)) != floor(log2(nbEquipes))){
        printf("Veuillez saisi un nombre d'equipes qui est une puissance de 2");
        exit(1);
    }

    if (argc<2) {perror("veillez precisez le fichier text qui contient les equipes"); exit(2);}

    /* ouverture du fichier */
    fd1 = openFile(argv[2]);
    if (fd1 == -1) {
        perror("open");
        return 2;
    }

    // allocate memory for the array of strings
    teams = (char**)malloc(max_teams * sizeof(char*));

    if (readFile(fd1, teams, &numTeams, &max_teams, &dur) == -1) {
        perror("read");
        return 3;
    }

    printTeams(teams, numTeams, dur);
    
    // Fermeture du fichier
    if (close(fd1)==-1) {perror("close"); return 4;}

    // Initialisation du générateur de nombres aléatoires
    srand(getpid());

    // Mélange des lignes dans le tableau
    shuffleTeams(teams, numTeams);


    key_t key = ftok(".", getpid());
    if (key == -1) {
        perror("Erreur lors de la génération de la clé");
        return 1;
    }

    shared = shmalloc(key, sizeof(Shared));
    if (shared==0)
    {
        perror("shmalloc failed");
        return 5;
        cleanup(5, key, (int) log2(nbEquipes));
    }
    for (int i = 0; i < numTeams; i++) {
        strcpy(shared->tab[i].name, teams[i]);
        shared->tab[i].status=1;
        free(teams[i]);
    }

    mutMatch = semalloc(key+2, 1);
    if (mutMatch==-1)
    {
        perror("semalloc failed");
        cleanup(6, key, (int) log2(nbEquipes));
        return 6;
    }

    int n=nbEquipes;
    int nbtour = (int) (log2(nbEquipes) + 1);
    tabSem = malloc(sizeof(int *) * nbtour);

    for (int i = 0; i <nbtour; i++)
    {
        tabSem[i] = (int *)malloc(sizeof(int) * n);
        for (int j = 0; j < n; j++)
        {
            if (i==0)
            {
                tabSem[i][j] = semalloc(((key+3)*(i+1))+j, 1);
                if (tabSem[i][j] == -1)
                {
                    perror("semalloc failed");
                    semfree(((key+3)*(i+1))+j);
                    exit(5);
                }
            }else{
                tabSem[i][j] = semalloc(((key+3)*(i+1))+j, 0);
                if (tabSem[i][j] == -1)
                {
                    perror("semalloc failed");
                    semfree(((key+3)*(i+1))+j);
                    exit(5);
                }
            }
        }
        n/=2;
    }

    n=nbEquipes;
    for (int i = 0; i < nbtour; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d \t", tabSem[i][j]);
        }
        printf("\n");
        n/=2;
    }

    /* ouverture du fichier de sortie */
    if (access(OUTPUTFILE, F_OK) != -1) {
        fprintf(stderr, "Le fichier existe déjà\n");
        exit(1);
    }
    fd2 = open(OUTPUTFILE, O_WRONLY | O_CREAT | O_EXCL | O_EXCL, 00644);
    /* ouverture impossible */
    if (fd2==-1) {              
        perror("open");
        exit(8);
    } 

    puts ("***** STRIKE <CR> TO START, STOP THE PROGRAM *****");
    getchar();

    int tour=1;
    n=nbEquipes/2;
    while (n>0)
    {
        // création de n processus
        int idMatch=nFork(n);
        if(idMatch==-1){
            perror("Creation de processus");
            cleanup(7, key+4, nbtour);
            return 7;
        }
        if(idMatch>0){
            /* fils */
            if (strcmp(argv[3],"-man")==0)
                simule_man(idMatch, tour);

            simule(idMatch, tour, fd2);
            exit(0);
        }
        n=n/2;
        tour++;
    }

    /* père */
    for (int i = 0; i < (nbEquipes-1); i++)
    {
        while(waitpid(0,0,0) < 0);
    }
    end_time = clock();

    elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Total elapsed time: %f seconds\n", elapsed_time);
    
    /* now wait for the user to strike CR, then stop all tasks */
    getchar();
    sleep(1); /* enough if MAXTIME < 1s */
    close(fd2);
    remove(OUTPUTFILE);
    cleanup(0, key, nbtour);
    free(teams);

    return 0;
}
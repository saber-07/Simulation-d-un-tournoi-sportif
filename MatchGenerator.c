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
int tabSem[4][8];
int mutMatch;

void simule(int id, int t){

    srandom(getpid());
    int k = id-1;

    P(tabSem[t-1][2*k]);
    P(tabSem[t-1][(2*k)+1]);
    // printf("%d, %d\n", tabSem[t-1][2*k], tabSem[t-1][(2*k)+1]);

    // printf("Valeur du semaphore %d : %d\n", tabSem[t-1][2*k], semctl(tabSem[t-1][2*k], 0, GETVAL));
    // printf("Valeur du semaphore %d : %d\n", tabSem[t-1][(2*k)+1], semctl(tabSem[t-1][(2*k)+1], 0, GETVAL));

    P(mutMatch);

    int i1=((int)pow(2, t))*k;
    // printf("%d\n", i1);
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
    V(mutMatch);

    V(tabSem[t][k]);
    // printf("%d", tabSem[t][k]);
    while(wait(NULL) < 0);
    
}

void simule_man(int id){
    int nbGoal1, nbGoal2;

    srandom(getpid());
    // simule une execution
    P(mutMatch);
    int i1=0;
    while (shared->tab[i1].status!=1){
        i1++;
    }
    int i2=i1+1;
    while (shared->tab[i2].status!=1){
        i2++;
    }
    shared->tab[i1].status=-1;
    shared->tab[i2].status=-1;
    printf("veillez saisir le score de l'equipe %s :", shared->tab[i1].name);
    scanf("%d", &nbGoal1);
    printf("veillez saisir le score de l'equipe %s :", shared->tab[i1].name);
    scanf("%d", &nbGoal2);
    V(mutMatch);
    sleep(1);
    usleep(random()%MAXTIME);
    P(mutMatch);
    shared->tab[i1].status=1;
    shared->tab[i2].status=1;
    if (nbGoal2<nbGoal1)
        shared->tab[i2].status=0;
    else
        shared->tab[i1].status=0;
    printf("%s : %d - %d : %s \t (idMatch %d)\n", shared->tab[i1].name, nbGoal1, nbGoal2, shared->tab[i2].name, id);
    V(mutMatch);
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
    exit(status);
}

int main(int argc, char *argv[]){    

    int nbEquipes;

    int fd;
    ssize_t desc;
    char buffer[BUFFER_SIZE];
    char line_buffer[BUFFER_SIZE];
    int line_pos = 0;
    char** teams=NULL;
    int max_teams = TEAMS; // maximum number of teams that can be stored in the array
    int numTeams=0;
    if(argc<2){printf("Veuillez saisi le nombre d'equipes\n");}
    int p1=atoi(argv[1]);
    //printf("%s",argv[1]);
    if (ceil(log2(p1)) != floor(log2(p1))){
        printf("Veuillez saisi un nombre d'equipes qui est une puissance de 2");
        exit(1);
    }else
        nbEquipes=p1;

    if (argc<2) {perror("veillez precisez le fichier text qui contient les equipes"); exit(2);}
    /* ouverture du fichier */
    fd = open(argv[2], O_RDONLY, 0777);
    if (fd == -1) { perror("open"); return 2; }

    // allocate memory for the array of strings
    teams = (char**)malloc(max_teams * sizeof(char*));

    // Lecture du fichier ligne par ligne
    do
    {
        desc = read(fd, buffer, BUFFER_SIZE);
        if(desc==-1){perror("read"); return 3;}

        int start = 0;

        // Traitement de chaque ligne
        for (int i = 0; i < desc; i++) {
            if (buffer[i] == '\n') {
                memcpy(line_buffer + line_pos, buffer + start, i - start);
                line_buffer[line_pos + i - start] = '\0';

                // Add the line to the array
                if (numTeams == max_teams) {
                    // if the array is full, reallocate it to double its size
                    max_teams *= 2;
                    teams = (char**)realloc(teams, max_teams * sizeof(char*));
                }
                teams[numTeams] = strdup(line_buffer); // add a copy of the string to the array
                numTeams++;
                
                /* avancée au début de la ligne suivante */
                start = i + 1;
                line_pos = 0;
            }
        }
        /* Traiter la dernière ligne */
        if(start < desc) {
            memcpy(line_buffer + line_pos, buffer + start, desc - start);
            line_pos += desc - start;
            line_buffer[line_pos] = '\0';
            // Add the line to the array
            if (numTeams == max_teams) {
                // if the array is full, reallocate it to double its size
                max_teams *= 2;
                teams = (char**)realloc(teams, max_teams * sizeof(char*));
            }
            teams[numTeams] = strdup(line_buffer); // add a copy of the string to the array
            numTeams++;
        }
    } while (desc>0);

    // print out the teams
    // for (int i = 0; i < p1*2; i++) {
    //     printf("%s\n", teams[i]);
    // }

    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));
    
    // Fermeture du fichier
    if (close(fd)==-1) {perror("close"); return 4;}

    // Mélange des lignes dans le tableau
    for (int i = numTeams-1; i > 0; i--) {
        int j = rand() % (i+1);
        char* temp = teams[i];
        teams[i] = teams[j];
        teams[j] = temp;
    }
    // mode manuele

    key_t key = ftok("keyGen.xml", getpid());
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

    mutMatch = semalloc(key+1, 1);
    if (mutMatch==-1)
    {
        perror("semalloc failed");
        return 6;
        cleanup(6, key+1, (int) log2(nbEquipes));
    }

    // int n=nbEquipes/2;
    // int nbtour = (int) log2(nbEquipes);
    // printf("%d\n", nbtour);
    // tabSem = malloc(sizeof(int *) * nbtour);
    // for (int i = 0; i <nbtour; i++)
    // {
    //     for (int j = 0; j < n; j++)
    //     {
    //         tabSem[j] = (int *)malloc(sizeof(int) * n);
    //         if (i==0)
    //         {
    //             tabSem[i][j] = semalloc(key*i+j, 1);
    //             if (tabSem[i][j] == -1)
    //             {
    //                 perror("semalloc failed");
    //                 return 6;
    //                 cleanup(6, key*i+j, nbtour);
    //             }
    //         }else{
    //             tabSem[i][j] = semalloc(key*i+j, 0);
    //             if (tabSem[i][j] == -1)
    //             {
    //                 perror("semalloc failed");
    //                 return 6;
    //                 cleanup(6, key*i+j, nbtour);
    //             }
    //         }
    //     }
    //     n/=2;
    // }

    int n=nbEquipes;
    int nbtour = (int) (log2(nbEquipes) + 1);

    for (int i = 0; i <nbtour; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i==0)
            {
                tabSem[i][j] = semalloc(((key+2)*(i+1))+j, 1);
                if (tabSem[i][j] == -1)
                {
                    perror("semalloc failed");
                    semfree(((key+2)*(i+1))+j);
                    exit(5);
                }
            }else{
                tabSem[i][j] = semalloc(((key+2)*(i+1))+j, 0);
                if (tabSem[i][j] == -1)
                {
                    perror("semalloc failed");
                    semfree(((key+2)*(i+1))+j);
                    exit(5);
                }
            }
        }
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
                simule_man(idMatch);

            simule(idMatch, tour);
            exit(0);
        }
        printf("fin tour %d\n",tour);
        // for (int i = 0; i < p1*2; i++)
        // {
        //     printf("%s\t%d\n", shared->tab[i].name, shared->tab[i].status);
        // }
        n=n/2;
        tour++;
    }

    /* père */
    for (int i = 0; i < (nbEquipes-1); i++)
    {
        while(waitpid(0,0,0) < 0);
    }
    
    /* now wait for the user to strike CR, then stop all tasks */
    getchar();
    sleep(1); /* enough if MAXTIME < 1s */
    cleanup(0, key+4, nbtour);
    free(teams);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
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

void display_details(int nbGoal1, int nbGoal2,int i1,int i2, int dur){
    printf("\n");
    int score1 = 0;
    int score2 = 0;
    int duree = MINTIME + random()%(dur - MINTIME);
    int duree_action = duree / (nbGoal1 + nbGoal2+1);
    struct timeval duree_start, duree_stop;
    int minute;
    /* start chrono */
    gettimeofday(&duree_start,0);
  
      int i = 0;
      while (score1 < nbGoal1 && score2 < nbGoal2) {
        
      int choix = random() % 2;
      i++;
      if (choix == 1 && score1 < nbGoal1) {
        score1++;
        minute = (90 * (i-1)/ (nbGoal1 + nbGoal2)) + random() % (90 / (nbGoal1 + nbGoal2));
        printf("\033[32m%d' %s a marqué un but\033[0m\n",minute,shared->tab[i1].name);
      }
      else if(choix == 0 && score2 < nbGoal2){
        score2++;
        minute = (90 * (i-1)/ (nbGoal1 + nbGoal2)) + random() % (90 / (nbGoal1 + nbGoal2));
        printf("\033[36m%d' %s a marqué un but\033[0m\n",minute,shared->tab[i2].name);
      }
        usleep(duree_action);
    }
    while (score1 < nbGoal1) {
      score1++;
      i++;
        minute = (90 * (i-1)/ (nbGoal1 + nbGoal2)) + random() % (90 / (nbGoal1 + nbGoal2));
      printf("\033[32m%d' %s a marqué un but\033[0m\n",minute,shared->tab[i1].name);
      usleep(duree_action);
    }
    while (score2 < nbGoal2) {
      score2++;
      i++;
      minute = (90 * (i-1)/ (nbGoal1 + nbGoal2)) + random() % (90 / (nbGoal1 + nbGoal2));
      printf("\033[36m%d' %s a marqué un but\033[0m\n",minute,shared->tab[i2].name);
      usleep(duree_action);
    }
    printf("\n\n\033[33m******** Récapitulatif *********\033[0m\n\n");
    int cj = random() % 8;
    int cj1 = random() % cj;
    int cj2 = cj - cj1;
    int cr = random() % 1;
    int cr1 = random() % 1;
    int cr2 = cr - cr1;
    int nb_t1 = nbGoal1 + random() % 10;
    int nb_t2 = nbGoal2 + random() % 10;
    int po1 = 30 + random() % 40;
    int po2 = 100 - po1;
  //Carton Jaune
   printf("\033[33mNombre de carton jaune :\t %d\tvs\t%d  \033[0m\n",cj1, cj2);
  // Carton Rouge
   printf("\033[33mNombre de carton rouge :\t %d\tvs\t%d  \033[0m\n",cr1, cr2);
  // Nombre de tentatives
   printf("\033[33mNombre de tirs cadrés :\t %d\tvs\t%d  \033[0m\n", nb_t1, nb_t2);
  // Possession de balle
   printf("\033[33mPossession de balles :\t %d\tvs\t%d   \033[0m\n", po1, po2);
  usleep(duree_action);
  /* stop chrono */
  gettimeofday(&duree_stop,0);
  printf("\033[33mdurée du match : %.3f s\033[0m\n",(duree_stop.tv_usec - duree_start.tv_usec + 1000000.0 * (duree_stop.tv_sec - duree_start.tv_sec))/1000000);
    printf("\n\033[33m********************************\n\033[0m\n");
}

void simule(int id, int t, int fd, int dur){

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
    int penalty = 0;
    int gagnant_penalty;
    if (nbGoal2<nbGoal1)
      shared->tab[i2].status=0;
    else if(nbGoal2>nbGoal1) {
      shared->tab[i1].status=0;
    }
    else {
      penalty = 1;
      gagnant_penalty = random() % 2;
      if (gagnant_penalty == 1) {
        shared->tab[i2].status=0;
      }
      else {
        shared->tab[i1].status=0;
      }
    }
    printf("> %s %d - %d %s \n", shared->tab[i1].name, nbGoal1, nbGoal2, shared->tab[i2].name); 
    if (penalty == 1) {
      if (gagnant_penalty == 1) {
        printf("  %s a gagné par tirs au buts \n",shared->tab[i1].name);
      }
      else {
        printf("  %s a gagné par tirs au buts \n",shared->tab[i2].name);
      }
    }
    printf("\n");
    display_details(nbGoal1,nbGoal2,i1,i2, dur);
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

            simule(idMatch, tour, fd2, dur);
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
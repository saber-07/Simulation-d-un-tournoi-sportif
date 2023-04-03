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

int id_msg;

// sémaphores
int **tabSem;
int mutMatch;


void display_details(int score1, int score2){
    printf("\n");
    int score1 = 0;
    int score2 = 0;
    int duree = 1000000 + random()%MAXTIME;
    int duree_action = duree / (nbGoal1 + nbGoal2+1);
    struct timeval duree_start, duree_stop;

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
    int cr = random() % 2;
    int cr1 = random() % cr;
    int cr2 = cr - cr1;
    int nb_t1 = nbGoal1 + random() % 10;
    int nb_t2 = nbGoal2 + random() % 10;
    int po1 = 30 + random() % 40;
    int po2 = 100 - po1;
  //Carton Jaune
   printf("Nombre de carton jaune : %d\tvs\t%d  \n",cj1, cj2);
  // Carton Rouge
   printf("Nombre de carton rouge : %d\tvs\t%d  \n",cr1, cr2);
  // Nombre de tentatives
   printf("Nombre de tirs cadrés : %d\tvs\t%d  \n", nb_t1, nb_t2);
  // Possession de balle
   printf("Possession de balles : %d %\tvs\t%d %\n", po1, po2);
  usleep(duree_action);
  /* stop chrono */
  gettimeofday(&duree_stop,0);
  printf("\033[33mdurée du match : %.3f s\033[0m\n",(duree_stop.tv_usec - duree_start.tv_usec + 1000000.0 * (duree_stop.tv_sec - duree_start.tv_sec))/1000000);
    printf("\n\033[33m********************************\033[0m\n");
    return 0;
}

void simule(int id, int t){

    srandom(getpid());
    int k = id-1;
    int minute = 0;
    
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
    //------------------------------------------------------------------------
    int nbGoal1 = random() % 6;
    int nbGoal2 = random() % 6;
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
    int score1 = 0;
    int score2 = 0;
    int duree = 1000000 + random()%MAXTIME;
    int duree_action = duree / (nbGoal1 + nbGoal2+1);
    struct timeval duree_start, duree_stop;

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
    int cr = random() % 2;
    int cr1 = random() % cr;
    int cr2 = cr - cr1;
    int nb_t1 = nbGoal1 + random() % 10;
    int nb_t2 = nbGoal2 + random() % 10;
    int po1 = 30 + random() % 40;
    int po2 = 100 - po1;
  //Carton Jaune
   printf("Nombre de carton jaune : %d\tvs\t%d  \n",cj1, cj2);
  // Carton Rouge
   printf("Nombre de carton rouge : %d\tvs\t%d  \n",cr1, cr2);
  // Nombre de tentatives
   printf("Nombre de tirs cadrés : %d\tvs\t%d  \n", nb_t1, nb_t2);
  // Possession de balle
   printf("Possession de balles : %d %\tvs\t%d %\n", po1, po2);
  usleep(duree_action);
  /* stop chrono */
  gettimeofday(&duree_stop,0);
  printf("\033[33mdurée du match : %.3f s\033[0m\n",(duree_stop.tv_usec - duree_start.tv_usec
+ 1000000.0 * (duree_stop.tv_sec - duree_start.tv_sec))/1000000);
    printf("\n\033[33m********************************\033[0m\n");
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

int main(int argc, char *argv[]){    

    int fd;
    ssize_t desc;
    char buffer[BUFFER_SIZE];
    char line_buffer[BUFFER_SIZE];
    int line_pos = 0;
    char** teams=NULL;
    int max_teams = TEAMS; // maximum number of teams that can be stored in the array
    int numTeams=0;
    if(argc<2){printf("Veuillez saisi le nombre d'equipes\n");}
    int nbEquipes=atoi(argv[1]);
    //printf("%s",argv[1]);
    if (ceil(log2(nbEquipes)) != floor(log2(nbEquipes))){
        printf("Veuillez saisi un nombre d'equipes qui est une puissance de 2");
        exit(1);
    }

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

    int id_msg = msgalloc(key+1);
    if (id_msg == -1)
    {
        perror("Alloc msg");
        cleanup(2, key, (int) log2(nbEquipes));
        return 2;
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

            simule(idMatch, tour);
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
    
    /* now wait for the user to strike CR, then stop all tasks */
    getchar();
    sleep(1); /* enough if MAXTIME < 1s */
    cleanup(0, key, nbtour);
    free(teams);

    return 0;
}
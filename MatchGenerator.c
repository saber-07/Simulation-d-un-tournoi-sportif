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

#include "ipcTools.h"
#include "list.h"

#define NBMATCH 8
#define TEAMNAMESIZE 30
#define MAXTIME 500000 // temps max pour simule()

// Var partagé
ListElement *shared;

// sémaphores
int mutMatch;

void simule(int id){

    srandom(getpid());
    usleep(random()%MAXTIME); // simule une execution
    P(mutMatch);
    List p=shared;
    for (int i = 0; i < id; i++)
    {
        p=p->next;
    }
    int nbGoal1=random()%5;
    int nbGoal2=random()%7;
    if (nbGoal1<nbGoal2)
    {
        p->status=0;
        p->next->status=1;
    }
    else
    {
        p->status=1;
        p->next->status=0;
    }
    printf("%s : %d - %d : %s \t (idMatch %d)\n", p->name, nbGoal1, nbGoal2, p->next->name, id);

    V(mutMatch);
}

void cleanup(int status, key_t key){
    semfree(mutMatch);
    shmfree(key);
    exit(status); 
}

int main(int argc, char *argv[]){    

    int nbMatch=NBMATCH;

    int fd;
    ssize_t desc;
    char buffer[BUFFER_SIZE];
    char line_buffer[BUFFER_SIZE];
    int line_pos = 0;
    char** teams=NULL;
    int max_teams = TEAMS; // maximum number of teams that can be stored in the array
    int numTeams=0;

    List l=new_list();

    if (argc!=2) {printf("Veuillez fournir le nom du fichier en argument.\n"); return 1;}

    /* ouverture du fichier */
    fd = open(argv[1], O_RDONLY, 0777);
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
        if (start < desc) {
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
    // for (int i = 0; i < numTeams; i++) {
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

    for (int i = 0; i < numTeams; i++) {
        l=push_back_list(l, teams[i]);
        free(teams[i]);
    }
    

    key_t key = ftok("keyGen.xml", 1);
    if (key == -1) {
        perror("Erreur lors de la génération de la clé");
        return 1;
    }

    shared = shmalloc(key, sizeof(ListElement));
    if (shared==0)
    {
        perror("shmalloc failed");
        return 5;
        cleanup(5, key+4);
    }
    shared=l;

    mutMatch = semalloc(key+1, 1);
    if (mutMatch==-1)
    {
        perror("semalloc failed");
        return 6;
        cleanup(6, key+4);
    }

    puts ("***** STRIKE <CR> TO START, PROGRAM *****");
    getchar();

    while (nbMatch>0)
    {
    // création de n processus
        int idMatch=nFork(nbMatch);
        if(idMatch==-1){
            perror("Creation de processus");
            cleanup(7, key+4);
            return 7;
        }
        if(idMatch>0){
            /* fils */
            simule(idMatch);
            return 0;
        }
    
    nbMatch=nbMatch/2;
   }
    
    /* père */
    for (int i = 0; i < nbMatch; i++)
    {
        while(waitpid(0,0,0) < 0);
    }

    //print_list(l);
    l=clear_list(l);
    free(teams);

    /* now wait for the user to strike CR, then stop all tasks */
    puts ("***** STRIKE <CR> TO STOP, PROGRAM *****");
    getchar();
    sleep(1); /* enough if MAXTIME < 1s */
    cleanup(0, key+4);

    return 0;
}
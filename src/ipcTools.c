/**

@file ipcTools.c
@brief Implémentation de fonctions pour la gestion des IPCs (sémaphores, mémoire partagée, files de messages)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>

#include "ipcTools.h"

// Fonction permettant de créer un certain nombre de processus fils
int nFork (int nbProcs){
    for (int i = 1; i <= nbProcs; i++)
    {
        int pid = fork();
        switch (pid)
        {
        case -1:
            perror("Creation de processus");
            return -1;
        
        case 0:
            return i;
        }
    }
    return 0;
}

// Fonction permettant de créer un sémaphore
int semalloc(key_t key, int valInit){
    // On essaie de récupérer l'identifiant du sémaphore
    int semid = semget (key, 1, 0);
    // Si le sémaphore n'existe pas, on le crée et on l'initialise à la valeur souhaitée
    if (semid==-1)
    {
        semid = semget (key, 1, IPC_CREAT|IPC_EXCL|0666);
        if (semid==-1) return -1;
        else{
            if(semctl(semid,0,SETVAL,valInit)==-1){
                return -1;
                semctl(semid,0,IPC_RMID,0);
            }
            else return semid;
        }
    }
    // Si le sémaphore existe déjà, on retourne son identifiant
    else
        return semid;
}

//------------------------------------------------------------------------------
// Opération P sur le sémaphore
static struct sembuf sbP={0, -1, 0};
void P(int semid){
    int result = semop(semid, &sbP, 1);
    if (result==-1) perror("P");
}
//------------------------------------------------------------------------------
// Opération V sur le sémaphore
static struct sembuf sbV={0, 1, 0};
void V(int semid){
    int result = semop(semid, &sbV, 1);
    if (result==-1) perror("V");
}
//------------------------------------------------------------------------------
// Fonction permettant de libérer un sémaphore
int semfree(int semid){
    return semctl(semid,0,IPC_RMID,0);
}
//**********************************************************************************
//**********************************************************************************

// Fonction permettant de créer une zone de mémoire partagée
void * shmalloc (key_t key, int size){
    void *res;
    int shmid = shmget(key, 1, 0);
    // Si la zone de mémoire partagée n'existe pas, on la crée
    if (shmid==-1) shmid = shmget(key, size, IPC_CREAT|IPC_EXCL|0666);
    if (shmid==-1) return 0;
    // On attache la zone de mémoire partagée au processus courant
    res = shmat(shmid, NULL, 0);
    // Si l'attachement a échoué, on supprime la zone de mémoire partagée
    if(res == (void *) -1){ 
        shmctl(shmid, IPC_RMID, NULL);
        return 0;
    }
    return res;
}
//------------------------------------------------------------------------------

// Fonction qui permet de libérer une région de mémoire partagée identifiée par sa clé
int shmfree (key_t key){
    // Récupération de l'identifiant de la mémoire partagée en utilisant la clé
    int shmid = shmget(key, 1, 0);
    // Si shmidest égal à -1, la fonction retourne -1 pour indiquer l'erreur
    if (shmid == -1) return -1;
    // Sinon, la fonction appelle la fonction shmctl() pour supprimer la région de mémoire partagée
    else return shmctl(shmid, IPC_RMID, NULL);
}
//**********************************************************************************
//**********************************************************************************


/**

    @brief Ouvre un fichier en mode lecture seule et renvoie un descripteur de fichier pour celui-ci.
    @param filename Le nom du fichier à ouvrir.
    @return En cas de réussite, le descripteur de fichier du fichier ouvert. En cas d'échec, -1 et errno est défini pour indiquer l'erreur.
    @see open(2)
*/
int openFile(char* filename) {
    int fd = open(filename, O_RDONLY, 0777);
    if (fd == -1) {
        perror("open");
        return -1;
    }
    return fd;
}
//------------------------------------------------------------------------------

/**
    @brief Fonction pour lire un fichier texte contenant les équipes et la durée de chaque match
    @param fd le descripteur du fichier à lire
    @param teams un pointeur vers le tableau de chaînes de caractères contenant les équipes
    @param numTeams un pointeur vers le nombre d'équipes lues
    @param max_teams un pointeur vers la taille maximale du tableau des équipes
    @param dur un pointeur vers la durée des matchs
    @return 0 si la lecture est réussie, -1 en cas d'erreur
*/
int readFile(int fd, char **teams, int *numTeams, int *max_teams, int *dur) {
    char buffer[BUFFER_SIZE];
    char line_buffer[BUFFER_SIZE];
    int desc, start, line_pos;
    bool isDigit;
    size_t length;
    char *strToken;
    do {
        desc = read(fd, buffer, BUFFER_SIZE);
        if (desc == -1) {
            return -1;
        }
        // Traitement de chaque ligne
        start = 0;
        line_pos = 0;
        for (int i = 0; i < desc; i++) {
            if (buffer[i] == '\n') {
                memcpy(line_buffer + line_pos, buffer + start, i - start);
                line_buffer[line_pos + i - start] = '\0';
                // Add the line to the array
                if (*numTeams == *max_teams) {
                    // if the array is full, reallocate it to double its size
                    *max_teams *= 2;
                    teams = (char**)realloc(teams, *max_teams * sizeof(char*));
                }
                teams[*numTeams] = strdup(line_buffer); // add a copy of the string to the array
                (*numTeams)++;
                /* avancée au début de la ligne suivante */
                start = i + 1;
                line_pos = 0;
            }
        }
        /* Traiter la dernière ligne  */
        if(start < desc) {
            memcpy(line_buffer + line_pos, buffer + start, desc - start);
            line_pos += desc - start;
            line_buffer[line_pos] = '\0';
            
            char * strToken = strtok ( line_buffer, " ");          
            bool isDigit = true;
            size_t length = strlen(strToken);
            for(int i=0; i<length; i++)
                if (!isdigit(strToken[i]))
                    isDigit = false;
            if (isDigit){
                *dur = atoi(strToken);
            }else{
                // Add the line to the array
                if (numTeams == max_teams) {
                    // if the array is full, reallocate it to double its size
                    *max_teams *= 2;
                    teams = (char**)realloc(teams, *max_teams * sizeof(char*));
                }
                teams[*numTeams] = strdup(line_buffer); // add a copy of the string to the array
                numTeams++;
            }
        }
    } while (desc>0);
    return 0;
}
//**********************************************************************************

/**

    @brief Effectue un mélange aléatoire des équipes dans un tableau.
    @param teams Le tableau contenant les noms des équipes à mélanger.
    @param numTeams Le nombre d'équipes dans le tableau.
*/
void shuffleTeams(char **teams, int numTeams) {
    // Initialize random number generator
    srand(getpid());
    // Shuffle the teams
    for (int i = numTeams - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char *temp = teams[i];
        teams[i] = teams[j];
        teams[j] = temp;
    }
}

//**********************************************************************************

/**
    @brief Imprime les noms des équipes stockés dans le tableau passé en paramètre
    @param teams : tableau de pointeurs de caractères contenant les noms des équipes
    @param numTeams : nombre d'équipes stockées dans le tableau
*/
void printTeams(char **teams, int numTeams) {
    // Print the teams
    for (int i = 0; i < numTeams; i++) 
        printf("%s\n", teams[i]);
    }

//**********************************************************************************

/**
    @brief Enregistre le résultat d'un match dans un fichier
    @param fd Le descripteur de fichier du fichier de sortie
    @param name1 Le nom de la première équipe
    @param nbGoal1 Le nombre de buts marqués par la première équipe
    @param name2 Le nom de la deuxième équipe
    @param nbGoal2 Le nombre de buts marqués par la deuxième équipe
    @param id L'identifiant unique du match
    @param t Le tour du match dans le tournoi
*/
void saveResult(int fd, char *name1, int nbGoal1, char *name2, int nbGoal2, int t){
    ssize_t bytes_written;               /* le nombre d’octets par transfert */
    char buffer[BUFFER_SIZE];            /* le buffer de transfert*/
    char *format = "%s : %d - %d : %s \t (tour N° %d)\n";
    if (snprintf(buffer, BUFFER_SIZE, format, name1, nbGoal1, nbGoal2, name2, t)<0)
    {
        perror("snprintf");
        exit(9);
    }
    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written == -1) {
        perror("write");
        exit(10);
    }
}

//**********************************************************************************

/**
    @brief Enregistre le résultat d'un match dans un fichier
    @param fd Le descripteur de fichier du fichier de sortie
    @param name1 Le nom de la première équipe
    @param nbGoal1 Le nombre de buts marqués par la première équipe
    @param name2 Le nom de la deuxième équipe
    @param nbGoal2 Le nombre de buts marqués par la deuxième équipe
    @param cj1 Le nombre de cartes jaunes de la première équipe
    @param cj2 Le nombre de cartes jaunes de la deuxième équipe
    @param cr1 Le nombre de cartes rouges de la première équipe
    @param cr2 Le nombre de cartes rouges de la deuxième équipe
    @param nb_t1 Le nombre de tentatives de la première équipe
    @param nb_t2 Le nombre de tentatives de la deuxième équipe
    @param pot1 le pourcentage de la première équipe
    @param pot2 le pourcentage de la deuxième équipe
    
*/
void saveDetail(int fd,char *name1, int nbGoal1, char *name2, int nbGoal2,int cj1,int cj2, int cr1,int cr2,int nb_t1, int nb_t2, int pot1, int pot2){
    ssize_t bytes_written;               /* le nombre d’octets par transfert */
    char buffer[BUFFER_SIZE];            /* le buffer de transfert*/
    char *format = "%s : %d - %d : %s \n\n";
    if (snprintf(buffer, BUFFER_SIZE, format, name1, nbGoal1, nbGoal2, name2)<0)
    {
        perror("snprintf");
        exit(9);
    }
    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written == -1) {
        perror("write");
        exit(10);
    }
    format = "Nombre de carton jaune : \t %d\tvs\t%d \n";
    if (snprintf(buffer, BUFFER_SIZE, format, cj1, cj2)<0)
    {
        perror("snprintf");
        exit(9);
    }
    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written == -1) {
        perror("write");
        exit(10);
    }
    format = "Nombre de carton rouge : \t %d\tvs\t%d \n";
    if (snprintf(buffer, BUFFER_SIZE, format, cr1, cr2)<0)
    {
        perror("snprintf");
        exit(9);
    }
    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written == -1) {
        perror("write");
        exit(10);
    }
    format = "Nombre de tirs cadrés : \t %d\tvs\t%d \n";
    if (snprintf(buffer, BUFFER_SIZE, format, nb_t1, nb_t2)<0)
    {
        perror("snprintf");
        exit(9);
    }
    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written == -1) {
        perror("write");
        exit(10);
    }
    format = "Possession de balles : \t     %d\tvs\t%d \n\n";
    if (snprintf(buffer, BUFFER_SIZE, format, pot1, pot2)<0)
    {
        perror("snprintf");
        exit(9);
    }
    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written == -1) {
        perror("write");
        exit(10);
    }
}

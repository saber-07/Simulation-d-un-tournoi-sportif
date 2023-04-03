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

//**********************************************************************************
//**********************************************************************************

int semalloc(key_t key, int valInit){

    int semid = semget (key, 1, 0);
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
    else{
        printf("ce semaphore existe\n");
        return semid;
    }
}

//------------------------------------------------------------------------------

static struct sembuf sbP={0, -1, 0};
void P(int semid){
    int result = semop(semid, &sbP, 1);
    if (result==-1) perror("P");
}

//------------------------------------------------------------------------------

static struct sembuf sbV={0, 1, 0};
void V(int semid){
    int result = semop(semid, &sbV, 1);
    if (result==-1) perror("V");
}

//------------------------------------------------------------------------------

int semfree(int semid){
    return semctl(semid,0,IPC_RMID,0);
}

//**********************************************************************************
//**********************************************************************************

void * shmalloc (key_t key, int size){

    void *res;
    int shmid = shmget(key, 1, 0);

    if (shmid==-1) shmid = shmget(key, size, IPC_CREAT|IPC_EXCL|0666);
    if (shmid==-1) return 0;
    res = shmat(shmid, NULL, 0);
    if(res == (void *) -1){ 
        shmctl(shmid, IPC_RMID, NULL);
        return 0;
    }
    return res;
}

//------------------------------------------------------------------------------

int shmfree (key_t key){

    int shmid = shmget(key, 1, 0);

    if (shmid == -1) return -1;
    else return shmctl(shmid, IPC_RMID, NULL);
}

//**********************************************************************************
//**********************************************************************************

int msgalloc(key_t key){
    int mqid = msgget(key, 0);
    if(mqid == -1) mqid = msgget(key, IPC_CREAT|IPC_EXCL|0600);
    if(mqid == -1) return -1;
    else return mqid;
}

//------------------------------------------------------------------------------

int msgfree (int msgqid){
    return msgctl(msgqid, IPC_RMID, 0);
}

//------------------------------------------------------------------------------

static struct 
{
    long type;
    char msg[MAXMSGSIZE];
}buffer;

//------------------------------------------------------------------------------

int msgsend(int msqid, char* msg, int msgSize){
    if (msgSize>MAXMSGSIZE)
    {
        perror("Message too long");
        return -1;
    }
    buffer.type=1;
    strncpy(buffer.msg, msg, msgSize);
    return msgsnd(msqid, &buffer, msgSize, 0);
}

//------------------------------------------------------------------------------

int msgrecv(int msqid, char* msg, int msgSize){
    if (msgSize>MAXMSGSIZE)
    {
        perror("Message too long");
        return -1;
    }
    buffer.type=1;
    int res = msgrcv(msqid, &buffer, msgSize, 1, 0);
    if(res!=-1) strncpy(msg, buffer.msg, msgSize);
    return res;
}

//**********************************************************************************
//**********************************************************************************

// Opens the file specified by filename and returns the file descriptor.

int openFile(char* filename) {
    int fd = open(filename, O_RDONLY, 0777);
    if (fd == -1) {
        perror("open");
        return -1;
    }
    return fd;
}

//------------------------------------------------------------------------------

// Reads the contents of the file with the given file descriptor into the teams array.
// Returns -1 on error, 0 otherwise.
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

void printTeams(char **teams, int numTeams, int dur) {
    // Print the teams
    for (int i = 0; i < numTeams; i++) {
        printf("%s\n", teams[i]);
    }
    // Print the duration
    printf("%d\n", dur);
}

//**********************************************************************************

void saveResult(int fd, char *name1, int nbGoal1, char *name2, int nbGoal2, int id, int t){

    ssize_t bytes_written;               /* le nombre d’octets par transfert */
    char buffer[BUFFER_SIZE];              /* le buffer de transfert*/

    char *format = "%s : %d - %d : %s \t (idMatch : %d \t tour : %d)\n";
    if (snprintf(buffer, BUFFER_SIZE, format, name1, nbGoal1, nbGoal2, name2, id, t)<0)
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
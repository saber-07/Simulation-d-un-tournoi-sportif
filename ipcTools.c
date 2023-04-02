#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>
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

static struct sembuf sbP={0, -1, 0};
void P(int semid){
    int result = semop(semid, &sbP, 1);
    if (result==-1) perror("P");
}

static struct sembuf sbV={0, 1, 0};
void V(int semid){
    int result = semop(semid, &sbV, 1);
    if (result==-1) perror("V");
}

int semfree(int semid){
    return semctl(semid,0,IPC_RMID,0);
}

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

int shmfree (key_t key){

    int shmid = shmget(key, 1, 0);

    if (shmid == -1) return -1;
    else return shmctl(shmid, IPC_RMID, NULL);
}

int msgalloc(key_t key){
    int mqid = msgget(key, 0);
    if(mqid == -1) mqid = msgget(key, IPC_CREAT|IPC_EXCL|0600);
    if(mqid == -1) return -1;
    else return mqid;
}

int msgfree (int msgqid){
    return msgctl(msgqid, IPC_RMID, 0);
}

static struct 
{
    long type;
    char msg[MAXMSGSIZE];
}buffer;


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
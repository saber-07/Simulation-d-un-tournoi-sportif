#include <stdio.h>
#include <sys/types.h>

#define MAXMSGSIZE 1500
#define NBMATCH 8
#define TEAMNAMESIZE 30
#define MAXTIME 500000 // temps max pour simule()

// typedef struct{
//     char name[TEAMNAMESIZE+1];
//     int nbGoal;
// }TeamMatch;
// typedef struct
// {
//     TeamMatch team1, team2;
//     int n;
// } Match;

// static struct 
// {
//     long type;
//     Match msg[NBMATCH];
//     int i;
// }buffer;

int nFork (int nbProcs);
int semalloc(key_t key, int valInit);
void P(int semid); 
void V(int semid);
int semfree(int semid);
void * shmalloc (key_t key, int size);
int shmfree (key_t key);
int msgalloc(key_t key);
int msgfree (int msgqid);
int msgsend(int msqid, char* msg, int msgSize);
int msgrecv(int msqid, char* msg, int msgSize);
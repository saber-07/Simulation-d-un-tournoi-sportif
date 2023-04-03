#include <stdio.h>
#include <sys/types.h>

#define MAXMSGSIZE 1500
#define TEAMNAMESIZE 50
#define MAXTIME 5000000 // temps max pour simule()
#define MINTIME 1000000 // temps min pour simule()
#define NAMEZIZE 30
#define BUFFER_SIZE 1024
#define TEAMS 48
#define OUTPUTFILE "result.txt"


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
int openFile(char *filename);
int readFile(int fd, char **teams, int *numTeams, int *max_teams, int *dur);
void shuffleTeams(char **teams, int numTeams);
void printTeams(char **teams, int numTeams, int dur);
void saveResult(int fd, char *name1, int nbGoal1, char *name2, int nbGoal2, int id, int t);
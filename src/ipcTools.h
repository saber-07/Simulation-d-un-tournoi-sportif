/**
@file ipcTools.h
@brief Contient les déclarations des fonctions de gestion de mémoire partagée, sémaphores et files de messages
*/
#include <stdio.h>
#include <sys/types.h>

#define MAXMSGSIZE 1500 /* Taille maximale d'un message */
#define TEAMNAMESIZE 50 /* Taille maximale d'un nom d'équipe */
#define TIME 5000000 /* Temps pa défaut pour simule() */
#define MINTIME 1000000 // temps minimal pour simule()
#define NAMEZIZE 30 /* Taille maximale d'un nom */
#define BUFFER_SIZE 1024 /* Taille maximale d'un buffer */
#define TEAMS 48 /* Nombre maximal d'équipes */
#define NBTEAMS 16 /* Nombre d'équipes par défaut */
#define OUTPUTFILE "result.txt" /* Fichier text de sortie pour les résultats */
#define OUTPUTCSV "result.csv"  /* Fichier csv de sortie pour les résultats */

/**
    @brief Crée un certain nombre de processus fils
    @param nbProcs Nombre de processus fils à créer
    @return int Renvoie l'identifiant du processus fils créé si le processus est fils, 0 si le processus est le père et -1 en cas d'erreur
*/
int nFork (int nbProcs);
/**
    @brief Alloue un nouveau sémaphore et initialise sa valeur
    @param key Clé utilisée pour identifier le sémaphore
    @param valInit Valeur initiale du sémaphore
    @return int Renvoie l'identifiant du sémaphore en cas de succès, -1 en cas d'erreur
*/
int semalloc(key_t key, int valInit);

/**
    @brief Effectue une opération P sur un sémaphore
    @param semid Identifiant du sémaphore
*/
void P(int semid); 

/**
    @brief Effectue une opération V sur un sémaphore
    @param semid Identifiant du sémaphore
*/
void V(int semid);

/**
    @brief Libère un sémaphore
    @param semid Identifiant du sémaphore à libérer
    @return int Renvoie 0 en cas de succès, -1 en cas d'erreur
*/
int semfree(int semid);

/**
    @brief Alloue de la mémoire partagée
    @param key Clé utilisée pour identifier la mémoire partagée
    @param size Taille de la mémoire partagée à allouer
    @return void* Renvoie un pointeur vers la zone de mémoire partagée en cas de succès, NULL en cas d'erreur
*/
void * shmalloc (key_t key, int size);

/**
    @brief Libère la mémoire partagée
    @param key Clé utilisée pour identifier la mémoire partagée
    @return int Renvoie 0 en cas de succès, -1 en cas d'erreur
*/
int shmfree (key_t key);

/**
    @brief Alloue une nouvelle file de messages
    @param key Clé utilisée pour identifier la file de messages
    @return int Renvoie l'identifiant de la file de messages en cas de succès, -1 en cas d'erreur
*/

int openFile(char *filename);
/**
    @brief Lit les données d'un fichier donné et les stocke dans les tableaux appropriés.
    @param fd Descripteur de fichier du fichier à lire.
    @param teams Pointeur vers un tableau de pointeurs de caractères dans lequel stocker les noms d'équipe lus.
    @param numTeams Pointeur vers la variable dans laquelle stocker le nombre d'équipes lues.
    @param max_teams Pointeur vers la variable dans laquelle stocker le nombre maximum d'équipes autorisées.
    @param dur Pointeur vers la variable dans laquelle stocker la durée de la simulation.
    @return Retourne 0 en cas de succès, et -1 en cas d'erreur.
*/
int readFile(int fd, char **teams, int *numTeams, int *max_teams, int *dur);

/**
    @brief Mélange les équipes dans le tableau donné.
    @param teams Tableau de pointeurs de caractères représentant les noms d'équipe à mélanger.
    @param numTeams Nombre d'équipes dans le tableau.
*/
void shuffleTeams(char **teams, int numTeams);

/**
    @brief Affiche les noms des équipes avec des intervalles de temps spécifiés.
    @param teams Tableau de pointeurs de caractères représentant les noms d'équipe à afficher.
    @param numTeams Nombre d'équipes dans le tableau.
*/
void printTeams(char **teams, int numTeams);

/**
    @brief Enregistre le résultat d'un match dans un fichier donné.
    @param fd Descripteur de fichier du fichier dans lequel enregistrer le résultat.
    @param name1 Nom de la première équipe.
    @param nbGoal1 Nombre de buts marqués par la première équipe.
    @param name2 Nom de la deuxième équipe.
    @param nbGoal2 Nombre de buts marqués par la deuxième équipe.
    @param id Identifiant unique du match.
    @param t Temps total de jeu en microsecondes.
*/
void saveResult(int fd, char *name1, int nbGoal1, char *name2, int nbGoal2, int t);

/**
    @brief Enregistre le résultat d'un match dans un fichier donné.
    @param fd Descripteur de fichier du fichier dans lequel enregistrer le résultat.
    @param name1 Nom de la première équipe.
    @param nbGoal1 Nombre de buts marqués par la première équipe.
    @param name2 Nom de la deuxième équipe.
    @param nbGoal2 Nombre de buts marqués par la deuxième équipe.
    @param id Identifiant unique du match.
    @param t Temps total de jeu en microsecondes.
*/
void saveDetail(int fd,char *name1, int nbGoal1, char *name2, int nbGoal2,int cj1,int cj2, int cr1,int cr2,int nb_t1, int nb_t2, int pot1, int pot2);

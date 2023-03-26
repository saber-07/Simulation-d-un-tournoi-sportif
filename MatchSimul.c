
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>


typedef struct cellule* ListeE;
struct cellule {
    char nom[100]; // Nom de l'equipe
    struct cellule* suivant; //Pointeur vers la prochain noeud de la liste
};

char* NomEquipe(ListeE equipe) {
  return equipe->nom;
}

ListeE suivant(ListeE l) {
  return l->suivant;
}


ListeE creer_equipe(char* nom) {
    // Allocation mémoire
    ListeE equipe = (struct cellule*) malloc(sizeof(struct cellule));
    // Copie du nom de l'equipe dans la liste
    strcpy(equipe->nom, nom);
    // Initialisation du pointeur vers NULL
    equipe->suivant = NULL;
    return equipe;
}


// Ajout d'une nouvelle equipe à la fin de la liste chainée
ListeE ajouter_equipe(ListeE tete, char* nom) {
    ListeE nouvelle_equipe = creer_equipe(nom);
    // Si tete est NULL alors, la nouvelle equipe devient la tete de liste
    if (tete == NULL) {
        return nouvelle_equipe;
    }
    // Sinon, parcours de la liste jusqu'a la dernier equipe
    ListeE courant = tete;
    while (courant->suivant != NULL) {
        courant = courant->suivant;
    }
    // On ajoute l'equipe a la fin de la liste (après la dernière equipe si elle existe)
    courant->suivant = nouvelle_equipe;
    return tete;
}

ListeE supprimer_equipe(ListeE equipe) {
    if (equipe == NULL) {
        return NULL; // Si la liste est vide, rien à supprimer
    }
    ListeE equipeSupp = equipe;
    equipe = suivant(equipe);
    free(equipeSupp);
    return equipe;
}





void afficher_equipes(ListeE tete) {
    ListeE courant = tete;
    while (courant != NULL) {
        printf("Equipe : %s\n",NomEquipe(courant));
        courant = courant->suivant;
    }
    printf("\n ************* \n");
}




int main() {
    char* noms[32] = {
        "Real Madrid", "Barcelone", "Manchester United", "Bayern Munich",
        "AC Milan", "Liverpool", "Juventus", "Chelsea",
        "Paris Saint-Germain", "Borussia Dortmund", "Inter Milan", "Arsenal",
        "Manchester City", "Tottenham", "Atlético Madrid", "Valence",
        "Ajax Amsterdam", "Benfica Lisbonne", "Porto", "Spartak Moscou",
        "Galatasaray", "Fenerbahce", "CSKA Moscou", "PAOK Salonique",
        "Celtic Glasgow", "Rangers Glasgow", "Boca Juniors", "River Plate",
        "Flamengo", "Santos", "Palmeiras", "Corinthians"
    };
    ListeE* memoire_partagee;
    ListeE equipes = NULL; // Initialisation de la liste à NULL

    int nbEquipes = 32;
    int i;
    for (i = 0; i < nbEquipes; i++) {
        equipes = ajouter_equipe(equipes, noms[i]);
    }
     // Création de la clé pour la mémoire partagée
    key_t key = ftok("/etc/passwd", 12);
    if (key == -1) {
        perror("Erreur lors de la création de la clé");
        return 1;
    }

    // Création de la mémoire partagée pour la liste des équipes
    int tailleMemoire = sizeof(ListeE);
    int shmid = shmget(key, tailleMemoire, IPC_CREAT | IPC_EXCL | 0644);
    if (shmid == -1) {
        perror("Erreur lors de la création de la mémoire partagée");
        exit(30);
    }

    // initialiser semaphore
    int semid;
    struct sembuf sb = {0,0,0};
    
    /* etape 1: creation du semaphore */
    semid = semget(key,1,IPC_CREAT | IPC_EXCL | 0600);
    if (semid == -1) {
        perror("semget failed");
        return 2;
    }
    /* etape 2: initialisation du semaphore a 1 */
    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl failed");
        semctl(semid,0,IPC_RMID,0);   /* detruit le semaphore */
        exit(3);
    }
  
    // Attachement de la mémoire partagée au processus courant
    memoire_partagee = (ListeE*) shmat(shmid, 0, 0);
    if (memoire_partagee == (ListeE) -1) {
      perror("Erreur lors de l'attachement de la mémoire partagée");
      exit(31);
    }

    *memoire_partagee = equipes;
    int nbMatchs = 16;
    while (nbMatchs > 0) {
        int i;
        for (i = 0; i < nbMatchs; i++) {
            pid_t pid = fork();

            if (pid < 0) {
                perror("Erreur fork");
                shmctl(shmid,IPC_RMID,0); 
                semctl(semid,0,IPC_RMID,0);
                exit(1);
            } else if (pid == 0) {
                srand(getpid());
                int score1 = rand() % 5;
                int score2 = rand() % 5;
                char equipe1[100], equipe2[100];
                sb.sem_op = -1; // sb.sem_num = 0, sb.sem_op = -1, sb.sem_flg = 0
                if (semop(semid, &sb, 1) == -1) {
                    perror("Erreur lors de la mise en attente du semaphore");
                    semctl(semid,0,IPC_RMID,0);
                    exit(1);
                }
                strcpy(equipe1, NomEquipe(*memoire_partagee));
                *memoire_partagee = supprimer_equipe(*memoire_partagee);
                strcpy(equipe2, NomEquipe(*memoire_partagee));
                *memoire_partagee = ajouter_equipe(*memoire_partagee, equipe1);
                printf("%s %d - %d %s\n", equipe1, score1, score2, equipe2);
                sb.sem_op = 1; // sb.sem_num = 0, sb.sem_op = 1, sb.sem_flg = 0
                if (semop(semid, &sb, 1) == -1) {
                    perror("Erreur lors de la mise en attente du semaphore");
                    semctl(semid,0,IPC_RMID,0);
                    exit(1);
                }
                exit(0);
            }
        }
    while (wait(0) > 0);
    printf("\nFin du tour %d\n\n", nbMatchs);
    nbMatchs /= 2;
  }
  // libere memoire_partagee
  shmdt(memoire_partagee);
  shmctl(shmid,IPC_RMID,0);
    //liberer semaphore
  semctl(semid,0,IPC_RMID,0);
  return 0;
}


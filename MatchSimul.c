#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#define TAILLE_TABLEAU 32

typedef struct {
    char nom[50];
} Equipe;

int main() {
    
    // Création de la clé pour le segment de mémoire partagée
    key_t cle = ftok(".", 'a');
    if (cle == -1) {
        perror("Erreur lors de la création de la clé");
        exit(1);
    }

    // Création du segment de mémoire partagée
    int shmid = shmget(cle, TAILLE_TABLEAU * sizeof(Equipe), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Erreur lors de la création du segment de mémoire partagée");
        exit(1);
    }

    // Attachement du segment de mémoire partagée à l'espace d'adressage du processus
    Equipe* tableau_gagnants = (Equipe*) shmat(shmid, NULL, 0);
    if (tableau_gagnants == (Equipe*) -1) {
        perror("Erreur lors de l'attachement du segment de mémoire partagée");
        exit(1);
    }
    
    char* noms[32] = {
        "Real Madrid", "Barcelone", "Manchester United", "Bayern Munich",
        "AC Milan", "Liverpool", "Juventus", "Chelsea",
        "Paris Saint-Germain", "Borussia Dortmund", "Inter Milan", "Arsenal",
        "Manchester City", "Tottenham", "Atlético Madrid", "Valence",
        "Ajax Amsterdam", "Benfica Lisbonne", "Porto", "Spartak Moscou",
        "Galatasaray", "Fenerbahçe", "CSKA Moscou", "PAOK Salonique",
        "Celtic Glasgow", "Rangers Glasgow", "Boca Juniors", "River Plate",
        "Flamengo", "Santos", "Palmeiras", "Corinthians"
    };

    Equipe equipes[32];

    int i;
    for (i = 0; i < 32; i++) {
        strcpy(equipes[i].nom, noms[i]);
    }

    int nbMatchs = 16;

    while (nbMatchs > 0) {
        for (int i = 0; i < nbMatchs; i++) {
            pid_t pid = fork();

            if (pid < 0) {
                perror("Erreur fork");
                exit(1);
            } else if (pid == 0) {
                srand(getpid()); // Initialisation de rand pour chaque processus enfant
                int score1 = rand() % 5;
                int score2 = rand() % 5;
                printf("%s %d - %d %s\n", equipes[i*2].nom, score1, score2, equipes[i*2+1].nom);
                
                if (score1 >= score2) {
                    tableau_gagnants[i] = equipes[i*2];
                } // Le processus enfant renvoie l'indice de l'équipe gagnante
                else {
                    tableau_gagnants[i] = equipes[i*2 + 1];
                }
                exit(0);
            }
        }
        int status;
        for (i = 0; i < nbMatchs; i++) {
            wait(&status);
        }
        for (i = 0; i < nbMatchs; i++) { 
          equipes[i] = tableau_gagnants[i];
        }
        printf("\nFin du tour %d\n\n", nbMatchs);
        nbMatchs /= 2;
    }

    printf("Le vainqueur est : %s\n", tableau_gagnants[0].nom);

    // Détachement du segment de mémoire partagée
    if (shmdt(tableau_gagnants) == -1) {
        perror("Erreur lors du détachement du segment de mémoire partagée");
        exit(1);
    }

    // Suppression du segment de mémoire partagée
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Erreur lors de la suppression du segment de mémoire partagée");
        exit(1);
    }

    return 0;
}

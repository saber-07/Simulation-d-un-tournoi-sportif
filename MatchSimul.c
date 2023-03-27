#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

typedef struct {
    char nom[50];
} Equipe;

int main() {
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
    Equipe gagnants[16];

    int i;
    for (i = 0; i < 32; i++) {
        strcpy(equipes[i].nom, noms[i]);
    }

    int nbMatchs = 16;
    int nbGagnants = 0;

    while (nbMatchs > 0) {
        int nbGagnants = 0;
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
                exit(score1 >= score2 ? 0 : 1); // Le processus enfant renvoie l'indice de l'équipe gagnante
            }
        }
        int status;
        for (i = 0; i < nbMatchs; i++) {
            wait(&status);
            if (WIFEXITED(status)) {
                int indiceGagnant = i*2 + WEXITSTATUS(status);
                gagnants[nbGagnants] = equipes[indiceGagnant];
                nbGagnants++;
            }
        }
        for (i = 0; i < nbMatchs; i++) { 
          equipes[i] = gagnants[i];
        }
        printf("\nFin du tour %d\n\n", nbMatchs);
        nbMatchs /= 2;
    }

    printf("Le vainqueur est : %s\n", gagnants[0].nom);

    return 0;
}

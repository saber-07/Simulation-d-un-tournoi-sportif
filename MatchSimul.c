#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

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

    int i;
    for (i = 0; i < 32; i++) {
        int j = rand() % 32;
        strcpy(equipes[i].nom, noms[j]);
    }

    int nbMatchs = 16;

    while (nbMatchs > 0) {
        int resultats[16] = {0};

        int i;
        for (i = 0; i < nbMatchs; i++) {
            pid_t pid = fork();

            if (pid < 0) {
                perror("Erreur fork");
                exit(1);
            } else if (pid == 0) {
                srand(getpid());
                int score1 = rand() % 5;
                int score2 = rand() % 5;
                printf("%s %d - %d %s\n", equipes[i*2].nom, score1, score2, equipes[i*2+1].nom);
                exit(0);
            }
        }
        int status;
        while (wait(&status) > 0);
    printf("\nFin du tour %d\n\n", nbMatchs);

    nbMatchs /= 2;
}

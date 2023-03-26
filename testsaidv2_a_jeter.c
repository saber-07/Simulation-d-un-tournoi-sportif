#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>


//Création de la structure qui contiendra les equipes. (j'ai choisi une liste chainée)
typedef struct equipe Equipe;
struct equipe {
    char nom[100]; // Nom de l'equipe
    Equipe* suivant; //Pointeur vers la prochain noeud de la liste
    int score;
};

// Structure pour les résultats de chaque match
typedef struct resultat Resultat;
struct resultat {
    char equipe1[100];
    char equipe2[100];
    int score1;
    int score2;
};
// Création d'un nouveau noeud
Equipe* creer_equipe(char* nom) {
    // Allocation mémoire
    Equipe* equipe = (Equipe*) malloc(sizeof(Equipe));
    // Copie du nom de l'equipe dans la liste
    strcpy(equipe->nom, nom); //copie le nom de l'equipe a partir du fichier pour le placer dans la structure
    // Initialisation du pointeur vers NULL
    equipe->suivant = NULL;
    return equipe;
}

// Ajout d'une nouvelle equipe à la fin de la liste chainée
Equipe* ajouter_equipe(Equipe* tete, char* nom) {
    Equipe* nouvelle_equipe = creer_equipe(nom);
    // Si estvideL() alors, la nouvelle equipe devient la tete de liste
    if (tete == NULL) {
        return nouvelle_equipe;
    }
    // Si non, parcours de la liste jusqu'a la dernier equipe
    Equipe* courant = tete;
    while (courant->suivant != NULL) {
        courant = courant->suivant;
    }
    // On ajoute l'equipe a la fin de la liste (aprés la derniere equipe si existante)
    courant->suivant = nouvelle_equipe;
    return tete;
}

// Fonction pour charger les equipes presentes dans le fichier texte
Equipe* charger_equipes(const char* nom_fichier) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("Erreur: impossible d'ouvrir le fichier,%s\n", nom_fichier);
        return NULL;
    }
    Equipe* tete = NULL;
    char ligne[101];
    while (fgets(ligne, 101, fichier) != NULL) { //fgets (ou_stocker_la_ligne,taille_ligne,nom_fichier) 
        char* nom_equipe = strtok(ligne, "\n"); 
        tete = ajouter_equipe(tete, nom_equipe);
    }
    fclose(fichier);
    return tete;
}

void afficher_equipes(Equipe* tete) {
    Equipe* courant = tete;
    while (courant != NULL) {
        printf("%s\n", courant->nom);
        courant = courant->suivant;
    }
}

Resultat* simuler_match_equipe(Equipe equipe, int duree_match, int shm_id, int equipe_num) {
    Resultat* resultat = (Resultat*) shmat(shm_id, 0, 0); // obtenir l'adresse partagée de la structure résultat
    if (resultat == (void*) -1) { // vérifier si l'attachement à la mémoire partagée a réussi
        perror("Erreur lors de l'attachement à la mémoire partagée");
        exit(1);
    }

    srand(time(NULL)); // initialiser le générateur de nombres aléatoires avec une graine basée sur l'heure actuelle

    int score_equipe1 = rand() % 6; // générer un score aléatoire pour l'équipe 1 (compris entre 0 et 5)
    int score_equipe2 = rand() % 6; // générer un score aléatoire pour l'équipe 2 (compris entre 0 et 5)

    // déterminer le gagnant ou s'il y a match nul
    if (score_equipe1 > score_equipe2) {
        resultat->gagnant = equipe_nom(equipe);
        resultat->score_gagnant = score_equipe1;
        resultat->score_perdant = score_equipe2;
    } else if (score_equipe2 > score_equipe1) {
        resultat->gagnant = equipe_nom(equipe2);
        resultat->score_gagnant = score_equipe2;
        resultat->score_perdant = score_equipe1;
    } else {
        resultat->gagnant = "match nul";
        resultat->score_gagnant = resultat->score_perdant = score_equipe1;
    }

    resultat->duree_match = duree_match;
    resultat->equipe_num = equipe_num;

    return resultat;
}
// Simuler un match entre deux equipes
Resultat* simuler_match(Equipe* equipe1, Equipe* equipe2, int duree_match, int id) {
    c
    // Initialisation de la shared memory pour stocker les scores des équipes
    int shm_id = shmget(IPC_PRIVATE, sizeof(int) * 2, IPC_CREAT | 0666);
    int* scores = (int*) shmat(shm_id, NULL, 0);
    scores[0] = 0;
    scores[1] = 0;

    // Initialisation du processus fils pour simuler le match de l'équipe 1
    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("Erreur lors de la création du processus fils pour l'équipe 1");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) {
        // Simulation de match de l'équipe 1
        Resultat* resultat1 = simuler_match_equipe(equipe1, duree_match, shm_id, 0);
        // Ecriture des résultats dans un fichier
        char filename[20];
        sprintf(filename, "resultat_equipe%d.txt", id * 2);
        FILE* f = fopen(filename, "w");
        fprintf(f, "%s %d - %d %s\n", equipe1->nom, resultat1->score1, resultat1->score2, equipe2->nom);
        fclose(f);
        // Détachement de la shared memory
        shmdt(scores);
        // Fin du processus fils
        exit(EXIT_SUCCESS);
    }

    // Initialisation du processus fils pour simuler le match de l'équipe 2
    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("Erreur lors de la création du processus fils pour l'équipe 2");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
        // Simulation de match de l'équipe 2
        Resultat* resultat2 = simuler_match_equipe(equipe2, duree_match, shm_id, 1);
        // Ecriture des résultats dans un fichier
        char filename[20];
        sprintf(filename, "resultat_equipe%d.txt", id * 2 + 1);
        FILE* f = fopen(filename, "w");
        fprintf(f, "%s %d - %d %s\n", equipe2->nom, resultat2->score2, resultat2->score1, equipe1->nom);
        fclose(f);
        // Détachement de la shared memory
        shmdt(scores);
        // Fin du processus fils
        exit(EXIT_SUCCESS);
    }

    // Attente de la fin des deux processus fils
    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

    // Récupération des scores des deux équipes
    scores = (int*) shmat(shm_id, NULL, 0);
    int score1 = scores[0];
    int score2 = scores[1];
    shmdt(scores);
    shmctl(shm_id, IPC_RMID, NULL); // suppression de la shared memory

    // Création du résultat final
    Resultat* resultat = (Resultat*) malloc(sizeof(Resultat));
    resultat->score1 = score1;
    resultat->score2 = score2;
    resultat->equipe1 = equipe1;
    resultat->equipe2 = equipe2;

    // Affichage du résultat du match
    printf("%s %d - %d %s\n", equipe1->nom, score1, score2, equipe2->nom);

    return resultat;
}
// Simuler un tournoi avec les equipes donnees
void simuler_tournoi(Equipe* tete, int duree_match) {
    Equipe* equipe1;
    Equipe* equipe2;
    Resultat* resultat;
    // Boucle principale pour simuler chaque match
    while (tete != NULL && tete->suivant != NULL) {
        equipe1 = tete;
        equipe2 = tete->suivant;
        tete = tete->suivant->suivant;
        resultat = simuler_match(equipe1, equipe2, duree_match);
        printf("Résultat de match entre %s et %s: %d - %d\n", equipe1->nom, equipe2->nom, resultat->score1, resultat->score2);
        // Mettre a jour les scores des equipes
        equipe1->score += resultat->score1;
        equipe2->score += resultat->score2;
        // Si les equipes ont le meme score, on decide aleatoirement de l'ordre
        if (equipe1->score == equipe2->score) {
            if (rand() % 2 == 0) {
                equipe1->suivant = equipe2;
                equipe2->suivant = NULL;
                tete = equipe1;
            } else {
                equipe2->suivant = equipe1;
                equipe1->suivant = NULL;
                tete = equipe2;
            }
        } else if (equipe1->score < equipe2->score) {
            equipe2->suivant = tete;
            tete = equipe2;
            equipe1->suivant = NULL;
        } else {
equipe1->suivant = tete;
tete = equipe1;
equipe2->suivant = NULL;
}
free(resultat); // Liberation de la memoire allouee pour le resultat
}
// Affichage des resultats finaux du tournoi
printf("\nResultats finaux du tournoi:\n");
int classement = 1;
while (tete != NULL) {
printf("%d. %s (score: %d)\n", classement, tete->nom, tete->score);
tete = tete->suivant;
classement++;
}
}

int main() {
 // Chargement des équipes depuis le fichier
    Equipe* tete_equipes = charger_equipes("equipessaid.txt");
    if (tete_equipes == NULL) {
        return 1;
    }
// Simulation du tournoi avec une duree de match de 30 secondes
simuler_tournoi(tete_equipes, 30);

// Liberation de la memoire allouee pour les equipes
//liberer_equipes(tete_equipes);

return 0;
}

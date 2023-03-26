#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Fonctions utilisées : fopen,fgets,strcpy,
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

void simulation(Equipe equipe1, Equipe equipe2,Resultat* resultat) {
    resultat->score1 =0;
    resultat->score2 =0;
    strcpy(resultat->equipe1, equipe1.nom);
    strcpy(resultat->equipe2, equipe2.nom);
    // Simulation des actions qui changent le score (but/essai/etc.)
    srand(time(NULL));
    for (int i=0; i < 90;i++) {
        if (rand() %2 == 0){
            equipe1.score++;
            resultat->score1++;
        } else {    
            equipe2.score++;
            resultat->score2++;
        }
    }
    //Affichage du score final 
    printf("%s %d - %d %s \n", equipe1.nom, equipe1.score, equipe2.score, equipe2.nom);
}

int main() {

    // Chargement des équipes depuis le fichier
    Equipe* tete_equipes = charger_equipes("equipessaid.txt");
    if (tete_equipes == NULL) {
        return 1;
    }

    // Affichage des équipes chargées
    printf("Equipes:\n");
    afficher_equipes(tete_equipes);

    // Simulation des matchs entre les équipes
    Resultat* resultat = (Resultat*) malloc(sizeof(Resultat));
    Equipe* equipe1 = tete_equipes;
    while (equipe1 != NULL) {
        Equipe* equipe2 = equipe1->suivant;
        while (equipe2 != NULL) {
            simulation(*equipe1, *equipe2, resultat);
            equipe2 = equipe2->suivant;
        }
        equipe1 = equipe1->suivant;
    }

    // Affichage des résultats finaux
    printf("Résultats:\n");
    Equipe* courant = tete_equipes;
    while (courant != NULL) {
        printf("%s : %d\n", courant->nom, courant->score);
        courant = courant->suivant;
    }

    return 0;
}

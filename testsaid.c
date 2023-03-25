#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ipcTools.h>
// Fonctions utilisées : fopen,fgets,strcpy,
//Création de la structure qui contiendra les equipes. (j'ai choisi une liste chainée)
typedef struct equipe Equipe;
struct equipe {
    char nom[100]; // Nom de l'equipe
    Equipe* suivant; //Pointeur vers la prochain noeud de la liste
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
    char ligne[100];
    while (fgets(ligne, 100, fichier) != NULL) { //fgets (ou_stocker_la_ligne,taille_ligne,nom_fichier) 
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

int main() {
    Equipe* tete = charger_equipes("equipes.txt"); // mon fichier equipes.txt contient 10 équipes avec 1 equipe par ligne
    if (tete == NULL) {
        return 1;
    }
    afficher_equipes(tete);
    return 0;
}
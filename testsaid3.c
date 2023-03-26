#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "ipcTools.h"
#define MAX_equipe 32
#define max_nom 50
#define duree 90
#define cle 1234

// Création de la structure qui contiendra les equipes. (j'ai choisi une liste
// chainée)

typedef struct e {
  char nom[max_nom]; // Nom de l'equipe
  int score;
} Equipe;

// Structure pour les matchs
typedef struct {
  int equipe1;
  int equipe2;
  int duration;
  int score1;
  int score2;
} Match;

void chargement_equipe(char *filename, Equipe *equipe, int *num_teams) {
  FILE *fichier = fopen(filename, "r");
  if (fichier == NULL) {
    fprintf(stderr, "Erreur lors de l'ouverture du fichier %s\n", filename);
    exit(1);
  }
  char buff[max_nom + 1]; // taille du char + 1 (\0)
  while (fgets(buff, max_nom + 1, fichier) != NULL) {
    buff[strcspn(buff, "\n\r")] =
        '\0'; // \n et \r en rapport avec le TP4 exo 2 de microcontroleur
    if (*num_teams >= MAX_equipe) {
      fprintf(stderr, "Trop d'équipes (le maximum est de %d)\n", MAX_equipe);
      exit(1);
    }
    strncpy(equipe[*num_teams].nom, buff, max_nom);
    equipe[*num_teams].score = 0;
    (*num_teams)++;
  }
  fclose(fichier);
  if (((*num_teams) & (*num_teams - 1)) != 0) {
    fprintf(stderr, "Le nombre d'équipes doit être une puissance de 2\n");
    exit(1);
  }
}

void simulation_match(Match *match, Equipe *equipe) {
  int score1, score2;

  // Géneration d'un score aleatoire pour les equipes 1 et 2
  score1 = rand() % 6;
  score2 = rand() % 6;

  printf("%s %d - %d %s \n", equipe[match->equipe1].nom, match->score1,
         match->score2, equipe[match->equipe2].nom);
}

void simulation_tournoi(int num_teams, Equipe *equipe) {
  int i, j, k;
  int num_tour = num_teams - 1;
  Match match[num_tour * num_teams / 2];
  k = 0;
  for (i = 0; i < num_teams; i++) {
    for (j = 0; j < num_teams; j++) {
      match[k].equipe1 = i;
      match[k].equipe2 = j;
      match[k].duration = duree;
      match[k].score1 = 0;
      match[k].score2 = 0;
      k++;
    }
  }
  // Création de la mémoire partagée pour les resultats de match
  int shm_id =shmget(cle, sizeof(Match) * num_tour * num_teams / 2, IPC_CREAT | 0666);
  if (shm_id < 0) {
    perror("erreur shmget");
    exit(1);
  }
  Match *resultat_match = shmat(shm_id, NULL, 0);
  if (resultat_match == NULL) {
    perror("erreur shmat");
    exit(1);
  }

  // Utilisation des processus pour simuler le tournoi
  for (i = 0; i < num_tour * num_teams / 2; i++) {
    pid_t pid = fork();
    if (pid < 0) {
      perror("erreur fork");
      exit(1);
    } else if (pid == 0) {
      simulation_match(&match[i], equipe);
      resultat_match[i] = match[i];
      exit(0);
    }
  }

  // Attente des processus fils
  for (i = 0; i < num_tour * num_teams / 2; i++) {
    int status;
    pid_t pid = wait(&status);
    if (pid < 0) {
      perror("erreur wait");
      exit(1);
    }
    if (status != 0) {
      fprintf(stderr, "Erreur wait %d\n", status);
      exit(1);
    }
  }
  // Suppression de la memoire partagée
  if (shmdt(resultat_match) == -1) {
    perror("erreur shmdt");
    exit(1);
  }
  if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
    perror("erreur shmctl");
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int num_teams = 0;
  Equipe equipe[MAX_equipe];
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <fichier d'equipes>\n", argv[0]);
    exit(1);
  }

  // Chargement des equipes depuis le fichier
  chargement_equipe(arg[1], equipe, &num_teams);

  // Simulation du tournoi
  simulation_tournoi(num_teams, equipe);

  return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/uio.h>
#include <math.h>
#include <sys/sem.h>
#include <ctype.h>
#include <stdbool.h>

#include "ipcTools.h"

// Var partagé
typedef struct 
{
  char name[NAMEZIZE+1];
  int status; // 1:qualifié, 0:non-qualifié
}Team;

typedef struct
{
  Team tab[TEAMNAMESIZE];
}Shared;
Shared *shared;

// sémaphores
int **tabSem;
int mutMatch;

/**
  @brief Affiche les détails d'un match de football simulé.
  @param nbGoal1 Le nombre de buts à marquer par la première équipe.
  @param nbGoal2 Le nombre de buts à marquer par la deuxième équipe.
  @param i1 L'indice de la première équipe dans un tableau de structures représentant toutes les équipes.
  @param i2 L'indice de la deuxième équipe dans un tableau de structures représentant toutes les équipes.
  @param dur La durée maximale du match, en secondes.
*/
void display_details(int nbGoal1, int nbGoal2,int i1,int i2, int dur,int fd){
  // Affichage d'une ligne vide pour séparer les affichages précédents de ceux du match.
  printf("\n");
  // Initialisation des scores de chaque équipe à 0.
  int score1 = 0;
  int score2 = 0;
  // Génération d'une durée de match aléatoire entre MINTIME et la durée maximale du match.

  int duree = MINTIME + random()%(dur - MINTIME);
  // Calcul de la durée d'une action en fonction du nombre de buts à marquer et de la durée du match.

  int duree_action = duree / (nbGoal1 + nbGoal2+1);
  // Initialisation d'une structure timeval pour le début du match.
  struct timeval duree_start, duree_stop;
  // Initialisation d'une variable pour stocker le temps écoulé en minutes pendant le match.

  int minute;
  /* start chrono */

  // Récupération du temps de début de la fonction.
  gettimeofday(&duree_start,0);
  // Boucle principale qui simule le déroulement du match jusqu'à ce que l'un des deux scores atteigne le nombre de buts à marquer.

    int i = 0;
    while (score1 < nbGoal1 && score2 < nbGoal2) {
      
    int choix = random() % 2;
    i++;
    /* Si l'équipe 1 marque un but et que son score est inférieur au nombre de buts à marquer, 
      affichage du but avec le temps écoulé en minutes. */

    if (choix == 1 && score1 < nbGoal1) {
      score1++;
      minute = (90 * (i-1)/ (nbGoal1 + nbGoal2)) + random() % (90 / (nbGoal1 + nbGoal2));
      printf("\033[32m%d' %s a marqué un but\033[0m\n",minute,shared->tab[i1].name);
    }
    else if(choix == 0 && score2 < nbGoal2){
      score2++;
      // Calcule la minute du but en fonction du nombre de buts de chaque équipe et ajoute un facteur aléatoire
      minute = (90 * (i-1)/ (nbGoal1 + nbGoal2)) + random() % (90 / (nbGoal1 + nbGoal2));
      printf("\033[36m%d' %s a marqué un but\033[0m\n",minute,shared->tab[i2].name);
    }
      usleep(duree_action);
  }
  while (score1 < nbGoal1) {
    score1++;
    i++;
    // Calcule la minute du but en fonction du nombre de buts de chaque équipe et ajoute un facteur aléatoire
      minute = (90 * (i-1)/ (nbGoal1 + nbGoal2)) + random() % (90 / (nbGoal1 + nbGoal2));
    printf("\033[32m%d' %s a marqué un but\033[0m\n",minute,shared->tab[i1].name);
    usleep(duree_action);
  }
  while (score2 < nbGoal2) {
    score2++;
    i++;
    // Calcule la minute du but en fonction du nombre de buts de chaque équipe et ajoute un facteur aléatoire
    minute = (90 * (i-1)/ (nbGoal1 + nbGoal2)) + random() % (90 / (nbGoal1 + nbGoal2));
    printf("\033[36m%d' %s a marqué un but\033[0m\n",minute,shared->tab[i2].name);
    usleep(duree_action);
  }
  // Affiche le récapitulatif du match avec des statistiques aléatoires
  printf("\n\n\033[33m******** Récapitulatif *********\033[0m\n\n");
  int cj1 = random() % 8;//Génère un nombre aléatoire de cartons jaunes
  int cj2 = 8 - cj1;//Génère un nombre aléatoire de cartons jaunes pour l'équipe 1
  int cr1 = random() % 2; //Calcule le nombre de cartons jaunes restants pour l'équipe 2
  int cr2 = random() % 2; // Génère un nombre aléatoire de cartons rouges (0 ou 1)
  int nb_t1 = nbGoal1 + random() % 10; // Génère un nombre aléatoire de tirs cadrés pour l'équipe 1
  int nb_t2 = nbGoal2 + random() % 10; // Génère un nombre aléatoire de tirs cadrés pour l'équipe 2
  int po1 = 30 + random() % 40; // Génère un pourcentage aléatoire de possession de balle pour l'équipe 1
  int po2 = 100 - po1; // Génère un pourcentage aléatoire de possession de balle pour l'équipe 2
  //Carton Jaune
  printf("\033[33mNombre de carton jaune : \t %d\tvs\t%d  \033[0m\n",cj1, cj2);
  // Carton Rouge
  printf("\033[33mNombre de carton rouge : \t %d\tvs\t%d  \033[0m\n",cr1, cr2);
  // Nombre de tentatives
  printf("\033[33mNombre de tirs cadrés : \t %d\tvs\t%d  \033[0m\n", nb_t1, nb_t2);
  // Possession de balle
  printf("\033[33mPossession de balles :   \t %d\tvs\t%d   \033[0m\n", po1, po2);
  usleep(duree_action);
  /* stop chrono */
  gettimeofday(&duree_stop,0);
  printf("\033[33mdurée du match : %.3f s\033[0m\n",(duree_stop.tv_usec - duree_start.tv_usec + 1000000.0 * (duree_stop.tv_sec - duree_start.tv_sec))/1000000);
  printf("\n\033[33m********************************\n\033[0m\n");

  saveDetail(fd,shared->tab[i1].name,nbGoal1,shared->tab[i2].name,nbGoal2,cj1,cj2,cr1,cr2,nb_t1,nb_t2,po1,po2);
}

/**
  @brief Simule un match de football entre deux équipes.
  @param id Identifiant du processus.
  @param t Numéro du tour.
  @param fd Descripteur de fichier pour la sauvegarde des résultats.
  @param dur Durée du match.
  @return void.
*/
void simule(int id, int t, int fd, int dur, int f_csv, bool detail){

  srandom(getpid());
  int k = id-1;

  // Attente des sémaphores de début de match
  P(tabSem[t-1][2*k]);
  P(tabSem[t-1][(2*k)+1]);
  // Exclusion mutuelle pour accéder à la zone partagée
  P(mutMatch);
  // Recherche des deux équipes qui vont participer au match

  int i1=((int)pow(2, t))*k;
  while (shared->tab[i1].status!=1)
      i1++;
  
  int i2=i1+1;
  while (shared->tab[i2].status!=1)
      i2++;

  // Fin de l'exclusion mutuelle
  V(mutMatch);
  // Simulation du match
  int nbGoal1=random()%5;
  int nbGoal2=random()%5;

  // simulation pour le cas ou l'utilisateur veut pas de details
  if (!detail)
    usleep(random()%dur);
  // Exclusion mutuelle pour accéder à la zone partagée

  P(mutMatch);
  // Détermination de l'équipe gagnante
  int penalty = 0;
  int gagnant_penalty;
  if (nbGoal2<nbGoal1)
    shared->tab[i2].status=0;
  else if(nbGoal2>nbGoal1) {
    shared->tab[i1].status=0;
  }
  else {
    penalty = 1;
    gagnant_penalty = random() % 2;
    if (gagnant_penalty == 1) {
      shared->tab[i2].status=0;
    }
    else {
      shared->tab[i1].status=0;
    }
  }
  // Affichage du résultat du match
  printf("> %s %d - %d %s \t (tour N°%d)\n", shared->tab[i1].name, nbGoal1, nbGoal2, shared->tab[i2].name, t); 
  if (penalty == 1) {
    if (gagnant_penalty == 1) {
      printf("  %s a gagné par tirs au buts \n",shared->tab[i1].name);
    }
    else {
      printf("  %s a gagné par tirs au buts \n",shared->tab[i2].name);
    }
  }
  printf("\n");
  if (detail)
    // Affichage des détails du match
    display_details(nbGoal1,nbGoal2,i1,i2, dur,fd);
  else
    // Sauvegarde des résultats dans le fichier
    saveResult(fd, shared->tab[i1].name, nbGoal1, shared->tab[i2].name, nbGoal2, t);

  char data[200];
  sprintf(data, "%s, %d, %d, %s, %d\n", shared->tab[i1].name, nbGoal1, nbGoal2, shared->tab[i2].name, t);
  write(f_csv, data, strlen(data));

  // Fin de l'exclusion mutuelle
  V(mutMatch);
  // Libération du sémaphore de fin de match pour l'équipe
  V(tabSem[t][k]);
}

/**
  @brief Simule un match de football joué entre deux équipes de façon manuelle.
  @param id Identifiant du match
  @param t Tour de jeu
*/
void simule_man(int id, int t, int fd, int dur, int f_csv, bool detail){
  srandom(getpid());
  int k = id - 1;

  P(tabSem[t - 1][2 * k]);
  P(tabSem[t - 1][(2 * k) + 1]);
  P(mutMatch);

  int i1 = ((int)pow(2, t)) * k;
  while (shared->tab[i1].status != 1)
    i1++;

  int i2 = i1 + 1;
  while (shared->tab[i2].status != 1)
    i2++;

  //------------------------------------------------------------------------------

  int nbGoal1, nbGoal2;
  printf("\t%s VS %s \nEntrez le score (séparé par un espace) : ", shared->tab[i1].name, shared->tab[i2].name);
  scanf("%d %d", &nbGoal1, &nbGoal2);


  //------------------------------------------------------------------------------

  if (nbGoal2 < nbGoal1)
    shared->tab[i2].status = 0;
  else
    shared->tab[i1].status = 0;

  int penalty = 0;
  int gagnant_penalty;
  if (nbGoal2 < nbGoal1)
    shared->tab[i2].status = 0;
  else if (nbGoal2 > nbGoal1) {
    shared->tab[i1].status = 0;
  } else {
    penalty = 1;
    gagnant_penalty = random() % 2;
    if (gagnant_penalty == 1) {
      shared->tab[i2].status = 0;
    } else {
      shared->tab[i1].status = 0;
    }
  }
  printf("> %s %d - %d %s \t (tour N°%d) \n", shared->tab[i1].name, nbGoal1, nbGoal2, shared->tab[i2].name, t);
  if (penalty == 1) {
    if (gagnant_penalty == 1) {
      printf("  %s a gagné par tirs au buts \n", shared->tab[i1].name);
    } else {
      printf("  %s a gagné par tirs au buts \n", shared->tab[i2].name);
    }
  }
  printf("\n");
  if (detail)
    display_details(nbGoal1,nbGoal2,i1,i2, dur,fd);
  else
    saveResult(fd, shared->tab[i1].name, nbGoal1, shared->tab[i2].name, nbGoal2, t);
  
  char data[200];
  sprintf(data, "%s, %d, %d, %s, %d\n", shared->tab[i1].name, nbGoal1, nbGoal2,
          shared->tab[i2].name, t);
  write(f_csv, data, strlen(data));
  V(mutMatch);
  V(tabSem[t][k]);
}

/**
  @brief Nettoie les ressources partagées utilisées pour le tournoi.
  @param status Le code de sortie à utiliser pour terminer le programme.
  @param key La clé utilisée pour accéder à la mémoire partagée.
  @param n Le nombre d'équipes participant au tournoi.
*/
void cleanup(int status, key_t key, int n) {
  int m = n;
  // Calcul du nombre de tours pour le tournoi.
  int nbtour = (int) (log2(n) + 1);
  // Parcours de tous les sémaphores utilisés pour le tournoi.
  for (int i = 0; i < nbtour; i++) {
    for (int j = 0; j < m; j++) {
      // Libération du sémaphore correspondant à l'équipe j au tour i.
      semfree(tabSem[i][j]);
    }
    m /= 2;
  }
  // Libération du sémaphore utilisé pour protéger la zone critique.
  semfree(mutMatch);
  // Libération de la mémoire partagée utilisée pour stocker les données du tournoi.
  shmfree(key);
  // Terminaison du programme avec le code de sortie spécifié.
  exit(status);
}

//------------------------------------------------------------------------------

/**
 * @brief Fonction principale du programme.
 * 
 * @param argc Le nombre d'arguments passés au programme.
 * @param argv Tableau de chaînes de caractères représentant les arguments passés au programme.
 * @return int La valeur de retour de la fonction.
 */
int main(int argc, char *argv[]){   

  int fd2;                     /* le descripteur pour le fichier de sortie */

  clock_t start_time, end_time;
  double elapsed_time;

  start_time = clock();

  int fd1;                     /* le descripteur pour le fichier d'entrée */
  ssize_t desc;
  char buffer[BUFFER_SIZE];
  char line_buffer[BUFFER_SIZE];
  int line_pos = 0;
  char** teams=NULL;
  int max_teams = TEAMS; // maximum number of teams that can be stored in the array
  int numTeams=0;
  int dur=TIME;
  int nbEquipes=NBTEAMS;

    // Vérifier le nombre d'arguments passés au programme
    if(argc>2){
      nbEquipes=atoi(argv[1]);
      if (ceil(log2(nbEquipes)) != floor(log2(nbEquipes))){
        printf("Veuillez saisir un nombre d'equipes qui est une puissance de 2\n");
        exit(1);
      }
    }

    if (argc<3) {

      if (nbEquipes>32)
      {
          printf("Veuillez saisir un nombre d'equipes qui est inférieur a 32 ou saisir un fichier d'entrée\n");
          exit(2);
      }
      
      // tableau d'équipe pour dans le cas ou pas de fichier est déclaré
      char* team_names[32] = {
          "Real Madrid", "Barcelone", "Manchester United", "Bayern Munich",
          "AC Milan", "Liverpool", "Juventus", "Chelsea",
          "Paris Saint-Germain", "Borussia Dortmund", "Inter Milan", "Arsenal",
          "Manchester City", "Tottenham", "Atlético Madrid", "Valence",
          "Ajax Amsterdam", "Benfica Lisbonne", "Porto", "Spartak Moscou",
          "Galatasaray", "Fenerbahçe", "CSKA Moscou", "PAOK Salonique",
          "Celtic Glasgow", "Rangers Glasgow", "Boca Juniors", "River Plate",
          "Flamengo", "Santos", "Palmeiras", "Corinthians"
      };

      teams = (char**)malloc(nbEquipes * sizeof(char*));
      for (int i = 0; i < nbEquipes; i++){
        teams[i] = (char*)malloc((strlen(team_names[i]) + 1) * sizeof(char));
        strcpy(teams[i], team_names[i]);
      }

    }else{
      /* ouverture du fichier */
      fd1 = openFile(argv[2]);
      if (fd1 == -1) {
        perror("open");
        return 2;
      }

      // Alloue de la mémoire pour un tableau de caractères
      teams = (char**)malloc(max_teams * sizeof(char*));

      if (readFile(fd1, teams, &numTeams, &max_teams, &dur) == -1) {
        perror("read");
        return 3;
      }

      // fontion qui affiche les équipes tirer au hasard 
      // printTeams(teams, numTeams);
      
      // Fermeture du fichier
      if (close(fd1)==-1) {perror("close"); return 4;}

      // Initialisation du générateur de nombres aléatoires
      srand(getpid());

      // Mélange des lignes dans le tableau
      shuffleTeams(teams, numTeams);
    }


    key_t key = ftok(".", getpid());
    if (key == -1) {
        perror("Erreur lors de la génération de la clé");
        return 1;
    }

  shared = shmalloc(key, sizeof(Shared));
  if (shared==0)
  {
      perror("shmalloc failed");
      return 5;
      cleanup(5, key, (int) log2(nbEquipes));
  }
  for (int i = 0; i < nbEquipes; i++) {
      strcpy(shared->tab[i].name, teams[i]);
      shared->tab[i].status=1;
      free(teams[i]);
  }

  mutMatch = semalloc(key+2, 1);
  if (mutMatch==-1)
  {
    perror("semalloc failed");
    cleanup(6, key, (int) log2(nbEquipes));
    return 6;
  }

  int n=nbEquipes;
  int nbtour = (int) (log2(nbEquipes) + 1);
  tabSem = malloc(sizeof(int *) * nbtour);

  for (int i = 0; i <nbtour; i++)
  {
    // Alloue de la mémoire pour stocker les sémaphores de la i-ème itération
    tabSem[i] = (int *)malloc(sizeof(int) * n);
    // Boucle pour initialiser chaque sémaphore du tableau de la i-ème itération
    for (int j = 0; j < n; j++)
    {
      if (i==0)
      {
      // Alloue un nouveau sémaphore et l'initialise à 1
      tabSem[i][j] = semalloc(((key+3)*(i+1))+j, 1);
      // Si semalloc a échoué, affiche une erreur et libère les ressources
      if (tabSem[i][j] == -1)
      {
        perror("semalloc failed");
        semfree(((key+3)*(i+1))+j);
        exit(5);
      }
      // Alloue un nouveau sémaphore et l'initialise à 0
    }else{
        tabSem[i][j] = semalloc(((key+3)*(i+1))+j, 0);
        // Si semalloc a échoué, affiche une erreur et libère les ressources
        if (tabSem[i][j] == -1)
        {
          perror("semalloc failed");
          semfree(((key+3)*(i+1))+j);
          exit(5);
        }
      }
    }
    // Divise le nombre d'équipe par 2 pour le prochain tour
    n/=2;
  }

  // Ouvre le fichier OUTPUTFILE en mode écriture uniquement, en le créant s'il n'existe pas, avec des permissions 644.
  fd2 = open(OUTPUTFILE, O_WRONLY | O_CREAT | O_TRUNC, 00644);
  /* ouverture impossible */
  if (fd2==-1) {              
      perror("open");
      exit(8);
  } 

  int f_csv;
  // Ouvre le fichier "result.csv" en mode écriture uniquement, en le créant s'il n'existe pas, avec des permissions 644.
  f_csv = open("result.csv", O_WRONLY | O_CREAT | O_TRUNC,
              0644); // open file for writing
  if (f_csv == -1) {
      printf("Error opening file.\n");
      return 1;
  }
  char headers[] = "Equipe 1, score 1, score 2, Equipe 2, Tour\n";
  // Définit une chaîne de caractères 'headers' qui contient l'en-tête du fichier CSV
  write(f_csv, headers, strlen(headers));
  // Écrit l'en-tête dans le fichier CSV

  printf("Voulez-vous les détails du déroulement des match durant le tournoi : O/N ");
  char rep = getchar();
  rep = toupper(rep);
  bool detail;
  if (rep=='O')
    detail=true;
  else
    detail=false;

  // Affiche un message à l'utilisateur et attend une entrée clavier
  puts ("***** STRIKE <CR> TO START, STOP THE PROGRAM *****");
  getchar();


  int tour=1;
  n=nbEquipes/2;
  while (n>0)         // Tant qu'il reste des matchs à jouer
  {
    // création de n processus
    int idMatch=nFork(n);
    if(idMatch==-1){
        perror("Creation de processus");
        cleanup(7, key+4, nbtour);
        return 7;
    }
    if(idMatch>0){
        /* fils */
        // Si l'argument 3 du programme est "-man"
        if (strcmp(argv[3],"-man")==0)
          simule_man(idMatch, tour, fd2, dur, f_csv, detail);  
        else
          // Lance la simulation pour le match correspondant
          simule(idMatch, tour, fd2, dur, f_csv, detail);
        exit(0);
    }
    // Réduit de moitié le nombre de matchs restants et incrémente le numéro de tour
    n=n/2;
    tour++;
  }

  /* père */
  // attends la fin de tous les processus fils
  for (int i = 0; i < (nbEquipes-1); i++)
  {
    while(waitpid(0,0,0) < 0);
  }

  // recuperer le gagnant du tournoi dans le tableau 'shared'
  int indice_winner = 0;
  while (indice_winner < nbEquipes && shared->tab[indice_winner].status == 0) {
      indice_winner++;
  }
  // afficher le gagnant    
  printf("Le gagnant du tournoi : \033[32m %s \033[0m \n",shared->tab[indice_winner].name);
  printf("\033[31mFIN DU TOURNOI \t \033[0m\n");

  /* attend maintenant que l'utilisateur tape CR, pour arrêter toutes les tâches */
  getchar();
  sleep(1); 
  close(fd2);
  close(f_csv); 
  cleanup(0, key, nbtour);
  free(teams);

  return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "list.h"

int main(int argc, char *argv[]){    

    int fd;
    ssize_t n;
    char buffer[BUFFER_SIZE];
    char* teams[TEAMS];
    int numTeams=0;
    List l=new_list();

    if (argc!=2) {printf("Veuillez fournir le nom du fichier en argument.\n"); return 1;}

    /* création (ou écrasement) du fichier "toto" */
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) { perror("open"); return 2; }

    // Lecture du fichier ligne par ligne
    while ((n = read(fd, buffer, BUFFER_SIZE)) > 0) {
        // Traitement de chaque ligne
        for (int i = 0; i < n; i++) {
            if (buffer[i] == '\n') {
                // Stockage de la ligne dans le tableau
                teams[numTeams] = malloc(i+1);
                strncpy(teams[numTeams], buffer, i+1);
                numTeams++;
            }
        }
    }

    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));
    
    // Fermeture du fichier
    if (close(fd)==-1) {perror("close"); return 4;}

    // Mélange des lignes dans le tableau
    for (int i = numTeams-1; i > 0; i--) {
        int j = rand() % (i+1);
        char* temp = teams[i];
        teams[i] = teams[j];
        teams[j] = temp;
    }

    for (int i = 0; i < numTeams; i++) {
        l=push_back_list(l, teams[i]);
        free(teams[i]);
    }

    return 0;
}
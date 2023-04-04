
# Simulation d'un tournoi sportif

Ce projet est réalisé dans le cadre de l’UE Système D'Exploitation du Semestre 6 de la L3 Informatique.
Le projet consiste à développer un logiciel de simulation de tournoi sportif, il doit gérer un nombre d'équipes fixé en entrée et chaque match doit être simulé sur un processus particuliers. Les scores seront affichés en temps réel et un fichier de sortie sera généré à la fin du tournoi.
La simulation doit prendre en compte la synchronisation des matches, où un match du tour N+1 ne peut pas démarrer tant que les deux équipes concernées n'ont pas terminé leur match du tour N.

Le programme que nous avons réalisé peut être lancé via une console sur un système d'exploitation Unix après avoir installer les bibliothéques nécessaires.

## Contributeurs

- [@saber-07](https://github.com/saber-07)
- [@Saiid12](https://github.com/Saiid12)
- [@NazimKs](https://github.com/NazimKs)

## 🚀 À propos de Nous

Nous sommes Said MESBAHI, Nazim KESKES , Saber ABDERRAHMANE, etudiants en L3 Informatique à CY Cergy Paris Université.

## Lancement du Programme

## Compilation

La compilation de ce programme nécessite gcc, makefile. Pour la documentation, doxygen et graphviz sont requis.

Si l'un des exécutables est manquant, il faut l'installer avec la commande

```
sudo apt install nom_de_l_executable_a_installer
```

### Programme

Pour (re)compiler le programme, il faut utiliser une des  commandes suivantes :

```
make main
```

Pour lancer le programme en mode automatique
```
./main [nombre d'équipes] [fichier d'entrée] -auto
```

Pour lancer le programme en mode manuel
```
./main [nombre d'équipes] [fichier d'entrée] -man 
```

Pour lancer le programme avec un nombre d'équipes par défaut

```
./main [fichier d'entrée]
```

Pour lancer le programme avec un nombre d'équipes et une liste d'équipe par défaut

```
./main
```

### Documentation

Une documentation Doxygen est disponible. Une fois générée, celle-ci se trouvera dans le dossier `doc`. Pour la générer, il suffit d'entre la commande

```
make doxygen
```

Pour re-générer la documentation, il faut utiliser les commandes

```
make clean
make doxygen
```

### Remarque

Pour compiler le programme et la documentation en même temps, la commande suivante peut être utilisée.

```
make all
```

Pour nettoyer le répertoire, il y a la commande

```
make clean
```

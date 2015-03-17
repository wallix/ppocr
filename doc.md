# Séparation des caractères

La première phase consiste à transformer l'image en noir et blanc et supprimmer l'aliasing. Ensuite, l es caractères sont coupés via des lignes vertivales si aucuns pixels de caractère à gauche n'est proche d'un pixel de caractère à droite.

....
..?. (x+1, y-1) or
.@?. (x+1, y  ) or
..?. (x+1, y+1)
....

..........           ......│....
...@@@.@..           ...@@@│.@..
..@....@..           ..@...│.@..
.@....@@@.           .@....│@@@.
.@.....@.. 2 lettres .@....│.@..
.@.....@..           .@....│.@..
..@....@..           ..@...│.@..
...@@@.@@.           ...@@@│.@@.
..........           ......│....

.........            .........
.@....@..            .@....@..
.@....@..            .@....@..
.@...@@@.            .@...o@@.
.@@@@.@..  1 lettre  .@@@o.@..
.@..@.@..            .@..@.@..
.@..@.@..            .@..@.@..
.@@@@.@@.            .@@@@.@@.
.........            .........


Pour le moment, il n'y a pas de travaille dessus. Mais le caractère pourrait être redécoupé si la largeur est considéré trop grande ou quand les résultats optenus sont incohérents.


# Déterminer le caractère

Il existe plusieurs méthodes qui s'appliquent:
 - sur un caractère isolé
 - sur une connaissance typographique
 - sur un contexte ou post-traitement

Note: les exemples qui suivent se feront avec le caractère suivant:

...............
.@@.........@@.
..@@.......@...
...@@.....@....
....@@...@.....
.....@@.@......
......@@@......
.....@.@.@.....
.....@...@.....
......@@@......
...............

## Caractère isolé

Permet d'extraire des informations sur une lettre sans connaissance de son environnement.

### Alternance de pixel noir/blanc

L'image est traversé par plusieurs lignes verticales et horizontales et les alternances noir/blanc sont gardés.

...............
.@@.........@@.                   ┌─────────────┐
..@@.......@...                   │@@.........@@│
┌─────────────┐                   │.@@.......@..│ @.@
│..@@.....@...│  .@.@.            │..@@.....@...│
└─────────────┘                   └─────────────┘
....@@...@.....                   ....@@...@.....
.....@@.@......                   .....@@.@......
┌─────────────┐                   ......@@@......
│.....@@@.....│  .@.              ┌─────────────┐
└─────────────┘                   │....@.@.@....│
.....@.@.@.....                   │....@...@....│ .@.
.....@...@.....                   │.....@@@.....│
......@@@......                   └─────────────┘
...............


      .@.@                             @.         @.

.......┌─┐.......                    ┌────┐.....┌────┐
.@@....│.│....@@.                    │@@..│.....│..@@│
..@@...│.│...@...                    │.@@.│.....│.@..│
...@@..│.│..@....                    │..@@│.....│@...│
....@@.│.│.@.....                    │...@│@...@│....│
.....@@│.│@......                    │....│@@.@.│....│
......@│@│@......                    │....│.@@@.│....│
.....@.│@│.@.....                    │....│@.@.@│....│
.....@.│.│.@.....                    │....│@...@│....│
......@│@│@......                    │....│.@@@.│....│
.......└─┘.......                    └────┘.....└────┘


### Orientation des pixels

Le caractère est couper en 4 parties égales et seul la partie la plus grande est gardée.

.......│ │.......
.@@....│ │....@@.    La partie en haut à gauche est la plus peuplé
..@@...│ │...@...
...@@..│ │..@....
....@@.│ │.@.....
───────┘ └───────
───────┐ ┌───────
......@│ │@......
.....@.│ │.@.....
.....@.│ │.@.....
......@│ │@......
.......│ │.......

D'autres alternatives peuvent être utilisés:
 - garder la partie la moins peuplé (pas encore fait)
 - couper en diagonale


### Centre de gravité

Chaque pixel possède un poid de plus en plus grand en fonction de la distance par rapport au centre.

L'angle est gardé, pas la direction (ni la distance<!-- TODO à faire -->).

-6 -5 -4 -3 -2 -1 0 1 2 3 4 5 6
..  .  .  .  .  . . . . . . . ..
.@  @  .  .  .  . . . . . . @ @.  4
..  @  @  .  .  . . . . . @ . ..  3
..  .  @  @  .  . . . . @ . . ..  2
..  .  .  @  @  . . . @ . . . ..  1
..  .  .  .  @  @ + @ . . . . ..  0
..  .  .  .  .  @ @ @ . . . . .. -1
..  .  .  .  @  . @ . @ . . . .. -2
..  .  .  .  @  . . . @ . . . .. -3
..  .  .  .  .  @ @ @ . . . . .. -4
..  .  .  .  .  . . . . . . . ..


<!-- ### Fût
https://fr.wikipedia.org/wiki/F%C3%BBt_%28typographie%29

https://fr.wikipedia.org/wiki/Contreforme


-->

### Proportion

Par exemple, '(' est beaucoup plus grand que 'c' mais proportionellement moins large.



## Connaissance typographique

Les connaissances typographique les plus "facile" à optenir sont la ligne de pied et l'hauteur d'x. Elle permettent principalement de différencier minuscule et majuscule (p/P, o/O).

Ses lignes sont déterminées en fonction des lettres déjà trouvées via les méthodes précédentes mais peuvent aussi être enregistré pour des jeux de données identiques (il est peu probable que le style d'une barre de titre change).

La connaissance de la police utilisé permet également de réduire le périmètre de recherche et revenir dans un contexte plus général si les résultats optenus ne sont pas satisfaisant.


<!-- Une métrique utile pourrait être de connaître la lettre sans la différence min/maj -->

<!-- Il reste la ligne de jambage, taille du corps, la hauteur de capitale, ascendantes, descendantes, talus

https://fr.wikipedia.org/wiki/Corps_%28typographie%29#/media/File:Corps_typo.svg


 les glyphes des caractères a, c, e, m, n, o, r, s et u peuvent légèrement dépasser ces lignes théoriques, pour des raisons d’équilibre optique.
-->


## Post-traitement

Parmit les post-traitements possible, on trouve
 - la distinction entre lettres latines ou cyrilliques en fonction des lettres déjà trouvées
 - l'utilisation d'un dictionnaire


<!--# Apprentissage


# Format

## actuel
## à venir-->



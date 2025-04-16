# Projet Tetris - notes : 

## Fonctionnement de SDL

- Dessin du board avec un Renderer
- Avoir un tableau 2D qui représente le board sans la pièce en cours
- Fonction qui permet de dessiner une pièce à des coordonnées données
- Fonction qui permet d'updater le board 


## Règles du jeu

- Plateau de dimensions 10*40 blocs, mais on ne laisse apparaître que 20 blocs verticaux

- Les pièces doivent avoir des couleurs différentes

- Couleurs des pièces (les lettres correspondent à leur forme) : 
    - I : Cyan
    - O : Jaune
    - T : Violet
    - S : Vert
    - Z : Rouge
    - J : Bleu
    - L : Orange

- Il y a des points de rotation spéciaux pour chaque pièce, il faut en prendre compte


## Process

### Boucle de gameplay

- Faire apparaître les pièces à un index unique d'apparition (coin haut-droit de la pièce)
    - Si ce n'est pas possible de faire apparaître la pièce, faire perdre le joueur

- Faire descendre la pièce d'un bloc à intervalle régulier
    - Après chaque mouvement vers le bas d'une pièce, vérifier si elle peut encore descendre ou non
        - Si oui, tout se passe normalement (update des tableaux)
        - si non, la partie est perdue à la fin de l'intervalle en cours
    - Alternative (plus simple, moins de vérifications à effectuer)
        - Si l'indice du coin haut-gauche de la pièce est au-dessus de la partie visible, et que la pièce ne peut pas descendre, alors la partie est perdue (le check se fait seulement si on dépasse le visible)

- Ecouter pour d'éventuels inputs de l'utilisateur (A faire tout le temps dès qu'une pièce est visible)
    - Touches **Enter** ou **Space** : Rotation de la pièce (90° vers la gauche)
    - Touches **Q** et **Left_Arrow** : Déplacement de la pièce vers la gauche
    - Touches **D** et **Right_arrow** : Déplacement de la pièce vers la droite
    - Touche **S** et **Down_Arrow** : Réduction de l'intervalle de descente d'un bloc tant que la touche est pressée
    - Touche **Escape** : Fermeture de la fenêtre de jeu

- Condition de perte
    - Un des carrés de la pièce placée dépasse de l'écran 


### Déplacement et rotation d'une pièce

- Cette section est pertinente seulement pour les déplacements engendrés par les inputs utilisateur (excepté les touches **S**, **Down_Arrow** et **Escape**, qui n'ont pas d'incidence sur le comportement classique d'une pièce)

- **Simulation d'un déplacement ou d'une rotation**
    - Sauvegarder la position courante de la pièce
    - Effacer la pièce du tableau de simulation
    - Vérifier si la pièce hypothétiquement déplacée se superpose ou non avec une autre
        - Si oui, reprendre la position précédente (aucune incidence sur la vue)
        - Si non, permettre le déplacement et mettre à jour les données des deux tableaux

- **Implications de l'utilisation des tableaux carrés pour le stockage des pièces lors de la rotation**
    - La rotation est effectuée directement dans le tableau contenant la pièce
        - Il faut garder en mémoire vive un indice de rotation (pour revenir en arrière si mouvement invalide), ou deux versions de la pièce (position courante et après rotation)
        - Lors de la vérification de validité d'une position, calculer les positions occupées par les cubes uniques de la pièce en fonction de son haut gauche (origine de la pièce) et des chiffres dans le tableau de la pièce

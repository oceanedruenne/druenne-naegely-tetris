# Projet Tetris - notes : 

## Process 

- Récupérer les différentes pièces dans une image bitmap

- WIP...



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

- Location de départ des pièces (**Pas très important**)

- Pour la descente d'une pièce, pour éviter que la détection d'autres pièces ne pause problème avec elle-même
    - Sauvegarder la position de la pièce
    - Effacer la pièce du tableau de simulation
    - Vérifier si la pièce déplacée se superpose ou non avec une autre (fonction à programmer)
        - Si oui, reprendre la position précédente
        - Si non, permettre le déplacement et mettre à jour les données des deux tableaux

- Si on utilise des tableaux carrés pour le stockage des pièces
    - faire tourner les pièces dans leur tableau carré respectif ?
        - Implications :
            - garder soit un indice de rotation, soit deux versions de la pièce (position courante et suivante)
            - Lors de la vérification de validité d'une position, calculer les positions occupées par les cubes uniques de la pièce en fonction de son haut gauche (origine de la pièce) et des chiffres dans le tableau de la pièce







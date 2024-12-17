# crypt-sym-projet-avance-automne-2024
Lien du dépôt GitHub: https://github.com/Avita-jpg/crypt-sym-projet-avance-automne-2024/
Groupe 10

## Remarques:
### sym_crypt (Partie 1)
Il faut faire attention à l'encodage des fichiers en entrée et sortie lors du chiffrage et déchiffrage si on est sous Windows en utilisant WSL: faire attention que tout soit en LF qui est l'encodage des sauts à la ligne sous Unix, car les fichiers crées sous Windows sont automatiquement en CRLF.
### Crack (Partie 3)
Le message original doît suivre les fréquences d'apparition de lettres pour que crack c2 ait un effet.
Les textes en français prennent beaucoup plus longtemps que les textes en anglais.
Il faut connaître la taille de la clé recherchée. On pourrait imaginer utiliser ce programme dans une boucle pour parcourir les longueurs de clé possibles.
Il faut que toutes les caractères soient encodés en 1 octet. Pour cette raison, les textes avec accent ne sont pas supportés.
On essaye de trouver une clé d'un message chiffré seulement avec la méthode xor.

## Problèmes connus:
### sym_crypt (Partie 1)
Les fichiers images parfois ne marchent pas avec la méthode cbc.
### Crack (Partie 3)
Rechercher une clé de longueur égale ou supérieure à 6 produit des exécutions innatendues. Nous pensons que c'est dû à la grande quantité de mémoire utilisée et peut être une mauvaise gestion des listes chaînées.
Le programme principal peut produire des exécutions innnatendues si les commandes tappées par l'utilisateur ne respectent pas le format, l'ordre ou le nombre des arguments.
Pour réduire le nombre de clées testées en crack c3, nous n'avons pas une méthode qui fonctionne tout le temps. Parfois on peut rater la clé à cause d'une division incorrecte de l'ensemble de clés possibles. Nous avons exploré un peu les possibilités en nous basant sur des méthodes statistiques (comme la méthode des centilles ou de la distance relative) mais par manque de temps nous ne l'avons pas implémenté. 



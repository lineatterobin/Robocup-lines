Robocup-lines
=============

Compilation
-----------

Un fichier `.pro` se trouve à la racine du projet :

    qmake
    make

Exécution
---------

Le programme prend comme paramètres cinq fichiers XML contenant respectivement :

- les images sources à traiter,
- les images résultat du programme de détection du terrain,
- les images résultat du programme de détection de la balle,
- les vérités terrain,
- les chemins de destination des images résultats.

Des fichiers XML exemples se trouvent dans le dossier `images/`.

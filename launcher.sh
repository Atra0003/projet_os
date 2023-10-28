#!/bin/bash

# Vérification du nombre d'arguments
if [ $# -lt 2 ]; then
    echo "Utilisation : $0 [-i | --interactive | -a | --automatic] chemin_image [autre_argument]"
    exit 1
fi

# Vérification du chemin de l'image
if ! [ -f "$2" ]; then
    echo "Le paramètre 2 n'est pas un fichier image valide."
    exit 2
fi

if [ "$1" == "-i" -o "$1" == "--interactive" ]; then # Mode interactif
    bankImage=() # Création d'une banque d'images.

    while true; do
        read -p "Entrer le chemin d'une image (Ctrl+D pour quitter) : " input

        if [ -z "$input" ]; then
            echo -e "\nFin de la préparation de la banque d'images."
            break
        fi

        # Vérification de la présence du fichier dans le dossier.
        if [ -f "$input" ]; then
            echo "Le fichier est présent dans le dossier."
            bankImage+=("$input") # Ajout d'image dans la banque d'images.
        else
            echo "Le fichier n'est pas présent dans le dossier."
        fi
    done
    # Appeler img-search.
elif [ "$1" == "-a" -o "$1" == "--automatic" ]; then # Mode automatique
    # Appeler img-search.
    make -B

    # Vérifie si la compilation a réussi
    if [ $? -eq 0 ]; then
        echo "Compilation réussie."
        ./img-search $2 $3
        # Exécute votre programme ou effectue d'autres actions si nécessaire
    else
        echo "Échec de la compilation."
    fi


else
    echo "Option non reconnue : $1"
    exit 3
fi

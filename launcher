#!/bin/bash

# Vérification du nombre d'arguments
if [ $# -lt 2 ]; then
    # echo "Utilisation : $0 [-i | --interactive | -a | --automatic] chemin_image [autre_argument]"
    echo "No similar image found (no comparison could be performed successfully)."
    exit 1
fi


# Vérification du chemin de l'image
if find img -type f -path "$2" -print | grep -q "$2"; then

    if [ "$1" == "-i" -o "$1" == "--interactive" ]; then # Mode interactif
    bankImage=() # Création d'une banque d'images.

    while true; do
        read -p "Entrer le chemin d'une image (Ctrl+D pour quitter) : " input

        if [ -z "$input" ]; then # Fin de la préparation de la banque d'images.
            break
        fi

        # Vérification de la présence du fichier dans le dossier.
        if find img -type f -path "$input" -print | grep -q "$input"; then
            echo "Le fichier est présent dans le dossier."
            bankImage+=("$input") # Ajout d'image dans la banque d'images.
        else
            echo "Le fichier n'est pas présent dans le dossier."
        fi
    done
    make -B

    if [ $? -eq 0 ]; then # Compilation réussie.
        ./img-search $2 $bankImage 
        # Exécute votre programme ou effectue d'autres actions si nécessaire
    fi
fi

else
    # echo "Le paramètre 2 (image) n'est pas une image du dossier image."
    echo "No similar image found (no comparison could be performed successfully)."
    #exit 1
fi

# === automatique === 


if [ "$1" == "-a" -o "$1" == "--automatic" ]; then
    if [ ! -d "$3" ]; then # Le paramètre 3 n'est pas un dossier 
        exit 1
    fi

    make -B
    #Vérifie si la compilation a réussi
    if [ $? -eq 0 ]; then
        ./img-search $2 $3 # Compilation réussie. 
    fi

fi
#!/bin/bash

if [ $# -eq 1 ]; then
    if [ -d $1 ]; then 
        for file in "$1"/*; do
            if [ -f "$file" ]; then
                echo "$(basename "$file")"
            fi
        done
    else
        echo "The specified path is not a directory."
        exit 2
    fi
elif [ $# -eq 0 ]; then
    echo "Missing directory name."
    exit 1
fi



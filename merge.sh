#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <first_dir> <ancestor_dir> <second_dir>"
    echo "Example: $0 first/ second/ third/"
    exit 1
fi

FIRST="$1"
ANCESTOR="$2"
SECOND="$3"

for file in "$FIRST"/*; do
	if [ -f "$file" ]; then

        filename=$(basename "$file")
        second="$SECOND/$filename"
        ancestor="$ANCESTOR/$filename"

        touch "$ancestor"

        if [ -f "$second" ] && [ -f "$ancestor" ]; then
            git merge-file "$file" "$ancestor" "$second"

            if [ $? -eq 0 ]; then
                echo "[+] Merged $filename\n"
            elif [ $? -eq 1 ]; then
                echo "[-] Conflict in $filename\n"
            else
                echo "Could not merge files\n"
            fi

        else
            echo "File: $filename was not found in $second or $ancestor\n"
        fi
    else 
        echo "Could not open file: $file\n"
    fi
done


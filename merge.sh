#!/bin/bash

if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <first_dir> <ancestor_dir> <second_dir> <subdir>"
    echo "Example: $0 first/ second/ third/ subdir/"
    exit 1
fi

FIRST="$1"
ANCESTOR="$2"
SECOND="$3"
SUBDIR="$4"

for file in "$FIRST/$SUBDIR"/*; do
	if [ -f "$file" ]; then

        filename=$(basename "$file")
        second="$SECOND/$SUBDIR/$filename"

        touch "$filename"

        if [ -f "$second" ] && [ -f "$filename" ]; then
            git merge-file "$file" "$filename" "$second"

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
	rm "$filename"
    else 
        echo "Could not open file: $file\n"
    fi
done

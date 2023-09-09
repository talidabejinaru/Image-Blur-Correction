#!/bin/bash

folder1="./blurry"
folder2="./results"

for file in "$folder1"/*; do
    base_file=$(basename "$file")
    if [ -f "$folder2/$base_file" ]; then
        ./main "$folder1/$base_file" "$folder2/$base_file"
    fi
done

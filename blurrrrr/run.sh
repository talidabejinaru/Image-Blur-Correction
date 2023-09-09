#!/bin/bash

# folder="./images" # Replace with the path to the folder

# for file in "$folder"/*.jpg; do
	# filename=$(basename "$file")
	# extension="${filename##*.}"
	# filename="${filename%.*}"
	# convert "$file" "$folder/$filename.png"
	# rm "$file"
# done

#!/bin/bash

input_folder="./blurry" # Replace with the path to the input folder
output_folder="./results" # Replace with the path to the output folder

# Iterate through all PNG files in the input folder
for file in "$input_folder"/*.png; do
    # Get the filename without the path
    filename=$(basename "$file")

    # Convert image to grayscale
    convert "$file" -colorspace Gray "$output_folder/$filename"

    # Add noise to the image
    convert "$output_folder/$filename" -noise 5 "$output_folder/$filename"

done

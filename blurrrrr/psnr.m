# Define the folder containing the images
folder = 'D:\workspace\blurrrrr\images\';
dest   = 'D:\workspace\blurrrrr\results\';

# Get a list of all PNG files in the folder
image_files = dir(strcat(folder, '*.png'));

# Loop through each image file
for i = 1:length(image_files)
    # Load the original image
    original = imread(strcat(folder, image_files(i).name));

    # Load the compressed image with the same name
    compressed = imread(strcat(dest, image_files(i).name));

    # Calculate the mean squared error
    mse = sum((original(:) - compressed(:)).^2) / numel(original);

    # Calculate the PSNR
    psnr = 10 * log10(255^2 / mse);

    # Print the PSNR and the file name
    fprintf("%10s: %f\n", image_files(i).name, psnr)
end


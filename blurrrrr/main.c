#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICT_SIZE 100 // size of dictionary
#define SPARSE_CODE_SIZE 20 // maximum number of non-zero coefficients

#include "stb_image.h"

void load_image(const char* file_name, float** data, int* width, int* height, int* channels)
{
    // Load image using stb_image
    *data = stbi_loadf(file_name, width, height, channels, 0);
    if (!*data)
    {
        printf("Error: Unable to load image %s\n", file_name);
        exit(1);
    }
}

#include "stb_image_write.h"

void save_image(const char* file_name, float* data, int width, int height, int channels)
{
    // Convert data to unsigned char
    unsigned char* char_data = (unsigned char*)malloc(width * height * channels * sizeof(unsigned char));
    for (int i = 0; i < width * height * channels; i++)
        char_data[i] = (unsigned char)(data[i] * 255);

    // Save image using stb_image_write
    if (!stbi_write_png(file_name, width, height, channels, char_data, width * channels))
    {
        printf("Error: Unable to save image %s\n", file_name);
        exit(1);
    }

    // Clean up
    free(char_data);
}


void load_image(const char* file_name, float** data, int* data_size)
{
    // Open file
    FILE* file = fopen(file_name, "r");
    if (!file)
    {
        printf("Error: Unable to open file %s\n", file_name);
        exit(1);
    }

    // Count number of data points
    *data_size = 0;
    float value;
    while (fscanf(file, "%f", &value) == 1)
        (*data_size)++;

    // Allocate memory for data
    *data = (float*)malloc(*data_size * sizeof(float));

    // Read data from file
    rewind(file);
    for (int i = 0; i < *data_size; i++)
        fscanf(file, "%f", &(*data)[i]);

    // Close file
    fclose(file);
}

void normalize(float* data, int data_size)
{
    float norm = 0;
    for (int i = 0; i < data_size; i++)
        norm += data[i] * data[i];
    norm = sqrt(norm);
    for (int i = 0; i < data_size; i++)
        data[i] /= norm;
}

void ksvd(float* data, int data_size, float* dict, int dict_size, float* sparse_coef, int sparse_coef_size)
{
    // Initialize dictionary with random values
    for (int i = 0; i < dict_size; i++)
        for (int j = 0; j < data_size; j++)
            dict[i*data_size + j] = (float)rand() / RAND_MAX;

    // Iterative process to update dictionary and sparse coefficients
    for (int iter = 0; iter < MAX_ITER; iter++)
    {
        // sparse representation step
        bomp(data, data_size, dict, dict_size, sparse_coef, sparse_coef_size);

        // dictionary update step
        for (int i = 0; i < dict_size; i++)
        {
            // Find the data points associated with the current dictionary atom
            int associated_data_points[data_size];
            int num_associated_data_points = 0;
            for (int j = 0; j < data_size; j++)
                if (sparse_coef[i*data_size + j] != 0)
                    associated_data_points[num_associated_data_points++] = j;

            // Update the current dictionary atom
            for (int j = 0; j < data_size; j++)
                dict[i*data_size + j] = 0;
            for (int j = 0; j < num_associated_data_points; j++)
                for (int k = 0; k < data_size; k++)
                    dict[i*data_size + k] += sparse_coef[i*data_size + associated_data_points[j]] * data[associated_data_points[j]*data_size + k];
            normalize(dict + i*data_size);
        }
    }
}

void bomp(float* data, int data_size, float* dict, int dict_size, float* sparse_coef, int sparse_coef_size)
{
    for (int i = 0; i < data_size; i++)
    {
        // Initialize sparse coefficients to zero
        for (int j = 0; j < dict_size; j++)
            sparse_coef[i*dict_size + j] = 0;

        // Find the atom in the dictionary that is most correlated with the current data point
        int max_correlation_index = 0;
        float max_correlation = -1;
        for (int j = 0; j < dict_size; j++)
        {
            float correlation = 0;
            for (int k = 0; k < data_size; k++)
                correlation += data[i*data_size + k] * dict[j*data_size + k];
            if (correlation > max_correlation)
            {
                max_correlation = correlation;
                max_correlation_index = j;
            }
        }
        // Set the corresponding sparse coefficient to the value of the correlation
        parsing_coef[i*dict_size + max_correlation_index] = max_correlation;

        // Orthogonalize the remaining atoms
        for (int j = 0; j < dict_size; j++)
        {
            if (j == max_correlation_index)
                continue;
            float projection = 0;
            for (int k = 0; k < data_size; k++)
                projection += dict[j*data_size + k] * data[i*data_size + k];
            for (int k = 0; k < data_size; k++)
                dict[j*data_size + k] -= projection * dict[max_correlation_index*data_size + k];
        }
    }
}

int main(int argc, char* argv[])
{
    // Load noisy image
    float* noisy_image;
    int image_size;
    load_image(argv[1], &noisy_image, &image_size);

    // Initialize dictionary and sparse coefficients
    float* dict = (float*)malloc(DICT_SIZE*image_size*sizeof(float));
    float* sparse_coef = (float*)malloc(image_size*DICT_SIZE*sizeof(float));

    // Perform K-SVD
    ksvd(noisy_image, image_size, dict, DICT_SIZE, sparse_coef, SPARSE_CODE_SIZE);

    // Perform sparse representation
    bomp(noisy_image, image_size, dict, DICT_SIZE, sparse_coef, SPARSE_CODE_SIZE);

    // Reconstruct image
    float* denoised_image = (float*)malloc(image_size*sizeof(float));
    for (int i = 0; i < image_size; i++)
    {
        denoised_image[i] = 0;
        for (int j = 0; j < DICT_SIZE; j++)
            denoised_image[i] += dict[j*image_size + i] * sparse_coef[j*image_size + i];
    }

    // Save denoised image
    save_image(argv[2], denoised_image, image_size);

    // Clean up
    free(noisy_image);
    free(dict);
    free(sparse_coef);
    free(denoised_image);

    return 0;
}

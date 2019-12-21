#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pixel {
    unsigned char r, g, b;
} Pixel;

typedef struct image {
    unsigned int width, height, max;
    Pixel **data;
} Image;

typedef struct kernel {
    unsigned int n, scale;
    int** table;
} Kernel;

Image *readPPM(char *file_name){

    FILE *file = fopen(file_name, "r");
    if (!file){
        fprintf(stderr, "Unable to open file \"%s\"\n", file_name);
        printf("UNABLE TO OPEN FILE\n");
        return NULL;
    }

    char format[3];
    fscanf(file, "%s\n", format);
    if(strcmp(format, "P3")){
        printf("P3 COMPARISON FAILED\n");
        return NULL;
    }

    Image *image = malloc(sizeof(Image));

    if(fscanf(file, "%u %u %u", &image->width, &image->height, &image->max) != 3){ 
        printf("FSCANF DID NOT RETURN 3 for width height and max\n");
        return NULL;
    }

    image->data = malloc(sizeof(Pixel *) * image->height);
    int i;
    for(i = 0; i < image->height; i++){
        image->data[i] = malloc(sizeof(Pixel) * image->width);
    }

    int j;
    int k;
    for(j = 0; j < image->height; j++){
        for(k = 0; k < image->width; k++){
            int pixels_read = fscanf(file, "%hhu %hhu %hhu", &image->data[j][k].r, &image->data[j][k].g, &image->data[j][k].b);
            if(pixels_read != 3){
                return NULL;
            }
        }
    }

    fclose(file);
    return image;

}

Image* createOutput(unsigned int width, unsigned int height, unsigned int max){

    Image *image = malloc(sizeof(Image));

    image->width = width;
    image->height = height;
    image->max = max;

    image->data = malloc(sizeof(Pixel *) * image->height);
    int i;
    for(i = 0; i < image->height; i++){
        image->data[i] = malloc(sizeof(Pixel) * image->width);
    }

    return image;

}

Kernel* readKernel(char* fileName){

    FILE *file = fopen(fileName, "r");
    if (!file){
        fprintf(stderr, "Unable to open kernel\n");
        return NULL;
    }

    Kernel *kernel = malloc(sizeof(Kernel));

    if(fscanf(file, "%u %u", &kernel->n, &kernel->scale) != 2){ 
        return NULL;
    }

    kernel->table = malloc(sizeof(int *) * kernel->n);
    int i;
    for(i = 0; i < kernel->n; i++){
        kernel->table[i] = malloc(sizeof(int) * kernel->n);
    }

    int j;
    int k;
    for(j = 0; j < kernel->n; j++){
        for(k = 0; k < kernel->n; k++){
            int values_read = fscanf(file, "%d", &kernel->table[j][k]);
            if(values_read != 1){
                printf("Error reading kernel table");
                return NULL;
            }
        }
    }

    return kernel;

}

int writePPM(char *file_name, Image *image){

    FILE *file = fopen(file_name, "w");
    if (!file){
        fprintf(stderr, "Unable to open file \"%s\"\n", file_name);
        return -1;
    }

    fprintf(file, "P3\n");
    fprintf(file, "%u %u\n", image->width, image->height);
    fprintf(file, "%u\n", image->max);

    int i;
    int j;
    for (i = 0; i < image->height; i++){
        for (j = 0; j < image->width; j++){
            fprintf(file, "%u %u %u ", image->data[i][j].r, image->data[i][j].g, image->data[i][j].b);
        }
        fprintf(file, "\n");
    }    

    fclose(file);
    return 0;

}

int main(int argc, char** argv){

    if(argc != 4){
        printf("Incorrect number of arguments\n");
        return -1;
    }

    Image* img = readPPM(argv[1]);
    if(!img){
        printf("Error reading input PPM\n");
        return -1;
    }
    Image* output = createOutput(img->width, img->height, img->max);
    Kernel* kernel = readKernel(argv[2]);

    int sumR;
    int sumG;
    int sumB;

    int i;
    int j;
    int y;
    int x;

    for(i = 0; i < img->height; i++){
        for(j = 0; j < img->width; j++){

            sumR = 0;
            sumG = 0;
            sumB = 0;
            int balance = ((kernel->n) - 1) / 2;

            for(y = -1 * (balance); y <= balance; y++){
                for(x = -1 * (balance); x <= balance; x++){
                    if((i + y) >= 0 && (i + y) < img->height && (j + x) >= 0 && (j + x) < img->width){
                        sumR += (img->data[i + y][j + x].r) * kernel->table[y + balance][x + balance];
                        sumG += (img->data[i + y][j + x].g) * kernel->table[y + balance][x + balance];
                        sumB += (img->data[i + y][j + x].b) * kernel->table[y + balance][x + balance];
                    }
                }
            }

            sumR /= kernel->scale;
            sumG /= kernel->scale;
            sumB /= kernel->scale;

            if(sumR < 0) sumR = 0;
            if(sumG < 0) sumG = 0;
            if(sumB < 0) sumB = 0;

            if(sumR > output->max) sumR = output->max;
            if(sumG > output->max) sumG = output->max;
            if(sumB > output->max) sumB = output->max;

            output->data[i][j].r = sumR;
            output->data[i][j].g = sumG;
            output->data[i][j].b = sumB;

        }
    }

    return writePPM(argv[3], output);

}
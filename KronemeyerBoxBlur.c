/*
 * Box Blurs a 24-bit BMP Image
 *
 * @author Kronemeyer, Joshua & Acuna, Ruben
 * @date 3-17-19
 *
 */


////////////////////////////////////////////////////////////////////////////////
//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

////////////////////////////////////////////////////////////////////////////////
//MACRO DEFINITIONS

#pragma warning(disable: 4996)

//problem assumptions
#define BMP_HEADER_SIZE_BYTES 14
#define BMP_DIB_HEADER_SIZE_BYTES 40
#define MAXIMUM_IMAGE_SIZE 256

//bmp compression methods
//none:
#define BI_RGB 0

// Number of Threads to use
#define NUM_OF_THREADS 4

////////////////////////////////////////////////////////////////////////////////
//DATA STRUCTURES
#pragma pack(push, 1)
struct BMP_Header {
	char signature[2];		//ID field
	unsigned int size;		        //Size of the BMP file
	short reserved1;		//Application specific
	short reserved2;		//Application specific
	unsigned int offset_pixel_array;  //Offset where the pixel array (bitmap data) can be found
};

struct DIB_Header {
    struct BMP_Header file_header;
    unsigned int h_size;
    int width;
    int height;
    short planes;
    short bpp;
    unsigned int comp;
    unsigned int size;
    int xppm;
    int yppm;
    unsigned int clrUsed;
    unsigned int clrImportant;
};
#pragma pack(pop)

struct BGR_data {
    unsigned int b;
    unsigned int g;
    unsigned int r;
};

struct args {
    unsigned char* bitmapImage;
    struct DIB_Header header;
    int size;
};



////////////////////////////////////////////////////////////////////////////////
//MAIN PROGRAM CODE
void* blur(void* arguments) {
    // Pull in our arguments
    struct args* arg = arguments;
    struct DIB_Header header = arg->header;
    int start = arg->size;
    unsigned char* blurredImage = (unsigned char*)malloc(header.size/NUM_OF_THREADS);
    unsigned char* bitmapImage = arg->bitmapImage;

    // Our pixel row size in bytes
    int bRowSize = header.width*3;

    // It is important for j to be independent of i for multithreading
    int j = 0;
    // LET THE "BLURRING" COMMENCE!
    for (int i = start-(header.size/NUM_OF_THREADS); i < start-1; i += 3) {
        struct BGR_data bgr = {0};
        // Is not a corner or side
        if((i%bRowSize) > 0 && (i-(bRowSize-3))%bRowSize > 0 && i < bRowSize*(header.height-1)) {
            bgr.b += bitmapImage[i];
            bgr.g += bitmapImage[i+1];
            bgr.r += bitmapImage[i+2];

            bgr.b += bitmapImage[i+3];
            bgr.g += bitmapImage[i+4];
            bgr.r += bitmapImage[i+5];

            bgr.b += bitmapImage[i-1];
            bgr.g += bitmapImage[i-2];
            bgr.r += bitmapImage[i-3];

            bgr.b += bitmapImage[i+bRowSize];
            bgr.g += bitmapImage[i+(bRowSize+1)];
            bgr.r += bitmapImage[i+(bRowSize+2)];

            bgr.b += bitmapImage[i+(bRowSize+3)];
            bgr.g += bitmapImage[i+(bRowSize+4)];
            bgr.r += bitmapImage[i+(bRowSize+5)];

            bgr.b += bitmapImage[i+(bRowSize-1)];
            bgr.g += bitmapImage[i+(bRowSize-2)];
            bgr.r += bitmapImage[i+(bRowSize-3)];

            bgr.b += bitmapImage[i-bRowSize];
            bgr.g += bitmapImage[i-(bRowSize+1)];
            bgr.r += bitmapImage[i-(bRowSize+2)];

            bgr.b += bitmapImage[i-(bRowSize-1)];
            bgr.g += bitmapImage[i-(bRowSize-2)];
            bgr.r += bitmapImage[i-(bRowSize-3)];

            bgr.b += bitmapImage[i-(bRowSize+3)];
            bgr.g += bitmapImage[i-(bRowSize+4)];
            bgr.r += bitmapImage[i-(bRowSize+5)];
            bgr.b /= 9;
            bgr.g /= 9;
            bgr.r /= 9;
        }
        // Left Side
        else if(i > bRowSize-1 && i < bRowSize*(header.height-1)) {
            bgr.b += bitmapImage[i];
            bgr.g += bitmapImage[i+1];
            bgr.r += bitmapImage[i+2];

            bgr.b += bitmapImage[i+3];
            bgr.g += bitmapImage[i+4];
            bgr.r += bitmapImage[i+5];

            bgr.b += bitmapImage[i+bRowSize];
            bgr.g += bitmapImage[i+(bRowSize+1)];
            bgr.r += bitmapImage[i+(bRowSize+2)];

            bgr.b += bitmapImage[i+(bRowSize+3)];
            bgr.g += bitmapImage[i+(bRowSize+4)];
            bgr.r += bitmapImage[i+(bRowSize+5)];


            bgr.b += bitmapImage[i-bRowSize];
            bgr.g += bitmapImage[i-(bRowSize+1)];
            bgr.r += bitmapImage[i-(bRowSize+2)];


            bgr.b += bitmapImage[i-(bRowSize+3)];
            bgr.g += bitmapImage[i-(bRowSize+4)];
            bgr.r += bitmapImage[i-(bRowSize+5)];
            bgr.b /= 6;
            bgr.g /= 6;
            bgr.r /= 6;
        }
        // Right side
        else if((i+3)%header.width == 0 && i != bRowSize-3 && i != header.size-3) {
            bgr.b += bitmapImage[i];
            bgr.g += bitmapImage[i-1];
            bgr.r += bitmapImage[i-2];

            bgr.b += bitmapImage[i-3];
            bgr.g += bitmapImage[i-4];
            bgr.r += bitmapImage[i-5];

            bgr.b += bitmapImage[i+bRowSize];
            bgr.g += bitmapImage[i+(bRowSize-1)];
            bgr.r += bitmapImage[i+(bRowSize-2)];

            bgr.b += bitmapImage[i+(bRowSize-3)];
            bgr.g += bitmapImage[i+(bRowSize-4)];
            bgr.r += bitmapImage[i+(bRowSize-5)];


            bgr.b += bitmapImage[i-bRowSize];
            bgr.g += bitmapImage[i-(bRowSize-1)];
            bgr.r += bitmapImage[i-(bRowSize-2)];


            bgr.b += bitmapImage[i-(bRowSize-3)];
            bgr.g += bitmapImage[i-(bRowSize-4)];
            bgr.r += bitmapImage[i-(bRowSize-5)];
            bgr.b /= 6;
            bgr.g /= 6;
            bgr.r /= 6;
        }
        // Bottom
        else if (i < (header.width*3)-3 && i != 0) {
            bgr.b += bitmapImage[i];
            bgr.g += bitmapImage[i+1];
            bgr.r += bitmapImage[i+2];

            bgr.b += bitmapImage[i+3];
            bgr.g += bitmapImage[i+4];
            bgr.r += bitmapImage[i+5];

            bgr.b += bitmapImage[i];
            bgr.g += bitmapImage[i-1];
            bgr.r += bitmapImage[i-2];

            bgr.b += bitmapImage[i-3];
            bgr.g += bitmapImage[i-4];
            bgr.r += bitmapImage[i-5];

            bgr.b += bitmapImage[i+bRowSize];
            bgr.g += bitmapImage[i+(bRowSize-1)];
            bgr.r += bitmapImage[i+(bRowSize-2)];

            bgr.b += bitmapImage[i+(bRowSize-3)];
            bgr.g += bitmapImage[i+(bRowSize-4)];
            bgr.r += bitmapImage[i+(bRowSize-5)];

            bgr.b /= 6;
            bgr.g /= 6;
            bgr.r /= 6;
        }
        // Top
        else if (i > bRowSize*(header.height-1) && i < header.size-4) {
            bgr.b += bitmapImage[i];
            bgr.g += bitmapImage[i+1];
            bgr.r += bitmapImage[i+2];

            bgr.b += bitmapImage[i+3];
            bgr.g += bitmapImage[i+4];
            bgr.r += bitmapImage[i+5];

            bgr.b += bitmapImage[i-1];
            bgr.g += bitmapImage[i-2];
            bgr.r += bitmapImage[i-3];

            bgr.b += bitmapImage[i-bRowSize];
            bgr.g += bitmapImage[i-(bRowSize+1)];
            bgr.r += bitmapImage[i-(bRowSize+2)];

            bgr.b += bitmapImage[i-(bRowSize-1)];
            bgr.g += bitmapImage[i-(bRowSize-2)];
            bgr.r += bitmapImage[i-(bRowSize-3)];

            bgr.b += bitmapImage[i-(bRowSize+3)];
            bgr.g += bitmapImage[i-(bRowSize+4)];
            bgr.r += bitmapImage[i-(bRowSize+5)];
            bgr.b /= 6;
            bgr.g /= 6;
            bgr.r /= 6;
        }
        // WHELP, MUST BE A CORNER
        else {
            bgr.b /= 4;
            bgr.g /= 4;
            bgr.r /= 4;
        }

        // Our new image
        blurredImage[j] = (unsigned char)bgr.b;
        blurredImage[j+1] = (unsigned char)bgr.g;
        blurredImage[j+2] = (unsigned char)bgr.r;
        j += 3;

    }
    return blurredImage;
}


int main(int argc, char* argv[]) {
    unsigned char* bitmapImage;
	pthread_t tid[NUM_OF_THREADS];
	char* filein = "../test1wonderbread.bmp";
	char* fileout = "../2.bmp";

	// Cmd Arguments
	if (argc > 2) {
	    filein = argv[1];
	    fileout = argv[2];
	}

	// Attempt to open input file
	FILE* file = fopen(filein, "rb");
	if (!file) {
	    printf("Opening file failed...");
	    return 1;
	}

	struct DIB_Header header;

	// Read in our BMP Header information
	fread(&header.file_header.signature, sizeof(char)*2, 1, file);
	fread(&header.file_header.size, sizeof(int), 1, file);
	fread(&header.file_header.reserved1, sizeof(short), 1, file);
	fread(&header.file_header.reserved2, sizeof(short), 1, file);
	fread(&header.file_header.offset_pixel_array, sizeof(int), 1, file);

	fread(&header.h_size, sizeof(int), 1, file);
	fread(&header.width, sizeof(int), 1, file);
	fread(&header.height, sizeof(int), 1, file);
    fread(&header.planes, sizeof(short), 1, file);
    fread(&header.bpp, sizeof(short), 1, file);
    fread(&header.comp, sizeof(int), 1, file);
    fread(&header.size, sizeof(int), 1, file);
    fread(&header.xppm, sizeof(int), 1, file);
    fread(&header.yppm, sizeof(int), 1, file);
    fread(&header.clrUsed, sizeof(int), 1, file);
    fread(&header.clrImportant, sizeof(int), 1, file);

    // Error for wrong header signature
    if (header.file_header.signature[0] != 'B' && header.file_header.signature[0] != 'M') {
        printf("This is not a bitmap file...");
        return 0;
    }
    // Error for files that support more than 3 bytes per pixel (i.e. rgba)
    if (header.size != (header.height*(header.width*3))) {
        printf("This image appears to have an alpha mask (or more values than just rgb per pixel), which is not supported.\n");
        printf("Expected: %d bytes Received: %d bytes", header.width*header.height*3, header.size);
        return 0;
    }

    // Print some basic image info
	printf("signature: %c%c\n", header.file_header.signature[0], header.file_header.signature[1]);
	printf("size: %d\n", header.file_header.size);
	printf("reserved1: %d\n", header.file_header.reserved1);
	printf("reserved2: %d\n", header.file_header.reserved2);
	printf("offset_pixel_array: %d\n", header.file_header.offset_pixel_array);
	printf("image width: %d\n", header.width);
	printf("image height: %d\n", header.height);
	printf("image size: %d\n", header.size);
	printf("Colors: %d\n", header.clrUsed);
	printf("Bits Per Pixel: %d\n", header.bpp);

	// Obtain Pixel Data
    bitmapImage = (unsigned char*)malloc(header.size);
    fseek(file, sizeof(char) * header.file_header.offset_pixel_array, SEEK_SET);
    fread(bitmapImage, sizeof(char), header.size, file);
    fclose(file);

    // Rev Up Those Fryers, because I am NUM_OF_THREADS ready to blur your image
    struct args arg[NUM_OF_THREADS];
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        arg[i].header = header;
        arg[i].bitmapImage = bitmapImage;
        arg[i].size = (header.size / NUM_OF_THREADS) * (i + 1);
        pthread_create(&tid[i], NULL, &blur, &arg[i]);
    }

    // Open and Write our 24-bit bitmap header
    FILE* out = fopen(fileout, "wb");
    if (!out) {
        printf("Opening file failed...");
        return 1;
    }
    fwrite(&header, sizeof(char), sizeof(header), out);
    fseek(out, sizeof(char) * header.file_header.offset_pixel_array, SEEK_SET);

    // All good things must come to an end, close threads and write to file
    for (int i = 0; i < NUM_OF_THREADS; i++) {
        unsigned char* temp;
        pthread_join(tid[i], (void**)&temp);
        fwrite(temp, sizeof(char), header.size/NUM_OF_THREADS, out);
    }

    free(bitmapImage);
    fclose(out);
    printf("Successfully blurred %s", filein);
	return 0;
}
#ifndef FRAMEIO_H
#define FRAMEIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct _rgb
{
    int w;
    int h;
    unsigned char *data;
} rgb;

// Function declarations
FILE* fio_OpenReadStream(const char *filename,int rows, int cols);
FILE* fio_OpenWriteStream(const char* filename,int rows,int cols);

void fio_WriteFrame(rgb *binframe, FILE *out);
int fio_ReadFrame(rgb *binframe, FILE *in);

void fio_close(FILE* fd);

int fio_imread(const char* filename, rgb *binframe, int rows, int cols);
void fio_imwrite(const char* filename, rgb *binframe);

#endif

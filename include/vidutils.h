#ifndef VIDUTILS_H
#define VIDUTILS_H

#include <stdlib.h>
#include <stdio.h>

/* Local libraries */
#include "netutils.h"
#include "frameio.h"

void build_i2f(float *i2f);
void split_filename(char *p, char *f, char *e, char *filename);
void bin2frame(rgb *binframe, image *frame, float *i2f);
void frame2bin(image *frame, rgb *binframe);
void write_boxes(FILE *fout, netinfo *rcnn, int cnt, int rows, int cols);

#endif

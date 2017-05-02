#ifndef DRAW_BOX_H
#define DRAW_BOX_H

#include "netutils.h"

/* Required Darknet libraries */
#include "image.h"

void draw_onebox(image *img, int left, int top, int width, int height, char *text, float *color, image **alphabet);
void draw_boxes(image *img, netinfo *rcnn, image **alphabet);
void generate_color(int class, int nclasses, float *color);

#endif

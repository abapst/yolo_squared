#include "draw_box.h"


void draw_onebox(image *img, int left, int top, int width, int height, char *text, float *color, image **alphabet)
{
    image label;
    int linewidth = img->h*.012;

    draw_box_width(*img,left,top,left+width,top+height,linewidth,color[0],color[1],color[2]);
    label = get_label(alphabet, text, (img->h*.03)/10);
    draw_label(*img,top+linewidth,left,label,color);
}

void draw_boxes(image *img, netinfo *rcnn, image **alphabet)
{
    int ii, jj, class, left, top, width, height;
    int rows = img->h;
    int cols = img->w;
    float prob = 0.;
    float color[3]; // RGB color
    box b;

    char text[1024];

    for (ii = 0; ii < rcnn->nboxes; ii++) {
        for (jj = 0; jj < rcnn->ntargets; jj++) {
			class = max_index(rcnn->probs[ii], rcnn->nclasses);
			prob = rcnn->probs[ii][class];
			if ((class == rcnn->targets[jj]) && (prob > rcnn->thresh[jj])) {
				b = rcnn->boxes[ii];
				left = (b.x-b.w/2.)*cols;
				top = (b.y-b.h/2.)*rows;
				width = b.w*cols;
				height = b.h*rows;

				if (left < 0) left = 0;
				if (top < 0) top = 0;
				if (left+width > cols-1) width = cols-1-left;
				if (top+height > rows-1) height = rows-1-top;

                generate_color(class,rcnn->nclasses,color);

                sprintf(text,"%s %.2f",rcnn->names[class],prob);
				draw_onebox(img, left, top, width, height, text, color, alphabet);
			}
        }
    } 
}

void generate_color(int class, int nclasses, float *color)
{
    int offset = class*123457 % nclasses;
    color[0] = get_color(2,offset,nclasses);
    color[1] = get_color(1,offset,nclasses);
    color[2] = get_color(0,offset,nclasses);
}

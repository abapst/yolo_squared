#include "vidutils.h"


void build_i2f(float *i2f)
{
    int ii;

    for (ii = 0; ii < 256; ii++) {
        i2f[ii] = (float)(ii*(1/255.));
    }
}

void split_filename(char *p, char *f, char *e, char *filename)
{
    char *next, *current = filename;

    /* Extract filepath */
    while ((next = strpbrk(current+1, "\\/"))) {
        current = next;
    }
    if (filename != current) current++;
    strncpy(p, filename, current-filename);
    p[current-filename] = 0;

    /* Extract filename */
    next = strrchr(current+1,'.');
    strncpy(f, current, next-current);
    f[next-current] = 0;
    
    /*Extract extension */
    strcpy(e,next);
}

void write_boxes(FILE *fout, netinfo *rcnn, int cnt, int rows, int cols)
{
    int ii, jj, class, left, top, width, height;
    float prob;
    box b;

    for (ii = 0; ii < rcnn->nboxes; ii++) {
        class = max_index(rcnn->probs[ii],rcnn->nclasses); 
        prob = rcnn->probs[ii][class];

        for (jj = 0; jj < rcnn->ntargets; jj++) {
			if((class == rcnn->targets[jj]) && (prob > rcnn->thresh[rcnn->targets[jj]])) {
				b = rcnn->boxes[ii];
				left = (b.x-b.w/2.)*cols;
				top = (b.y-b.h/2.)*rows; 
				width = b.w*cols;
				height = b.h*rows;

				if (left < 0) left = 0;
				if (top < 0) top = 0;
				if (left+width > cols-1) width = cols-1-left;
				if (top+height > rows-1) height = rows-1-top;

				fprintf(fout,"%d,%d,%d,%d,%d,%f,%s\n",cnt,left,top,width,height,prob,rcnn->names[class]);
			}
        }
    } 
}

#ifndef XBLOCK
#define XBLOCK 16
#endif

#ifndef YBLOCK
#define YBLOCK 1
#endif

void bin2frame(rgb *binframe, image *frame, float *i2f)
{
    int x,y,i,j;
    int rows = binframe->h;
    int cols = binframe->w;
    unsigned char val;
    unsigned char *ucptr;
    unsigned char *uctmpptr;
    float         *fptr;
    float         *ftmpptr;

    ucptr = binframe->data;
    fptr  = frame->data;

    for(y = 0; y < rows; y+=YBLOCK){
        for(x = 0; x < cols; x+=XBLOCK){
            for(j = 0; j < YBLOCK; j++){
                uctmpptr = ucptr + (y+j)*cols*3 + x*3;
                ftmpptr  = fptr  + (y+j)*cols + x;
                for(i = 0; i < XBLOCK; i++,uctmpptr+=3,ftmpptr++){
                    val = *uctmpptr;
                    *ftmpptr = i2f[val];
                }
            }
        }
    }

    ucptr = binframe->data+1;
    fptr  = frame->data+(rows*cols);

    for(y = 0; y <rows; y+=YBLOCK){
        for(x = 0; x < cols; x+=XBLOCK){
            for(j = 0; j < YBLOCK; j++){
                uctmpptr = ucptr + (y+j)*cols*3 + x*3;
                ftmpptr  = fptr  + (y+j)*cols + x;
                for(i = 0; i < XBLOCK; i++,uctmpptr+=3,ftmpptr++){
                    val = *uctmpptr;
                    *ftmpptr = i2f[val];
                }
            }
        }
    }

    ucptr = binframe->data+2;
    fptr  = frame->data+(2*rows*cols);

    for(y = 0; y < rows; y+=YBLOCK){
        for(x = 0; x < cols; x+=XBLOCK){
            for(j = 0; j < YBLOCK; j++){
                uctmpptr = ucptr + (y+j)*cols*3 + x*3;
                ftmpptr  = fptr  + (y+j)*cols + x;
                for(i = 0; i < XBLOCK; i++,uctmpptr+=3,ftmpptr++){
                    val = *uctmpptr;
                    *ftmpptr = i2f[val];
                }
            }
        }
    }
}

void bin2frame_noblock(rgb *binframe, image *frame, float *i2f)
{
    int x,y,k;
    int rows = binframe->h;
    int cols = binframe->w;
    frame->h = rows;
    frame->w = cols;
    frame->c = 3;
    unsigned char val;

    for(y = 0; y < rows; ++y){
        for(x = 0; x < cols; ++x){
            for(k = 0; k < 3; ++k){
                val = binframe->data[y*cols*3 + x*3 + k];
                frame->data[k*rows*cols + y*cols + x] = i2f[val];
            }
        }
    }
}



void frame2bin(image *frame, rgb *binframe)
{
    int x,y,k;
    int rows = frame->h;
    int cols = frame->w;
    int channels = frame->c;
    binframe->h = rows;
    binframe->w = cols; 
    float val;

    for(y = 0; y < rows; ++y){
        for(x = 0; x < cols; ++x){
            for(k = 0; k < channels; ++k){
                val = frame->data[k*rows*cols + y*cols + x];   
                binframe->data[y*cols*channels + x*channels + k] = (unsigned char)(val*255);
            }
        }
    }
}

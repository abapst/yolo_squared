#ifndef INFERENCE_H
#define INFERENCE_H

/* Required Darknet libraries */
#include "region_layer.h"

/* Local libraries */
#include "frameio.h"
#include "vidutils.h"
#include "netutils.h"

struct vidresults {
    char path[MAXLINE];
    char filename[MAXLINE];
    char ext[64];
    int nframes,h,w;
    float inference_time;
    float wall_time;
};

float frame_detect(netinfo *rcnn, image *frame);
void video_detect(netinfo *rcnn, struct vidresults *vid_data);
void print_results(struct vidresults *results);
void print_results_short(struct vidresults *results);

#endif

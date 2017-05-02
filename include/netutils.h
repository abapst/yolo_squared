#ifndef NETUTILS_H
#define NETUTILS_H

#include <stdlib.h>
#include <stdio.h>

/* Local libraries */
#include "fileio.h"
#include "linked_list.h"

/* Required Darknet libraries */
#include "utils.h"
#include "option_list.h"
#include "parser.h"


typedef struct _netinfo {
    int rows, cols;
    float *thresh;
    float hier_thresh;
    float nms;
    int nboxes;
    int nclasses;
    int nlayers;
    int ntargets;
    int *targets;
    network net; // Darknet NN structure
    box *boxes;
    float **probs;
    char **names;
} netinfo;

netinfo *init_rcnn(char *netcfg_file,
                   char *weight_file,
                   char *classname_file,
                   char *query_file,
                   float nms);

network init_net(char *netcfg_file,
                 char *weight_file);

void delete_rcnn(netinfo *rcnn);
int *get_target_index(char **query, int ntargets, char **names, int nclasses);

#endif

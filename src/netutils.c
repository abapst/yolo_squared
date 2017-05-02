#include "netutils.h"


netinfo *init_rcnn(char *netcfg_file,
                   char *weight_file,
                   char *classname_file,
                   char *query_file,
                   float nms)
{
    int ii;

    struct strlist *items = read_strlist(classname_file);
    char **names = items->data;
    int nclasses = 1+items->n;
    free(items);

    float *thresh = calloc(nclasses,sizeof(float));
    for (ii=0; ii < nclasses; ii++) {
        thresh[ii] = 1.;
    }

    items = read_strlist(query_file);
    char **query = items->data;
    int ntargets = items->n;
    free(items);

    int *targets = get_target_index(query,ntargets,names,nclasses-1);

    for (ii=0; ii < ntargets; ii++) {
        thresh[targets[ii]] = 0.20; // TODO: custom thresholds
    }

    network net = init_net(netcfg_file,weight_file);
    int nlayers = net.n;
    layer l = net.layers[nlayers-1];
    int nboxes = l.w*l.h*l.n;
    nclasses = l.classes+1;
    printf("netcfg_file = %s\n",netcfg_file);
    printf("weight_file = %s\n",weight_file);
    printf("nclasses=%d\n",nclasses);
    printf("nlayers=%d\n",nlayers);
    printf("nboxes=%d\n",nboxes);
    printf("ntargets=%d\n",ntargets);
    printf("targets=%s(%d)",names[targets[0]],targets[0]);
    for (ii=1; ii < ntargets; ii++) {
        printf(",%s(%d)",names[targets[ii]],targets[ii]);
    }
    printf("\n");

    netinfo *rcnn = malloc(sizeof(netinfo));

    rcnn->thresh = thresh;
    rcnn->hier_thresh = 0.; // Not used, todo maybe?
    rcnn->nms = nms;
    rcnn->net = net;
    rcnn->nboxes = nboxes;
    rcnn->nclasses = nclasses;
    rcnn->ntargets = ntargets;
    rcnn->targets = targets;
    rcnn->nlayers = nlayers;
    rcnn->names = names;

    /* Allocate rcnn structures */
    rcnn->boxes = calloc(nboxes,sizeof(box));
    rcnn->probs = calloc(nboxes,sizeof(float *));
    for (ii = 0; ii < nboxes; ii++) {
        rcnn->probs[ii] = calloc(nclasses,sizeof(float));
    }
    
    return rcnn; 
}

network init_net(char *netcfg_file, char *weight_file)
{
    network net = parse_network_cfg(netcfg_file);
    load_weights(&net, weight_file);
    set_batch_network(&net, 1);
    return net;
}

void delete_rcnn(netinfo *rcnn)
{
    free_ptrs((void **)rcnn->probs,rcnn->nclasses);
    free(rcnn->boxes);
    free(rcnn->targets);
    free(rcnn->thresh);
    free(rcnn);
}

int *get_target_index(char **query, int ntargets, char **names, int nclasses)
{
    int ii,jj,*targets = malloc(ntargets*sizeof(int));

    for(ii = 0; ii < ntargets; ii++) {
        for(jj = 0; jj < nclasses; jj++) {
            if(!strcmp(query[ii],names[jj])) {
                targets[ii] = jj;
            }
        }             
    }
    return targets;
}

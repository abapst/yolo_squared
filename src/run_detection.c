#include "inference.h"


int main(int argc, char **argv)
{
    if(argc < 2){
        fprintf(stderr, "usage: %s <function>\n", argv[0]);
        return 0;
    }
    gpu_index = find_int_arg(argc, argv, "-i", 0);
    if(find_arg(argc, argv, "-nogpu")) {
        gpu_index = -1;
    }

#ifndef GPU
    gpu_index = -1;
#else
    if(gpu_index >= 0){
        cuda_set_device(gpu_index);
    }
#endif

    char *video_file = argv[1];
    const char *model_file = argv[2];
    char *query_file = argv[3];
    float nms;

    struct strlist *items = read_strlist(model_file); 
    char *netcfg_file = items->data[0];
    char *weight_file = items->data[1];
    char *classname_file = items->data[2];
    sscanf(items->data[3],"%f",&nms);

    /* parse input filename */
    char p[MAXLINE];
    char f[MAXLINE];
    char e[64];
    split_filename(p,f,e,video_file);

    netinfo *rcnn = init_rcnn(netcfg_file,
                              weight_file,
                              classname_file,
                              query_file,
                              nms);

    struct vidresults *vid_data = malloc(sizeof(struct vidresults));
    strcpy(vid_data->path,p);
    strcpy(vid_data->filename,f);
    strcpy(vid_data->ext,e);
    video_detect(rcnn, vid_data);
    print_results_short(vid_data);

    /* Cleanup */
    free(vid_data);
    delete_rcnn(rcnn);
    return 0;
}

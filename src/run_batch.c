#include "inference.h"

/* Remove trailing newline */
#define removen(s) (s[strcspn(s,"\n")] = 0)

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

    const char *model_file = argv[1];
    char *videolist_file = argv[2];
    char *query_file = argv[3];
    float nms;

    struct strlist *items = read_strlist(model_file); 
    char *netcfg_file = items->data[0];
    char *weight_file = items->data[1];
    char *classname_file = items->data[2];
    sscanf(items->data[3],"%f",&nms);

    netinfo *rcnn = init_rcnn(netcfg_file,
                              weight_file,
                              classname_file,
                              query_file,
                              nms);

    FILE *fin = fopen(videolist_file,"r");
    clock_t start = clock();
    float elapsed;
    int n, cnt=0;
    char filename[MAXLINE];
    char path[MAXLINE];
    char ext[64];

    /* Read number of files */
    fgets(filename,MAXLINE,fin);
    sscanf(filename,"%d",&n);

    /* Read directory path */
    fgets(path,MAXLINE,fin);
    removen(path);

    /* Read ext */
    fgets(ext,64,fin);
    removen(ext);

    struct vidresults *vid_data = malloc(sizeof(struct vidresults));
    strcpy(vid_data->path,path);
    strcat(vid_data->path,"/");
    
    fprintf(stderr,"Processing %d videos...\n",n); 
    while(NULL != fgets(filename,MAXLINE,fin)) {
        cnt++;
        removen(filename);
        strcpy(vid_data->filename,filename);
        strcpy(vid_data->ext,ext);
        video_detect(rcnn,vid_data);
        fprintf(stderr,"(%d/%d) ",cnt,n);
        print_results_short(vid_data);
    }

    if (cnt == n) {
        printf("All videos processed, cleaning up.\n");
    } else {
        printf("Done, but some videos were not processed.\n");
    }
    elapsed = sec(clock()-start);
    printf("%d videos processed in %.2f s\n",cnt,elapsed);

    /* Cleanup */
    free(vid_data);
    delete_rcnn(rcnn);
    return 0;
}

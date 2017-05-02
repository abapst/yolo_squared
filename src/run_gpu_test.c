#include "inference.h"

int main(int argc, char **argv)
{
#ifndef GPU
    gpu_index = -1;
#else
    if(gpu_index >= 0){
        cuda_set_device(gpu_index);
    }
#endif

    const char *model_file = argv[1];
    char *im_file = argv[2];
    char *query_file = argv[3];
    float nms;

    int iters;
    sscanf(argv[4],"%d",&iters);

    struct strlist *items = read_strlist(model_file); 
    char *netcfg_file = items->data[0];
    char *weight_file = items->data[1];
    char *classname_file = items->data[2];
    sscanf(items->data[3],"%f",&nms);

    int ii;
    float elapsed = 0;

    float i2f[256];
    build_i2f(i2f);

    rgb binframe = {0,0,NULL};

    netinfo *rcnn = init_rcnn(netcfg_file,
                              weight_file,
                              classname_file,
                              query_file,
                              nms);

    /* Read a single frame, resized for net */
    fio_imread(im_file,&binframe,rcnn->net.h,rcnn->net.w);
    image img = make_image(rcnn->net.w,rcnn->net.h,3);

    for (ii = 0; ii < iters; ii++) {
        fprintf(stderr,"\r(%d/%d)",ii+1,iters);
        elapsed += frame_detect(rcnn, &img);
    }

    printf("\n%d frames processed at %.2f FPS\n",iters,iters / elapsed);

    /* Cleanup */
    free(binframe.data);
    free_image(img);

    return 0;
}

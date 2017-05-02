#include "inference.h"


float frame_detect(netinfo *rcnn, image *frame)
{
    srand(2222222);
    clock_t start;
    float elapsed;
    
    float *X = frame->data; // frame is already resized for net input
    layer l = rcnn->net.layers[rcnn->nlayers-1];

    start = clock();
    network_predict(rcnn->net,X);
    elapsed = sec(clock()-start);

    get_region_boxes_fast(l, 1, 1,
                     rcnn->thresh,
                     rcnn->probs,
                     rcnn->boxes);
    if (l.softmax_tree && rcnn->nms) {
        do_nms_obj(rcnn->boxes, 
                   rcnn->probs,
                   rcnn->nboxes,
                   rcnn->nclasses, 
                   rcnn->nms);
    } else if (rcnn->nms) {
        do_nms_sort(rcnn->boxes,
                    rcnn->probs,
                    rcnn->nboxes,
                    rcnn->nclasses,
                    rcnn->nms);
    }

    return elapsed;
}

void video_detect(netinfo *rcnn, struct vidresults *vid_data)
{
    int rows, cols, cnt = 0;
    float wall_time, inference_time = 0;
    image frame = make_image(rcnn->net.w,rcnn->net.h,3);
    rgb binframe;
    binframe.data = NULL;
    clock_t start = clock();
    FILE *fin, *fout;

    /* Parse input filename */
    char infile[MAXLINE];
    strcpy(infile,vid_data->path);
    strcat(infile,vid_data->filename);
    strcat(infile,vid_data->ext);

    /* Build output filename */
    char outfile[MAXLINE];
    strcpy(outfile,"output/");
    strcat(outfile,vid_data->filename);
    strcat(outfile,".csv");

    /* Lookup table for converting RGB to float */
    float i2f[256];
    build_i2f(i2f);

    /* Read a single frame without resizing to get original size */
    fin = fio_OpenReadStream(infile,-1,-1);
    fio_ReadFrame(&binframe,fin);
    fio_close(fin);
    rows = binframe.h;
    cols = binframe.w;
    free(binframe.data);
    binframe.data = NULL;

    fin = fio_OpenReadStream(infile,rcnn->net.h,rcnn->net.w);
    fout = fopen(outfile,"w");
    fprintf(fout,"frame,left,top,width,height,conf,class\n");

    while(fio_ReadFrame(&binframe,fin)) {
        bin2frame(&binframe,&frame,i2f);
        inference_time += frame_detect(rcnn, &frame);
        fprintf(stderr,"\r(%d)",++cnt);
        write_boxes(fout, rcnn, cnt, rows, cols);
    }
    fprintf(stderr,"\r");

    /* Cleanup */
    free_image(frame);
    fio_close(fin);
    fclose(fout);

    wall_time = sec(clock()-start);

    /* Build output struct */
    vid_data->nframes = cnt;
    vid_data->h = rows;
    vid_data->w = cols;
    vid_data->inference_time = inference_time;
    vid_data->wall_time = wall_time;
}

void print_results_short(struct vidresults *results)
{
    char fullname[MAXLINE];
    strcpy(fullname,results->filename);
    strcat(fullname,results->ext);
    fprintf(stderr,"%s : %d frames in %.1fs : %.2f/%.2f FPS\n",
        fullname,results->nframes,results->wall_time,
        (float)results->nframes/results->wall_time,
        (float)results->nframes/results->inference_time);
}

void print_results(struct vidresults *results)
{
    char fullname[MAXLINE];
    strcpy(fullname,results->filename);
    strcat(fullname,results->ext);
    printf("+--------------------------------------------------------+\n");
    printf("|%s\n",fullname);
    printf("|Processed %d frames of size (%dx%d)\n",results->nframes,results->h,results->w);
    printf("|Wall FPS: %.2f\n",(float)results->nframes/results->wall_time);
    printf("|GPU FPS: %.2f\n",(float)results->nframes/results->inference_time);
    printf("+--------------------------------------------------------+\n");
    printf("|  Total wall time: %.2f\n",results->wall_time);
    printf("+--------------------------------------------------------+\n");
}

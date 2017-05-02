#include "draw_box.h"
#include "frameio.h"
#include "vidutils.h"
#include "linked_list.h"
#include "fileio.h"

#define MAX_CLASSES 20
static char *classNames[MAX_CLASSES];
static int numClasses=0;

static int getClassIx(char *name)
{
    int k;
	for (k=0 ; k<numClasses ; ++k)
		if (!strcmp(name,classNames[k]))
			return k;

	classNames[numClasses] = malloc(strlen(name)+1);
    strcpy(classNames[numClasses], name);
	return numClasses++;
}

int main(int argc, char **argv)
{
    int left=0, top=0, width=0, height=0;
    int fcnt=0, bcnt=1, vidfin=0, csvfin=0;
    char *videofile = argv[1];
    char *boxesfile = argv[2];
    char *outputfile = argv[3];
    const char *labelpath = argv[4];
    float prob = 0.;
    float color[3];
	char classname[64];
	
    rgb binframe = {0,0,NULL};
    image img;

    image **alphabet = load_alphabet(labelpath);
    char text[1024];

    FILE *fin = fopen(boxesfile,"r");
    FILE *vidin = fio_OpenReadStream(videofile,-1,-1);  
    FILE *vidout = NULL;

    /* Lookup table for converting RGB to float */
    float i2f[256];
    build_i2f(i2f);

    char buf[MAXLINE];
    fgets(buf, MAXLINE, fin); // Skip first line

    while(!vidfin) {

        /* Read line from csv */
        if(NULL == fgets(buf,MAXLINE,fin)) {
            bcnt = -1;
            csvfin = 1;
        } else {
            //sscanf(buf,"%d,%d,%d,%d,%d,%f,%d",&bcnt,&left,&top,&width,&height,&prob,&class);
            sscanf(buf,"%d,%d,%d,%d,%d,%f,%63s",&bcnt,&left,&top,&width,&height,&prob,classname);
        }

        /* Write frame with drawn boxes */
        if (fcnt > 0 && (fcnt < bcnt)) {
            frame2bin(&img,&binframe);
            fio_WriteFrame(&binframe,vidout);
        }

        /* Read frames until they catch up */
		while(csvfin || fcnt < bcnt) {
            if (fio_ReadFrame(&binframe,vidin)) {
			    fprintf(stderr,"\r(%d)",++fcnt);
           	    if (vidout == NULL) {
				    vidout = fio_OpenWriteStream(outputfile,binframe.h,binframe.w);
				    img = make_image(binframe.w,binframe.h,3);
			    }
                if (fcnt == bcnt)
			        bin2frame(&binframe,&img,i2f);
                else
                    fio_WriteFrame(&binframe,vidout);
            } else {
                vidfin = 1;
                fio_close(vidin);
                fio_close(vidout);
                break;
            }
        }

        /* Draw box */
        if (fcnt == bcnt) {
            generate_color(getClassIx(classname),MAX_CLASSES,color);
			sprintf(text,"%s %.2f",classname,prob);
			draw_onebox(&img,left,top,width,height,text,color,alphabet);
        }
    } 
    printf("\nDone.\n");

    /* Cleanup */
    fclose(fin);
    free_image(img);
    return 0;
}

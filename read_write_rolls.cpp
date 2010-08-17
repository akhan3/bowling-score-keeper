#include <stdio.h>
#include <stdlib.h>

// **************************************
// random integer generator
// **************************************
int rand_atob(const int a, const int b) {
    double r = rand() / (double)RAND_MAX;
    r = a + (b-a+1) * r;
    return (int)r;
}


// *****************************************
// generate random rolls and write to file
// *****************************************
int generate_rolls( const int frames,
                    const int pins_per_frame,
                    const int rolls_per_frame,
                    const char *filename )
{
    FILE *fh = fopen(filename, "w");
    if(fh == NULL) {
        fprintf(stderr, "FATAL ERROR: %s:%d couldn't open file %s\n", __FILE__, __LINE__, filename);
        return EXIT_FAILURE;
    }
    for(int i = 0; i < frames; i++) {
        int n1 = 0;
        int n2 = 0;
        n1 = rand_atob(0, pins_per_frame);
        fprintf(fh, "%d\n", n1);
        if(n1 != pins_per_frame) {
            n2 = rand_atob(0, pins_per_frame - n1);
            fprintf(fh, "%d\n", n2);
        }
        // additional rolls in last frame
        if(i == frames-1) {
            if(n1 == pins_per_frame) {
                int n = rand_atob(0, pins_per_frame - n1);
                fprintf(fh, "%d\n", n);
            }
            if(n1+n2 == pins_per_frame) {
                int n = rand_atob(0, pins_per_frame - n1);
                fprintf(fh, "%d\n", n);
            }
        }
    }
    fclose(fh);
    return EXIT_SUCCESS;
}


// **************************************
// load rolls data from file
// **************************************
int load_rolls( const int frames,
                const int rolls_per_frame,
                const char *filename,
                int *rolls_data )
{
    FILE *fh = fopen(filename, "r");
    if(fh == NULL) {
        fprintf(stderr, "FATAL ERROR: %s:%d couldn't open file %s\n", __FILE__, __LINE__, filename);
        return EXIT_FAILURE;
    }
    for(int i = 0; i < frames*rolls_per_frame+1; i++)
        int dummy = fscanf(fh, "%d", &rolls_data[i]);
    fclose(fh);
    return EXIT_SUCCESS;
}


// **************************************
// write cumulative score to file
// **************************************
int write_score( const int frames,
                 const char *filename,
                 const int *score )
{
    char filename_output[1000];
    sprintf(filename_output, "%s.score", filename);
    FILE *fh = fopen(filename_output, "w");
    if(fh == NULL) {
        fprintf(stderr, "FATAL ERROR: %s:%d couldn't open file %s\n", __FILE__, __LINE__, filename);
        return EXIT_FAILURE;
    }
    int score_cum = 0;
    for(int f = 0; f < frames; f++) {
        score_cum += score[f];
        fprintf(fh, "%d\n", score_cum);
    }
    fclose(fh);
    return EXIT_SUCCESS;
}


// **************************************
// roll the ball
// **************************************
int roll( const int keyboard,
          int *rolls_data,
          const int index )
{
    if(keyboard) {
        int n;
        // printf("enter knocked down pins: ");
        int dummy = scanf("%d", &n);
        rolls_data[index] = n;
    }
    return rolls_data[index];
}

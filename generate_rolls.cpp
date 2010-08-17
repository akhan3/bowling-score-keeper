#include <stdio.h>
#include <stdlib.h>

// *************************************************
// roll the ball - read a number from file stream
// *************************************************
int roll(FILE *fh) {
    int n;
    int count = fscanf(fh, "%d", &n);
    if(count != 1) {
        fprintf(stderr, "FATAL ERROR: Insufficient data on %s. Score will be invalid!\n",
                          (fh == stdin) ? "stdin" : "given stream");
        fprintf(stdout, "FATAL ERROR: Insufficient data on %s. Score will be invalid!\n",
                          (fh == stdin) ? "stdin" : "given stream");
    }
    return n;
}

// **************************************
// random integer generator
// **************************************
int rand_atob(const int a, const int b) {
    double r = rand() / (double)RAND_MAX;
    r = a + (b-a+1) * r;
    return (int)r;
}

// *************************************************
// generate random rolls and write to file stream
// *************************************************
int generate_rolls( const int total_frames,
                    const int pins_per_frame,
                    const int rolls_per_frame,
                    FILE *fh )
{
    for(int i = 0; i < total_frames; i++) {
        int n1 = 0;     // pins knocked down on 1st roll
        int n2 = 0;     // pins knocked down on 2nd roll
        int n3 = 0;     // pins knocked down on 3rd roll
        int strike = 0; // current frame is a strike
        int spare = 0;  // current frame is a spare

    // first roll
        n1 = rand_atob(0, pins_per_frame);
        strike = (n1 == pins_per_frame);
        fprintf(fh, "%d ", n1);
    // second roll if needed
        if(!strike) {
            n2 = rand_atob(0, pins_per_frame - n1);
            spare = (n1+n2 == pins_per_frame);
            fprintf(fh, "%d ", n2);
        }
    // additional rolls in the last frame
        if(i == total_frames-1) {
            if(spare) {
                n3 = rand_atob(0, pins_per_frame);
                fprintf(fh, "%d ", n3);
            }
            if(strike) {
                n2 = rand_atob(0, pins_per_frame);
                fprintf(fh, "%d ", n2);
                if(n2 == pins_per_frame)
                    n3 = rand_atob(0, pins_per_frame);
                else
                    n3 = rand_atob(0, pins_per_frame - n2);
                fprintf(fh, "%d ", n3);
            }
        }
    }
    fprintf(fh, "\n");
    return EXIT_SUCCESS;
}


// **********************************************
// display debugging info and score on the fly
// **********************************************
int show_debugging_info( const int f,
                         const int total_frames,
                         const int n1,
                         const int n2,
                         const int n3,
                         const int *score,
                         FILE *fh )
{
    int score_cum = 0;
    if(f != total_frames-1)
        fprintf(fh, "DEBUG: frame#%d = (%2d %2d): ", f+1, n1, n2);
    else
        fprintf(fh, "DEBUG: frame#%d = (%2d %2d %2d): ", f+1, n1, n2, n3);
    fprintf(stderr, "score = {");
    for(int i = 0; i <= f; i++) {
        score_cum += score[i];
        fprintf(fh, "%3d ", score_cum);
    }
    fprintf(fh, "}\n");
    return EXIT_SUCCESS;
}

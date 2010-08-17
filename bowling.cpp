#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// **************************************
// function prototypes
// **************************************
// generate random rolls
int generate_rolls( const int frames,
                    const int pins_per_frame,
                    const int rolls_per_frame,
                    const char *filename );

// load rolls data from file
int load_rolls( const int frames,
                const int rolls_per_frame,
                const char *filename,
                int *rolls_data );

// write cumulative score to file
int write_score( const int frames,
                 const char *filename,
                 const int *score );

// roll the ball
int roll( const int keyboard,
          int *rolls_data,
          const int index );


// **************************************
//        Main function
// **************************************
int main(int argc, char **argv)
{
    int status = 0;

// default value of command line arguments
    int keyboard = 1;
    char filename[1000] = "keyboard";
    char mode[100] = "play";
    int seed = time(NULL);

// read command line arguments
    if(argc >= 2 && (!strcmp(argv[1], "help") ||  !strcmp(argv[1], "--help") ||  !strcmp(argv[1], "-h"))) {
        printf("Usage: %s [rolls_datafile] [mode] [random_seed]\n", argv[0]);
        printf("rolls_datafile = {keyboard | file to read rolls data from}. ");
        printf("keyboard will read from stdin.\n");
        printf("mode = {play | generate_play}\n");
        return EXIT_SUCCESS;
    }
    if(argc >= 2)
        sscanf(argv[1], "%s", filename);
    if(argc >= 3)
        sscanf(argv[2], "%s", mode);
    if(argc >= 4)
        sscanf(argv[3], "%d", &seed);
    srand(seed);
    if(strcmp(filename, "keyboard"))
        keyboard = 0;
    // printf("filename = %s, mode = %s, keyboard = %d\n", filename, mode, keyboard);

// constants
    const int frames = 10;
    const int pins_per_frame = 10;
    const int rolls_per_frame = 2;
    const int total_rolls = frames * rolls_per_frame + 1;

// generate rolls data if needed
    if(!keyboard) {
        if(argc >= 3 && !strcmp(mode, "generate_play")) {
            status |= generate_rolls(frames, pins_per_frame, rolls_per_frame, filename);
            if(status) return EXIT_FAILURE;
            printf("INFO: Rolls data generated in file %s\n", filename);
        }
    }

// initialize rolls array
    int rolls_data[total_rolls];
    for(int i = 0; i < total_rolls; i++)
        rolls_data[i] = -1;

// initialize score array
    int score[frames];
    for(int i = 0; i < frames; i++)
        score[i] = 0;

// read rolls data from file if needed
    if(!keyboard) {
        status |= load_rolls(frames, rolls_per_frame, filename, rolls_data);
        if(status) return EXIT_FAILURE;
        printf("INFO: Rolls data loaded from file %s\n", filename);
    }

// display rolls data
    // for(int i = 0; i < total_rolls; i++)
        // printf("%2d ", rolls_data[i]);
    // printf("\n");

// book-keeping variables
    int prev2_strike = 0;   // last to last frame was a strike
    int prev_strike = 0;    // last frame was a strike
    int prev_spare = 0;     // last frame was a spare
    int r = 0;              // rolls index


// ================================
// 1-10 frames loop
// main logic of scoring is here
// ================================
    printf("enter number of pins knocked down on each roll and press enter...\n");
    for(int f = 0; f < frames; f++) {
        int strike = 0; // current frame is a strike
        int spare = 0;  // current frame is a spare
        int n1 = 0;     // pins knocked down on 1st roll
        int n2 = 0;     // pins knocked down on 2nd roll
        int n3 = 0;     // pins knocked down on 3rd roll

    // first roll of current frame
        n1 = roll(keyboard, rolls_data, r++);
        strike = (n1 == pins_per_frame);

    // second roll of current frame
        if(!strike) {
            n2 = roll(keyboard, rolls_data, r++);
            spare = (n1+n2 == pins_per_frame);
        }

    // do additional 1 or 2 rolls in the last frame
        if(f == frames-1) {
            if(strike)
                n2 = roll(keyboard, rolls_data, r++);
            if(strike || spare)
                n3 = roll(keyboard, rolls_data, r++);
        }

    // score current frame
        score[f] = n1 + n2 + n3;

    // update score of previous frames
        if(f >= 1)
            score[f-1] += (prev_strike ? n1 + n2 : 0) +
                          (prev_spare  ? n1      : 0) ;
        if(f >= 2)
            score[f-2] += (prev2_strike && prev_strike) ? n1 : 0;

    // update book-keeping variables
        prev2_strike = prev_strike;
        prev_strike = strike;
        prev_spare = spare;

    // display score
        int score_cum = 0;
        if(f != frames-1)
            printf("frame#%2d = (%2d %2d): ", f+1, n1, n2);
        else
            printf("frame#%2d = (%2d %2d %2d): ", f+1, n1, n2, n3);
        printf("score = {");
        for(int i = 0; i <= f; i++) {
            score_cum += score[i];
            printf("%3d ", score_cum);
        }
        printf("}\n");
    } // frames loop

// write score file as output
    status |= write_score(frames, filename, score);
    if(status) return EXIT_FAILURE;
    printf("INFO: written score file %s.score\n", filename);

// closing
    return status ? EXIT_FAILURE : EXIT_SUCCESS;
}

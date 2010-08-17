#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// **************************************
// function prototypes
// **************************************
// roll the ball - read a number from file stream
int roll(FILE *fh);

// generate random rolls and write to file stream
int generate_rolls( const int total_frames,
                    const int pins_per_frame,
                    const int rolls_per_frame,
                    FILE *fh );

// display debugging info and score on the fly
int show_debugging_info( const int f,
                         const int total_frames,
                         const int n1,
                         const int n2,
                         const int n3,
                         const int *score,
                         FILE *fh );


// **************************************
//        Main function
// **************************************
int main(int argc, char **argv)
{
    int status = 0;

// default value of command line arguments
    char g_arg[100];
    int seed = time(NULL);

// help and usage notes
    if(argc >= 2 && (!strcmp(argv[1], "-h") ||  !strcmp(argv[1], "--help") ||  !strcmp(argv[1], "-help"))) {
        printf("Usage: %s [-g [RANDOM_SEED]]\n", argv[0]);
        printf("Notes:\n");
        printf("If run without arguments, the program will read the rolls from stdin and display the score on stdout.\n");
        printf("If run with -g argument, the program will only generate valid random rolls on stdout that may be redirected as its input in a separate run, like, %s < %s -g \n", argv[0], argv[0]);
        printf("If no seed is given, default will be current time.\n");
        return EXIT_SUCCESS;
    }
// read command line arguments
    if(argc >= 2)
        sscanf(argv[1], "%s", g_arg);
    if(argc >= 3)
        sscanf(argv[2], "%d", &seed);

// game constants
    const unsigned int total_frames = 10;
    const unsigned int pins_per_frame = 10;
    const unsigned int rolls_per_frame = 2;

// generate rolls data if requested
    if(argc >= 2 && !strcmp(g_arg, "-g")) {
        srand(seed); // seed the random generator
        status |= generate_rolls(total_frames, pins_per_frame, rolls_per_frame, stdout);
        if(status) return EXIT_FAILURE;
        return EXIT_SUCCESS; // and exit
    }

// instructions to enter data on stdin
    fprintf(stderr, "Enter number of pins knocked down on each valid roll.\n");
    fprintf(stderr, "They may be entered either one-by-one or as space separated numbers.\n");
    fprintf(stderr, "Important: 0 for the second roll of a strike frame must not be entered at all, since it is not a valid roll.\n");
    fprintf(stderr, "Some debugging output will be written on-the-fly on stderr.\n");

// ============================================================================
// main logic of scoring the games goes bleow here
// ============================================================================
// initialize score array to hold individual scores of each frame
    int *score = (int*)calloc(total_frames, sizeof(int));
    if(score == NULL) {
        fprintf(stderr, "FATAL ERROR: Couldn't allocate memory at %s:%d\n", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }

// previous state variables
    int prev2_strike = 0;   // last to last frame was a strike
    int prev_strike = 0;    // last frame was a strike
    int prev_spare = 0;     // last frame was a spare

// ===============================================
// loop over frames
// ===============================================
    for(unsigned int f = 0; f < total_frames; f++)
    {
        unsigned int n1 = 0;     // pins knocked down on 1st roll
        unsigned int n2 = 0;     // pins knocked down on 2nd roll
        unsigned int n3 = 0;     // pins knocked down on 3rd roll
        int strike = 0; // current frame is a strike
        int spare = 0;  // current frame is a spare

    // ================================
    // roll for the current frame
    // also sanitize all the inputs
    // ================================
    // first roll of current frame
        while((n1 = roll(stdin)) > pins_per_frame)
            fprintf(stderr, "TRY AGAIN: %d exceeds %d available pins!\n", n1, pins_per_frame);
        strike = (n1 == pins_per_frame);
    // second roll of current frame (if needed)
        if(!strike) {
            while((n2 = roll(stdin)) > pins_per_frame - n1)
                fprintf(stderr, "TRY AGAIN: %d exceeds %d available pins!\n", n2, pins_per_frame - n1);
            spare = (n1+n2 == pins_per_frame);
        }
    // do additional 1 or 2 rolls in the last frame
        if(f == total_frames-1) {
            if(spare)
                while((n3 = roll(stdin)) > pins_per_frame)
                    fprintf(stderr, "TRY AGAIN: %d exceeds %d available pins!\n", n3, pins_per_frame);
            if(strike) {
                while((n2 = roll(stdin)) > pins_per_frame)
                    fprintf(stderr, "TRY AGAIN: %d exceeds %d available pins!\n", n2, pins_per_frame);
                if(n2 == pins_per_frame)
                    while((n3 = roll(stdin)) > pins_per_frame)
                        fprintf(stderr, "TRY AGAIN: %d exceeds %d available pins!\n", n3, pins_per_frame);
                else
                    while((n3 = roll(stdin)) > pins_per_frame - n2)
                        fprintf(stderr, "TRY AGAIN: %d exceeds %d available pins!\n", n3, pins_per_frame - n2);
            }
        }

    // score current frame
        score[f] = n1 + n2 + n3;

    // update score of previous frames
        if(f >= 1)
            score[f-1] += (prev_strike ? n1 + n2 : 0) |
                          (prev_spare  ? n1      : 0) ;
        if(f >= 2)
            score[f-2] += (prev2_strike && prev_strike) ? n1 : 0;

    // update previous state variables
        prev2_strike = prev_strike;
        prev_strike = strike;
        prev_spare = spare;

    // display debugging info and score on the fly
        show_debugging_info(f, total_frames, n1, n2, n3, score, stderr);

    } // frames loop

// write final cumulative score on stdout
    int score_cum = 0;
    for(unsigned int f = 0; f < total_frames; f++) {
        score_cum += score[f];
        fprintf(stdout, "%d ", score_cum);
    }
    fprintf(stdout, "\n");

// closing
    return status ? EXIT_FAILURE : EXIT_SUCCESS;

} // main

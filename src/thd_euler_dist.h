#ifndef THD_EDT_INCLUDED
#define THD_EDT_INCLUDED



/* struct of quantities for running Euler Distance Transform (EDT) 

    do_sqrt      : if True, the output image of EDT values is distance
                   values; otherwise, the values are distance**2 (because
                   that is what the program works with).

    bounds_are_zero : switch for how to treat FOV boundaries for
                   nonzero-ROIs; True means they are ROI boundaries
                   (so FOV is a "closed" boundary for the ROI), and
                   False means that the ROI continues "infinitely" at
                   FOV boundary (so it is "open").  Zero-valued ROIs
                   (= background) are not affected by this value.

    zeros_are_zeroed : if False, EDT values are output for the
                   zero-valued region; otherwise, zero them out, so EDT
                   values are only reported in the non-zero ROIs. NB: the
                   EDT values are still calculated everywhere; it is just
                   a question of zeroing out later (no time saved for True).

    edims        : (len=3 fl arr) element dimensions (here, voxel edge lengths)

    shape        : (len=3 int arr) matrix size in each direction
*/
typedef struct {

   char *input_name;      

   char *prefix;          

   int zeros_are_zeroed;  
   int bounds_are_zero;   
   int do_sqrt;           

   float edims[3];        
   int   shape[3];        

} PARAMS_euler_dist;

/* function to initialize EDT params */
PARAMS_euler_dist set_euler_dist_defaults(void);


#endif

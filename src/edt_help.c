#include "mrilib.h"

/*****************************************************************************
  This software is copyrighted and owned by the Medical College of Wisconsin.
  See the file README.Copyright for details.
******************************************************************************/


#undef  AFNI_DEBUG
#undef  CLUST_DEBUG
#define STATUS(x) /* nada */
#define ENTRY(x)  /* nada */
#define EXRETURN  return
#define RETURN(x) return(x)


static char * helpstring =
    "EDITING OPTIONS APPLIED TO EACH INPUT DATASET:\n"
    "  -1thtoin         = Copy threshold data over intensity data.\n"
    "                       This is only valid for datasets with some\n"
    "                       thresholding statistic attached.  All\n"
    "                       subsequent operations apply to this\n"
    "                       substituted data.\n"
    "  -2thtoin         = The same as -1thtoin, but do NOT scale the\n"
    "                       threshold values from shorts to floats when\n"
    "                       processing.  This option is only provided\n"
    "                       for compatibility with the earlier versions\n"
    "                       of the AFNI package '3d*' programs.\n"
    "  -1noneg          = Zero out voxels with negative intensities\n"
    "  -1abs            = Take absolute values of intensities\n"
    "  -1clip val       = Clip intensities in range (-val,val) to zero\n"
    "  -2clip v1 v2     = Clip intensities in range (v1,v2) to zero\n"
    "  -1uclip val      = These options are like the above, but do not apply\n"
    "  -2uclip v1 v2        any automatic scaling factor that may be attached\n"
    "                       to the data.  These are for use only in special\n"
    "                       circumstances.  (The 'u' means 'unscaled'.  Program\n"
    "                       '3dinfo' can be used to find the scaling factors.)\n"
    "               N.B.: Only one of these 'clip' options can be used; you cannot\n"
    "                       combine them to have multiple clipping executed.\n"
    "  -1thresh thr     = Use the threshold data to censor the intensities\n"
    "                       (only valid for 'fith', 'fico', or 'fitt' datasets).\n"
    "               N.B.: The value 'thr' is floating point, in the range\n"
    "                           0.0 < thr < 1.0  for 'fith' and 'fico' datasets,\n"
    "                       and 0.0 < thr < 32.7 for 'fitt' datasets.\n"
    "  -1blur_sigma bmm = Gaussian blur with sigma = bmm (in mm)\n"
    "  -1blur_rms bmm   = Gaussian blur with rms deviation = bmm\n"
    "  -1blur_fwhm bmm  = Gaussian blur with FWHM = bmm\n"
    "  -t1blur_sigma bmm= Gaussian blur of threshold with sigma = bmm(in mm)\n"
    "  -t1blur_rms bmm  = Gaussian blur of threshold with rms deviation = bmm\n"
    "  -t1blur_fwhm bmm = Gaussian blur of threshold with FWHM = bmm\n"
    "  -1zvol x1 x2 y1 y2 z1 z2\n"
    "                   = Zero out entries inside the 3D volume defined\n"
    "                       by x1 <= x <= x2, y1 <= y <= y2, z1 <= z <= z2 ;\n"
    "               N.B.: The ranges of x,y,z in a dataset can be found\n"
    "                       using the '3dinfo' program. Dimensions are in mm.\n"
    "               N.B.: This option may not work correctly at this time, but\n"
    "                       I've not figured out why!\n"
    "\n"
    " CLUSTERING\n"
    "  -dxyz=1  = In the cluster editing options, the spatial clusters\n"
    "             are defined by connectivity in true 3D distance, using\n"
    "             the voxel dimensions recorded in the dataset header.\n"
    "             This option forces the cluster editing to behave as if\n"
    "             all 3 voxel dimensions were set to 1 mm.  In this case,\n"
    "             'rmm' is then the max number of grid cells apart voxels\n"
    "             can be to be considered directly connected, and 'vmul'\n"
    "             is the min number of voxels to keep in the cluster.\n"
    "       N.B.: The '=1' is part of the option string, and can't be\n"
    "             replaced by some other value.  If you MUST have some\n"
    "             other value for voxel dimensions, use program 3drefit.\n"
    " \n"
    "  The following cluster options are mutually exclusive: \n"
    "  -1clust rmm vmul = Form clusters with connection distance rmm\n"
    "                       and clip off data not in clusters of\n"
    "                       volume at least vmul microliters\n"
    "  -1clust_mean rmm vmul = Same as -1clust, but all voxel intensities \n"
    "                            within a cluster are replaced by the average\n"
    "                            intensity of the cluster. \n"
    "  -1clust_max rmm vmul  = Same as -1clust, but all voxel intensities \n"
    "                            within a cluster are replaced by the maximum\n"
    "                            intensity of the cluster. \n"
    "  -1clust_amax rmm vmul = Same as -1clust, but all voxel intensities \n"
    "                            within a cluster are replaced by the maximum\n"
    "                            absolute intensity of the cluster. \n"
    "  -1clust_smax rmm vmul = Same as -1clust, but all voxel intensities \n"
    "                            within a cluster are replaced by the maximum\n"
    "                            signed intensity of the cluster. \n"
    "  -1clust_size rmm vmul = Same as -1clust, but all voxel intensities \n"
    "                            within a cluster are replaced by the size \n"
    "                            of the cluster (in multiples of vmul).   \n"
    "  -1clust_order rmm vmul= Same as -1clust, but all voxel intensities \n"
    "                            within a cluster are replaced by the cluster\n"
    "                            size index (largest cluster=1, next=2, ...).\n"
    " \n"
    "  The following commands produce erosion and dilation of 3D clusters.  \n"
    "  These commands assume that one of the -1clust commands has been used.\n"
    "  The purpose is to avoid forming strange clusters with 2 (or more)    \n"
    "  main bodies connected by thin 'necks'.  Erosion can cut off the neck.\n"
    "  Dilation will minimize erosion of the main bodies.                   \n"
    "  Note:  Manipulation of values inside a cluster (-1clust commands)    \n"
    "         occurs AFTER the following two commands have been executed.   \n"
    "  -1erode pv    For each voxel, set the intensity to zero unless pv %  \n"
    "                of the voxels within radius rmm are nonzero.           \n"
    "  -1dilate      Restore voxels that were removed by the previous       \n"
    "                command if there remains a nonzero voxel within rmm.   \n"
    " \n"
    "  The following filter options are mutually exclusive: \n"
    "  -1filter_mean rmm   = Set each voxel to the average intensity of the \n"
    "                          voxels within a radius of rmm. \n"
    "  -1filter_nzmean rmm = Set each voxel to the average intensity of the \n"
    "                          non-zero voxels within a radius of rmm. \n"
    "  -1filter_max rmm    = Set each voxel to the maximum intensity of the \n"
    "                          voxels within a radius of rmm. \n"
    "  -1filter_amax rmm   = Set each voxel to the maximum absolute intensity\n"
    "                          of the voxels within a radius of rmm. \n"
    "  -1filter_smax rmm   = Set each voxel to the maximum signed intensity \n"
    "                          of the voxels within a radius of rmm. \n"
    "  -1filter_aver rmm   = Same idea as '_mean', but implemented using a\n"
    "                          new code that should be faster.\n"
    " \n"
    "  The following threshold filter options are mutually exclusive: \n"
    "  -t1filter_mean rmm   = Set each correlation or threshold voxel to the \n"
    "                          average of the voxels within a radius of rmm. \n"
    "  -t1filter_nzmean rmm = Set each correlation or threshold voxel to the \n"
    "                          average of the non-zero voxels within \n"
    "                          a radius of rmm. \n"
    "  -t1filter_max rmm    = Set each correlation or threshold voxel to the \n"
    "                          maximum of the voxels within a radius of rmm. \n"
    "  -t1filter_amax rmm   = Set each correlation or threshold voxel to the \n"
    "                          maximum absolute intensity of the voxels \n"
    "                          within a radius of rmm. \n"
    "  -t1filter_smax rmm   = Set each correlation or threshold voxel to the \n"
    "                          maximum signed intensity of the voxels \n"
    "                          within a radius of rmm. \n"
    "  -t1filter_aver rmm   = Same idea as '_mean', but implemented using a\n"
    "                          new code that should be faster.\n"
    " \n"
#ifdef ALLOW_SCALE_TO_MAX
    "  -1scale          = Linearly scale intensities so that max is 10000\n"
#endif
    "  -1mult factor    = Multiply intensities by the given factor\n"
    "  -1zscore         = If the sub-brick is labeled as a statistic from\n"
    "                     a known distribution, it will be converted to\n"
    "                     an equivalent N(0,1) deviate (or 'z score').\n"
    "                     If the sub-brick is not so labeled, nothing will\n"
    "                     be done.\n"
    "\n"
    "The above '-1' options are carried out in the order given above,\n"
    "regardless of the order in which they are entered on the command line.\n"
    "\n"
    "N.B.: The 3 '-1blur' options just provide different ways of\n"
    "      specifying the radius used for the blurring function.\n"
    "      The relationships among these specifications are\n"
    "         sigma = 0.57735027 * rms = 0.42466090 * fwhm\n"
    "      The requisite convolutions are done using FFTs; this is by\n"
    "      far the slowest operation among the editing options.\n"
;

char * EDIT_options_help(void)
{
   return helpstring ;
}

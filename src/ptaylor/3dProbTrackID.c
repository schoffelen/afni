/* 
   Probabilistic tracking,  first draft at AFNIfication, March 2012.
   using FACTID code, from Taylor, Kuan-Hung, Lin and Biswal (2012)

	Sept 2012: fixing some memory stuff.

	Nov 2012: ability to use ROI labels which are non-consecutive ints.

	Dec 2012: 
	        + allow non-FA map to define `WM' for tracks,
			  + can include brainmask, if wanted.
			  + allow both nifti and afni inputs to be read 
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <debugtrace.h>
#include <mrilib.h>     
#include <3ddata.h>     
#include <DoTrackit.h>
#include <time.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <TrackIO.h>


													


void usage_ProbTrackID(int detail) 
{
	printf(
"  \n"
"  FACTID-based code, from Taylor, Cho, Lin and Biswal (some year!)\n"
"  \n"
"  Estimate locations of WM associated with GM ROIs, particularly between\n"
"    pairs of GM in a network;  can process several networks in a given run.\n"
"\n"
"  Reads in DT-related data from, e.g., 3dDWItoDTI, and\n"
"    also uses results from 3dDWUncert for uncertainty measures.\n"
"\n\n"
"  + OUTPUTS, named using prefix; done per network of N_ROI ROIs, and \n"
"    having (N_ROI+1) subbricks:\n"
"    1) MAP/MASK file, AFNI format with \n"
"         0th brick containing the number of tracts being a mask of all\n"
"         voxels through which more than the threshold number of tracts\n"
"         passed through, traversing individual and all combinatorial pairs\n"
"         of ROIs; re. threshold, see `NmNsFr' in input description.\n"
"         i-th brick (i running from 1 to N_ROI) containing the voxels\n"
"         comprising WM ROIs.  Voxels connecting i-th and j-th GM ROIs have\n"
"         value 2^j, the values are summed if a given voxel is in multiple\n"
"         WM ROIs; voxels in tracts which just past through i-th WM ROI\n"
"         have value 2^i.  (NB: this kind of assumes that not more than\n"
"         ~15 GM ROIs will form a given network.)\n"
"    2) GRID file, simple statistics in matrix format (N_ROI by N_ROI per \n"
"         network).  In file, first there is the N_ROI in that network, and\n"
"         the following N_ROI by N_ROI matrices (all have diagonal entries,\n"
"         where diagonal is for tracts just through (i-1)th ROI alone):\n"
"         1- Numbers of tracts connecting ROIs (pairwise AND logic)\n"
"         2- Ratio of numbers of tracts connecting ROIS to total number\n"
"            of tracts found through all iters (prob not too meaningful)\n"
"         3- Mean of FA value in WM ROIs connecting GM-ROIs \n"
"         4- Associated stdev of FA value in WM ROIs connecting two GM-ROIs\n"
"         5- Mean of MD value in WM ROIs connecting GM-ROIs \n"
"         6- Associated stdev of MD value in WM ROIs connecting two GM-ROIs\n"
"         7- Mean of RD value in WM ROIs connecting GM-ROIs \n"
"         8- Associated stdev of RD value in WM ROIs connecting two GM-ROIs\n"
"         9- Mean of L1 value in WM ROIs connecting GM-ROIs \n"
"        10- Associated stdev of L1 value in WM ROIs connecting two GM-ROIs\n"
"        11- Numbers of voxels in WM ROI connecting GM-ROIs \n"
"\n\n"
"  + TRACTOGRAPHY OPTIONS via {-algopt [file of tractog options}: \n"
"        MinFA    :FA cutoff value, lower bound\n"
"        MaxAng   :max angle (in deg) to propagate between vox\n"
"        MinL     :min physical length (in mm) of tracts to keep\n"
"        NmNsFr   :fractional threshold for number of tracts which must\n"
"                  pass through vox to be included in WM ROI (e.g., 0.05).\n"
"                  It is the frac of Nseed*Nmonte to provide threshold.\n"
"        Nseed    :number of seeds per vox, will be placed randomly\n"
"        Nmonte   :number of Monte Carlo iterations\n"
"        M        :number of grads used to scan (not used yet)\n"
"        bval     :non-b=0 value (mm^2/s), e.g., 1000 (not used yet)\n"
"\n\n"
"  + RUNNING, need to provide:\n"
"    -netrois ROIS   :mask(s) of ROIs- can be many subbricks!\n"
"    -uncert  FILE   :uncertainty values [6 subbricks] \n"
"    -prefix  PREFIX :output file name part\n"
"    -input   INPREF :Basename of DTI volumes output by, e.g., 3dDWItoDT.\n"
"                     NB- following volumes are currently expected:\n"
"                     INPREF_V1, INPREF_MD, INPREF_L1, INPREF_FA,\n" 
"                     INPREF_V2 and INPREF_V3\n"
"    -algopt  ALGO   :ASCII file with eight numbers defining parameter \n"
"                     quantities just above. NB: Diff than that for\n"
"                     3dTrackID!!!\n"
"    -extra_set SET  :if you want to use a non-FA derived definition for the\n"
"                     WM skeleton in which tracts run, you can input one, and\n"
"                     then the threshold in the ALGO file refers to values\n"
"                     in this map. (Then the uncertainty in FA measures does\n"
"                     not really matter during the Monte Carlo part.)\n"
"    -mask   MASK    :can include a brainmask within which to calculate \n"
"                     things. Otherwise, data should be masked already.\n"
"    -cut_at_rois    :for pairwise track connections, only keep voxels in\n"
"                     tracks between the ROIs they connect (i.e., cut off\n"
"                     tracks which run beyond ROIs). Even with this switch\n"
"                     on, for individual ROI stats and total tracking maps,\n"
"                     all voxels are kept.\n"
"    -write_opts :Write out all the option values into PREFIX.niml.opts.\n" 
"    -write_rois :Write out all the ROI labels in PREFIX.roi.labs, in 3cols:\n" 
"                 Input_ROI   Condensed_form_ROI   Power_of_2_label\n"
"    -verb VERB :Verbosity level, default is 0.\n"
"\n\n"
"  + EXAMPLE:\n"
"      Download and install this archive:\n"
"         curl -O http://..../FACTID_draft.tar.gz \n"
"      tar xvzf FACTID_draft.tar.gz\n"
"      (requires output of 3dDWUncert program's sample command)\n"
"      3dProbTrackID \\\n"
"        -netrois TEST_FILES/DTI/masks_together+orig.BRIK \\\n"
"        -uncert TEST_FILES/DTI/o.UNCERT_TESTb_UNC+orig.BRIK \\\n"
"        -prefix TEST_FILES/DTI/o.PROB5 \\\n"
"        -input TEST_FILES/DTI/DT \\\n"
"        -algopt TEST_FILES/ALGOPTS_PROB.dat\n"
"\n\n"
"  +NB: The integer labels of ROIs no longer have to be purely\n"
"       consecutive; this allows a bit more flexibility.  But in \n"
"       order to have the labels not get *too* huge or have blank \n"
"       output briks, the ROI labels get mapped as follows: \n"
"       Say you have 3 ROIs but the labels are {1,2,24}, for whatever \n"
"       reason; the output labels will condense this to treat it LIKE \n"
"       {1,2,3}-- thus, the labels stay as small as possible (which is a \n"
"       a legit concern since labels are 2^{ROI #}).\n"
"\n\n"
"  If you use this program, please reference for the tractography alg:\n"
"    Taylor PA, Cho K-H, Lin C-P, Biswal BB (2012) Improving DTI\n"
"    Tractography by including Diagonal Tract Propagation. PLoS ONE\n"
"    7(9): e43415. \n");
return;
}
		
		

int main(int argc, char *argv[]) {
	int i,j,k,m,n,ii,jj,kk,mm,gg,nn,hh,bb,cc,rr,ss,tt;
	int iarg;
	int nmask1=0;
	int nmask2=0;
	THD_3dim_dataset *insetV1 = NULL,*insetV2 = NULL,*insetV3 = NULL;
	THD_3dim_dataset *insetL1 = NULL;
	THD_3dim_dataset *insetFA = NULL,*insetMD = NULL,*insetUC = NULL;
	THD_3dim_dataset *mset1=NULL; 
	THD_3dim_dataset *networkMAPS=NULL;
	THD_3dim_dataset *networkMAPS2=NULL;

	THD_3dim_dataset *insetEXTRA=NULL; 
	char *in_EXTRA="name";//[300];
	int EXTRAFILE=0; // switch for whether other file is input as WM map

	THD_3dim_dataset *MASK=NULL;
	char in_mask[300];
	int HAVE_MASK=0;
	int ***mskd; // define mask of where time series are nonzero

	char *prefix="tracky" ;
	char in_FA[300];
	char in_V1[300];
	char in_V2[300];
	char in_V3[300];
	char in_MD[300];
	char in_L1[300];

	char OUT_bin[300];
	char OUT_grid[300];
	char OUT_map[300];
	char OUT_mask[300];
	char OUT_rois[300];
	FILE *fin4, *fout1;

	// FACT algopts
	float MinFA=0.2,MaxAngDeg=45.0,MinL=20.0,NmNsFr=0.05;
	float MaxAng;
	int NmNsThr=0;
	int ArrMax=0;
	int ROIS_OUT=0;
	
	int Nvox=-1;   // tot number vox
	int Ndata=-1;
	int Dim[3]={0,0,0}; // dim in each dir
	int Nseed=5,M=30,bval=1000;
	float **LocSeed=NULL; // fractional locations within voxel, 
	float Ledge[3]; // voxel edge lengths

	int **Tforw=NULL, **Tback=NULL;
	int **Ttot=NULL;
	float **flTforw=NULL, **flTback=NULL;
	float **coorded=NULL;
	float **copy_coorded=NULL; // copy will have perturbed info;
	int ***INDEX=NULL,***INDEX2=NULL;
	int **MAPROI=NULL; // store what ROIs are where, per data voxel
	int ****NETROI=NULL; // store connection info
	int len_forw, len_back; // int count of num of squares through
	float phys_forw[1], phys_back[1];
	int idx,idx2;

	int in[3]; // to pass to trackit
	float physin[3]; // also for trackit, physical loc, 
	int totlen; 
	float totlen_phys;
	int Numtract; 

	float READS_fl;
	int end[2][3];
	int test_ind[2];
	int onoff[3];
	int BreakAddCont=0;

	char dset_or[4] = "RAI";
	char voxel_order[4]="---";
	THD_3dim_dataset *dsetn=NULL;
	int TV_switch[3] = {0,0,0};

	int ***Prob_grid=NULL;// will be a NROIxNROI grid, where NROI is num of ROIs
	float ****Param_grid=NULL;// same size as Prob_Grid (extra dim for ROI stats)
	int *list_rois=NULL, *temp_list=NULL;
	int MAXNROI=0;
	int Nmonte=250;
	int N_nets=0;
	float w2,w3,thetval;
	float tempv[3];
	float tempvmagn;
	float testang;

	char **prefix_netmap=NULL;
	char **prefix_netmap2=NULL;
	char **prefix_mapfile=NULL;
	int *NROI=NULL,*INVROI=NULL;
	int **ROI_LABELS=NULL, **INV_LABELS=NULL; // allow labels to be non-consec
	long seed;   
	const gsl_rng_type * T=NULL;
	gsl_rng *r=NULL;
	NI_element *nel=NULL;
	int dump_opts=0;
	int ONLY_BT = 0; // don't cut off tracks at ROI endpts
	int OUT_MAPFILE=0; // output maskdump-like version of each set of ROIs
	char *postfix[4]={"+orig.HEAD\0",".nii.gz\0",".nii\0","+tlrc.HEAD\0"};
	int  FOUND = -1;

	// for random number generation
	srand(time(0));
	seed = time(NULL) ;
	gsl_rng_env_setup();
	T = gsl_rng_default;
	r = gsl_rng_alloc (T);
	gsl_rng_set (r, seed);
  
	mainENTRY("3dProbTrackID"); machdep(); 
  
	// ****************************************************************
	// ****************************************************************
	//                    load AFNI stuff
	// ****************************************************************
	// ****************************************************************
  
	INFO_message("version: OMICRON");

	// scan args 
	if (argc == 1) { usage_ProbTrackID(1); exit(0); }
	iarg = 1;
	while( iarg < argc && argv[iarg][0] == '-' ){
		if( strcmp(argv[iarg],"-help") == 0 || 
			 strcmp(argv[iarg],"-h") == 0 ) {
			usage_ProbTrackID(strlen(argv[iarg])>3 ? 2:1);
			exit(0);
		}

		if( strcmp(argv[iarg],"-verb") == 0) {
			if( ++iarg >= argc ) 
				ERROR_exit("Need argument after '-verb'") ;
			set_tract_verb(atoi(argv[iarg]));
			iarg++ ; continue ;
		}
	 
		if( strcmp(argv[iarg],"-write_opts") == 0) {
			dump_opts=1;
			iarg++ ; continue ;
		}
    
		if( strcmp(argv[iarg],"-write_rois") == 0) {
			ROIS_OUT=1;
			iarg++ ; continue ;
		}

		if( strcmp(argv[iarg],"-netrois") == 0 ){
			if( ++iarg >= argc ) 
				ERROR_exit("Need argument after '-netrois'") ;
			mset1 = THD_open_dataset( argv[iarg] ) ;
			if( mset1 == NULL ) 
				ERROR_exit("Can't open netrois dataset '%s'", argv[iarg]) ;
			DSET_load(mset1) ; CHECK_LOAD_ERROR(mset1) ;
			nmask1 = DSET_NVOX(mset1) ;
			N_nets = DSET_NVALS(mset1) ;
      
			prefix_netmap = calloc( N_nets,sizeof(prefix_netmap));  
			for(i=0 ; i<N_nets ; i++) 
				prefix_netmap[i] = calloc( 300,sizeof(char)); 
			prefix_netmap2 = calloc( N_nets,sizeof(prefix_netmap2));  
			for(i=0 ; i<N_nets ; i++) 
				prefix_netmap2[i] = calloc( 300,sizeof(char)); 
			prefix_mapfile = calloc( N_nets,sizeof(prefix_mapfile));  
			for(i=0 ; i<N_nets ; i++) 
				prefix_mapfile[i] = calloc( 300,sizeof(char)); 
			NROI = (int *)calloc(N_nets, sizeof(int)); 
			INVROI = (int *)calloc(N_nets, sizeof(int)); 

			if( (NROI == NULL) || (INVROI == NULL) || (prefix_netmap == NULL) ||
				 (prefix_netmap2 == NULL) || (prefix_mapfile == NULL)) {
				fprintf(stderr, "\n\n MemAlloc failure.\n\n");
				exit(122);
			}
      
			for( i=0 ; i<N_nets ; i++) {
				// NROI[i] = (int) THD_subbrick_max(mset1, i, 1);
				INVROI[i] = (int) THD_subbrick_max(mset1, i, 1);
				//if( NROI[i]>MAXNROI )
				//	MAXNROI = NROI[i];
			}
			
			ROI_LABELS = calloc( N_nets,sizeof(ROI_LABELS));  
			for(i=0 ; i<N_nets ; i++) 
				ROI_LABELS[i] = calloc(INVROI[i]+1,sizeof(int)); 
			INV_LABELS = calloc( N_nets,sizeof(INV_LABELS));  
			for(i=0 ; i<N_nets ; i++) 
				INV_LABELS[i] = calloc(INVROI[i]+1,sizeof(int)); 
			if( (ROI_LABELS == NULL) || (ROI_LABELS == NULL) ) {
				fprintf(stderr, "\n\n MemAlloc failure.\n\n");
				exit(123);
			}
			
			// after this, ROI_LABELS will have network labels store compactly
			// and NROI will have the actual number of ROI per netw, not just 
			// the max label value
			// INV_LABELS has compact index labels at input label locations,
			// hence is inverse of ROI_LABELS
			// ** This function allows us to switch to talk about with i-th index
			// label is used, using NROI to define sizes of things
			bb = ViveLeRoi(mset1, ROI_LABELS, INV_LABELS, NROI, INVROI);
			if( bb != 1)
				ERROR_exit("Problem loading/assigning ROI labels");
			
			for( i=0 ; i<N_nets ; i++) {
				if( NROI[i]>MAXNROI )
					MAXNROI = NROI[i];
			}

			for( bb=0 ; bb<N_nets ; bb++)
				INFO_message("Number of ROIs in netw[%d]=%d",bb+1,NROI[bb]);
      
			iarg++ ; continue ;
		}
    
		if( strcmp(argv[iarg],"-uncert") == 0 ){
			if( ++iarg >= argc ) 
				ERROR_exit("Need argument after '-uncert'") ;
			insetUC = THD_open_dataset( argv[iarg] ) ;
			if( insetUC == NULL ) 
				ERROR_exit("Can't open uncert dataset '%s'",
							  argv[iarg]) ;
			DSET_load(insetUC) ; CHECK_LOAD_ERROR(insetUC);
			nmask2 = DSET_NVOX(insetUC);
			iarg++ ; continue ;
      }
    
		if( strcmp(argv[iarg],"-prefix") == 0 ){
			iarg++ ; if( iarg >= argc ) 
							ERROR_exit("Need argument after '-prefix'");
			prefix = strdup(argv[iarg]) ;
			if( !THD_filename_ok(prefix) ) 
				ERROR_exit("Illegal name after '-prefix'");
			iarg++ ; continue ;
		}
      
		if( strcmp(argv[iarg],"-input") == 0 ){
			iarg++ ; if( iarg >= argc ) 
							ERROR_exit("Need argument after '-input'");
			//sprintf(in_FA,"%s_FA+orig", argv[iarg]); 
			
			for( i=0 ; i<4 ; i++) {
				sprintf(in_FA,"%s_FA%s", argv[iarg],postfix[i]); 
				if(THD_is_ondisk(in_FA)) {
					FOUND = i;
					break;
				}
			}
			insetFA = THD_open_dataset(in_FA) ;//argv[iarg] ) ;
			if( insetFA == NULL ) 
				ERROR_exit("Can't open dataset '%s':FA",in_FA);
			DSET_load(insetFA) ; CHECK_LOAD_ERROR(insetFA) ;
			Nvox = DSET_NVOX(insetFA) ;
			Dim[0] = DSET_NX(insetFA); Dim[1] = DSET_NY(insetFA); 
			Dim[2] = DSET_NZ(insetFA); 
			Ledge[0] = fabs(DSET_DX(insetFA)); Ledge[1] = fabs(DSET_DY(insetFA)); 
			Ledge[2] = fabs(DSET_DZ(insetFA)); 
			// check tot num vox match (as proxy for dims...)
			if( (Nvox != nmask1) || (Nvox != nmask2) )
				ERROR_exit("Input dataset does not match both mask volumes!");
        
			// this stores the original data file orientation for later use,
			// as well since we convert everything to RAI temporarily, as
			// described below
			voxel_order[0]=ORIENT_typestr[insetFA->daxes->xxorient][0];
			voxel_order[1]=ORIENT_typestr[insetFA->daxes->yyorient][0];
			voxel_order[2]=ORIENT_typestr[insetFA->daxes->zzorient][0];
			voxel_order[3]='\0';
			for( i=0 ; i<3 ; i++) 
				TV_switch[i] = !(dset_or[i]==voxel_order[i]);
			
			FOUND = -1;
			for( i=0 ; i<4 ; i++) {
				sprintf(in_V1,"%s_V1%s", argv[iarg],postfix[i]); 
				if(THD_is_ondisk(in_V1)) {
					FOUND = i;
					break;
				}
			}
			//sprintf(in_V1,"%s_V1+orig", argv[iarg]); 
			insetV1 = THD_open_dataset(in_V1);
			if( insetV1 == NULL ) 
				ERROR_exit("Can't open dataset '%s':V1",in_V1);
			DSET_load(insetV1) ; CHECK_LOAD_ERROR(insetV1) ;

			
			FOUND = -1;
			for( i=0 ; i<4 ; i++) {
				sprintf(in_V2,"%s_V2%s", argv[iarg],postfix[i]); 
				if(THD_is_ondisk(in_V2)) {
					FOUND = i;
					break;
				}
			}
			//sprintf(in_V2,"%s_V2+orig", argv[iarg]); 
			insetV2 = THD_open_dataset(in_V2);
			if( insetV2 == NULL ) 
				ERROR_exit("Can't open dataset '%s':V2",in_V2);
			DSET_load(insetV2) ; CHECK_LOAD_ERROR(insetV2) ;

			FOUND = -1;
			for( i=0 ; i<4 ; i++) {
				sprintf(in_V3,"%s_V3%s", argv[iarg],postfix[i]); 
				if(THD_is_ondisk(in_V3)) {
					FOUND = i;
					break;
				}
			}
			//sprintf(in_V3,"%s_V3+orig", argv[iarg]); 
			insetV3 = THD_open_dataset(in_V3);
			if( insetV3 == NULL ) 
				ERROR_exit("Can't open dataset '%s':V3",in_V3);
			DSET_load(insetV3) ; CHECK_LOAD_ERROR(insetV3) ;

			FOUND = -1;
			for( i=0 ; i<4 ; i++) {
				sprintf(in_MD,"%s_MD%s", argv[iarg],postfix[i]); 
				if(THD_is_ondisk(in_MD)) {
					FOUND = i;
					break;
				}
			}
			//sprintf(in_MD,"%s_MD+orig", argv[iarg]); 
			insetMD = THD_open_dataset(in_MD);
			if( insetMD == NULL ) 
				ERROR_exit("Can't open dataset '%s':MD",in_MD);
			DSET_load(insetMD) ; CHECK_LOAD_ERROR(insetMD) ;
			
			FOUND = -1;
			for( i=0 ; i<4 ; i++) {
				sprintf(in_L1,"%s_L1%s", argv[iarg],postfix[i]); 
				if(THD_is_ondisk(in_L1)) {
					FOUND = i;
					break;
				}
			}
			//sprintf(in_L1,"%s_L1+orig", argv[iarg]); 
			insetL1 = THD_open_dataset(in_L1);
			if( insetL1 == NULL ) 
				ERROR_exit("Can't open dataset '%s':L1",in_L1);
			DSET_load(insetL1) ; CHECK_LOAD_ERROR(insetL1) ;

			iarg++ ; continue ;
		}
    
		if( strcmp(argv[iarg],"-algopt") == 0 ){
			iarg++ ; 
			if( iarg >= argc ) 
				ERROR_exit("Need argument after '-algopt'");
      
			if (!(nel = ReadProbTractAlgOpts(argv[iarg]))) {
				ERROR_message("Failed to read options in %s\n", argv[iarg]);
				exit(19);
			}
			if (NI_getProbTractAlgOpts(nel, &MinFA, &MaxAngDeg, &MinL, 
												&NmNsFr,&Nseed,&Nmonte,&M,&bval)) {
				ERROR_message("Failed to get options");
				exit(1);
			}
			NI_free_element(nel); nel=NULL;
				
			iarg++ ; continue ;
		}
		
		if( strcmp(argv[iarg],"-extra_set") == 0) {
			if( ++iarg >= argc ) 
				ERROR_exit("Need argument after '-extra_set'");
			EXTRAFILE = 1; // switch on
			in_EXTRA = argv[iarg];
			
			insetEXTRA = THD_open_dataset(in_EXTRA);
			if( (insetEXTRA == NULL ) )
				ERROR_exit("Can't open dataset '%s': for extra set.",in_EXTRA);
			DSET_load(insetEXTRA) ; CHECK_LOAD_ERROR(insetEXTRA) ;
			
			if( !((Dim[0] == DSET_NX(insetEXTRA)) && 
					(Dim[1] == DSET_NY(insetEXTRA)) &&
					(Dim[2] == DSET_NZ(insetEXTRA))))
				ERROR_exit("Dimensions of extra set '%s' don't match those of the DTI prop ones ('%s', etc.).",in_EXTRA, in_FA);
			
			iarg++ ; continue ;
		}

		if( strcmp(argv[iarg],"-mask") == 0 ){
			iarg++ ; if( iarg >= argc ) 
							ERROR_exit("Need argument after '-mask'");
			HAVE_MASK=1;
			
			sprintf(in_mask,"%s", argv[iarg]); 
			MASK = THD_open_dataset(in_mask) ;
			if( (MASK == NULL ))
				ERROR_exit("Can't open time series dataset '%s'.",in_mask);
			
			DSET_load(MASK); CHECK_LOAD_ERROR(MASK);
			
			iarg++ ; continue ;
		}

		if( strcmp(argv[iarg],"-cut_at_rois") == 0) {
			ONLY_BT=1;
			iarg++ ; continue ;
		}

		if( strcmp(argv[iarg],"-dump_trks") == 0) {
			OUT_MAPFILE=1;
			iarg++ ; continue ;
		}

		ERROR_message("Bad option '%s'\n",argv[iarg]) ;
		suggest_best_prog_option(argv[0], argv[iarg]);
		exit(1);
	}

	if (iarg < 5) {
		ERROR_message("Too few options. Try -help for details.\n");
		exit(1);
	}

	if (dump_opts) {
		nel = NI_setProbTractAlgOpts(NULL, &MinFA, &MaxAngDeg, &MinL, 
											  &NmNsFr,&Nseed,&Nmonte, &M, &bval);
		WriteTractAlgOpts(prefix, nel);
		NI_free_element(nel); nel=NULL;
	}

	if(ROIS_OUT) {
		for( k=0 ; k<N_nets ; k++) { // each netw gets own file
			sprintf(OUT_rois,"%s_%03d.roi.labs",prefix,k+1);
			if( (fout1 = fopen(OUT_rois, "w")) == NULL) {
				fprintf(stderr, "Error opening file %s.",OUT_rois);
				exit(19);
			}
			for( i=1 ; i<=NROI[k] ; i++ ) {
				fprintf(fout1,"%d\t\t%d\t\t%d\n",ROI_LABELS[k][i],i,
						  (int) pow(2,i));
			}
			fclose(fout1);    
		}
	}
	

	// Process the options a little 
	LocSeed = calloc(Nseed,sizeof(LocSeed)); 
	for(i=0 ; i<Nseed ; i++) 
		LocSeed[i] = calloc(3,sizeof(float)); 
  
	// convert to cos of rad value for comparisons, instead of using acos()
	MaxAng = cos(CONV*MaxAngDeg); 
  
	// will take stats on voxels with number of tracts >=NmNsThr
	NmNsThr =  (int) ceil(NmNsFr*Nseed*Nmonte); 
	INFO_message("Effective Monte Iters:%d. Voxel threshold:%d.",
					 Nseed*Nmonte,NmNsThr);


	// have all be RAI for processing here
	if(TV_switch[0] || TV_switch[1] || TV_switch[2]) {
		dsetn = r_new_resam_dset(insetFA, NULL, 0.0, 0.0, 0.0,
										 dset_or, RESAM_NN_TYPE, NULL, 1, 0);
		DSET_delete(insetFA); 
		insetFA=dsetn;
		dsetn=NULL;
    
		dsetn = r_new_resam_dset(insetMD, NULL, 0.0, 0.0, 0.0,
										 dset_or, RESAM_NN_TYPE, NULL, 1, 0);
		DSET_delete(insetMD); 
		insetMD=dsetn;
		dsetn=NULL;
    
		dsetn = r_new_resam_dset(insetV1, NULL, 0.0, 0.0, 0.0,
										 dset_or, RESAM_NN_TYPE, NULL, 1, 0);
		DSET_delete(insetV1); 
		insetV1=dsetn;
		dsetn=NULL;
    
		dsetn = r_new_resam_dset(insetV2, NULL, 0.0, 0.0, 0.0,
										 dset_or, RESAM_NN_TYPE, NULL, 1, 0);
		DSET_delete(insetV2); 
		insetV2=dsetn;
		dsetn=NULL;
    
		dsetn = r_new_resam_dset(insetV3, NULL, 0.0, 0.0, 0.0,
										 dset_or, RESAM_NN_TYPE, NULL, 1, 0);
		DSET_delete(insetV3); 
		insetV3=dsetn;
		dsetn=NULL;
    
		dsetn = r_new_resam_dset(insetL1, NULL, 0.0, 0.0, 0.0,
										 dset_or, RESAM_NN_TYPE, NULL, 1, 0);
		DSET_delete(insetL1); 
		insetL1=dsetn;
		dsetn=NULL;
    
		dsetn = r_new_resam_dset(mset1, NULL, 0.0, 0.0, 0.0,
										 dset_or, RESAM_NN_TYPE, NULL, 1, 0);
		DSET_delete(mset1); 
		mset1=dsetn;
		dsetn=NULL;
    
		dsetn = r_new_resam_dset(insetUC, NULL, 0.0, 0.0, 0.0,
										 dset_or, RESAM_NN_TYPE, NULL, 1, 0);
		DSET_delete(insetUC); 
		insetUC=dsetn;
		dsetn=NULL;

		if(EXTRAFILE) {
			dsetn = r_new_resam_dset(insetEXTRA, NULL, 0.0, 0.0, 0.0,
											 dset_or, RESAM_NN_TYPE, NULL, 1, 0);
			DSET_delete(insetEXTRA); 
			insetEXTRA=dsetn;
			dsetn=NULL;
		}

		if(HAVE_MASK) {
			dsetn = r_new_resam_dset(MASK, NULL, 0.0, 0.0, 0.0,
											 dset_or, RESAM_NN_TYPE, NULL, 1, 0);
			DSET_delete(MASK); 
			MASK=dsetn;
			dsetn=NULL;
		}
	
	}
  
  
  
	// ****************************************************************
	// ****************************************************************
	//                    make arrays for tracking
	// ****************************************************************
	// ****************************************************************
  
	// for temp storage array, just a multiple of longest dimension!
	// essentially, a buffer size per tract we're making
	if(Dim[0] > Dim[1])
		ArrMax = Dim[0] * 4;
	else
		ArrMax = Dim[1] * 4;
	if(4*Dim[2] > ArrMax)
		ArrMax = Dim[2] * 4;
  
	// will hold indices of all voxels with actual data, i.e., takes in
	// (i,j,k) coor and gives index, for efficiency of storage because
	// lots of zeros in (Dimx, Dimy,Dimz grid).
	// INDEX:  for afni THD_* things
	// INDEX2: for coor*, NETROI
	INDEX = (int ***) calloc( Dim[0], sizeof(int **) );
	for ( i = 0 ; i < Dim[0] ; i++ ) 
		INDEX[i] = (int **) calloc( Dim[1], sizeof(int *) );
	for ( i = 0 ; i < Dim[0] ; i++ ) 
		for ( j = 0 ; j < Dim[1] ; j++ ) 
			INDEX[i][j] = (int *) calloc( Dim[2] , sizeof(int) );
	INDEX2 = (int ***) calloc( Dim[0] , sizeof(int **) );
	for ( i = 0 ; i < Dim[0] ; i++ ) 
		INDEX2[i] = (int **) calloc( Dim[1] , sizeof(int *) );
	for ( i = 0 ; i < Dim[0] ; i++ ) 
		for ( j = 0 ; j < Dim[1] ; j++ ) 
			INDEX2[i][j] = (int *) calloc( Dim[2], sizeof(int) );
  
	mskd = (int ***) calloc( Dim[0], sizeof(int **) );
	for ( i = 0 ; i < Dim[0] ; i++ ) 
		mskd[i] = (int **) calloc( Dim[1], sizeof(int *) );
	for ( i = 0 ; i < Dim[0] ; i++ ) 
		for ( j = 0 ; j < Dim[1] ; j++ ) 
			mskd[i][j] = (int *) calloc( Dim[2], sizeof(int) );
	
   if( (INDEX == NULL) || (INDEX2 == NULL) || (mskd == NULL) ) {
		fprintf(stderr, "\n\n MemAlloc failure.\n\n");
		exit(121);
	}

   
	Ndata = 0;
	idx = 0;
	// determine how many voxels of actual data there are
	for( k=0 ; k<Dim[2] ; k++ ) 
		for( j=0 ; j<Dim[1] ; j++ ) 
			for( i=0 ; i<Dim[0] ; i++ ) {
				INDEX[i][j][k] = idx;
				if( ((HAVE_MASK==0) && (THD_get_voxel(insetL1,idx,0)>EPS_V) ) ||
					 ( HAVE_MASK && (THD_get_voxel(MASK,idx,0)>0) ) )
					{ 
						mskd[i][j][k]=1;
						Ndata+=1;
						INDEX2[i][j][k] = Ndata;
					}
				else
					INDEX2[i][j][k] = 0; 
				idx+=1;
			}
	// now, Ndata is number of voxels of actual data, will be size of grids.
	
	if( Ndata<1) {
		fprintf(stderr, "\n\n Too few data voxels-- wrong scale of things?\n\n");
		exit(1221);
	}
	INFO_message("Ndata: %d.  Nvox: %d",Ndata,Nvox);

	// 3 comp of V1 and FA for each data voxel
	coorded = calloc( (Ndata+1),sizeof(coorded)); // to have all ind be >=1
	for(i=0 ; i<=Ndata ; i++) 
		coorded[i] = calloc(4,sizeof(float)); 
	// copy for perturb
	copy_coorded = calloc( (Ndata+1),sizeof(copy_coorded)); 
	for(i=0 ; i<=Ndata ; i++) // to have all ind be >=1
		copy_coorded[i] = calloc(4,sizeof(float)); 

	MAPROI = calloc( (Ndata+1),sizeof(MAPROI)); // to have all ind be >=1
	for(i=0 ; i<=Ndata ; i++) 
		MAPROI[i] = calloc(N_nets,sizeof(int)); 

	NETROI = (int ****) calloc( (Ndata+1), sizeof(int ***) );
	for ( i = 0 ; i<=Ndata ; i++ ) 
		NETROI[i] = (int ***) calloc( N_nets, sizeof(int **) );
	for ( i=0 ; i<=Ndata ; i++ ) 
		for ( j=0 ; j<N_nets ; j++ ) // jth net has NROI[j] rois
			NETROI[i][j] = (int **) calloc( NROI[j], sizeof(int *) );
	for ( i=0 ; i<=Ndata ; i++ ) 
		for ( j=0 ; j<N_nets ; j++ ) 
			for ( k=0 ; k<NROI[j] ; k++ ) 
				NETROI[i][j][k] = (int *) calloc( NROI[j], sizeof(int) );

	if( (coorded == NULL) || (copy_coorded == NULL) 
		 || (NETROI == NULL) || (MAPROI == NULL)) {
		fprintf(stderr, "\n\n MemAlloc failure.\n\n");
		exit(122);
	}
  
	Prob_grid = (int ***) calloc( N_nets, sizeof(int **));
	for ( i = 0 ; i < N_nets ; i++ ) 
		Prob_grid[i] = (int **) calloc( (NROI[i]), sizeof(int *));
	for ( i = 0 ; i < N_nets ; i++ ) 
		for ( j = 0 ; j < (NROI[i]) ; j++ ) 
			Prob_grid[i][j] = (int *) calloc( (NROI[i]), sizeof(int));

	Param_grid = (float ****) calloc( N_nets, sizeof(float ***));
	for ( i = 0 ; i < N_nets ; i++ ) 
		Param_grid[i] = (float ***) calloc( (NROI[i]), sizeof(float **));
	for ( i = 0 ; i < N_nets ; i++ ) 
		for ( j = 0 ; j < (NROI[i]) ; j++ ) 
			Param_grid[i][j] = (float **) calloc( (NROI[i]), sizeof(float *));
	for ( i = 0 ; i < N_nets ; i++ ) 
		for ( j = 0 ; j < (NROI[i]) ; j++ ) 
			for ( k = 0 ; k < (NROI[i]) ; k++ )// mu and std of FA,MD,RD,L1; count
				Param_grid[i][j][k] = (float *) calloc( 9, sizeof(float));

	temp_list = ( int *)calloc((MAXNROI+1), sizeof( int)); 
	list_rois = ( int *)calloc((MAXNROI+1), sizeof( int)); 

	Tforw = calloc(ArrMax,sizeof(Tforw)); 
	for(i=0 ; i<ArrMax ; i++) 
		Tforw[i] = calloc(3,sizeof(int)); 
	Ttot = calloc(2*ArrMax,sizeof(Ttot)); 
	for(i=0 ; i<2*ArrMax ; i++) 
		Ttot[i] = calloc(3,sizeof(int)); 
	Tback = calloc(ArrMax,sizeof(Tback)); 
	for(i=0 ; i<ArrMax ; i++) 
		Tback[i] = calloc(3,sizeof(int)); 
	// temp storage whilst tracking, physical loc
	flTforw = calloc(ArrMax,sizeof(flTforw)); 
	for(i=0 ; i<ArrMax ; i++) 
		flTforw[i] = calloc(3,sizeof(int)); 
	flTback = calloc(ArrMax,sizeof(flTback)); 
	for(i=0 ; i<ArrMax ; i++) 
		flTback[i] = calloc(3,sizeof(int)); 
	if(  (flTback == NULL) || (Tforw == NULL) || (Tback == NULL) 
		  || (flTforw == NULL) || (temp_list == NULL) || (Ttot == NULL) 
		  || (list_rois == NULL) || (Param_grid == NULL)
		  || (Prob_grid == NULL) ) {
		fprintf(stderr, "\n\n MemAlloc failure.\n\n");
		exit(12);
	}
  
	// this will be what we fill in 
	for( k=0 ; k<N_nets ; k++ )
		for( i=0 ; i<(NROI[k]) ; i++ )
			for( j=0 ; j<(NROI[k]) ; j++ ) {
				Prob_grid[k][i][j] = 0;  
				for( ii=0 ; ii<9 ; ii++)
					Param_grid[k][i][j][ii] = 0.;  
			}
  
	// set up eigvecs in 3D coord sys,
	// mark off where ROIs are and keep index handy
	for( k=0 ; k<Dim[2] ; k++ ) 
		for( j=0 ; j<Dim[1] ; j++ ) 
			for( i=0 ; i<Dim[0] ; i++ ) 
				if(mskd[i][j][k]) {
					idx = INDEX2[i][j][k];
					for( m=0 ; m<3 ; m++ ) 
						coorded[idx][m]=copy_coorded[idx][m]=
							THD_get_voxel(insetV1,INDEX[i][j][k],m);
					coorded[idx][3]=copy_coorded[idx][3]=
						THD_get_voxel(insetFA,INDEX[i][j][k],0); 
     
					// apparently, some |V1| != 1... gotta fix
					tempvmagn = sqrt(copy_coorded[idx][0]*copy_coorded[idx][0]+
										  copy_coorded[idx][1]*copy_coorded[idx][1]+
										  copy_coorded[idx][2]*copy_coorded[idx][2]);
					if(tempvmagn<0.99) 
						for( m=0 ; m<3 ; m++ ) {
							copy_coorded[idx][m]/=tempvmagn;
							coorded[idx][m]/=tempvmagn;
						}
     
					for( m=0 ; m<N_nets ; m++ ) {
						// allow indentification by index --
						// now, since we allow non-consecutive ROI labels,
						// just use the compacted list numbers via INV_LABELS
						if( THD_get_voxel(mset1, INDEX[i][j][k], m)>0.5 )
							MAPROI[idx][m] = INV_LABELS[m][(int) THD_get_voxel(mset1,INDEX[i][j][k],m)];
						else
							MAPROI[idx][m] = 0;
       
						// counter for number of kept tracks passing through
						for( mm=0 ; mm<NROI[m] ; mm++ )
							for( rr=0 ; rr<NROI[m] ; rr++ )
								NETROI[idx][m][mm][rr] = 0;
					}
				}
  
  
	// *************************************************************
	// *************************************************************
	//                    Beginning of main loops
	// *************************************************************
	// *************************************************************

	Numtract = 0;
 
	for (gg=0 ; gg<Nmonte ; gg++) {
    
		// relative location of each seed within voxel for this iter
		for( k=0 ; k<Nseed ; k++ ) 
			for( j=0 ; j<3 ; j++ ) 
				LocSeed[k][j] = rand()*1.0/RAND_MAX;
        
		if( gg>0) // first time through is no change
			for( k=0 ; k<Dim[2] ; k++ ) 
				for( j=0 ; j<Dim[1] ; j++ ) 
					for( i=0 ; i<Dim[0] ; i++ ) {
						// only do region in brain mask
						if(mskd[i][j][k]) {
							idx = INDEX2[i][j][k];
							
							// these are weights determined by rotation angle,
							// (prob. determined by jackknifing with 3dDWUncert)
							// each tips in the +/- direc toward/away from each evec 
							// by averaging and that's why tan of angle is taken
							thetval = pow(THD_get_voxel(insetUC,INDEX[i][j][k],0),2) +
								pow(THD_get_voxel(insetUC,INDEX[i][j][k],1),2); // MSE
							testang = gsl_ran_gaussian_ziggurat(r,1.0)*sqrt(thetval);
							w2 = tan(testang); 

							thetval = pow(THD_get_voxel(insetUC,INDEX[i][j][k],2),2) +
								pow(THD_get_voxel(insetUC,INDEX[i][j][k],3),2); // MSE
							testang = gsl_ran_gaussian_ziggurat(r,1.0)*sqrt(thetval);
							w3 = tan(testang);
							for( m=0 ; m<3 ; m++)
								tempv[m] = coorded[idx][m] + 
									w2*THD_get_voxel(insetV2,INDEX[i][j][k],m) + 
									w3*THD_get_voxel(insetV3,INDEX[i][j][k],m);
							tempvmagn = sqrt(tempv[0]*tempv[0]+
												  tempv[1]*tempv[1]+tempv[2]*tempv[2]);
              
							for( m=0 ; m<3 ; m++)
								tempv[m]/= tempvmagn;
							for( m=0 ; m<3 ; m++)
								copy_coorded[idx][m] = tempv[m];
              
							copy_coorded[idx][3] = 
								THD_get_voxel(insetFA,INDEX[i][j][k],0) + 
								THD_get_voxel(insetUC,INDEX[i][j][k],4) +
								( THD_get_voxel(insetUC,INDEX[i][j][k],5) * 
								  gsl_ran_gaussian_ziggurat(r,1.0) );
							
							if(copy_coorded[idx][3] <0)
								copy_coorded[idx][3] =0.;
							if(copy_coorded[idx][3] >1)
								copy_coorded[idx][3] =1.;
						}
						else
							for( m=0 ; m<4 ; m++)
								copy_coorded[idx][m] = 0;
					}

		// this is where we start the tracking for a given data set
		// start of Monte Carlo loop
		for( k=0 ; k<Dim[2] ; k++ ) 
			for( j=0 ; j<Dim[1] ; j++ ) 
				for( i=0 ; i<Dim[0] ; i++ ) 
					if( (EXTRAFILE==0 && (copy_coorded[INDEX2[i][j][k]][3]>=MinFA))
						 || (EXTRAFILE && 
							  (THD_get_voxel(insetEXTRA, INDEX[i][j][k], 0)>=MinFA)) )
						for( kk=0 ; kk<Nseed ; kk++ ) {
							
							in[0] = i;
							in[1] = j;
							in[2] = k;
              
							for( jj=0 ; jj<3 ; jj++ ) 
								physin[jj] = ((float) in[jj]+LocSeed[kk][jj])*Ledge[jj];
              
							len_forw = TrackItP(copy_coorded, in, physin, Ledge, Dim, 
													  MinFA, MaxAng, ArrMax, Tforw, 
													  flTforw, 1, phys_forw,INDEX2);

							in[0] = i; // reset, because it's changed in TrackIt func
							in[1] = j;
							in[2] = k;
         
							for( jj=0 ; jj<3 ; jj++ ) 
								physin[jj] = ((float) in[jj]+LocSeed[kk][jj])*Ledge[jj];
         
							len_back = TrackItP(copy_coorded, in, physin, Ledge, Dim, 
													  MinFA, MaxAng, ArrMax, Tback, 
													  flTback, -1, phys_back,INDEX2);
         
							totlen = len_forw+len_back-1; // b/c of overlap of starts
							totlen_phys = phys_forw[0] + phys_back[0];
         
							if( totlen_phys >= MinL ) {
								Numtract += 1; //keeping tally of tot num of tracts

								// glue together for simpler notation later
								for( n=0 ; n<len_back ; n++) { // all of this
									rr = len_back-n-1; // read in backward
									for(m=0;m<3;m++)
										Ttot[rr][m] = Tback[n][m];
								}
								for( n=1 ; n<len_forw ; n++) { // skip first->overlap
									rr = n+len_back-1; // put after
									for(m=0;m<3;m++)
										Ttot[rr][m] = Tforw[n][m];
								}
      
								// <<So close and orthogonal condition>>:
								// test projecting ends, to see if they abut ROI.  
								// first, default choice, just retest known ends as default
								test_ind[0] = INDEX2[Ttot[0][0]][Ttot[0][1]][Ttot[0][2]];
								test_ind[1] = INDEX2[Ttot[totlen-1][0]][Ttot[totlen-1][1]][Ttot[totlen-1][2]]; 
								for(m=0;m<3;m++) { //actual projected ends
									end[1][m] = 2*Ttot[totlen-1][m]-Ttot[totlen-2][m];
									end[0][m] = 2*Ttot[0][m]-Ttot[1][m];
								}
								// make sure ok to test, then test, if->switch
								for( m=0 ; m<2 ; m++) 
									if( (end[m][0]>=0) && (end[m][1]>=0) && 
										 (end[m][2]>=0) && (end[m][0]<Dim[0]) && 
										 (end[m][1]<Dim[1]) && (end[m][2]<Dim[2]) )
										if( INDEX2[end[m][0]][end[m][1]][end[m][2]] != 0 )
											test_ind[m] =  INDEX2[end[m][0]][end[m][1]][end[m][2]];
                  
								for( hh=0 ; hh<N_nets ; hh++) {
									for( n=0 ; n<=MAXNROI ; n++)
										list_rois[n] = temp_list[n] = 0;
        
									// have to go all the way through each track 
									// checking every vox for intersections
									for( n=0 ; n<totlen ; n++) {
										rr = INDEX2[Ttot[n][0]][Ttot[n][1]][Ttot[n][2]];
										if(MAPROI[rr][hh]>0)
											list_rois[MAPROI[rr][hh]]=1;
									}
									// actually test if the ends, extra condition
									if(MAPROI[test_ind[0]][hh]>0)
										list_rois[MAPROI[test_ind[0]][hh]]=1;
									if(MAPROI[test_ind[1]][hh]>0)
										list_rois[MAPROI[test_ind[1]][hh]]=1;
        
									// now, for this track, record 
									// first, write shorter list of which ones were hit
									m = 0;
									for( n=1 ; n<=NROI[hh] ; n++)
										if(list_rois[n]>0 ) {
											// values stored are 1...NROI -> 0...NROI-1
											// keep track of which was hit
											temp_list[m] = n-1; 
											m = m+1;
										}
        
									// let's keep track of where tracts connecting 
									// regions go.
									// we'll keep stats on individ ROI tracks
									if( m>0) {
										if( (ONLY_BT==0) || (m==1) ) {

											for( mm=0 ; mm<totlen ; mm++) { // @@@
												rr = INDEX2[Ttot[mm][0]][Ttot[mm][1]][Ttot[mm][2]];// @@@
												
												for( bb=0 ; bb<m ; bb++)
													for( cc=0 ; cc<m ; cc++) { // only individual, or keep all
														NETROI[rr][hh][temp_list[bb]][temp_list[cc]]+=1;
														if(NETROI[rr][hh][temp_list[bb]][temp_list[cc]]==NmNsThr) 
															ss=ScoreTrackGrid(Param_grid,
																					INDEX[Ttot[mm][0]][Ttot[mm][1]][Ttot[mm][2]],
																					hh, temp_list[cc],temp_list[bb], 
																					insetFA,insetMD,insetL1);
													}
											}// @@@
										} // end of 'if only_bt or m==1'
										else{
											// first do diagonal/individual ones, because now we
											// have options for the pairwise connectors

											// DIAGONAL
											for( mm=0 ; mm<totlen ; mm++) {
												rr = INDEX2[Ttot[mm][0]][Ttot[mm][1]][Ttot[mm][2]];
												
												for( bb=0 ; bb<m ; bb++) {
													tt = temp_list[bb];
													NETROI[rr][hh][tt][tt]+=1;
													if(NETROI[rr][hh][tt][tt]==NmNsThr) 
														ss=ScoreTrackGrid(Param_grid,
																				INDEX[Ttot[mm][0]][Ttot[mm][1]][Ttot[mm][2]],
																				hh, tt,tt, 
																				insetFA,insetMD,insetL1);
												}
											}

											// CONNECTORS: walk through mult times

											// just do unique connectors (we know that m>=2 here...)
											for( bb=0 ; bb<m ; bb++)
												for( cc=bb+1 ; cc<m ; cc++) {
													// 2 switches for finding ROI, and 1 for current 'FIND'
													onoff[0]=0; onoff[1]=0; onoff[2]=0;
													BreakAddCont=0;

													// test pre-beginning
													// +1 on the temp_list[] entry because the value
													// of temp_list is an index, one less than the Label...
													if( MAPROI[test_ind[0]][hh]==temp_list[bb]+1 )
														onoff[0]=1;
													if( MAPROI[test_ind[0]][hh]==temp_list[cc]+1 )
														onoff[1]=1;

													// now walk through each vox, keep testing and
													// evaluating at each step
													for( mm=0 ; mm<totlen ; mm++) {
														rr = INDEX2[Ttot[mm][0]][Ttot[mm][1]][Ttot[mm][2]];
														if( MAPROI[rr][hh]==temp_list[bb]+1 ) { // hit 1
															onoff[0]=1;
															onoff[2]=1;
															//printf("A");
														}
														else if( MAPROI[rr][hh]==temp_list[cc]+1 ){ // hit 2
															onoff[1]=1;
															onoff[2]=1;
															//printf("B");
														}
														else {// a miss, could be either in b/t or outside
															onoff[2]=0;
															//printf("C");
														}
														switch(onoff[0]+onoff[1]) {
														case 2:
															if(onoff[2])
																BreakAddCont=1; // still in last ROI
															else{
																BreakAddCont=-1; // done
															}
															break;
														case 1:
															BreakAddCont=1; // in 1st or in b/t
															break;
														default:
															BreakAddCont=0; // just keep walking
															break;
														}
														
														//printf("(%d,%d)",mm,BreakAddCont);
														if(BreakAddCont==1) {// are in b/t
															// get both sides of param_grid, b/c just testing one,
															// and param_grid is symm
															NETROI[rr][hh][temp_list[bb]][temp_list[cc]]+=1;
															NETROI[rr][hh][temp_list[cc]][temp_list[bb]]+=1;
															//printf("+");
															if(NETROI[rr][hh][temp_list[bb]][temp_list[cc]]==NmNsThr) {
																ss=ScoreTrackGrid(Param_grid,
																						INDEX[Ttot[mm][0]][Ttot[mm][1]][Ttot[mm][2]],
																						hh, temp_list[cc],temp_list[bb], 
																						insetFA,insetMD,insetL1);
																ss=ScoreTrackGrid(Param_grid,
																						INDEX[Ttot[mm][0]][Ttot[mm][1]][Ttot[mm][2]],
																						hh, temp_list[bb],temp_list[cc], 
																						insetFA,insetMD,insetL1);
															}
														}
														else if(BreakAddCont==-1) {// done
															//printf("|");
															break;
														}
														else {// unnec cond...
															//printf(".");
															continue;
														}
														
													}
												}
										}
									}// end of 'if m>0'
									
									//will just have be symm
									// this will fill in UHT part of matrix 
									// store as values in range 1...NROI
									for( mm=0 ; mm<m ; mm++)
										for( nn=0 ; nn<m ; nn++) { 
											Prob_grid[hh][ temp_list[mm] ][ temp_list[nn] ]+= 1;
										}
								}
							}
						}
	} // end of Monte Carlo loop
	
	
	// **************************************************************
	// **************************************************************
	//                    Some outputs
	// **************************************************************
	// **************************************************************
  
  
	if(Numtract > 0 ) {

		for( k=0 ; k<N_nets ; k++) 
			for( i=0 ; i<NROI[k] ; i++ ) 
				for( j=0 ; j<NROI[k] ; j++ ) 
					if(Param_grid[k][i][j][8]>0.5) {
						for( m=0 ; m<4 ; m++) {
							// means
							Param_grid[k][i][j][2*m]/= Param_grid[k][i][j][8];
							// stdevs
							if(Param_grid[k][i][j][8]>1.5) {
								Param_grid[k][i][j][2*m+1]-= 
									Param_grid[k][i][j][8]*pow(Param_grid[k][i][j][2*m],2);
								Param_grid[k][i][j][2*m+1]/= Param_grid[k][i][j][8]-1;
								Param_grid[k][i][j][2*m+1] = sqrt(Param_grid[k][i][j][2*m+1]);
							}
							else
								Param_grid[k][i][j][2*m+1]=0.0;
						}
					}

    
		for( k=0 ; k<N_nets ; k++) { // each netw gets own file

			// apply threshold with all output stats.
			for( i=0 ; i<NROI[k] ; i++ ) 
				for( j=0 ; j<NROI[k] ; j++ ) 
					if(Prob_grid[k][i][j]<NmNsThr)
						Prob_grid[k][i][j]=0;

			// print out prob grid
			sprintf(OUT_grid,"%s_%03d.grid",prefix,k+1);
			if( (fout1 = fopen(OUT_grid, "w")) == NULL) {
				fprintf(stderr, "Error opening file %s.",OUT_grid);
				exit(19);
			}
    
			fprintf(fout1,"%d\n\n",NROI[k]);
			for( i=0 ; i<NROI[k] ; i++ ) {
				for( j=0 ; j<NROI[k]-1 ; j++ ) // b/c we put '\n' after last one.
					fprintf(fout1,"%d\t",Prob_grid[k][i][j]);
				fprintf(fout1,"%d\n",Prob_grid[k][i][j]);
			}
			fprintf(fout1,"\n");    
      
			for( i=0 ; i<NROI[k] ; i++ ) {
				for( j=0 ; j<NROI[k]-1 ; j++ ) 
					fprintf(fout1,"%e\t",Prob_grid[k][i][j]*1.0/Numtract);
				fprintf(fout1,"%e\n",Prob_grid[k][i][j]*1.0/Numtract);
			}
			fprintf(fout1,"\n");    

			for( m=0 ; m<9 ; m++) {
				for( i=0 ; i<NROI[k] ; i++ ) {
					for( j=0 ; j<NROI[k]-1 ; j++ ) 
						fprintf(fout1,"%e\t",Param_grid[k][i][j][m]);
					fprintf(fout1,"%e\n",Param_grid[k][i][j][m]);
				}
				fprintf(fout1,"\n");    
			}

			fclose(fout1);
		}

		// output AFNI files mapping WM; do thresholding here
		for( hh=0 ; hh<N_nets ; hh++) {
      
			sprintf(prefix_netmap[hh],"%s_%03d_PAIRMAP",prefix,hh+1); 
			// just get one of right dimensions!
			networkMAPS = EDIT_empty_copy( insetFA ) ; 
			EDIT_dset_items(networkMAPS,
								 ADN_datum_all , MRI_short , 
								 ADN_none ) ;
			EDIT_add_bricklist(networkMAPS ,
									 NROI[hh], NULL , NULL , NULL );
			
			sprintf(prefix_netmap2[hh],"%s_%03d_INDIMAP",prefix,hh+1); 
			// just get one of right dimensions!
			networkMAPS2 = EDIT_empty_copy( insetFA ) ; 
			EDIT_dset_items(networkMAPS2,
								 ADN_datum_all , MRI_float , 
								 ADN_none ) ;
			EDIT_add_bricklist(networkMAPS2 ,
									 NROI[hh], NULL , NULL , NULL );


			// first array for all tracks, 2nd for paired ones.
			// still just need one set of matrices output
			short int **temp_arr=NULL;
			float **temp_arr2=NULL;
			temp_arr = calloc( (NROI[hh]+1),sizeof(temp_arr));  // XYZ components
			for(i=0 ; i<(NROI[hh]+1) ; i++) 
				temp_arr[i] = calloc( Nvox,sizeof(short int)); 
			temp_arr2 = calloc( (NROI[hh]+1),sizeof(temp_arr2));  // XYZ components
			for(i=0 ; i<(NROI[hh]+1) ; i++) 
				temp_arr2[i] = calloc( Nvox,sizeof(float)); 

			if( (temp_arr == NULL) || (temp_arr2 == NULL)) {
				fprintf(stderr, "\n\n MemAlloc failure.\n\n");
				exit(124);
			}
      
			// threshold the `overall' (`0th') map; individual ones already have
			// been above, just by how they were created
			for( i=0 ; i<=Ndata ; i++ ) 
				for( j=0 ; j<NROI[hh] ; j++ ) {
					// apply thresholding by count
					for( k=0 ; k<NROI[hh] ; k++ ) 
						if( NETROI[i][hh][j][k] < NmNsThr)
							NETROI[i][hh][j][k]=0;
				}

			//for( bb=0 ; bb<=NROI[hh] ; bb++) // zero array
			//	for( k=0 ; k<Nvox ; k++ ) 
			//		temp_arr[bb][k]=0;

			for( bb=1 ; bb<=NROI[hh] ; bb++) {
				idx=0;
				for( k=0 ; k<Dim[2] ; k++ ) 
					for( j=0 ; j<Dim[1] ; j++ ) 
						for( i=0 ; i<Dim[0] ; i++ ) {
							temp_arr[bb][idx] = 0;
							// allow for more than one `connector' tract
							if(mskd[i][j][k]) 
								//if( THD_get_voxel(insetL1,idx,0)>EPS_V) 
								for( rr=0 ; rr<NROI[hh] ; rr++) 
									if(NETROI[INDEX2[i][j][k]][hh][bb-1][rr]>0) {
										// store connectors
										if(bb-1 != rr){
											//printf("\ncheck:%d = %d ?",bb-1,rr);
												temp_arr[0][idx] = 1; 
										}

										// store tracks through any ROI
										temp_arr2[0][idx] = (float)
											NETROI[INDEX2[i][j][k]][hh][bb-1][rr]; 
										//printf("\ncheck2:%d = %d ?",bb-1,rr);
										
										// zeroth is mask of all through ROI
										// older version, just tracks between pairs:
										//if(bb-1 != rr) // exclude non-paired tracks
										//temp_arr[bb][idx]+=rr+1; 
										// then add value if overlap
										if(bb-1 != rr)
											temp_arr[bb][idx]+=pow(2,rr+1);// unique decomp.
										
										temp_arr2[bb][idx] = (float)
											NETROI[INDEX2[i][j][k]][hh][bb-1][rr];
									}
							idx+=1;
						}
   
				EDIT_substitute_brick(networkMAPS, bb, MRI_short, temp_arr[bb]);
				temp_arr[bb]=NULL; // to not get into trouble...

				EDIT_substitute_brick(networkMAPS2, bb, MRI_float, temp_arr2[bb]);
				temp_arr2[bb]=NULL; // to not get into trouble...
			} 

			// FIRST THE PAIR CONNECTORS
			EDIT_substitute_brick(networkMAPS, 0, MRI_short, temp_arr[0]);
			temp_arr[0]=NULL;
			if(TV_switch[0] || TV_switch[1] || TV_switch[2]) {
				dsetn = r_new_resam_dset(networkMAPS, NULL, 0.0, 0.0, 0.0,
												 voxel_order, RESAM_NN_TYPE, 
												 NULL, 1, 0);
				DSET_delete(networkMAPS); 
				networkMAPS=dsetn;
				dsetn=NULL;
			}      
			EDIT_dset_items(networkMAPS,
								 ADN_prefix    , prefix_netmap[hh] ,
								 ADN_brick_label_one , "ALLROI",
								 ADN_none ) ;
			THD_load_statistics(networkMAPS);
			if( !THD_ok_overwrite() && THD_is_ondisk(DSET_HEADNAME(networkMAPS)) )
				ERROR_exit("Can't overwrite existing dataset '%s'",
							  DSET_HEADNAME(networkMAPS));
			tross_Make_History("3dProbTrackID", argc, argv, networkMAPS);
			THD_write_3dim_dataset(NULL, NULL, networkMAPS, True);
			DSET_delete(networkMAPS); 
			for( i=0 ; i<NROI[hh]+1 ; i++) // free all
				free(temp_arr[i]);
			free(temp_arr);

			// THEN THE INDIVID TRACKS
			EDIT_substitute_brick(networkMAPS2, 0, MRI_float, temp_arr2[0]);
			temp_arr2[0]=NULL;
			if(TV_switch[0] || TV_switch[1] || TV_switch[2]) {
				dsetn = r_new_resam_dset(networkMAPS2, NULL, 0.0, 0.0, 0.0,
												 voxel_order, RESAM_NN_TYPE, 
												 NULL, 1, 0);
				DSET_delete(networkMAPS2); 
				networkMAPS2=dsetn;
				dsetn=NULL;
			}      
			EDIT_dset_items(networkMAPS2,
								 ADN_prefix    , prefix_netmap2[hh] ,
								 ADN_brick_label_one , "ALLROI",
								 ADN_none ) ;
			THD_load_statistics(networkMAPS2);
			if( !THD_ok_overwrite() && THD_is_ondisk(DSET_HEADNAME(networkMAPS2)) )
				ERROR_exit("Can't overwrite existing dataset '%s'",
							  DSET_HEADNAME(networkMAPS2));
			tross_Make_History("3dProbTrackID", argc, argv, networkMAPS2);
			THD_write_3dim_dataset(NULL, NULL, networkMAPS2, True);
			DSET_delete(networkMAPS2); 
			for( i=0 ; i<NROI[hh]+1 ; i++) // free all
				free(temp_arr2[i]);
			free(temp_arr2);
      
		}
		INFO_message("Writing output. NB: it will be %s.",voxel_order);

		INFO_message("Brainwide total number of tracts found = %d",Numtract);
	}
	else{
		INFO_message(" No Tracts Found!!!");
    
		sprintf(OUT_map,"%s.pmap",prefix);
		if( (fout1 = fopen(OUT_map, "w")) == NULL) {
			fprintf(stderr, "Error opening file %s.",OUT_map);
			exit(19);
		}
    
		fprintf(fout1,"0!\n");    
		fclose(fout1);

		sprintf(OUT_grid,"%s.grid",prefix);
		if( (fout1 = fopen(OUT_grid, "w")) == NULL) {
			fprintf(stderr, "Error opening file %s.",OUT_grid);
			exit(19);
		}

		fprintf(fout1,"0!\n");
		fclose(fout1);
	}

 

	// ************************************************************
	// ************************************************************
	//                    Freeing
	// ************************************************************
	// ************************************************************
  
  
	DSET_delete(insetFA);
	DSET_delete(insetMD);
	DSET_delete(insetL1);
	DSET_delete(insetV1);
	DSET_delete(insetV2);
	DSET_delete(insetV3);
	DSET_delete(insetUC);
	DSET_delete(mset1);
  	DSET_delete(insetEXTRA);

	free(prefix);
	free(insetV1);
	free(insetV2);
	free(insetV3);
	free(insetL1);
	free(insetFA);
	free(insetUC);
  	free(insetMD);
	free(mset1);
	free(networkMAPS);
  	free(insetEXTRA);

	for( i=0 ; i<2*ArrMax ; i++) 
		free(Ttot[i]);
	free(Ttot);

	for( i=0 ; i<ArrMax ; i++) {
		free(Tforw[i]);
		free(Tback[i]);
		free(flTforw[i]);
		free(flTback[i]);
	}
	free(Tforw);
	free(Tback);
	free(flTforw);
	free(flTback);

	for( i=0 ; i<Nseed ; i++) 
		free(LocSeed[i]);
	free(LocSeed);
  
	for( k=0 ; k<=Ndata ; k++) 
		for( m=0 ; m<N_nets ; m++) 
			for( i=0 ; i<NROI[m] ; i++) 
				free(NETROI[k][m][i]);
	for( k=0 ; k<=Ndata ; k++) 
		for( m=0 ; m<N_nets ; m++) {
			free(NETROI[k][m]);
		}
	for( k=0 ; k<=Ndata ; k++) {
		free(coorded[k]);
		free(copy_coorded[k]);
		free(NETROI[k]);
		free(MAPROI[k]);
	}
	free(coorded);
	free(copy_coorded);
	free(NETROI);
	free(MAPROI);

	for( i=0 ; i<Dim[0] ; i++) 
		for( j=0 ; j<Dim[1] ; j++) {
			free(INDEX[i][j]);
			free(mskd[i][j]);
		}
	for( i=0 ; i<Dim[0] ; i++) {
		free(INDEX[i]);
		free(mskd[i]);
	}
	free(INDEX);
	free(mskd);

	for( i=0 ; i<Dim[0] ; i++) 
		for( j=0 ; j<Dim[1] ; j++) 
			free(INDEX2[i][j]);
	for( i=0 ; i<Dim[0] ; i++) 
		free(INDEX2[i]);
	free(INDEX2);
  
	for( i=0 ; i<N_nets ; i++) 
		for( j=0 ; j<NROI[i] ; j++) 
			for( k=0 ; k<NROI[i] ; k++) 
            free(Param_grid[i][j][k]);
	for( i=0 ; i<N_nets ; i++) 
		for( j=0 ; j<NROI[i] ; j++) { 
			free(Prob_grid[i][j]);
			free(Param_grid[i][j]);
		}
	for( i=0 ; i<N_nets ; i++) {
		free(Prob_grid[i]);
		free(Param_grid[i]);
		free(prefix_netmap[i]); 
		free(prefix_netmap2[i]); 
		free(prefix_mapfile[i]);
	}
	free(Prob_grid);
	free(Param_grid);
	free(prefix_netmap);
	free(prefix_netmap2);
	free(prefix_mapfile);

	free(temp_list);
	free(list_rois);
  
	for( i=0 ; i<N_nets ; i++) {
		free(ROI_LABELS[i]);
		free(INV_LABELS[i]);
	}
	free(ROI_LABELS);
	free(INV_LABELS);

	free(NROI);
	free(INVROI);
	gsl_rng_free(r); // free also

	return 0;
}

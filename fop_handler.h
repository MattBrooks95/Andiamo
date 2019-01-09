//! \file fop_handler.h describes a class that uses FOP to get TCs
#pragma once

#include <string>
#include <vector>
#include <fstream>

//allows use of std::pair, for storing FOP in -> FOP out file names
#include <utility>

#include "system_wrapper.h"
#include "deck.h"
#include "sdl_help.h"
#include "manager.h"
#include "input_maker.h"
#include "text_box.h"

//! define the path to the read-only optical model potentials folder
#define OMP_PATH "/Andiamo/FOP/OpticalModels/"

/*! \brief define the path to the folder where already calculated
 *transmission coefficients are stored */
#define TRANSMISSION_PATH "/Andiamo/FOP/TransmissionCoefficients/"

/*! \brief define the path to where files are stored while they are being
 *'worked on' by Andiamo and FOP so these may be FOP outputs
 *that haven't been mapped to HF inputs yet  */
#define SCRATCH_PATH "/Andiamo/FOP/FopScratch/"

using std::string;
using std::vector;

extern sdl_help* sdl_access;
extern manager* tile_access;
extern button_manager* button_access;
extern input_maker* io_access;

#define NUM_INPUT_CHANNELS 6

/*! \brief uses a data directory and Andiamo inputs
 *to use FOP to calculate the transmission coefficients */
class fop_handler{
  public:

	//! constructor for the fop_handler class
	fop_handler();

	//fop_handler(const fop_handler& other);

	//! destructor for the fop_handler class
	~fop_handler();

	//! do the work to get the transmission coefficients
	/*! the data directory, user input, and a Q-value calculating
	 *tool are used to do this. This function mostly just calls
	 *other functions */
	void fop_main();


    //! populates the fop_inputs vector with the appropriate file name pairs
	void make_FOP_pair(const int& A_proj, const int& Z_proj,
                       const int& A_targ, const int& Z_targ);


	//! figure out how many times to run FOP
	/*! Big thanks to Tom & Zach
	 *for helping me with this.
     *fills the parameters it is passed*/
	void calc_open_channels(int& A_proj, int& Z_proj,
							int& A_targ, int& Z_targ,
                             double& ecm_value);

	//! helper for calc_open_channels grabs ELAB parameter from Andiamo
	/*! this variable may need converted to center of mass energy */
	double find_elab();

	//! helper for calc_open_channels, gets ICM flag
	/*! this tells us if value ELAB in Andiamo input is in lab energy
	 * or in center of mass energy */
	int find_icm();

	//! helper for calc_open_channels
	/*! goes from beam energy to center of mass energy */
	void lab_to_ecm(double& ecm_value, const unsigned int& A_targ,
					const unsigned int& A_proj);

	//##### HELPERS FOR LAB_TO_ECM ############################################
	//! grabs compound A and Z values from Andiamo inputs
	void find_compound_A_Z(unsigned int& compound_A,unsigned int& compound_Z);


	//! grabs IENCH value from Andiamo inputs
	int find_IENCH();


	void calc_Ap_Zp_At_Zt(int IENCH, const int compound_A, const int compound_Z,
							int& A_proj, int& Z_proj, int& A_targ, int& Z_targ);
	//#########################################################################

	//! sets up the decks that will be FOP inputs
	/* only does so for each of the possible channels
	 *as determined by calc_open_channels() */
	void prepare_deck(int A_proj,int Z_proj,int A_targ,int Z_targ,
                      double ecm_value);

	//! run FOP once the cards are in place
	/*! ensure that calc_open_channels(), prepare_decks(), make_FOP_pair and
	 *run_fop() are called in that order */
	void run_fop();


	//! this is mostly for testing
	//void print_file_list();

    //! tells whether or not the TC are ready for FOP input file creation
    bool ready;

  private:

	//! keeps track of which input channels are open for this calculation
	/*! filled in by calc_open_channels, using David Resler's mass excess tool.
	 *Values set to TRUE mean "this matters, run fop for this channel"
	 *values set to FALSE mean "not physically possible, so
	 *don't run FOP for this channel.
	 *[0] = neutron
	 *[1] = alpha
	 *[2] = proton
	 *[3] = deuteron
	 *[4] = triton
	 *[5] = 3He */
	bool open_channels[NUM_INPUT_CHANNELS];

    //! stores the most FOP file names for each open channel
    std::pair<string,string> fop_file_names;

	//! store the decks of cards to run FOP with
	vector<deck*> fop_decks;

    //################ FOP PATH INFO ################################
    //! path to the optical model potentials folder
	string omp_path;

    //! path to the transmission coefficients folder
	string trans_path;

    //! path to the temporary storage space
    /*! This is where Andiamo's FOP handler stores the FOP input files
     *that it will run FOP on. These should be deleted after use, probably.*/
	string scratch_path;
    //###############################################################

};

//! this function calls the cross-compiled tool for finding Q values
/*! the tool will be in fortran, so the function definition must
 *be done in this way to allow it to be called directly from
 * a c function */
extern "C" {

	void cqvalue_(int* Z_proj, int* A_proj, int* Z_targ, int* A_targ,
				  double* qn, double* qp, double* qd, double* qt,
				  double* q3He, double* qalpha,int* tflag);

}











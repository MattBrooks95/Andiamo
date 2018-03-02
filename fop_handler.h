//! \file fop_handler.h describes a class that uses FOP to get TCs
#pragma once

#include <string>
#include <vector>
#include <fstream>

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
extern input_maker* io_access;
extern string HOME;

/*! \brief uses a data directory and Andiamo inputs
 *to use FOP to calculate the transmission coefficients */
class fop_handler{
  public:

	//! constructor for the fop_handler class
	fop_handler();

	//fop_handler(const fop_handler& other);

	//! destructor for the fop_handler class
	~fop_handler();

	//! populate vectors of file names
	/*!  from subdirectories OMP_PATH, TRANSMISSION_PATH, and SCRATCH_PATH */ 
	void get_files_list();


	//! do the work to get the transmission coefficients
	/*! the data directory, user input, and a Q-value calculating
	 *tool are used to do this. This function mostly just calls
	 *other functions */
	void fop_main();




	string make_FOP_output_name(const int& A_proj, const int& Z_proj,
							   const int& A_targ, const int& Z_targ);


	//! figure out how many times to run FOP
	/*! Big thanks to Tom & Zach
	 *for helping me with this */
	void calc_open_channels(int& A_proj, int& Z_proj,
							int& A_targ, int& Z_targ);

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
	void prepare_deck();

	//! run FOP once the cards are in place
	/*! ensure that calc_open_channels(), prepare_decks(), and
	 *run_fop() are called in that order
	 * \param outs is the output stream to the FOP output file*/
	void run_fop(std::ofstream& outs);


	//! this is mostly for testing
	void print_file_list();

  private:

	//! list of all file names in OMP_PATH subdirectory
	vector<string> optical_model_files;

	//! list of all files in the transmission coefficients files
	/*! these are files previously made by FOP */
	vector<string> tc_files;

	/*! list of all files that aren't yet ready for use by HF,
	 * but fop_handler may need */
	vector<string> scratch_files;

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
	bool open_channels[6];

	//! store the decks of cards to run FOP with
	vector<deck> fop_decks;

	//! store the name of the most recent FOP output file 
	string most_recent_FOP_out;

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











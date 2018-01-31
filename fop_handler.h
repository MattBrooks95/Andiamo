//! \file fop_handler.h is a class that manages the Andiamo->FOP->HF2002 workflow
#pragma once

#include <string>
#include <vector>
#include <fstream>

#include "deck.h"
#include "sdl_help.h"
#include "input_maker.h"


//define the path to the read-only optical model potentials folder
#define OMP_PATH "./FOP/OpticalModels/"
//define the path to the folder where already calculated transmission coefficients are stored
#define TRANSMISSION_PATH "./FOP/TransmissionCoefficients/"
/* define the path to where files are stored while they are being
  'worked on' by Andiamo and FOP so these may be FOP outputs
  that haven't been mapped to HF inputs yet  */
#define SCRATCH_PATH "./FOP/FopScratch/"

using std::string;
using std::vector;

extern sdl_help* sdl_access;
extern input_maker* io_access;

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

	void calc_open_channels();

	//! sets up the decks that will be FOP inputs
	/* only does so for each of the possible channels
	 *as determined by calc_open_channels() */
	void prepare_decks();

	//! run FOP once the cards are in place
	/*! ensure that calc_open_channels(), prepare_decks(), and
	 *run_fop() are called in that order */
	void run_fop();

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
	bool open_channels[7];
};

//! this function calls the cross-compiled tool for finding Q values
/*! the tool will be in fortran, so the function definition must
 *be done in this way to allow it to be called directly from
 * a c function */
extern "C" {

	void dqv(int* A,int* Z, double return_values[6]);

}











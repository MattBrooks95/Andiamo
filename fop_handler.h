//! \file fop_handler.h is a class that manages the Andiamo->FOP->HF2002 workflow
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <regex>


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

	void run_fop();//!< run FOP on a file

	//! this is mostly for testing
	void print_file_lists();

  private:

	//! list of all file names in OMP_PATH subdirectory
	vector<string> optical_model_files;

	//! list of all files in the transmission coefficients files
	/*! these are files previously made by FOP */
	vector<string> tc_files;

	/*! list of all files that aren't yet ready for use by HF,
	 * but fop_handler may need */
	vector<string> scratch_files;
};

//! data structure to store lines (cards) for the FOP input
class card{

  public:

	//! constructor saves a string, and the number of params the card needs
	card(string info_in = "",unsigned int num_params_in = 7);

	//! makes sure that the line is formatted properly
	/*! the default version of this function checks for
	 *num_params double values in the list. If they aren't all there
	 *something is wrong */ 
	virtual bool check();


  private:

	//! comma-separated list of expected parameters
	string info;

	//! stores the number of parameters the line expects
	unsigned int num_params;

};











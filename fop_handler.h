//! \file fop_handler.h is a class that manages the Andiamo->FOP->HF2002 workflow

#include <string>
#include <vector>
#include <fstream>

//define the path to the read-only optical model potentials folder
#define OMP_PATH "./FOP/OpticalModels/"
//define the path to the folder where already calculated transmission coefficients are stored
#define TRANSMISSION_PATH "./FOP/TransmissionCoefficients/"
//define the path to where files are stored while they are being 'worked on' by Andiamo and FOP
//so these may be FOP outputs that haven't been mapped to HF inputs yet
#define SCRATCH_PATH "./FOP/FopScratch/"

using std::string;
using std::vector;

class sdl_help;
class input_maker;

class fop_handler{
  public:
	fop_handler(sdl_help* sdl_helper_in, input_maker* io_manager_in);//!< constructor for the fop_handler class
	~fop_handler();//!< destructor for the fop_handler class

	//! populate vectors of file names
	/*!  from subdirectories OMP_PATH, TRANSMISSION_PATH, and SCRATCH_PATH */ 
	void get_files_list();

	void run_fop();//!< run FOP on a file

	//! this is mostly for testing
	void print_file_lists();

  private:
	sdl_help* sdl_helper; //!< handy access to the sdl_help class for graphics
	input_maker* io_manager;//!< handy access to the input maker, so these classes can communicate easily

	//! list of all file names in OMP_PATH subdirectory
	vector<string> optical_model_files;
	//! list of all files in the transmission coefficients files
	/*! these are files previously made by FOP */
	vector<string> transmission_coefficients_files;
	//! list of all files that aren't yet ready for use by HF, but fop_handler may need
	vector<string> scratch_files;
};










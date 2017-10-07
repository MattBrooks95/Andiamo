//! \file fop_handler.h is a class that manages the Andiamo->FOP->HF2002 workflow

#include <string>
#include <vector>
#include <fstream>

class sdl_help;
class input_maker;

class fop_handler{
  public:
	fop_handler(sdl_help* sdl_helper_in, input_maker* io_manager_in);//!< constructor for the fop_handler class
	~fop_handler();//!< destructor for the fop_handler class

	void browse();//!< file browser to find files
	void run_fop();//!< run FOP on a file





  private:
	std::string fop_folder_path;//!< path to the folder for managing fop files
	std::string tc_input_file_path;//!< path to the folder of ready transmission coefficients
	sdl_help* sdl_helper; //!< handy access to the sdl_help class for graphics
	input_maker* io_manager;//!< handy access to the input maker, so these classes can communicate easily
};

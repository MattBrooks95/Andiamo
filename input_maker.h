//! \file input_maker.h \brief is the class that handles file manipulation
/*! it handles creating the output file to be used by HF, and also reads in the config file "HF_config.txt"
 * and then makes decisions based on that config file */

#include<vector>

#include "ftran_structs.h"


//! input_maker is a class that reads a config file to configure itself, and then creates the outputs to be used with HF
class input_maker{
  public:
	//! this constructor sets the config-file related strings
	input_maker(std::string file_name_in);

	//! init() sets up the parameter vectors to the specifications of the config file
	/*! it's likely going to use regular expressions */
	void init();






  private:

	std::string config_p; //!< \brief contains a string that provides the relative path to the config files
	std::string file_name; //!< \brief set by the constructor to tell it which config file to use

	//! \param contains a variable number of in4 fortran-style variables
	/* it's length and contents should be specified in the config file */
	std::vector<param_int4> int4_params; 

	//! \param contains a variable number of real8 fortran-style variables
	/* it's length and contents should be specified in the config file */
	std::vector<param_real8> real8_params;

	//! \param contains a variable number of c++ style strings, which should be fortran friendly with .c_str()
	/* I think it'll play nicely with fortran just with .c_str() and .size() */
	std::vector<param_string> string_params;
};

//! \file input_maker.h \brief is the class that handles file manipulation
/*! it handles creating the output file to be used by HF, and also reads in the config file "HF_config.txt"
 * and then makes decisions based on that config file */
#pragma once

#include<vector>
#include<iostream>
#include<fstream>
#include<regex>
#include<cstdlib>

#include<map> //map is an associative array, like could be found in python or perl
	      //items are accessed by name (key)


#include "ftran_structs.h"
#include "string+.h"

#include "logger.h"
extern logger error_logger;
class button_manager;

//! input_maker is a class that reads a config file to configure itself, and then creates the outputs to be used with HF
/*! it exists within sdl_help, and interacts heavily with fields and the manager */

class input_maker{
  public:
	//! this constructor sets the config-file related strings
	input_maker(std::string output_file_name_in = "output.txt",std::string config_file_name_in = "HF_config.txt");

	//! the destructor calls the output() member, to make sure only the most recent data is output
	/*! I have it this way for now because I can imagine myself or someone else forgetting to call it
	 * from main before the whole program terminates */
	//~input_maker();

	//! init() sets up the parameter vectors to the specifications of the config file
	/*! It uses regular expressions, and the split functions in string+.h*/
	void init();

	//! this function takes a pointer to button_manager
	/*! this is necessary so that the input_maker can tell the form_buttons when to make their output lines */
	void set_bmanager_ptr(button_manager* b_manager_in){ b_manager = b_manager_in;}

	//! this function allows the input maker to access the button manager
	/*! reason being is that the form buttons are necessary to creating some of the later lines in the
	 *input manual */
	void get_bmanager_ptr(button_manager* b_manager_in) { b_manager = b_manager_in;}


	//! output() prints out the information in the order and format needed to be used by HF
	/*! for now it defaults to printing to ./output/output.txt, the output file can be specified
	 *by the output_fname button in class button_manager*/
	void output();
	
	//! check_map() traverses all of the maps in this object, and prints them to the given stream
	/*\param outs is the output stream to send the information to */
	void check_map();

	//################ GETTERS AND SETTERS #########################################

	//! return names_in_order BY REFERENCE
	//std::vector<std::string>& get_names_in_order(){return names_in_order;}

	//! return int4_params BY REFERENCE
	std::map<std::string,param_int4>& get_int4_params(){ return int4_params;}

	//! return real8_params BY REFERENCE
	std::map<std::string,param_real8>& get_real8_params(){ return real8_params;}

	//! return string_params BY REFERENCE
	std::map<std::string,param_string>& get_string_params(){ return string_params;}

	//! return int4_array_params BY REFERENCE
	std::map<std::string,param_int4_array>& get_i4_array_params(){ return int4_array_params;}

	//! return r8_params map BY REFERENCE
	std::map<std::string,param_r8_array>& get_r8_array_params(){ return r8_array_params;}
	//##############################################################################

	std::string output_file_name;//!< \brief name of the file in config_p's folder where output will be printed

	std::string TC_input_file_name;//!< \brief name of the file from which the transmission coefficients should be read

	//! starts off false, then goes positive the user generates an input file
	/*! this way, if an output file has not yet been made, we can make the user aware of that
	 *when they try to exit the program */
	bool output_was_made;


  private:

	button_manager* b_manager;//!< pointer allows access to the button manager, because form_buttons need to make output

	std::string config_p; //!< \brief contains a string that provides the relative path to the config files

	std::string output_p; //!< \brief path to the output folder
	std::string file_name; //!< \brief set by the constructor to tell it which config file to use

 	//! contains the names of parameters in the order in which they were read
	/*! the corresponding tile should be associated to its info by the tile name and parameter
	 *string name being the same */
	//std::vector<std::string> names_in_order;

	//! contains a variable number of in4 fortran-style variables
	/* its length and contents should be specified in the config file */
	//std::vector<param_int4> int4_params; 
	std::map<std::string,param_int4> int4_params;

	//! contains a variable number of real8 fortran-style variables
	/* its length and contents should be specified in the config file */
	//std::vector<param_real8> real8_params;
	std::map<std::string,param_real8> real8_params;

	//! contains a variable number of c++ style strings, which should be fortran friendly with .c_str()
	/* I think it'll play nicely with fortran just with .c_str() and .size() */
	//std::vector<param_string> string_params;
	std::map<std::string,param_string> string_params;	

	//! this map contains the ftran_struct int4 array parameters
	/*! a specific one can be accessed by using map's at(name_string) member function
	 *this being a map allows parameters to be looked up by name */
	std::map<std::string,param_int4_array> int4_array_params;

	//! this map contains the ftran_struct e_params
	std::map<std::string, param_r8_array> r8_array_params;



	std::vector<param_real8> ETAB;//!< store the ETAB numbers read in from the TC file
	std::vector<param_real8> TTAB;//!< store the TTAB numbers read in from the TC file

};

//##################################  NON MEMBER HELPERS  #############################################################
//! this is a helper function for input_maker::output(), it prints a string to the output file in the right format
/*! \param outs is the fstream to print to (such as output/output.txt)
 *\param size is the size read in by input_maker from the configuration file
 *\param output_me is the ftran_struct string_param to print to the file */
void output_string(std::ofstream& outs,const unsigned int& size,const std::string& string_in); 

//! this function sets up line one of the HF input file
void do_line1(std::ofstream& outs,const std::map<std::string,param_string>& string_params);

//! this function sets up line two of the HF input file
void do_line2(std::ofstream& outs,const std::map<std::string,param_real8>& real8_params,const std::map<std::string,param_int4>& int4_params);

//! this function implements the reading loop over the transmission coefficients
/* NENT, LMAX and NGF control the loops, and TC_input_file button sets this class's TC_input_file_name variable
 *and the coefficients are read in from there */
void do_TC_coefficients(const std::map<std::string,param_real8>& real8_params, const std::map<std::string,param_int4_array>& array_map,
			std::string TC_input_file_name,std::ofstream& outs);

//! this helper outputs the variables found in line 4 of the input description manual
void do_line4(std::ofstream& outs,const std::map<std::string,param_real8>& real8_params, const std::map<std::string,param_int4>& int4_params);

//! this helper outputs the variables in line 4A (if IENCH = 7, logic in output() )
void do_line4A(std::ofstream& outs,const std::map<std::string,param_real8>& real8_params,const std::map<std::string,param_int4>& int4_params);

//! this helper outputs the variables in line 4B (if IENCH = 7,logic in output() )
void do_line4B(std::ofstream& outs,const std::map<std::string, param_r8_array>& e_params);

//! this helper outputs the values for the parameters in line 6
void do_line6(std::ofstream& outs,const std::map<std::string,param_int4>& int4_params);

//! this helper outputs the values for the parameters in line 7
void do_line7(std::ofstream& outs,const std::map<std::string,param_real8>& real8_params);

//! this helper outputs the variables in line 8, if ICNTRL4 != 0 (logic should be in output(), which calls this)
void do_line8(std::ofstream& outs,const std::map<std::string,param_int4>& int4_params);

//! this helper outputs the variables in line 9
void do_line9(std::ofstream& outs,const std::map<std::string,param_int4>& int4_params,const std::map<std::string,param_real8>& real8_params);
//######################################################################################################################














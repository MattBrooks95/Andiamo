//! \brief \file input_maker.h contains the class that handles file manipulation
/*! \details \file input_maker.h handles creating the output file to be
 *used by HF, and also reads in the config file "HF_config.txt" and
 *then makes decisions based on that config file */
#pragma once

#include<vector>
#include<iostream>
#include<fstream>
#include<regex>
#include<cstdlib>

//string stream allows reading from a string like it was an input stream
//makes reading in form_button initializer lists easier
#include<sstream>

//map is an associative array, like could be found in python or perl
//items are accessed by name (key)
#include<map>

#include "ftran_structs.h"
#include "string+.h"
#include "logger.h"
#include "c_tuples.h"
#include "helpers.h"

#include "define.h"

using std::string;
using std::vector;
using std::map;
using std::ostream;
using std::regex;

extern logger error_logger;
class button_manager;

//! input_maker makes the HF input and loads in the default values
/*!reads a config file to configure itself, and then creates the outputs to
 *be used with HF it exists within sdl_help, and interacts heavily
 *with fields and the manager object */

class input_maker{
  public:
    //! this constructor sets the config-file related strings
    input_maker(string output_file_name_in = "output.txt",
                    string config_file_name_in = "HF_config.txt");

    //! sets up the parameter vectors to the specifications of the config file
    /*! It uses regular expressions, and the split functions in string+.h */
    void init(const string& alternate_config);

    //! attempts to parse a real8 declaration and put it in the real 8 map
    void process_real8(const string& line,const regex& nad_flag);

    //! attempts to parse an int4 declaration and put it in the int4 map
    void process_int4(const string& line,const regex& nad_flag);

    //! attempts to parse a string declaration and put it in the string map
    void process_string(const string& line,
                        const regex& string_array_size_pattern,
                        const regex& nad_flag);

    //! attempts to parse a list of form parameter initialization values
    void process_form_init(const string& line);

    //! prints out the information needed to be used by HF
    /*! for now it defaults to printing to ./output/output.txt,
     *the output file can be specified by the output_fname button
     *in class button_manager*/
    bool output(vector<string>& form_bad_inputs);

    //! prints all of the maps in this object to the error logger
    /* needs to be ran with the -v option */
    void check_map();

    //################ GETTERS AND SETTERS #####################################

    //! return int4_params BY REFERENCE
    map<string,param_int4>& get_int4_params(){ return int4_params;}

    //! return real8_params BY REFERENCE
    map<string,param_real8>& get_real8_params(){ return real8_params;}

    //! return string_params BY REFERENCE
    map<string,param_string>& get_string_params(){ return string_params;}

    //! return int4_array_params BY REFERENCE
    map<string,param_int4_array>& get_i4_array_params(){
        return int4_array_params;
    }

    //! return r8_params map BY REFERENCE
    map<string,param_r8_array>& get_r8_array_params(){ return r8_array_params;}
    //##########################################################################

    //! name of the file in config_p's folder where output will be printed
    string output_file_name;

    //! starts off false, then goes positive the user generates an input file
    /*! this way, if an output file has not yet been made, we can make the user
     *aware of that when they try to exit the program */
    bool output_was_made;

    //##########################################################################
    //! give the provided default values to their corresponding fild parameters
    /*! the names in the input_maker config file and the tile_input config
     *file must be the same in order for them to be associated with
     *one another */
    void initialize_fields();

    //! returns a message about not being able to find a parameter
    string get_string_init_error_message(const string& param_name);

    //! give all int4 values to the parameters in the manager's field map
    void give_int4_defaults();

    //! give all int4 array values to the parameters in the manager's field map
    // void give_int4_array_defaults();

    //! give all r8 values to the parameters in the manager's field map
    void give_r8_defaults();

    //! give all r8 array values to the parameters in the manager's field map
    // void give_r8_array_defaults();

    //! give all string values to the parameters in the manager's field map
    void give_string_defaults();
    //##########################################################################

    //! saves a field's user-input values to this data structure
    /*!
     *
     */
    bool grab_values(vector<string>& bad_input_list);

    //##########################################################################
    //! saves all parameters to the passed file
    /*! this will save the main context parameters and
     *form information */
    void save_context(ofstream& outs);

    //! helper for save_context, saves field info to passed file
    void save_fields(ofstream& context_out);

    // helper for work(), saves form info to passed file
    void save_forms(ofstream& context_out);
    //##########################################################################

    //! store a mapping of form_button names to their vector values
    /*! this allows forms to be pre-filled if the config file in use
     *specifies them */
    map<string,vector<string>> form_init_arrays;

    //! store the initialization lists for icntrl6's extra forms
    /*! this storage is here and not in the icntrl6_form_button
     *because the input maker is instantiated first */
    vector<vector<string>> icntrl6_extra_init_arrays;

  private:
    //! contains a string that provides the relative path to the config files
    string config_p;

    //! path to the output folder
    string output_p;

    //! set by the constructor to tell it which config file to use
    string file_name;

    //! contains a variable number of in4 fortran-style variables
    /* its length and contents should be specified in the config file */
    map<string,param_int4> int4_params;

    //! contains a variable number of real8 fortran-style variables
    /* its length and contents should be specified in the config file */
    map<string,param_real8> real8_params;

    //! contains string parameters
    map<string,param_string> string_params;

    //! this map contains the ftran_struct int4 array parameters
    /*! a specific one can be accessed by using map's at(name_string)
     *member function */
    map<string,param_int4_array> int4_array_params;

    //! this map contains the ftran_struct e_params
    map<string, param_r8_array> r8_array_params;

    //! store the ETAB numbers read in from the TC file
    vector<param_real8> ETAB;

    //! store the TTAB numbers read in from the TC file
    vector<param_real8> TTAB;

};

//#########################  NON MEMBER HELPERS  ##############################
//! helper function for input_maker::output()
/*! prints a string to the output file in the right format
 *\param outs is the fstream to print to (such as output/output.txt)
 *\param size is the size read in by input_maker from the configuration file
 *\param output_me is the ftran_struct string_param to print to the file */
void output_string(ofstream& outs,const unsigned int& size,\
                    const string& string_in);

//! sets up line one of the HF input file
void do_line1(ofstream& outs,const map<string,param_string>& string_params);

//! sets up line two of the HF input file
void do_line2(ofstream& outs,const map<string,param_real8>& real8_params,
                const map<string,param_int4>& int4_params);

//! implements the reading loop over the transmission coefficients
/* NENT, LMAX and NGF control the loops, and TC_input_file button sets
 *this class's TC_input_file_name variable and the coefficients
 * are read in from there */
void do_TC_coefficients(const map<string,param_real8>& real8_params,
                        const map<string,param_int4_array>& array_map,
                        ofstream& outs);

//! outputs the variables found in line 4 of the input description manual
void do_line4(ofstream& outs,const map<string,param_real8>& real8_params,
                const map<string,param_int4>& int4_params);


/* we can't handle IENCH = 7, Zach wants these disabled
// helper outputs the variables in line 4A (if IENCH = 7, logic in output() )
void do_line4A(ofstream& outs,const map<string,param_real8>& real8_params,
                const map<string,param_int4>& int4_params);

// helper outputs the variables in line 4B (if IENCH = 7,logic in output() )
void do_line4B(ofstream& outs,const map<string, param_r8_array>& e_params);
*/

//! prints out line 5
void do_line5(ofstream& outs,const map<string, param_int4>& int4_params);

//! prints line 5A, should only be called if ILV1 is == 6
void do_line5A(ofstream& outs, const map<string, param_real8>& real8_params);

//! outputs the values for the parameters in line 6
void do_line6(ofstream& outs,const map<string,param_int4>& int4_params);

//! outputs the values for the parameters in line 7
void do_line7(ofstream& outs,const map<string,param_real8>& real8_params);

//! outputs the variables in line 8, if ICNTRL4 != 0
void do_line8(ofstream& outs,const map<string,param_int4>& int4_params);

//! outputs the variables in line 9
void do_line9(ofstream& outs,const map<string,param_int4>& int4_params,
                const map<string,param_real8>& real8_params);

//! outputs the variables in line 10
void do_line10(ofstream& outs,const map<string,param_int4>& int4_params);

//! outputs the variables related to icntrl6
void do_icntrl6(ofstream& outs, int icntrl6_value,
                vector<string>& form_bad_inputs,
                vector<index_value>& icntrl6_bad_inputs);

//! outputs the variables related to icntrl8
void do_icntrl8(ofstream& outs,
                vector<string>& form_bad_inputs,
                vector<index_value>& icntrl8_bad_inputs);

//! outputs the variables related to icntrl10
void do_icntrl10(ofstream& outs,
                vector<string>& form_bad_inputs,
                vector<index_value>& icntrl10_bad_inputs);
//############################################################################
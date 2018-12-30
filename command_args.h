//! \file command_args.h
/*! \brief helper functions for processing command line arguments
 * to Andiamo */

#include<regex>
#include<fstream>
#include<iostream>

#include "system_wrapper.h"

using std::cout;
using std::endl;

extern system_wrapper* system_access;

//! prints the list of arguments that can be passed to Andiamo to the terminal
void options_help(){
	//cout << "Explaining options here" << endl;
	cout << "The following command lines may be of use to you\n"
		 << "when using Andiamo!" << endl;
	cout << "1) -v\n  -v turns on \"verbose mode\", which prints out\n"
		 << "  a bunch of additional debugging and non-critical\n"
		 << "  run time messages to the error_logs/ file for this session."
		 << "\n" << endl;
	cout << "2) -configf some_file_name.txt\n  changes the default"
		 << " values file used by Andiamo! to the provided file name.\n"
		 << "  This file must be in ~/Andiamo/config/custom_configs/, and must have"
		 << " file extension \".txt\".\n"
		 << "  The parameters in this file must match those in the current\n"
		 << "  graphical configuration file."
		 << "\n" << endl;

	cout << "3) -graphic_configf some_file.txt\n"
		 << "  specifies a new file that controls the appearance,"
		 << "number of tiles,\n  and user-facing strings of Andiamo."
		 << " Note that the provided file must\n  have HF parameter names"
		 << " That line up with those\n  in the currently used"
		 << " ~/Andiamo/config/something.txt file. Failure\n  to ensure so may"
		 << " cause undefined behavior.\n" << endl;

	cout << "4) -noguides\n"
		 << "  turns off the backdrops that highlight each\n"
		 << "      line of parameters, per the user manual" << endl;
}

//! processes command line arguments
/*! \return true if things went well
 *\return false if things did not go well */
bool process_args(int argc, char* argv[],string& input_maker_config_file,
				  string& manager_config_file,bool& line_guides){

	//start off assuming things will go well.
	//if these arguments are malformed, this will
	//be changed to false, and stop execution
	bool return_value = true;

	string home_path = system_access->get_home();

	if(argc > 1){

		bool ignore_next_arg = false;

		for(int c = 1; c < argc; c++){
			//don't do any processing of this argument, it was
			//a part of an earlier argument - reset the boolean
			//so the next argument is processsed normally
			if(ignore_next_arg){
				ignore_next_arg = false;
				continue;
			}

			string argument = argv[c];

			//-v makes the error logger print debugging and run time messages
			if(argument.compare("-v") == 0){

				cout << "Verbose mode on" << endl;
				output_access->verbose = true;

			} else if(argument.compare("-help") == 0){

				//function that prints all of the options to the screen
				options_help();
				//if they give us the -help option, don't try to open the GUI
				return_value = false;

			} else if(argument.compare("-configf") == 0){

				cout << "Allowing the user to specify default values." << endl;
				if( (c + 1) < argc ){

					regex test_file_name("[_A-Za-z0-9]+\\.txt");       

					argument = argv[c+1];
					cout << "Supplied file name: " << argument << endl; 

					string full_path = home_path + "/Andiamo/config/custom_configs/" + argument;
					std::ifstream test_open;
					cout << "Attempting to open file:" << full_path << endl;
					test_open.open(full_path.c_str());

					if(regex_match(argument,test_file_name) && !test_open.fail()){
						input_maker_config_file = argument;
						ignore_next_arg = true;
					} else {

						cout << "File argument:" << argument;
						cout << " Is seemingly not a proper config file.\n"
							 << " Please ensure it's in [andiamo_root]/config/custom_configs/,\n"
							 << "and that it ends in .txt"
							 << endl;
						ignore_next_arg = true;

					}
					test_open.close();

				} else {

					cout << "Failure to provide a file name for a custom "
						 << "default values file. Proper form is -configf some_file.txt."
						 << " Note that the file must be in ~/Andiamo/config/custom_configs/." << endl;

				}

			} else if(argument.compare("-graphic_configf") == 0){

				cout << "Allowing user to change graphical config." << endl;
				if( (c + 1) < argc ){
	
					regex test_file_name("[_A-Za-z0-9]+\\.txt");       
	
					argument = argv[c+1];
					cout << "Supplied file name: " << argument << endl; 
	
					string full_path = home_path + "/Andiamo/tile_Input/" + argument;
					std::ifstream test_open;
					cout << "Attempting to open file:" << full_path << endl;
					test_open.open(full_path.c_str());
	
					if(regex_match(argument,test_file_name) && !test_open.fail()){
						manager_config_file = argument;
						ignore_next_arg = true;
					} else {
	
						cout << "  File argument:" << argument;
						cout << "\n  Is seemingly not a proper graphics config file.\n"
							 << "  Please ensure that it's in [andiamo_root]/tile_Input/,\n"
							 << "  and that it ends in .txt"
							 << endl;
						ignore_next_arg = true;
					}

					test_open.close();

				}

			} else if(argument.compare("-noguides") == 0){

				line_guides = false;

			} else {

				cout << "Unknown command-line argument: " << argument << endl;

			}
		//cout << "Argument: " << c << " with value: " << argv[c] << endl;
		}

	}

	return return_value;
}

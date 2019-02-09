//! \file command_args.h
/*! \brief helper functions for processing command line arguments
 * to Andiamo */
#include<regex>
#include<fstream>
#include<iostream>
#include<string>

#include "system_wrapper.h"
#include "regex_manager.h"
#include "logger.h"

using namespace std;

extern system_wrapper* system_access;
extern regex_manager* regex_access;
extern logger* logger_access;

//! prints the list of arguments that can be passed to Andiamo to the terminal
void options_help(){
	//cout << "Explaining options here" << endl;
	cout << "The following command lines may be of use to you\n"
		 << "when using Andiamo!" << endl;
	cout << "1) --verbose\n  -v turns on \"verbose mode\", which prints out\n"
		 << "  a bunch of additional debugging and non-critical\n"
		 << "  run time messages to the error_logs/ file for this session."
		 << "\n" << endl;
	cout << "2) --configfolder some_folder_name\n\tsets the directory of "
		 << "initialization files used by Andiamo! The default folder is"
		 << " Andimo/config/configurations/default. These directories need"
		 << " to contain three files.\n"
		 << "\tHF_config.txt specifies the names, line structure and default values"
		 << "of the HF parameters.\n"
		 << "\tparameter_regular_expressions.txt maps the symbolic regular expression names"
		 << " used in HF_config to set up input checking for the parameters.\n"
		 << "\tgraphics.txt specifies which images get used for which parameter"
		 << "in the user interface, their size, and help dialog text\n"
		 << endl;

	cout << "3) --noguides\n"
		 << "  turns off the backdrops that highlight each\n"
		 << "      line of parameters, per the user manual" << endl;

	// cout << "3) -graphic_configf some_file.txt\n"
	// 	 << "  specifies a new file that controls the appearance,"
	// 	 << "number of tiles,\n  and user-facing strings of Andiamo."
	// 	 << " Note that the provided file must\n  have HF parameter names"
	// 	 << " That line up with those\n  in the currently used"
	// 	 << " ~/Andiamo/config/something.txt file. Failure\n  to ensure so may"
	// 	 << " cause undefined behavior.\n" << endl;
}

//! processes command line arguments
/*! \return true if things went well
 *\return false if things did not go well */
bool process_args(int argc, char* argv[],string& manager_config_folder,bool& line_guides){

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
			if(argument.compare("--verbose") == 0){

				cout << "Verbose mode on" << endl;
				logger_access->verbose = true;

			} else if(argument.compare("--help") == 0){

				//function that prints all of the options to the screen
				options_help();
				//if they give us the -help option, don't open the GUI
				return_value = false;

			} else if(argument.compare("--configfolder") == 0){

				cout << "Allowing the user to specify default values." << endl;
				if( (c + 1) < argc ){

					argument = argv[c+1];
					cout << "Supplied file name: " << argument << endl; 

					// string config_path = home_path + "/Andiamo/config/configurations/" + argument;
					// std::ifstream test_open;
					// cout << "Attempting to open file:" << config_path << endl;
					// test_open.open(config_path.c_str());

					// if(!test_open.fail()){
					manager_config_folder = argument;
					// 	ignore_next_arg = true;
					// } else {

					// 	cout << "Folder argument:" << argument;
					// 	cout << " Is seemingly not a proper config file.\n"
					// 		 << " Please ensure it's in [andiamo_root]/config/parameter_config/,\n"
					// 		 << "and that it ends in .txt" << endl;
					ignore_next_arg = true;

					// }
					// test_open.close();

				} else {

					cout << "Failure to provide a file name for a custom "
						 << "parameter configuration folder. Proper form is --configfolder some_file.txt."
						 << " Note that the folder must be in ~/Andiamo/config/parameter_config/." << endl;

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

			// } else if(argument.compare("-graphic_configf") == 0){

			// 	cout << "Allowing user to change graphical config." << endl;
			// 	if( (c + 1) < argc ){
	
			// 		regex test_file_name("[_A-Za-z0-9]+\\.txt");       
	
			// 		argument = argv[c+1];
			// 		cout << "Supplied file name: " << argument << endl; 
	
			// 		string full_path = home_path + "/Andiamo/tile_Input/" + argument;
			// 		std::ifstream test_open;
			// 		cout << "Attempting to open file:" << full_path << endl;
			// 		test_open.open(full_path.c_str());
	
			// 		if(regex_match(argument,test_file_name) && !test_open.fail()){
			// 			manager_config_file = argument;
			// 			ignore_next_arg = true;
			// 		} else {
	
			// 			cout << "  File argument:" << argument;
			// 			cout << "\n  Is seemingly not a proper graphics config file.\n"
			// 				 << "  Please ensure that it's in [andiamo_root]/tile_Input/,\n"
			// 				 << "  and that it ends in .txt"
			// 				 << endl;
			// 			ignore_next_arg = true;
			// 		}

			// 		test_open.close();

			// 	}

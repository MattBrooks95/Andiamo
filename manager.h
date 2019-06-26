//! \file manager.h declares the manager class, which manipulates and stores tiles

#pragma once
#include<vector>
#include<map>

#include<fstream>
#include<iostream>
#include<regex>

//for sorting the tiles based on width
#include<algorithm>

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<regex>

#include "std_helpers.h"

#include "regex_manager.h"
#include "field.h"
#include "input_maker.h"
#include "system_wrapper.h"
#include "logger.h"
#include "ftran_structs.h"

using std::string;
using std::regex;
using std::map;

extern logger* logger_access;;
extern button_manager* button_access;
extern system_wrapper* system_access;

extern regex_manager* regex_access;

//relative to Andiamo's root path
#define CONFIGURATION_PATH "config/parameter_config/"

#define REGEX_FILE_NAME "parameter_regular_expressions.txt"
#define HF_FILE_NAME "HF_config.txt"
#define PARAMETER_GRAPHICS_FILE_NAME "tiles.txt"

class button_manager;

//! The manager handles an STL map and  vector that contains parameter's graphics
class manager{
  public:
		//! this is the constructor for the manager class
		/*! the manager constructor doesn't do anything right now, as set up
	     * must occur after sdl_help's constructor */
		manager(const string& config_folder);

		//! destructor deletes the objects that the map & vector of tiles point to
		~manager();

		//! helper for constructor, sets up regular expressions for parameters
		void init_regular_expressions();

		//! helper for constructor, sets up the parameter default values
		void init_parameter_configurations();

		//! helper for constructor, sets up the parameter graphics, names and line placement
		void init_parameter_graphics();

		//! this function walks the map, and returns the width of the widest tile
		int get_widest_tile_width();

		//! sets the graphics for the main parameter fields, besides the text box
		// void init_fields_graphics();

		//! this function draws the tiles
		void draw();

	    //! searches the field parameters 2D map using the passed string as the key
	    field* get_param(const string& target_param);

	    //! searches only a specific line of parameters
	    field* get_param_from_line(const string& target_param,map<string,field*>& target_line);

		//! updates input_maker's vectors with the field's new values (from user)
		/*\return true means that there was no errors in string->data conversion
		 *\return false means that an stoi/stod function failed,
	     *and the file wasn't created */
		// bool update_io_maker(vector<string>& bad_input_list);

		//! does the work of locking all the fields in the passed map
		void update_locks_in_line(map<string,field*>& target_line, bool lock_value);

		//! convenience method that wraps update_locks_in_line, for locking
		void lock_line(map<string,field*>& target_line);

		//! convenience method that wraps update_locks_in_line, for unlocking
		void unlock_line(map<string,field*>& target_line);

		//! runs through the 2d map, and prints all of the lines and their fields
		/* \param outs is the output stream that will be printed to */
		void print_all();

		/*! \brief analyzes some parameter tiles, to implement the control logic
	     * of selector variables */
		void check_locks();

		//simple cases for locking and unlocking

		/* we can't do iench = 7 stuff
		// helper function for check_locks()
		void iench_locking();
		*/

		//! helper function for check_locks()
		void ilv1_locking();
		//###################################

		//###################################################
		//! helper for check_locks()
		void icntrl4_locking();

		/*! \brief helper for icntrl4_locking(), it's unlocked parameters
	     *must also be filled in to unlock form button */
		void ich4_nch4_locking();
		//###################################################

		//Complex parameter entry cases############################################

		//! helper function for check_locks()
		void icntrl8_locking();

	    //! helper function for icntrl10_locking, opens NNSIG parameter
	    void icntrl10_locking();

		//! helper function for check_locks()
		void ilv3_ilv5_locking();

		/*! \brief helper  for ilv3_ilv5_locking(), determines lock/unlock
	     *conditions of passed field */
		/*! \param target_param pointer to a field (ILV3 or ILV5)*/
		void ilv3_ilv5_locking_helper(field* this_field,
	                                  const regex& unlock_condition);

		//##### ICNTRL6 locking + helpers ######################################

		//! helper function for check_locks()
		void icntrl6_locking();

		//! helper function for icntrl6_locking()
		void inm1_locking();

		//! helper function for icntrl6_locking()
		void inm2_locking();

		//! helper function for icntrl6_locking()
		void iter_locking();
		//#######################################################################

		//! This member updates the window's dimension values win_w and win_h
		/*! This should usually only be called from a member of the sdl_help class,
		 *probably whenever the user resizes the screen, and at startup of Andiamo
		 *\param width_in is the desired new width
		 *\param height_in is the desired new height */
		void update_win(int width_in, int height_in);

		//! uses configuration_folder_path and the passed file name to build an absolute path
		string get_configuration_file_path(const string& file_name);

		void fill_vector_with_configuration_lines_from_file(const string& file_name,vector<string>& file_lines);

		//! store the line names in order
		/*! this is used by sdl_help::calc_corners to create the
		 *graphical labels that tell the user what line each line
		 *of parameters in the GUI corresponds to */
		vector<string> line_names_read_order;

		//! save the string names of lines & parameters in a vector
		/*! this is to be used for iterating over the parameters,
		 *so lines and their parameters can be processed in order.
		 *The fields map should be used for individual item lookup */
		vector<vector<field*>> fields_order;

		//! stores pointers to the fields in a 2D map
		/*! the first access should be the line name from the HF manual
		 *in which the desired parameter resides, then the second access
		 *should be the parameter name. */
		map<string,map<string,field*>*> fields;

		//! maps parameter names to their regular expression patters
		map<string,regex*> parameter_regexps;

		//! maps parameter names to their intial value as a string
		map<string,string> parameter_defaults;
	private:

		void insert_line_pointer(const std::string& line_name, std::map<std::string, field*>* line_map, std::vector<field*>& this_lines_parameters_in_order);

		//! path to the image directory for the tiles
		string image_path;

		//! a path string to the tile input file folder
		string configuration_folder_path;

		//! "default" unless set by --configfolder option
		string configuration_folder_name;

		//! keeps track of sdl window width
		int win_w;

		// keeps track of sdl window height
		int win_h;
};

void print_matching_message(const string& match_case, int expected_groups);

//! helper for modifies description_line by reference, to cut off 'c ' part
/*! initializes temp_descriptions to point to a new vector if it is NULL */
void handle_description_line(vector<string>*& temp_descriptions, const std::smatch& groups_from_line_match);

//! helper for init parameter graphics, returns image name out of a regex match
string handle_image_name_line(const std::smatch& groups_from_line_match);

//! helper for init parameter graphics, sets new field's image's size variables
void handle_field_size_match(const std::smatch& groups_from_line_match, int& width, int& height);

//! helper for init parameter graphics, sets the new field's tile & display names
void handle_name_line_match(const std::smatch& groups_from_line_match, string& tile_name, string& display_name);


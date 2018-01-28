/*! \file button_manager.h this file describes the button_manager
 *class that contains various derivations of class button */

#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<iostream>
#include<string>
#include<fstream>

#include "button.h"
#include "derived_buttons.h"
#include "form_buttons.h"
#include "sdl_help.h"


#include "logger.h"


using std::string;
using std::vector;
using std::regex;

extern logger error_logger;

extern sdl_help* sdl_access;

//! button_manager is a manager for the different derivations of class button
class button_manager{
  public:
	//! the constructor zeros this class's variables and nulls the pointers
	button_manager();
	~button_manager();

	//################### BUTTON TRAY STUFF ##################################//

	//! this member sets up the tray in which the buttons are placed
	void init_tray();

	/*! this member sets up the buttons that open and close forms
	 * for extensive parameter entry */
	void init_form_tray();

	/*! this member updates the lock graphics after a resize.
	 *  it is a helper function for location_update */
	void redo_locks();

	//! this member displays the tray to the screen
	void draw_tray();

	/*! this member displays the form button tray, and their locks,
	 *when appropriate */
	void draw_form_tray();

	/*! this member updates the tray's location in response to
	 *a window size change */
	void location_update();

	//########### FORM BUTTON GETTERS ########################################//
	//! returns reference to icntrl8 form button
	icntrl8_form_button& get_icntrl_8(){return icntrl_8;}

	//! returns reference to icntrl6 form button
	icntrl6_form_button& get_icntrl_6(){return icntrl_6;}

	//! returns reference to the icntrl10 form button
	icntrl10_form_button& get_icntrl_10(){return icntrl_10;}

	//! returns reference to the icntrl4 form button
	icntrl4_form_button& get_icntrl_4(){return icntrl_4;}

	//! returns a reference to the ilv3/ivl5 button
	ilv3_ilv5_form_button& get_ilv3_ilv5(){return ilv3_ilv5;}
	//########################################################################//


	//##############################################################################################//
	/*! this member calls the virtual init() memeber on each of
	 * button_manager's private buttons */
	void init_buttons();

	//! this member sets up the buttons that populate the forms tray
	void init_form_buttons();

	//! this member sets up the regex array parameters given to forms
	/*! the forms will use these to do type/input checking */
	void fill_regex_vectors(vector<regex>& icntrl_6_patterns,
							vector<regex>& icntrl_8_patterns,
					   		vector<regex>& icntrl_10_patterns,
							vector<regex>& icntrl_4_patterns,
							vector<regex>& ilv3_ilv5_patterns);

	//! this member calls each buttons virtual print_me() member
	void print_buttons();

	//! this member calls each button's virtual draw_me() member
	/*! it also calls draw_tray() at the very beginning,
	 *so that the tray is drawn "below" the buttons */
	void draw_buttons();

	//! logic to handle sdl_inputs and backspaces is here
	void text_box_loop(text_box_button* current_button,SDL_Event& event);

	//!  clean_up lets input_maker know which options the user has selected
	/*! this will be done by accessing the input_maker and
	 *changing the output paths. */
	int clean_up();

	//! displays warning messages about why file generation may have failed
	/*! it is a helper function for button_manager::clean_up().
	 *It mostly just displays some premade error message boxes on the screen
	 *for a couple of seconds before the program resumes */
	void clean_up_warnings(bool bad_output_fname,bool bad_tc_input_fname);

	//! displays a warning when tiles fail to convert user inputs
	/*! it used the vector of strings created by manger::update_io_maker
	 *to print the names of the tiles in question,
	 *and also changes them to be red, for a visual indicator */
	void bad_tile_input_warnings(vector<string>& bad_tile_input_warnings);

	//! this member calls each button's virtual handle_click() member
	/*! a notable exception is the 'exit dialogue' button, because it's
	 * logic is partially controlled by main()
	 *It shouldn't be placed with the other buttons, as it only shows up
	 *when the user tries to quit the project.
	 *Returns true if a button was clicked, and false elsewise */
	bool click_handling(SDL_Event& mouse_event);

	//! helper function for click_handling that makes an error message
	/*! \param form_bad_inputs is a string list of error messages from
	 * the form button's output members. */
	void make_form_error_message(const vector<string>& form_bad_inputs,
						SDL_Texture*& message_texture, SDL_Rect& destination);

	//! implements the loop that allows the user to view the form error message
	/*! scrolling is allowed, so the whole error message can be viewed */
	void form_error_message_loop(SDL_Event& mouse_event,
				SDL_Texture* message_texture, SDL_Rect& destination);


	//GETTERS AND SETTERS
	//! this member returns the exit button by reference
	exit_button& get_exit_dialogue(){ return exit_dialogue;}

	//! this member returns the fop opening button by reference
	button& get_fop_button(){ return fop_button;}

	//!< this is a string that describes the path to the button's image files
	string button_image_p;

  private:

	//! save the texture for the tray on which buttons sit
	SDL_Texture* button_tray_texture;

	//####################### FORM BUTTONS ####################################
	//! name for the form tray's image file
	string form_tray_image_name;

	//! save the texture for the tray on which the form buttons sit
	SDL_Texture* form_tray_texture;	

	//! allows user to enter extra lines as a resault of icntrl8 being used
	icntrl8_form_button icntrl_8;

	//! allows user to enter extra lines as a result of icntrl6 being used
	icntrl6_form_button icntrl_6;

	//! allows user to enter extra lines as a result of ilv_2 being used
	icntrl10_form_button icntrl_10;

	//! allows user to enter extra lines as a result of icntrl4 being used
	icntrl4_form_button icntrl_4;

	//! allows user to enter parameters for ILV3 or ILV5
	ilv3_ilv5_form_button ilv3_ilv5;

	//! drawing destination for the form tray
	SDL_Rect form_tray_rect;
	//#########################################################################


	//! name for the tray's image file.
	string tray_image_name;

	//! keep track of whether or not the tray should be drawn to the screen
	bool tray_shown;

	//! drawing destination for the button tray
	SDL_Rect tray_rect;

	//! button that launches the fop_handler
	fop_handler_button fop_button;

	//! button to allow user to select where to output the HF input file
	output_file_button output_fname;

	//! button to allow user to select the input transmission coefficients file
	TC_input_file_button t_coefficients;

	button lets_go;//!< button to generate output
	//! button to allow user to select graphing options
	//graphing_button graphing_options;

	//! first class that inherits from button default class, handles exiting
	exit_button exit_dialogue; 

};
















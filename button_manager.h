//! \file button_manager.h this file describes the button_manager class that contains various derivations of class button
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
extern logger error_logger;

//! button_manager is a manager for the different derivations of class button
/*! these buttons should either stay in a box below the tiles, OR be fixed to the window kind of like a HUD,
 *It needs access to a lot of sdl_helper fields, like the renderer, font, window size... */
class button_manager{
  public:
	//! the constructor expects only a pointer to the graphics class so it can draw the buttons
	/* \param sdl_helper_in is a pointer to the main graphics class sdl_help, found in sdl_help.h */
	button_manager(sdl_help* sdl_helper_in);
	~button_manager();

	//#################### BUTTON TRAY STUFF #######################################################//

	//! this member sets up the tray in which the buttons are placed
	void init_tray();

	//! this member sets up the buttons that open and close forms for extensive parameter entry
	void init_form_tray();

	//! this member updates the lock graphics after a resize - helper for location_update
	void redo_locks();


	//! this member displays the tray to the screen
	void draw_tray();

	//! this member displays the form button tray, and their locks, when appropriate
	void draw_form_tray();

	//! this member updates the tray's location in response to a window size change
	void location_update();

	//########### FORM BUTTON GETTERS ########################################################################//
	icntrl8_form_button& get_icntrl_8(){return icntrl_8;}//!< returns reference to icntrl8 form button
	icntrl6_form_button& get_icntrl_6(){return icntrl_6;}//!< returns reference to icntrl6 form button
	ilv2_form_button& get_ilv_2(){return ilv_2;}//!< returns reference to teh ilv2 form button
	icntrl10_form_button& get_icntrl_10(){return icntrl_10;}//!< returns reference to the icntrl10 form button
	icntrl4_form_button& get_icntrl_4(){return icntrl_4;}//!< returns reference to the icntrl4 form button
	ilv3_ilv5_form_button& get_ilv3_ilv5(){return ilv3_ilv5;}
	//########################################################################################################//


	//##############################################################################################//
	//! this member calls the virtual init() memeber on each of button_manager's private buttons
	void init_buttons();

	//! this member sets up the buttons that populate the forms tray
	void init_form_buttons();

	//! this member calls each buttons virtual print_me() member
	void print_buttons();

	//! this member calls each button's virtual draw_me() member
	/*! it also calls draw_tray() at the very beginning, so that the tray is drawn "below" the buttons */
	void draw_buttons();


	//! logic to handle sdl_inputs and backspaces is here
	/*! it has to be here, because the text boxes do not have reference to all the other buttons
	 *and can't draw them every frame, so whatever buttons you're not interacting with would disappear.
	 *doing the loop in the button_manager should fix this issue. */
	void text_box_loop(text_box_button* current_button,SDL_Event& event);

	//! the clean up function lets input_maker know which options the user has selected
	/*! this will be done by accessing input_maker through the button_managers sdl_helper pointer
	 *and changing the output paths. */
	int clean_up();

	//! this function displays some warning messages about why output file generation may have failed
	/*! it is a helper function for button_manager::clean_up(). It mostly just displays some
	 *premade error message boxes on the screen for a couple of seconds before the program
	 *resumes it's regular processes */
	void clean_up_warnings(bool bad_output_fname,bool bad_tc_input_fname);

	//! this function displays a warning message when tiles fail to convert user inputs to scientific values
	/*! it used the vector of strings created by manger::update_io_maker to print the names of the
	 * tiles in question, and also changes them to be red, for a visual indicator */
	void bad_tile_input_warnings(std::vector<std::string>& bad_tile_input_warnings);

	//! this member calls each button's virtual handle_click() member
	/*! a notable exception is the 'exit dialogue' button, because it's logic is partially controlled
	 *via main(), because it has special logic. It shouldn't be placed with the other buttons, as it
	 *only shows up when the user tries to quit the project. Returns true if a button was clicked,
	 *and false elsewise */
	bool click_handling(SDL_Event& mouse_event);

	//GETTERS AND SETTERS
	//! this member returns the exit button by reference, so its members can be called by other things
	exit_button& get_exit_dialogue(){ return exit_dialogue;}
	//! this member returns the default, example button by reference, so its members can be called
	button& get_default_test(){ return default_test;}

	std::string button_image_p;//!< this is a string that describes the path to the button's image files

	sdl_help* sdl_helper;//!< this is a pointer to the main graphics class, for renderer and font access
  private:

	SDL_Surface* button_tray_surf;//!< save the surface for the tray on which buttons sit
	SDL_Texture* button_tray_texture;//!< save the texture for the tray on which buttons sit

	//####################### FORM BUTTONS ##################################################################
	//! name for the form tray's image file. Defaults to button_image_p + "form_tray.png" in constructor
	std::string form_tray_image_name;

	SDL_Surface* form_tray_surface;//!< save the surface for the tray on which the form buttons sit
	SDL_Texture* form_tray_texture;//!< save the texture for the tray on which the form buttons sit
	
	icntrl8_form_button icntrl_8;//!< allows user to enter extra lines as a resault of icntrl8 being used
	icntrl6_form_button icntrl_6;//!< allows user to enter extra lines as a result of icntrl6 being used
	ilv2_form_button ilv_2;//!< allows user to enter extra lines as a result of ilv_2 being used
	icntrl10_form_button icntrl_10;//!< allows user to enter extra lines as a result of ilv_2 being used
	icntrl4_form_button icntrl_4;//!< allows user to enter extra lines as a result of icntrl4 being used
	ilv3_ilv5_form_button ilv3_ilv5;//!< allows user to enter parameters for ILV3 or ILV5

	SDL_Rect form_tray_rect;//!< drawing destination for the form tray

	//#######################################################################################################


	//! name for the tray's image file. Defaults to button_image_p + "button_tray.png" in constructor
	std::string tray_image_name;

	bool tray_shown;//!< keep track of whether or not the tray should be drawn to the screen
	SDL_Rect tray_rect;//!< drawing destination for the button tray


	//! This vector contains the "normal" buttons that actually sit on the button tray
	/*! there will be special buttons, like the exit dialogue, who do not belong here. */
	std::vector<button> buttons;


	button default_test;//!< example of a base class instantiation, not actually for use

	output_file_button output_fname;//!< button to allow user to select where to output the HF input file

	//! button to allow user to select the input transmission coefficients file
	TC_input_file_button t_coefficients;

	button lets_go;//!< button to generate output
	//graphing_button graphing_options;//!< button to allow user to select graphing options
	exit_button exit_dialogue; //! first class that inherits from button default class, handles exiting

};
















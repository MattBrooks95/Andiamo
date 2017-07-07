//! this file describes the button_manager class that contains various derivations of class button
#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<iostream>
#include<string>
#include<fstream>


#include "button.h"
#include "derived_buttons.h"

#include "sdl_help.h"

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

	//! this member displays the tray to the screen
	void draw_tray();

	//! this member updates the tray's location in response to a window size change
	void location_update();

	//##############################################################################################//
	//! this member calls the virtual init() memeber on each of button_manager's private buttons
	void init_buttons();
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

	//! this member calls each button's virtual handle_click() member
	/*! a notable exception is the 'exit dialogue' button, because it's logic is partially controlled
	 *via main(), because it has special logic. It shouldn't be placed with the other buttons, as it
	 *only shows up when the user tries to quit the project */
	void click_handling(SDL_Event& mouse_event);

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

	//! name for the tray's image file. Defaults to button_image_p + "button_tray.png" in constructor
	std::string tray_image_name;

	bool tray_shown;//!< keep track of whether or not the tray should be drawn to the screen
	SDL_Rect tray_rect;//!< drawing destion for the tray


	button default_test;//!< example of a base class instantiation, not actually for use

	output_file_button output_fname;//!< button to allow user to select where to output the HF input file

	//! button to allow user to select the input transmission coefficients file
	TC_input_file_button t_coefficients;

	button lets_go;//!< button to generate output
	graphing_button graphing_options;//!< button to allow user to select graphing options
	exit_button exit_dialogue; //! first class that inherits from button default class, handles exiting

};
















//! this file describes the button_manager class that contains various derivations of class button
#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#include<iostream>
#include<string>
#include<fstream>
#include "sdl_help.h"

#include "button.h"
#include "derived_buttons.h"

//! button_manager is a manager for the different derivations of class button
/*! these buttons should either stay in a box below the tiles, OR be fixed to the window kind of like a HUD,
 *It needs access to a lot of sdl_helper fields, like the renderer, font, window size... */
class button_manager{
  public:
	//! the constructor expects only a pointer to the graphics class so it can draw the buttons
	/* \param sdl_helper_in is a pointer to the main graphics class sdl_help, found in sdl_help.h */
	button_manager(sdl_help* sdl_helper_in);
	~button_manager();

	//! this member calls the virtual init() memeber on each of button_manager's private buttons
	void init_buttons();
	//! this member calls each buttons virtual print_me() member
	void print_buttons();

	//! this member calls each button's virtual draw_me() member
	void draw_buttons();

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

	sdl_help* sdl_helper;//!< this is a pointer to the main graphics class, for renderer access
  private:
	button default_test;//!< example of a base class instantiation, not actually for use
	exit_button exit_dialogue; //! first class that inherits from button default class, handles exiting
	//button make_output;
	//button output_file_set;

};

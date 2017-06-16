//! \file declares the classes that inherit from button.h's button class
#pragma once
#include "button.h"

//########################## EXIT BUTTON ###########################################################################

//! this class describes the exit dialogue that pops up when the main loop encounters SDL_QUIT
/*! it inherits members and fields from the default button class in button.h, so a lot of that
 *functionality doesn't need re-written here. The only things that need implemented here are new
 *variables and new members to act on them that the base class doesn't have */
class exit_button : public button{
  public:
	//! this doesn't do anything special for now
	exit_button();	
	//! set_corner_loc() has different logic than the base class's version button::set_corner_loc()
	/*! it places the exit dialogue in the exact center of the screen. It is not shown until the user
	 *sends an SDL_QUIT, and the button manager makes this button shown */
	void set_corner_loc();
	//! does the printing from the base class, along with extra lines to output the special information
	/*! it calls button::print_me, from the default class, and calls active_area::print_me() for
	 *the yes and no boxes. This is a good example of why class inheritance is cool. */
	void print_me();

	//###########################################################
	//! this special version of handle_click uses helper functions to implement the yes/no quit dialogue
	void handle_click(SDL_Event& mouse_event);
	//###########################################################
	//! this member is used instead of the (optional) virtual members in this case
	/*! this is used instead of the virtual members because the exit dialogue has two active areas
	 *the yes part and the no part, so the normal boolean won't work here, so it implements
	 *a no/yes_this/yes_that return value for 0 (click out of exit box, 1 (no was clicked), and 2
	 * (yes was clicked)
	 *\param mouse_event contains the necessary click information, param for active_area::clicked
	 *\return 0, neither yes or no was clicked
	 *\return 1, no was clicked
	 *\return 2, yes was clicked */
	int my_click(SDL_Event& mouse_event);
	//! used instead of optional default click_helper - does work according to return value of my_click
	/*! has logic for yes or no being hit
	 *\param which is used to control logic
	 *\param satisfied is used to stop the loop in exit_button::handle_click*/ 
	void my_click_helper(int which,bool& satisfied);

	//! set up active areas and corner location using set_corner_loc
	/* this calls button::init() to set up the import bits of the base class, then sets up
	 *the special bits of exit_button like the shown boolean defaulting to false, and a different
	 *corner location
	 *\param image_name_in parameter for button::init
	 *\param image_p_in parameter for button::init
	 *\param sdl_help_in parameter for button::init */
	void init(std::string image_name_in, std::string image_p_in,sdl_help* sdl_help_in);


  private:
	//! click area for the no button
	active_area no_area;
	//! click area for the yes button
	active_area yes_area;


};
//##################################################################################################################

//##################################################################################################################
//! implements the button that has the graphing options
/*! these options should include a checkbox to control whether or not graphing is done at all,
 *and a text input field for the desired output file name */
class graphing_button : public button{

  public:
	//! need a new destructor, to clear memory from the check box surfaces and the text box
	~graphing_button();
	//! draws different texture based on whether or not this button has been checked
	void draw_me();

	//! prints the base information, and the information special to this class
	void print_me();

	//! does button::init() and also sets up the checked texture
	void init(std::string image_name_in, std::string image_p_in,sdl_help* sdl_help_in);

	//! force_corner_lock does normal stuff, and forces the active area to update as well
	void force_corner_loc(int xloc_in, int yloc_in);

	//! overload click_helper, to toggle the checkmark mode
	/*! it will also have to interact with the text field */
	void click_helper(SDL_Event& mouse_event);


  private:
	bool show_check_version;//!< boolean used to decide whether we're drawing the check or not
	active_area check_box; //!< used to detect the check box being clicked

	SDL_Surface* checked_surface; //!< save the surface for the checked version
	SDL_Texture* checked_texture; //!< save the texture for the checked version

};
//##################################################################################################################






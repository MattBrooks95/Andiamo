/*! \file derived_buttons.h declares the classes that inherit from button.h's
 *button class */
#pragma once
#include<fstream>

#include "button.h"
#include "text_box.h"
#include "input_maker.h"
#include "fop_handler.h"

extern input_maker* io_access;
extern fop_handler* FOP_access;

//################## EXIT BUTTON ###############################################

using std::string;

//! describes the exit dialogue that pops up when an SDL_QUIT event happens
/*! it inherits members and fields from the default button class in button.h,
 *so a lot of that functionality doesn't need re-written here. The only things
 *that need implemented here are new variables and new members
 *to act on them that the base class doesn't have */
class exit_button : public button{
  public:
	//! this doesn't do anything special for now
	exit_button();	
	//! has different logic than the base class's version 
	/*! it places the exit dialogue in the exact center of the screen.
	 *It is not shown until the user sends an SDL_QUIT,
	 *and the button manager makes this button shown */
	void set_corner_loc();

	//! calls the base class's print, also prints the unique information
	/*! it calls button::print_me, from the default class, and calls
	 *active_area::print_me() for the yes and no boxes.
	 *This is a good example of why class inheritance is cool. */
	void print_me();

	/*! version of handle_click uses helper functions to implement
	 *the yes/no quit dialogue */
	bool handle_click(SDL_Event& mouse_event);

	//! used instead of the base class's my_click
	/*! this is used instead of the virtual members because the exit
	 *dialogue has two active areas the yes part and the no part,
	 *so the normal boolean won't work here, so it implements
	 *a no/yes_this/yes_that return value for 0
	 *(click out of exit box, 1 (no was clicked), and 2 (yes was clicked)
	 *\param mouse_event contains the click information
	 *\return 0, neither yes or no was clicked
	 *\return 1, no was clicked
	 *\return 2, yes was clicked */
	int my_click(SDL_Event& mouse_event);

	//! does work according to return value of my_click
	/*! has logic for yes or no being hit
	 *\param which is used to control logic
	 *\param satisfied is used to stop the loop in exit_button::handle_click */ 
	void my_click_helper(int which,bool& satisfied);

	//! set up active areas and corner location using set_corner_loc
	/* calls button::init(), then sets up the special bits of exit_button
	 *like the shown boolean defaulting to false, and a different
	 *corner location
	 *\param image_name_in parameter for button::init
	 *\param image_p_in parameter for button::init */
	void init(string image_name_in, string image_p_in);


  private:
	//! click area for the no button
	active_area no_area;
	//! click area for the yes button
	active_area yes_area;


};
//##############################################################################


//##############################################################################

class text_box_button : public button{
  public:
	//~text_box_button();

	//! needs new draw_me(), to draw its text box as well
	void draw_me();

	//! calls button:print_me(), but also prints info about its text box
	void print_me();
	//! this is a pure virtual member
	/*! it must be implemented in classes that inherit from this class,
	 *because some will want to READ from the input file,
	 *and some will want to WRITE to the output file */
	virtual int work() = 0;

	/*! overload parent's handle_resize() function, also
	 *updates the text box's coordinates */
	void handle_resize(int yloc_in);

	//! init also sets up the text box
	void init(const string& image_name_in,const string& image_p_in);

	//! force_corner_loc also updates the text box
	void force_corner_loc(int xloc_in, int yloc_in);

	//! text_box object
	text_box my_text_box;

  protected:
	//! pointer to the font to use for this button
	TTF_Font* sdl_help_font;
};



/*! this is derived from the text_box_button class, it should read
 *inputs from the given TC file */ 
class TC_input_file_button : public text_box_button{
  public:
	//! transmission coefficients can be read from a given file
	/*! it modifies the transmission coefficient input file name variable
	 *in input_maker in the event that the text box for the
	 *file name is empty, it will return -1 and prevent
	 *button_manager::clean_up() from executing */
	int work();
  private:

};

//! writes the HF input file (final product) to this file
class output_file_button : public text_box_button{
  public:
	//! makes sure input_maker writes to the given file name
	int work();

  private:

};

/*
// implements the button that has the graphing options
// these options should include a checkbox to control whether
//or not graphing is done at all, and a text input field
//for the desired output file name
class graphing_button : public text_box_button{

  public:
	// clears memory from the check box surfaces and the text box
	~graphing_button();
	//draws different texture based on whether or not this
	//button has been checked
	void draw_me();

	// prints the base information, and the information special to this class
	void print_me();

	// this work function needs implemented to make sure graphing is output to the given file name
	int work();

	// does button::init() and also sets up the checked texture
	void init(const string& image_name_in, const string& image_p_in);

	// force_corner_lock does normal stuff, and forces the active area to update as well
	void force_corner_loc(int xloc_in, int yloc_in);

	// overload handle_click, to toggle the checkmark mode
	bool handle_click(SDL_Event& mouse_event);

  private:
	// boolean used to decide whether we're drawing the check or not
	bool show_check_version;

	// used to detect the check box being clicked
	active_area check_box;

	// save the surface for the checked version
	SDL_Surface* checked_surface;
	// save the texture for the checked version
	SDL_Texture* checked_texture;

};
*/
//##############################################################################

class fop_handler_button : public button{

	//! overload handle_click, to open interactive FOP feature
	void click_helper(SDL_Event& mouse_event);

	//! this function implementation opens a fop_handler object
	void work();

};









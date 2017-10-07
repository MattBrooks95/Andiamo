//! \file derived_buttons.h declares the classes that inherit from button.h's button class
#pragma once
#include<fstream>

#include "button.h"
#include "text_box.h"
#include "input_maker.h"
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
	bool handle_click(SDL_Event& mouse_event);
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

class text_box_button : public button{
  public:
	//~text_box_button();

	//! needs new draw_me(), to draw its text box as well
	void draw_me();

	//! print me calls button:print_me(), but also prints info about its text box
	void print_me();
	//! this is a pure virtual member
	/*! it must be implemented in classes that inherit from this class, because some will want to
	 *READ from the input file, and some will want to WRITE to the output file */
	virtual int work(input_maker& io_handler) = 0;

	//! overload parent's handle_resize() function, also updates the text box coordinates
	void handle_resize(int yloc_in);

	//! init also sets up the text box
	void init(const std::string& image_name_in,const std::string& image_p_in,sdl_help* sdl_help_in);

	//! force_corner_loc also updates the text box
	void force_corner_loc(int xloc_in, int yloc_in);

	text_box my_text_box;

  protected:
	TTF_Font* sdl_help_font;
};



//! this is derived from the text_box_button class, it should read inputs from the given TC file 
class TC_input_file_button : public text_box_button{
  public:
	//! this function should make sure that the transmission coefficients are read from the given file
	/*! it modifies the transmission coefficient input file name variable in input_maker
	 *in the event that the text box for the file name is empty, it will return -1 and
	 *prevent button_manager::clean_up() from executing */
	int work(input_maker& io_helper);
  private:

};
//! this is derived from the text_box_button class, it should WRITE the HF input file (final product) to this file
class output_file_button : public text_box_button{
  public:
	//! this function should make sure input_maker writes to the given file name
	int work(input_maker& io_helper);

  private:

};


//! implements the button that has the graphing options
/*! these options should include a checkbox to control whether or not graphing is done at all,
 *and a text input field for the desired output file name */
class graphing_button : public text_box_button{

  public:
	//! need a new destructor, to clear memory from the check box surfaces and the text box
	~graphing_button();
	//! draws different texture based on whether or not this button has been checked
	void draw_me();

	//! prints the base information, and the information special to this class
	void print_me();

	//! this work function needs implemented to make sure graphing is output to the given file name
	int work(input_maker& io_handler);

	//! does button::init() and also sets up the checked texture
	void init(const std::string& image_name_in, const std::string& image_p_in,sdl_help* sdl_help_in);

	//! force_corner_lock does normal stuff, and forces the active area to update as well
	void force_corner_loc(int xloc_in, int yloc_in);

	//! overload handle_click, to toggle the checkmark mode
	bool handle_click(SDL_Event& mouse_event);

  private:

	bool show_check_version;//!< boolean used to decide whether we're drawing the check or not
	active_area check_box; //!< used to detect the check box being clicked

	SDL_Surface* checked_surface; //!< save the surface for the checked version
	SDL_Texture* checked_texture; //!< save the texture for the checked version

};
//##################################################################################################################

class fop_handler_button : public button{

	//! overload handle_click, to open interactive FOP feature
	void click_helper(SDL_Event& mouse_event);
	void work();//! this function implementation opens a fop_handler object




};









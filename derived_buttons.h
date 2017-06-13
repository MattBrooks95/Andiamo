//! \file declares the classes that inherit from button.h's button class
#pragma once
#include "button.h"

//! contains the information needed for rectangular "click areas", like the yes/no buttons on the exit dialogue
/*! this should be useful for many different types of buttons */
struct active_area{
	//! constructor defaults stuff to 0
	active_area(){
		xloc = 0;
		yloc = 0;
		width= 0;
		height=0;
	}
	//! boolean variable to check mouse click coords against this active area's location
	bool clicked(SDL_Event& event){
		if( (event.button.x > xloc && event.button.x < xloc + width) &&
		    (event.button.y > yloc && event.button.y < yloc + height) ){
			return true;
		}
		return false;
	}

	int xloc;
	int yloc;
	int width;
	int height;
};

//! this class describes the exit dialogue that pops up when the main loop encounters SDL_QUIT
class exit_button : public button{
  public:
	exit_button();
	void set_corner_loc();

	void print_me();

	//###########################################################
	void handle_click(SDL_Event& mouse_event);
	//###########################################################
	//! this member is used instead of the (optional) virtual members in this case
	/*! this is used instead of the virtual members because the exit dialogue has two active areas
	 *the yes part and the no part, so the normal logic won't work here */
	int my_click(SDL_Event& mouse_event);
	//! used instead of optional default click_helper
	/*! has logic for yes or no being hit */ 
	void my_click_helper(int which);

	//! set up active areas and corner location using set_corner_loc
	void init(std::string image_name_in, std::string image_p_in,sdl_help* sdl_help_in);


  private:
	//! click area for the no button
	active_area no_button;
	//! click area for the yes button
	active_area yes_button;


};

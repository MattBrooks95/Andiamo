//! \file button.h has declarations for the buttons class, which manages static buttons that enact important functions

#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#include<iostream>
#include<string>
#include<fstream>
#include "sdl_help.h"

#include "asset_manager.h"
#include "logger.h"

using std::string;

extern logger error_logger;

//! this is the base class for the static buttons that implement features
/*! some possible features are going to include graphing options, the exit dialogue opening,
 *output file name entry*/
class button{
  public:
	//! constructor for the base part of all inherited classes
	/* the constructor does the same thing everytime, but it has an sdl_help* param that can be left blank.
	 *I'm leaving it in, in case it is useful later
	 *\param sdl_help_in defaults to NULL, and is later set by init */
	button(sdl_help* sdl_help_in = NULL);

	//! this frees the memory held by the surface and texture of a class
	/*! if an inherited class implements more textures and surfaces, then that derived class
	 *should have a special destructor written for it. */
	virtual ~button();

	//! this is the virtual print_me() function
	/*! it prints the default variables contained in a button class. If a class needs more printing,
	 *it needs to implement a print_me() function that first calls this one via button::print_me() 
	 *(to print the base variables) and then does it special logic that prints its own extra variables */
	virtual void print_me();

	//! this is the virtual draw_me() function
	/*! so long as the class has the same basic structure as the default class, this shouldn't need
	 *a special implementation. It bases its logic on the virtual set_corner_logic() and make_rect() logic,
	 *so those should be written to make sure the button is drawn where you need it to be */
	virtual void draw_me();

	//! this is the virtual make_rect() function
	/*! it really just packages the data set by set_corner_logic() into an SDL_Rect, to be used for
	 *rendercopy destinations (where it needs to be drawn) */
	virtual void make_rect();

	//! this is the virtual set_corner_logic() function
	/*! this will likely need overwritten if you want the button to be drawn somewhere special, and not in the
	 *same location as more default-like buttons */
	virtual void set_corner_loc();

	//! this member is used to set a custom corner location
	/*! an example would be button_manager calling this to move the buttons on top of the button tray */
	virtual void force_corner_loc(int xloc_in, int yloc_in);

	//! update y location in response to the window getting taller or shorter
	virtual void handle_resize(int yloc_in);

	//###########################################################
	//! this virtual function handles the button being clicked
	/*! it uses virtual was_clicked() to figure out if the user actually clicked on it, and then
	 *it calls virtual click_helper() to implement the "work" resultant of it being clicked
	 *\param mouse_event is the SDL_Event that contains the click information */
	virtual bool handle_click(SDL_Event& mouse_event);
	//! this virtual function does the boolean logic for handle_click()
	/*! \param mouse_event is the SDL_Event that contains the click information */
	virtual bool was_clicked(SDL_Event& mouse_event);
	//! this virtual function does the work when the button is clicked
	/*! \param mouse_event is the SDL_Event that contains the click information */
	virtual void click_helper(SDL_Event& mouse_event);
	//###########################################################
	//! this virtual init function sets up the image name, path, and the sdl_help pointer
	/*! \param image_name_in is what the name of the image will be set to, like "default_button.png"
	 *\param image_p_in is what the path to the button's asset directory will be set to
	 *\param sdl_help_in is the pointer to the main graphics class that will be saved in sdl_helper */
	virtual void init(const string& image_name_in,const string& image_p_in,
			  sdl_help* sdl_help_in);

	//##################### GETTERS AND SETTERS ###########################################
	int get_xloc()   { return xloc;}
	int get_yloc()   { return yloc;}
	int get_width()  { return width;}
	int get_height() { return height;}

	//#####################################################################################

	bool shown;//!< this defaults to true for most buttons, so they are drawn on screen



  protected:
	string image_name;//!< name of the image file
	string total_image_p;//!< image file directory's path

	sdl_help* sdl_helper;//!< pointer to the main graphics class

	int xloc;//!< horizontal location of the button's corner
	int yloc;//!< vertical location of the button's corner

	int width;//!< width of the button's texture
	int height;//!< height of the button's texture

	SDL_Rect my_rect;//!< xloc, yloc, width and height in a SDL_Rect, for SDL functions

	SDL_Texture* button_texture;//!< save the texture pointer from the access manager

};

//! contains the information needed for rectangular "click areas", like the yes/no buttons on the exit dialogue
/*! this should be useful for many different types of buttons */
struct active_area{
	//! constructor defaults stuff to 0
	active_area(){
		xloc   = 0;
		yloc   = 0;
		width  = 0;
		height = 0;
	}
	//! this function allows the setting of variables in this struct
	void set_loc(int xloc_in, int yloc_in, int width_in, int height_in){
		xloc = xloc_in;
		yloc = yloc_in;
		width = width_in;
		height = height_in;
	}

	//! boolean variable to check mouse click coords against this active area's location
	/*! \param event is the SDL event from main that contains the click information */
	bool clicked(SDL_Event& event){

		if( (event.button.x > xloc && event.button.x < xloc + width) &&
		    (event.button.y > yloc && event.button.y < yloc + height) ){
			return true;
		}
		return false;
	}
	//! this print function is most useful when called from a button's print_me() member
	/*! it can be used to "line up" the actual button part that responds to clicks with its texture's
	 *shape and location, like the "yes" and "no" click areas in exit_button */
	void print_me(){
		error_logger.push_msg("Printing active area");
		error_logger.push_msg(std::to_string(xloc)+":"+std::to_string(yloc)+"     "+std::to_string(width)+":"
				      +std::to_string(height));
	}

	int xloc;//!< corner'ss horizontal location
	int yloc;//!< corner's vertical location
	int width;//!< texture's width
	int height;//!< texture's height
};














//! \file scroll_bar.h houses the class declaration for scroll bar objects

#pragma once
#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#include "logger.h"
#include "asset_manager.h"

using std::string;

extern logger* output_access;;

/*! \brief scroll_bar contains the internal bookkeeping necessary for creating scroll bars, and also 
 *references to the pertinent fields of the sdl_help object that contains these objects */
/*! it's pretty cool. The sdl_help() constructor calls this class's init() function and sets it up with
 *pointers to the sdl_help object's important information, so that when they update in the sdl_help object
 *the scroll bar gets access to that updated information instantly */ 
class scroll_bar{

  public:
	/*! the scroll_bar() constructor sets all of it's fields to bad default values,
	 * they must be init() 'd */
	scroll_bar();

	//! init_corner_texture sets up texture, and it's xloc and yloc
	/* it bases it's logic off of the given image path's loaded texture. width > height = horizontal
	 *  scroll bar, height < width = vertical scroll bar */
	void init_corner_texture();

	/*! \brief init takes in pointers to several fields of the sdl_help object and sets this object's 
         *fields to point to the fields in the sdl_help object. It also sets up the image path. */
	/*! This function sets up the fields that point to the sdl_help object's fields, and calls its
	 * helper function init_corner_texture().
         * The pointer fields are pretty cool. They update in tandem with the sdl_help object's fields.
	 *\param x_scroll_in integer pointer to the containing sdl_help object's x_scroll field
	 *\param y_scroll_in integer pointer to the containing sdl_help object's y_scroll field
	 *\param area_width_in integer pointer to the containing sdl_help object's area_width field
	 *\param area_height_in integer pointer to the containing sdl_help object's area_height field
	 *\param window_width_in integer pointer to the containing sdl_help object's window_s.width field
	 *\param window_height_in integer pointer to the containing sdl_help object's window_s.height field
	 *\param renderer_in SDL_Renderer pointer to the containing sdl_help object's renderer pointer
	 *\param image_p_in string that contains the desired image name */ 
	void init(int* x_scroll_in, int* y_scroll_in, const int* window_width_in,
		  const int* window_height_in,SDL_Renderer* renderer_in, string image_name_in);

	//! draw_me uses it's own information to draw itself to the renderer used by the sdl_help object
	void draw_me();

	//! this function should be called whenever the window in sdl_help is resized
	/*! it updates the scroll bars's coordinates */
	void handle_resize();

	/*! this member should update the scroll bar's location*/
	void update();

	//! boolean helper for clicked. Returns true if a clicks x&y values fall within the texture's area
	bool in(int click_x, int click_y) const;

	//! clicked returns true if the call to in() returns true, and calls functions that do things
	bool clicked(int click_x, int click_y) const;

	//################## GETTERS AND SETTERS ############################
	//! returns true if scroll bar is being moved, and false otherwise
	//getters
	bool is_scrolling();

	//! get ycoord of top of the scroll bar (used by vertical bars)
	int get_top(){ return yloc; }
	//! get ycoord of bottom of the scroll bar (used by vertical bars)
	int get_bottom(){ return yloc + height; }

	//! get xcoord of the left side of the scroll bar (use on horiz bars)
	int get_left(){ return xloc; }
	//! get xcoord of the right side of the scroll bar (use on horiz bars)
	int get_right(){ return xloc + width; }

	//setters

	/*! toggles the scrolling_mode boolean based on if the user is 
	 *trying to scroll with one of the scroll bars */
	void scroll_mode_change(bool val_in);
	//###################################################################

  private:
	//**************** SCROLL BAR STUFF *********************************/
	int xloc; //!> keep track of the scroll bar's corner's x coordinate
	int yloc; //!> keep track of the scroll bar's corner's y coordinate
	int width; //!> keep track of scroll bar width
	int height; //!> keep track of the scroll bar height
	string image_p; //!> reference to image source
	SDL_Texture* my_tex; //!> texture for the scroll bar

	//! keeps track of whether or not we are in scroll mode
	/*! where scroll mode refers to the point in time in which the user
         *has clicked and held down on the left mouse button, and may be
         *dragging the scroll bar in order to scroll the page */
	bool scrolling_mode;
	//*******************************************************************/

	//**************** SDL STUFF ****************************************/
	int* x_scroll;
	int* y_scroll;
	const int* window_width;
	const int* window_height;
	SDL_Renderer* renderer;
	//*******************************************************************/



};

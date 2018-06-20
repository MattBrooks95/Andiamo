//! \file cursor.h describes the class that implements the text editing cursor

#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<iostream>

#include "asset_manager.h"
#include "logger.h"

using std::string;
using std::ostream;

extern logger error_logger;
extern sdl_help* sdl_access;

//! \class cursor draws and keeps track of where the cursor is drawn
class cursor{
  public:

	//! cursor sets up the surface and texture for the cursor
	cursor();
	////! copy constructor ensures no double frees 
	//cursor(const cursor& other);

	//! clean up memory
	~cursor();

	//! this function sets up the cursor class
	void init(SDL_Rect* box_location_in);

	//! this function calculates the cursor's raw location and returns it
	/*! it also sets up the cursor_dest rect to later be drawn by draw_me */
	int calc_location(TTF_Font* font, const string& text,
						 const unsigned int& editing_location);

	//! this function draws the cursor the passed renderer
	void draw_me();

	//! draw the cursor with a modified location
	/*! used in text boxes that require scrolling interaction */
	void draw_me(const int x_change, const int y_change);

	//! this function moves the cursor to the left
	/*! this also decrements the editing_location variable
	 *\param text is a const reference to the text from the text box
	 *\param editing_location the editing_location to be modified
	 *\param changed the boolean telling the text box to redraw  */
	void left(const string& text,unsigned int& editing_location,bool& changed);
	//! this function moves the cursor to the left
	/*! this also decrements the editing_location variable
	 *\param text is a const reference to the text from the text box
	 *\param editing_location the editing_location to be modified
	 *\param changed the boolean telling the text box to redraw */
	void right(const string& text,unsigned int& editing_location,bool& changed);

	//! grab the cursor's location
	SDL_Rect& get_cursor_dest(){ return cursor_dest;}

  private:

	//! texture for this object
	SDL_Texture* my_texture;

	//!< pointer to text box's location info
	SDL_Rect* box_location;

	//! save where the cursor should be drawn
	/*! the height and width here will be figured out by using the size
	 *of the character that the cursor is hovering over */
	SDL_Rect cursor_dest;

};
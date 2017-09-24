//! \file text_box.h this file describes a struct that has the information necessary for text boxes
#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

#include<iostream>
#include<string>

#include "sdl_help.h"

struct text_box{

	//! this constructor can initialize the location of the text box, runs without params
	/*! it does so because I believe the default constructor can get called before the containing class
	 *has been properly set up, causing issues. So I've been writing init functions to fix that data
	 *once the containing class is set up */
	text_box(sdl_help* sdl_helper = NULL,TTF_Font* font_in=NULL,
		 std::string text_in = "",int xloc_in = 0, int yloc_in = 0,int width_in = 0,
		 int height_in = 0);

	//! copy constructor prevents double free() crashes when temporary text_boxes are pushed into vectors
	text_box(const text_box& other);

	//! the destructor frees the memory for the sdl surfaces and textures
	~text_box();

	//! this function sets up the location of the text box on the screen and the renderer
	/* this information should be passed in from whatever tile or button is instantiating the text box */
	void init(sdl_help* sdl_helper_in,TTF_Font* font_in,std::string text_in,int xloc_in,
		  int yloc_in,int width_in, int height_in);

	//##################### CLICK FUNCTIONS ############################################################
	bool was_clicked(SDL_Event& mouse_event);
	//##################################################################################################


	//! this function prints all of this structs information, usually for debugging
	void print_me();

	//! this function draws the text box to the screen
	void draw_me();

	//! this function draws the text editing cursor
	void draw_cursor();

	//! this function updates the SDL_Rect storage of the text boxes location, for rendering
	void make_rect();

	//! this function updates the text that is rendererd to the screen
	/*! it uses update_text_bounds_check to make sure that text can't go off the edge */
	void update_text(std::string& new_text);

	//! this function is a helper used for update_text to make sure user can't write off of the edge
	/*! it first creates a copy of the text box's text string, then inserts the proposed string into it.
	 * Then it checks the width of the text against the width of the box.
	 * \return true if text fits within the box's bounds, or false if it does not */
	bool update_text_bounds_check(std::string& new_text) const;

	//! update the texture when the text is changed
	void update_texture();

	//! this removes one character in the string if it is not empy
	void back_space();


	SDL_Rect my_rect;//!< location information stored in an SDL_Rect for rendering

	SDL_Color text_color;//!< contains the color of the text in the box

	int xloc;//!< the horizontal coordinate of the upper left corner
	int yloc;//!< the vertical coordinate of the upper left corner

	int width;//!< the width should be set by the init function or the constructor
	int height;//!< the height should be set by the init function or the constructor

	//! save the dimensions of the text in the button
	SDL_Rect text_dims;
	SDL_Surface* cursor_surface;//!< save the surface for the text editing surface
	SDL_Texture* cursor_texture;//!< save the texture for the text editing surface

	int editing_location;//!< saves the insertion and deletion point
	std::string text;//!< the text that is rendered to the screen and changed by the user


	sdl_help* sdl_helper;//!< pointer to the sdl_help class
	TTF_Font* sdl_help_font;//!< pointer to the font in the sdl_help class


	SDL_Surface* text_box_surface;//!< surface for the text box
	SDL_Texture* text_box_texture;//!< texture for the text box

	SDL_Surface* text_surface;//!< surface for the text
	SDL_Texture* text_texture;//!< texture for the tex


	int editing_index; //!< keep track of the insertion point for text

};










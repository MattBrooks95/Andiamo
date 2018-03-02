/*! \brief \file text_box.h this file describes a struct that
 *has the information necessary for text boxes */
#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

#include<iostream>
#include<string>

#include "sdl_help.h"
#include "cursor.h"
#include "asset_manager.h"

using std::string;
using std::regex;

extern sdl_help* sdl_access;

//! implements variables and logic for text boxes used by forms & buttons
/*! different from the text boxes used by the field objects in the manager
 *class unfortunately. Homogenizing them is a goal. */
struct text_box{

	//! constructor initializes the location of the text box
	/*! it does so because I believe the default constructor can get called
	 *before the containing class has been properly set up, causing issues.
	 *So I've been writing init functions to fix that data
	 *once the containing class is set up */
	text_box(TTF_Font* font_in=NULL, string text_in = "",int xloc_in = 0,
				int yloc_in = 0,int width_in = 0, int height_in = 0);

	//! prevents double free crashes when text_boxes are pushed into vectors
	text_box(const text_box& other);

	//! the destructor frees the memory for the sdl surfaces and textures
	~text_box();

	//! sets up the location of the text box on the screen and the renderer
	/* this information should be passed in from whatever tile or button
	 *is instantiating the text box */
	void init(TTF_Font* font_in,string text_in,int xloc_in,
			  int yloc_in,int width_in, int height_in);

	//############## CLICK FUNCTIONS ###########################################
	bool was_clicked(SDL_Event& mouse_event);
	//##########################################################################


	//! prints all of this structs information, usually for debugging
	void print_me();

	//! this function draws the text box to the screen
	void draw_me();

	//! updates the SDL_Rect storage of the text boxes location, for rendering
	void make_rect();

	//! this function updates the text that is rendererd to the screen
	/*! it uses update_text_bounds_check to make sure that text can't
	 *go off the edge */
	void update_text(string& new_text);

	//! update the texture when the text is changed
	void update_texture();

	//! this version of update_text does input checking
	/*! \param new_text is the test to be added
	 *\param test is the pattern which indicates good input */ 
	void update_text(string& new_text,const regex& test);

	/*! decides whether or not the input is bad,
	 *and changes the color of the text box accordingly */
	void check_text(const regex& test);

	//! this function turns the textbox red
	/*! this is usually to indicate that the input is not consistent with
	 *what is expected by the Fortran code */
	void toggle_red();

	//! this function calls cursor's right member
	void inc_cursor(bool& text_was_changed);

	//! this function calls cursor's left member
	void dec_cursor(bool& text_was_changed);

	//! this removes one character in the string if it is not empty
	/*! this deletion is made at the editing location, if the cursor
	 *is not at the very beginning of the string */
	void back_space();

	//! this version of back space tests the text string
	/*! \param the regular expression which indicates good input */
	void back_space(const regex& test);

	//! location information stored in an SDL_Rect for rendering
	SDL_Rect my_rect;

	//! contains the color of the text in the box
	SDL_Color text_color;

	//! the horizontal coordinate of the upper left corner
	int xloc;
	//! the vertical coordinate of the upper left corner
	int yloc;

	//! the width should be set by the init function or the constructor
	int width;

	//! the height should be set by the init function or the constructor
	int height;

	//! save the absolute dimensions of the text in the button
	SDL_Rect text_dims;
    //! keep track of the currently shown subsection of the text
    SDL_Rect shown_area;


	//! class that handles drawing the text editing cursor
	cursor my_cursor;

	//! keep track of where the insertion point is for text
	unsigned int editing_location;

	//! the text that is rendered to the screen and changed by the user
	string text;
	//! keep track of whether this box has been given bad input or not
	bool bad_input;

	TTF_Font* font;//!< pointer to the font in the sdl_help class

	SDL_Texture* text_box_texture;//!< texture for the text box

	SDL_Surface* text_surface;//!< surface for the text
	SDL_Texture* text_texture;//!< texture for the tex

	SDL_Texture* bad_texture;//!< texture for text box to indicate bad input

};










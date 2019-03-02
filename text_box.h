/*! \brief \file text_box.h this file describes a struct that
 *has the information necessary for text boxes */
#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

#include<iostream>
#include<string>
#include<regex>

//#include "sdl_help.h"
#include "cursor.h"
#include "asset_manager.h"

using std::string;
using std::regex;

extern sdl_help* sdl_access;

class field;

//! implements variables and logic for text boxes used by forms & buttons
class text_box{
	public:
		//! constructor initializes the location of the text box
		/*! it does so because I believe the default constructor can get called
		 *before the containing class has been properly set up, causing issues.
		 *So I've been writing init functions to fix that data
		 *once the containing class is set up */
		text_box(TTF_Font* font_in=NULL, string text_in = "",int xloc_in = 0,
					int yloc_in = 0,int width_in = 0, int height_in = 0);

		//! prevents double free crashes when text_boxes are pushed into vectors
		text_box(const text_box& other);

		// the destructor frees the memory for the sdl surfaces and textures
		// ~text_box();

		//! sets up the location of the text box on the screen and the renderer
		/* this information should be passed in from whatever tile or button
		 *is instantiating the text box */
		void init(TTF_Font* font_in,string text_in,int xloc_in,int yloc_in,int width_in, int height_in);

		//! set a pointer to a test pattern for this box's input
		/*! this will enable live input checking */
		void set_regular_expression(regex* regular_expression_object);

		//! sets pointers to the sdl class's scrolling values
		/*! this is another initialization step for text boxes
		 *being used in a context that allows scrolling */
		void set_scrolling();

		//############## CLICK FUNCTIONS ###########################################
		bool was_clicked(SDL_Event& mouse_event);
		//##########################################################################

		//! loop that modifies the text box's contents based on user input
		void edit_loop(SDL_Event& event,string& command);

		//! helper for edit_loop, processes keystrokes
		void edit_key_helper(SDL_Keysym& key,bool& text_was_changed,string& command);

		//! prints all of this structs information, usually for debugging
		void print_me();

		//! this function draws the text box to the screen
		void draw_me();

		//! this version of draw_me draws the text box modified by scrolling
		void draw_me(const int x_scroll, const int y_scroll);

		//! updates the SDL_Rect storage of the text boxes location, for rendering
		void make_rect();

		//! update the text of this text box
		/*! \param new_text is the test to be added
		 *\param test is a pointer to the regex pattern to use.
		 *NULL if not needed. */
		void update_text(const string& new_text);

		//! update the texture when the text is changed
		void update_texture();

		bool is_input_bad(){ return bad_input;}

		/*! decides whether or not the input is bad,
		 *and changes the color of the text box accordingly */
		void check_text();

		//! is used to swap the box background texture to red
		/*! this is enacted by the manager when this tile fails to convert the user's information
		 *with stoi or stod. It serves as an error indicator for the fields that failed the test. */
		void set_error_state();

		//! returns a tile to normal after it is given proper input and "Let's Go" has been clicked
		void cancel_error_state();

		//! this function calls cursor's right member
		void inc_cursor(bool& text_was_changed);

		//! this function calls cursor's left member
		void dec_cursor(bool& text_was_changed);

		//! this version of back space tests the text string
		/*! \param the regular expression which indicates good input */
		void back_space();

		string get_text() const {return text;}
		void set_text(const string& new_text){text = new_text;}

		//! the horizontal coordinate of the upper left corner
		int xloc;

		//! the vertical coordinate of the upper left corner
		int yloc;

	private:
		//! location information stored in an SDL_Rect for rendering
		SDL_Rect my_rect;

		//! contains the color of the text in the box
		SDL_Color text_color;

		//! the width should be set by the init function or the constructor
		int width;

		//! the height should be set by the init function or the constructor
		int height;

		//! pointer to current x scrolling value, if it exists
		int* x_scroll;

		//! pointer to current y scrolling value, if it exists
		int* y_scroll;

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

		regex* input_test_regex;

		//! keep track of whether this box has been given bad input or not
		/*! true: input is not properly formatted
		 * false: input is properly formatted*/
		bool bad_input;

		//! pointer to the font in the sdl_help class
		TTF_Font* font;

		SDL_Texture* text_box_texture;

		SDL_Surface* text_surface;
		SDL_Texture* text_texture;

		//! texture for text box to indicate bad input
		SDL_Texture* bad_texture;

};

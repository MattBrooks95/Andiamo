#pragma once
//! \file form.h contains the descriptions of the form and page classes

/*! \class form enables dynamic entry of parameters as a result of
 *other parameters enabling extra lines. Used by the form_buttons class */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <ostream>
#include <vector>
#include <string>

#include "button.h"
#include "text_box.h"
#include "c_tuples.h"
#include "asset_manager.h"
#include "colors.h"
#include "page.h"

#include "command.h"

using std::string;
using std::vector;
using std::regex;

class page;

//! class that is housed by a form_button, allows for dynamic parameter entry
/*! Forms are only used for the handful of parametes in the input manual
 *that enable extra features of HF, which typically require a significant
 *amount of extra input. Each form is a collection of page objects, and
 *the various functions that are useful to its implementation.
 *I've tried to create it to be as dynamic as possible,
 *though inheriting from this class to create even more
 *specialized functionality is possible */
class form{
	public:
		//! zeroes or NULLs the form's values
		form();
		//! gets rid of the form's textures, giving that memory back to the OS
		~form();

		//! sets up the form's variables
		/*!
		*\param form_title_in sets the title at the top of the form
		*\param help_msg_image_name finishes the asset's path to the desired PNG
		*\param xloc_in sets the horizontal coordinate of the corner of the form
		*\param yloc_in sets the vertical coordinate of the corner of the form */
		void init(string form_title_in,string help_msg_image_name, int xloc_in,
					int yloc_in, const vector<regex*>& pattern_tests);

		//! this function changes the form's title
		/*! this is useful, for example, when a form can serve one or
		*two logical functions but the required information is the same.
		*The ilv3/ilv5  form is one such case. Make sure to account for the
		*form title area's background color, as that color is needed to fill
		*over the old title, so the new one can be applied. */
		void set_form_title(string new_title);

		//! form_event_loop enacts an event loop similar to the one in main
		/* it is specifically tailored to be useful for form parameter entry,
		*and it is virtual so it could be inherited by narrow forms
		*to allow specialized functionality */
		virtual void form_event_loop(SDL_Event& big_event);

		//! processes clicks and enacts the according object's work functions
		void handle_click(SDL_Event& mouse_event,bool& done,bool& click_lock);

		/*! presents the form to the screen, which involves printing
		* its currently active page as well */
		void draw_me();

		//! toggle_active() is used to flip the active boolean
		/*! the active boolean tells the application when the form
		*should be visible & interactable, and when it should not be */
		void toggle_active();

		//! is_active() is a by-value getter for the active boolean
		bool is_active(){ return active;}

		//! next_page presents the next page to the user, if possible
		void next_page();

		//! prev_page presents the previous page to the user, if possible
		void prev_page();

		//! this function checks all of the inputs to make sure they are correct
		bool check_values(vector<index_value>& error_details);

		//! getter for the vector that stores the pages of text boxes
		vector<page>& get_pages() { return pages;}

		//! this funciton sets page_count to the passed value
		void set_page_count(int page_count_in);

		//! updates the sprites that represent which page the user is currently on
		/*! these are the numbers at the top right of the form, next to the page
		*left and right buttons */
		void update_page_indicator();

		/*! destroys the pages that were previously created, so that
		*they can be recreated */
		void flush_pages();

		//these two functions are used to know when a form needs smashed
		//and recreated because the user specified another size
		//! remember if this form has been initialized or not
		bool prev_initialized;

		//! keep track of the value this form was initialized with
		int prev_init_value;

		//! title of the form, which is displayed at the top of the form
		string form_title;

	private:
		//! save the title surface so it can be blitted to the form's surface
		SDL_Surface* form_title_surface;

		//! save the form's surface
		SDL_Surface* form_surface;

		//! save the form's texture
		SDL_Texture* form_texture;

		//! save the help page's texture
		SDL_Texture* help_texture;

		/*! sprite sheet with numbers 0-9 on it,
		 * so parts can be used to show page numbers */
		SDL_Surface* number_sprites;

		//! keep track of the form's dimensions and location
		SDL_Rect form_area;

		//! keep track of whether or not this form is active
		/*! active means that it should be drawn to the screen, and it should
		 *check mouse clicks upon itself. This being false allows the form's
		 *state to be saved, but not visible to the user or
		 *interactable until it is reopened. */
		bool active;

		//! stores of whether to show the page's texture or the help page's
		bool help_shown;

		//! stores the clickable location for the exit button in the top left
		active_area exit;

		//! stores the clickable location for the help arrow in the top left
		active_area help;

		//! stores the clickable location for the page right arrow in the top right
		active_area right_arrow;

		//! stores the clickable location for the page left arrow in the top right
		active_area left_arrow;

		//! stores a vector of page objects
		vector<page> pages;

		//! stores how many pages there are, so it can be displayed in the top right
		int page_count;

		//! keep track of what page the user is currently interacting with
		/*! this will be shown in the top right, and will change as the
		 *user clicks the right and left arrows */
		int current_page;

		//! store the regular expressions for real time error checking
		/*! text box indicates if an invalid input has been typed */
		vector<regex*> my_patterns;

		//! pointer to the font in use in the graphic's class
		TTF_Font* sdl_font;
};

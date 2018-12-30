//! \file sdl_help.h declares the sdl_help class and the win_size struct

#pragma once
#include<string>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

//provides constants for the maximum and minimum values
//c++ primitive data types for the current system & compiler
#include<limits.h>



#include "scroll_bar.h"
#include "manager.h"
#include "input_maker.h"
#include "asset_manager.h"
#include "system_wrapper.h"
#include "logger.h"

using std::string;
using std::vector;

extern logger* output_access;;
extern string HOME;
extern system_wrapper* system_access;

//the distance between two adjacent tiles
#define HORIZ_TILE_PADDING 5

#define APP_ICON "andy_icon.png"

//forward declaration,so sdl can be passed a button_manager pointer
//without needing to #include "button_manager.h",
//because doing so is a circular dependency
class button_manager;


//! this structure is used to store the dimensions of the window itself
/*! eventually the user will be able to change this to a variety of sizes */
struct win_size{

	//! win_size()'s constructor takes no arguments
	/*! it initializes the window width and height to -1, these must be set by
	 *sdl helper and later by the user */
	win_size();

	//! print() prints this fields width and height fields
	void print();

	//! holds the width of the window
	int width;

	//! holds the height of the window
	int height;
};

//! is used by calc_corners to remember the tiles it's currently considering
struct names_and_width{
	string line_name;
	string param_name;
	unsigned int width;
};
bool compare_width(names_and_width& left, names_and_width& right);

//! Contains sdl objects and members to act on them.
/*! sdl_help objects are mostly a container for sdl renderers and the window.
 *Functionality that requires SDL should go here, as the manager and field */
class sdl_help{
  public:
	//! This is a constructor member for the sdl_help class.
	/*! It initializes SDL and SDL image. Window size is based on a
	 *SDL_GetCurrentDisplayMode call. \param name_in is the desired name of
		 * the window.*/
	sdl_help(string name_in = "Andiamo!",
					string HF_input_file_in = "HF_config.txt",
					string bg_image_name_in = "hubble_deep_field.png");

	//! This is a destructor member for the sdl_help class.
	/*! It enacts SDL_Quit() and IMG_Quit(). */
	~sdl_help();

	//! this function inits parts of the sdl class
	/*! this must be called after the asset manager
	 *is set up */
	void init();

	//! updates the window dimension variables in sdl_help.h and manager.h
	/* \param width_in is the desired new window width
	 * \param height_in is the desired new window height */
	void window_update(int width_in, int height_in);

	//! this function does all the drawing
	void draw();

	//! show the labels for the lines of HF parameters
	void draw_labels();

	//! show the parameter line back drops
	void draw_guides();

	//! This member enacts the draw members of vert_bar and horiz_bar
	/* This should likely be called directly below every call to draw_tiles() */ 
	void draw_sbars();

	//! presents the renderer and all of it's current textures to the screen
	void present();

	////! uses SDL_SetWindowResizeable to prevent or allow window resizing
	//void toggle_resizable();

	/**************SCROLLING FUNCTIONS ****************************************/
	//! implements an algorithm that places tiles in a space-efficient way
	/*! parameter order is preserved, and is in line with the input manual */
	void calc_corners();

	//! called by calc_corners per line of parameters
	void calc_corners_helper(vector<field*>& line_in,unsigned int& start_height,
							 int row_limit);

	//! create the labels for the lines in the main parameter screen
	/*! pushes the label into the line_titles vector */
	void make_line_label(const string& label,unsigned int& start_height);
	
	//! called per calc_corners_helper run, to create parameter line backdrops
	void make_line_guide(SDL_Rect backdrop_dims);

	//! changes this class's x_scroll and y_scroll values
	/*! This is being called from handlers.cc most of the time.
	 *\param x_scroll_in amount to increase or decrease horizontal
	 *scroll value by.
	 *\param y_scroll_in amount to increase or decrease
	 *vertical scroll value by */
	void update_scroll(int x_scroll_in,int y_scroll_in);

	//! This member changes this class's x_scroll and y_scroll values back to 0
	/*!This pretty much returns the user back to the top of the page.
	 *I think I'll make this a spacebar action */
	void reset_scroll();

	//! returns the greatest and least values for x and y
	/*! it requires four integers by reference,
	 *which will be filled with the mininum and maximum tile location for
	 *each direction. helper for update_scroll().
	 *\param rightmost will be filled with the rightmost edge of the tile
	 *furthest to the right.
	 *\param leftmost when given 0, will be filled with the leftmost edge
	 *of the tile that is furthest to the left.
	 *\param upmost when given -2048, is filled with the top edge
	 *of the highest tile.
	 *\param downmost when given 0, is filled with the the most bottom edge of 
	 *the lowest tile */
	void most(int& rightmost,int& leftmost,int& upmost,int& downmost);

	//! calls the scroll bars's scroll_bar::clicked() function (click detection)
	/*! this doesn't do any logic besides return boolean values from the
	 *scroll bars's clicked member
	 *\param click_x is the xcoord of the mouse click
	 *\param click_y is the ycoord of the mouse click
	 *\return returns true if a scroll bar was clicked, and false elsewise */
	int scroll_clicked(int click_x, int click_y) const;

	/**************************************************************************/

	//! prints the three important size variables: area, window, and display
	/*! Where display is the dimensions of the physical monitor the user has.
	 *window_s keeps track of the size of the window in which things
	 *can be seen - this is usually less than the actual area.
	 *area is the total size needed to render all objects - this is usually
	 *bigger than the actual area. */
	void print_size_info();

	//! sees if the user clicked on a tile or not
	/*! it walks linearly through the tile vector and enacts the clicked() member
	 *of the tile that the user clicked.
	 *this toggles their normal appearance with their help_mode appearance
	 *\param click_x mouse click's x value (distance from left side of window)
	 *\param click_y mouse click's y value (distance from top of window) */
	void click_detection(SDL_Event& event, int click_x, int click_y);

	//! this is a boolean helper for click_detection()
	/* this member just takes in a mouse click's x or y values,
	 *and calculates whether or not the click falls within the given
	 *SDL_Rect (a square)
	 *\param click_x the mouse click's x position
	 *\param click_y the mouse click's y position
	 *\param rect is an SDL_Rect that contains the box to be checked,
	 *usually a tile's location */
	bool in(int click_x, int click_y,const SDL_Rect& rect) const;


	/**********GETTERS AND SETTERS*********************************/
	//! This member gets the current frame count
	unsigned long int get_frame_count(){ return frame_count; }

	//! This member returns a pointer to window_s field
	win_size* get_win_size();

	//! this member is a getter (by reference) for the horizontal scroll bar
	scroll_bar& get_h_bar(){ return horiz_bar; }
	//! this member is a getter (by reference) for the vertical scroll bar
	scroll_bar& get_v_bar(){ return vert_bar; }

	//! this member returns current horizontal scrolling value by value
	int get_xscroll(){return x_scroll;}

	//! returns a pointer to the x scrolling variable
	int* get_xscroll_ptr(){return &x_scroll;}

	//! this member returns current vertical scrolling value by value
	int get_yscroll(){return y_scroll;}

	//! returns a pointer to the y scrolling variable
	int* get_yscroll_ptr(){return &y_scroll;}

	//! this member serves as a getter for the SDL window
	SDL_Window* get_window(){ return window;}

	/*****************************************************************/

	//! contains a running total of how many times a frame has been drawn
	/*! as of right now this is just paying lip service to worrying about
	 *framerate a bunch of decisions still have to be made in that regard. */
	unsigned long int frame_count;

	//! pointer to the renderer object
	SDL_Renderer* renderer;

	//! pointer to the font for the line labels
	TTF_Font* label_font;

	//! pointer to the font created from the font.ttf file in /config
	TTF_Font* font;

	//! store whether or not to show the line backdrops
	bool show_line_guides;

  private:
	//! A string that contains the window name, usually Andiamo."
	string window_name;

	//! a path string to the algorithm's input file folder 
	string hf_input_p;

	//! a path string to the font folder
	string font_p;

	//! name of the background image to use
	string bg_image_name;

	//! stores the background image
	SDL_Texture* bg_texture;

	//! graphic info for 'title cards' that have line names on them
	vector<LINE_TITLE> line_titles;

	/*! \brief graphic info for 'backdrops' to illustrate which
	 *parameters belong to which lines */
	vector<LINE_GUIDE> line_guides;

	/************* FIELDS THAT PERTAIN TO SCROLLING ***************************/
	/*! \brief contains functions to act on, and draw, the vertical
	 *scroll bar */
	scroll_bar vert_bar;

	/*! \brief similarly contains functions act on, and draw, the
	 *horizontal scrollbar */
	scroll_bar horiz_bar;

	//! this integer controls how far up or down we've scrolled
	int y_scroll;

	//! \brief this integer controls how far right or left we've scrolled
	int x_scroll;

	/**************************************************************************/

	/*! a win_size struct that contains the window's current width and 
	 *height, this window limits what the user can see */
	win_size window_s;

	//! contains screen information
	SDL_DisplayMode display;

	//! queue that updates with user input
	SDL_Event big_event;

	//! pointer to the window object
	SDL_Window* window;
};

//################### non member helpers ########################################//
//! \file sdl_help.h declares the sdl_help class and the win_size struct

#pragma once
#include<string>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

#include "scroll_bar.h"
#include "manager.h"
#include "input_maker.h"

#include "logger.h"
extern logger error_logger;

class button_manager; //forward declaration,so sdl can be passed a button_manager pointer without needing
		      //to #include "button_manager.h", because doing so is a circular dependency


//! this structure is used to store the dimensions of the window itself
/*! eventually the user will be able to change this to a variety of sizes */
struct win_size{
	//! win_size()'s constructor takes no arguments
	/*! it initializes the window width and height to -1, these must be set by sdl helper and later by
	 *the user */
	win_size();
	//! print() print() prints this fields width and height fields
	void print();

	int width; //! < holds the width of the window, which is different than the screen info in display
	int height; //! < holds the height of the window, which is different than the info in display
};
//! the following struct is used by calc_corners to remember the tiles it's currently considering
struct names_and_width{
	std::string line_name;
	std::string param_name;
	unsigned int width;
};
bool compare_width(names_and_width& left, names_and_width& right);

//! Contains sdl objects and members to act on them.
/*! sdl_help objects are mostly a container for sdl renderers and the window. Functionality that requires 
 *SDL should go here, as the manager and field classes do not have access to them as of right now
 */
class sdl_help{
  public:
	//! This is a constructor member for the sdl_help class.
	/*! It initializes SDL and SDL image. Window size is based on a
	 *SDL_GetCurrentDisplayMode call. \param name_in is the desired name of
         * the window.*/
	sdl_help(std::string name_in = "Andiamo!",std::string HF_input_file_in = "HF_config.txt",std::string bg_image_name_in = "hubble_deep_field.png");

	//! This is a destructor member for the sdl_help class.
	/*! It enacts SDL_Quit() and IMG_Quit(). */
	~sdl_help();
	
	//! This member updates the window dimension variables in sdl_help.h and manager.h
	/* \param width_in is the desired new window width
         * \param height_in is the desired new window height */
	void window_update(int width_in, int height_in);

	//! this member ensures that the manager class can access all of the input manager
	/*! this is so that the fields and the input maker can interact with eachother
	 *relatively simply. I'm sure there's a better solution than this, so look at improving
	 *this later */
	void give_manager_io(input_maker* input_maker_hook);

	//! This member presents the renderer and all of it's current textures to the screen
	void present();

	//! This member traverses manager's vector and calls the field's draw_me function
	/*! it ensures that help boxes are always drawn above the tiles not in help mode
	 * but they will sometimes conflict with eachother and overlap */
	void draw_tiles();
	//! This member enacts the draw members of vert_bar and horiz_bar
	/* This should likely be called directly below every call to draw_tiles() */ 
	void draw_sbars();

	/**************************SCROLLING FUNCTIONS ************************************************/
	//! calc_corners implements a guessing algorithm that tries to place tiles in a space-efficient way
	void calc_corners();

	//! this function is a helper function for calc_corners(). It places the tiles for one line
	/*\param line_in is the name of the line that the function is doing
	 *\param map_in is the name of the map of the line it is making, that contains the fields
	 *\param start_height is the height at which the tile should start placing fields.
	 *This is what will allow the lines to not overlap each other. Before this function exits,
	 *it should update the start_height value to account for the then completed field placement. */
	void calc_corners_helper(const std::string line_in, std::map<std::string,field>& map_in,
				 unsigned int& start_height,int row_limit);

	//! This member changes this class's x_scroll and y_scroll values to the given parameters
	/*! This is being called from the handlers.cc implementations most of the time.
         * \param x_scroll_in amount to increase(positive) or decrease(negative) horizontal
         *scroll value by. \param y_scroll_in amount to increase or decrease vertical scroll value by*/
	void update_scroll(int x_scroll_in,int y_scroll_in);

	//! This member changes this class's x_scroll and y_scroll values back to 0
	/*!This pretty much returns the user back to the top of the page. I think I'll make this
	 *a spacebar action */
	void reset_scroll();

	//! this member traverses the tile_loc vector and returns the greatest and least values for x and y
	/*! it requires four integers by reference, which will be filled with the mininum and maximum
	 *tile location for each cardinal direction. For now, it is just a helper for update_scroll()
         *\param rightmost when given -2048, will be filled with
         *the rightmost edge of the tile furthest to the right. \param leftmost when given 0, will 
         *similarly be filled with the leftmost edge of the tile that is furthest to the left.
         *\param upmost when given -2048, is filled with the top edge of the highest tile.
         *\param downmost when given 0, is similarly filled with the the most bottom edge of 
         *the lowest tile */
	void most(int& rightmost,int& leftmost,int& upmost,int& downmost);

	//! this member calls the scroll bars's scroll_bar::clicked() function (click detection)
	/*! this doesn't do any logic besides return boolean values from the scroll bars's clicked member
	 *\param click_x is the xcoord of the mouse click
	 *\param click_y is the ycoord of the mouse click
	 *\return returns true if a scroll bar was clicked, and false elsewise */
	int scroll_clicked(int click_x, int click_y) const;
	/**********************************************************************************************/

	//! This member prints the sizes of the three important size variables: area, window, and display
	/*! Where display is the dimensions of the physical monitor the user has. window_s keeps track of
	 *the size of the window in which things can be seen - this is usually less than the actual area.
	 *area is the total size needed to render all objects - this is usually bigger than the actual area.
	 *Scrolling will be implemented to allow the user to see different sections of whole area using the
	 *limited window. */
	void print_size_info();


	//! This member traverses the tile_locations vector and prints all of their SDL_Rect fields
	/*! sdl_help has this functionality right now because it's using SDL_Rect structures. I may
	 *make the manager do this book keeping, but I'd have to make my own struct  \param outs is the 
         *desired output stream. Likely to be cout in development, but may later be a file to support error 
         *logging features*/
	void print_tile_locs(std::ostream& outs);


	//! This member traverses the tile location vector and sees if the user clicked on a tile or not
	/*! it walks linearly through the tile_locations vector and enacts the clicked() member
	 *of the tile that the user clicked on. For non-background tiles, this toggles their normal appearance
	 *with their help_mode appearance
	 *\param outs output stream to send messages to
	 *\param click_x mouse click's x value (distance horizontaly from left side of window)
	 *\param click_y mouse click's y value (distance vertically from top of window) */
	void click_detection(std::ostream& outs, SDL_Event& event,button_manager* b_manager, int click_x, int click_y);

	//! this function handles the mini-loop where the user can edit the text box
	/*! should turn off keybindings so keystrokes are interpreted as input to the respective fields
	 *temporary storage string. This member started off being in class field, but I've since moved it to
	 *sdl_help, because sdl_help needs to be able to draw in this loop */
	void text_box_mini_loop(std::ostream& outs, SDL_Event& event,button_manager* b_manager, field& current_tile);

	//! this function helps text_box_mini_loop by doing the functions related to the text input event
	void text_box_mini_loop_helper(SDL_Keysym& key,field& current_tile,bool& text_was_changed);

	//! this is a boolean helper for click_detection()
	/* this member just takes in a mouse click's x or y values, and calculates whether or not
         *the click falls within the given SDL_Rect (a square)
	 *\param click_x the mouse click's x position
	 *\param click_y the mouse click's y position
	 *\param rect is an SDL_Rect that contains the box to be checked, usually a tile's location*/
	bool in(int click_x, int click_y,const SDL_Rect& rect) const;

	//! This member merely calls the destructor at the programmer's discretion
	void quit();

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

	//! this member returns current vertical scrolling value by value
	int get_yscroll(){return y_scroll;}


	//!this member is a const getter for the tile_locations vector
	//const std::vector<SDL_Rect>& get_locations() const { return tile_locations;}
	//!this member is non-const getter for the tile_locations vector
	//std::vector<SDL_Rect>& get_locations(){ return tile_locations;}

	//! this member provides by-reference access to the input_maker object
	input_maker& get_io_handler() { return io_handler; }


        //! This member is a const getter for the tile/card manager
	/*! \return tile_bag is a const reference to the tile_bag field. It is accessed
	 * some_object.get_mgr().tile_bag_member(). Tile bag may not be changed*/
	const manager& get_const_mgr() const{ return tile_bag;}

	//! This member is a non-const getter for the tile/card manager
	/*! \return tile_bag is a const reference to the tile_bag field. It is accessed
	 * some_object.get_mgr().tile_bag_member(). Tile bag may be changed. Be careful. */
	manager& get_mgr(){ return tile_bag;}

	//!this member returns a copy of the tile_bag
	/*! this is useful when an algorithm would require removing objects from the vector as they are
	 * processed  */
	manager get_mgr_copy(){ return tile_bag;}
	/*****************************************************************/

	//! contains a running total of how many times a frame has been drawn
	/*! as of right now this is just paying lip service to worrying about framerate
	 *a bunch of decisions still have to be made in that regard. */
	unsigned long int frame_count;

	SDL_Renderer* renderer; //!< pointer to the renderer object
	TTF_Font* font;//!< pointer to the font created from the font.ttf file in /config

	/********* FRIENDS *******************************************/
	//I have no friends

  private:
	std::string window_name; //!<  \brief A string that contains the window name, usually Andiamo."
	std::string image_p; //!<  \brief a string that points to the resource image directory 
	std::string hf_input_p; //!< \brief a path string to the algorithm's input file folder 
	std::string font_p;//!< \brief a path string to the font folder

	std::string bg_image_name;
	SDL_Surface* bg_surface;
	SDL_Texture* bg_texture;


	/***************** FIELDS THAT PERTAIN TO SCROLLING ********************************/
	scroll_bar vert_bar;/*!< \brief contains functions to act on, and draw, the vertical
			     *scroll bar */
	scroll_bar horiz_bar;/*!< \brief similarly contains functions act on, and draw, the
			      *horizontal scrollbar */

	int y_scroll;//!< \brief this integer controls how far up or down we've scrolled
	int x_scroll;//!< \brief this integer controls how far right or left we've scrolled

	/************************************************************************************/

	//win_size area;/*!< \brief a win_size struct that contains the dimensions for the entire area,
	//	       *not just what is seen */
	win_size window_s; /*!< \brief a win_size struct that contains the window's current width and 
			    *height, this window limits what the user can see */

	SDL_DisplayMode display; //!< contains screen information
	SDL_Event big_event; //!< queue that updates with user input

	SDL_Window* window; //!< pointer to the window object

	//################## MANAGER OBJECT #####################################################
	manager tile_bag; //!< manager object that contains all the field objects
	//#######################################################################################

	//################## INPUT MAKER OBJECT #################################################
	input_maker io_handler; //!< object that manages the config and output files for HF relevant stuff
	//#######################################################################################
};

//################### non member helpers ########################################//

























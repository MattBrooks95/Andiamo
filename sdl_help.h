//! \file sdl_help.h declares the sdl_help class and the win_size struct

#pragma once
#include<string>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#include "scroll_bar.h"
#include "manager.h"
//! this structure is used to store the dimensions of the window itself
/*! eventually the user will be able to change this to a variety of sizes */
struct win_size{
	//! win_size()'s constructor takes no arguments
	/*! it initializes the window width and height to -1, these must be set by sdl helper and later by
	 *the user */
	win_size();
	//! print() print() prints this fields width and height fields, and a newline
	void print(std::ostream& outs);

	int width; //! < holds the width of the window, which is different than the screen info in display
	int height; //! < holds the height of the window, which is different than the info in display
};



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
	sdl_help(std::string name_in = "Andiamo!");

	//!this overloaded constructor requires 3 arguments: window name, width and height
	
	sdl_help(std::string name_in, int width, int height);

	//! This is a destructor member for the sdl_help class.
	/*! It enacts SDL_Quit() and IMG_Quit(). */
	~sdl_help();
	
	//! This member updates the window dimension variables in sdl_help.h and manager.h
	/* \param width_in is the desired new window width
         * \param height_in is the desired new window height */
	void window_update(int width_in, int height_in);

	//! This member presents the renderer and all of it's current textures to the screen
	void present();

	//! This member traverses manager's vector and loads all of the tile's SDL Surfaces
	/*! For now, this function is assuming that only two tile cards are going to happen per row
	 * and that the tiles are the same size. It will be improved to allow for different tile sizes
	 *using some sort of greedy algorithm to make sure they don't conflict with each other */
	void draw_tiles();
	//! This member enacts the draw members of vert_bar and horiz_bar
	/* This should likely be called directly below every call to draw_tiles() */ 
	void draw_sbars();
	/**************************SCROLLING FUNCTIONS ************************************************/

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

	/**********************************************************************************************/
	//! This member prints the sizes of the three important size variables: area, window, and display
	/*! Where display is the dimensions of the physical monitor the user has. window_s keeps track of
	 *the size of the window in which things can be seen - this is usually less than the actual area.
	 *area is the total size needed to render all objects - this is usually bigger than the actual area.
	 *Scrolling will be implemented to allow the user to see different sections of whole area using the
	 *limited window. \param outs is the desired output stream. Likely to be cout in development,
         *but may later be a file to support error logging features*/
	void print_size_info(std::ostream& outs);


	//! This member traverses the tile_locations vector and prints all of their SDL_Rect fields
	/*! sdl_help has this functionality right now because it's using SDL_Rect structures. I may
	 *make the manager do this book keeping, but I'd have to make my own struct  \param outs is the 
         *desired output stream. Likely to be cout in development, but may later be a file to support error 
         *logging features*/
	void print_tile_locs(std::ostream& outs);

	//! this member calls the scroll bars's scroll_bar::clicked() function (click detection)
	/*! this doesn't do any logic besides return boolean values from the scroll bars's clicked member
	 *\param outs is the stream that messages should be sent to
	 *\param click_x is the xcoord of the mouse click
	 *\param click_y is the ycoord of the mouse click
	 *\return returns true if a scroll bar was clicked, and false elsewise */
	bool scroll_clicked(std::ostream& outs, int click_x, int click_y) const;

	//! This member traverses the tile location vector and sees if the user clicked on a tile or not
	/*! it walks linearly through the tile_locations vector and enacts the clicked() member
	 *of the tile that the user clicked on. For now this means cute cout statements
	 *\param outs output stream to send messages to
	 *\param click_x mouse click's x value (distance horizontaly from left side of window)
	 *\param click_y mouse click's y value (distance vertically from top of window) */
	void click_detection(std::ostream& outs, int click_x, int click_y) const;

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
	//! This member returns a pointer to window_s field
	win_size* get_win_size();

	//! this member is a getter (by reference) for the horizontal scroll bar
	scroll_bar& get_h_bar(){ return horiz_bar; }
	//! this member is a getter (by reference) for the vertical scroll bar
	scroll_bar& get_v_bar(){ return vert_bar; }

	//!this member is a const getter for the tile_locations vector
	const std::vector<SDL_Rect>& get_locations() const { return tile_locations;}
	//!this member is non-const getter for the tile_locations vector
	std::vector<SDL_Rect>& get_locations(){ return tile_locations;}
        //! This member is a const getter for the tile/card manager
	/*! \return tile_bag is a const reference to the tile_bag field. It is accessed
	 * some_object.get_mgr().tile_bag_member(). Tile bag may not be changed*/
	const manager& get_mgr() const{ return tile_bag;}
	//! This member is a non-const getter for the tile/card manager
	/*! \return tile_bag is a const reference to the tile_bag field. It is accessed
	 * some_object.get_mgr().tile_bag_member(). Tile bag may be changed. Be careful. */
	manager& get_mgr(){ return tile_bag;}

	SDL_Renderer* renderer; //!< pointer to the renderer object

	/********* FRIENDS *******************************************/
	//I have no friends

  private:
	std::string window_name; //!<  \brief A string that contains the window name, usually Andiamo."
	std::string image_p; //!<  \brief a string that points to the resource image directory 
	std::string hf_input_p; //!< \brief a path string to the algorithm's input file folder 

	//! allows sdl_help to keep track of where tiles are
	/* the SDL_Rect's indices in this vector should line up with manager's tiles vector so 
	 *the members of the correct field can be invoked */
	std::vector<SDL_Rect> tile_locations;
	/***************** FIELDS THAT PERTAIN TO SCROLLING ********************************/
	scroll_bar vert_bar;
	scroll_bar horiz_bar;

	int y_scroll;//!< \brief this integer controls how far up or down we've scrolled
	int x_scroll;//!< \brief this integer controls how far right or left we've scrolled

	/************************************************************************************/

	bool area_size_set; /*!< \brief false on the onset of the program, set to true after draw_tiles() has
			     *been invoked once
			     *
			     * this logic will need to be changed if we allow tile loading at run time */ 
	win_size area;/*!< \brief a win_size struct that contains the dimensions for the entire area,
		       *not just what is seen */
	win_size window_s; /*!< \brief a win_size struct that contains the window's current width and 
			    *height, this window limits what the user can see */

	SDL_DisplayMode display; //!< contains screen information
	SDL_Event big_event; //!< queue that updates with user input

	SDL_Window* window; //!< pointer to the window object


	manager tile_bag; //!< manager object that contains all the field objects
};




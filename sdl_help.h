//! \file sdl_help.h declares the sdl_help class and the win_size struct

#pragma once
#include<string>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>


#include "manager.h"
//! this structure is used to store the dimensions of the window itself
/*! eventually the user will be able to change this to a variety of sizes */
struct win_size{
	//! win_size()'s constructor takes no arguments
	/*! it initializes the window width and height to -1, these must be set by sdl helper and later by
	 *the user */
	win_size();

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
	void window_update(int width_in, int height_in);

	//! This member presents the renderer and all of it's current textures to the screen
	void present();

	//! This member traverses manager's vector and loads all of the SDL Surfaces
	/*! For now, this function is assuming that only two tile cards are going to happen per row
	 * and that the tiles are the same size. It will be improved to allow for different tile sizes
	 *using some sort of greedy algorithm to make sure they don't conflict with each other */
	void draw_all();

	//! This member merely calls the destructor at the programmer's discretion
	void quit();

	/**********GETTERS AND SETTERS*********************************/

        //! This member is a const getter for the tile/card manager
	const manager& get_mgr() const{ return tile_bag;}
	//! This member is a non-const getter for the tile/card manager
	manager& get_mgr(){ return tile_bag;}

  private:
	std::string window_name; //!<  \brief A string that contains the window name, usually Andiamo."
	std::string image_p; //!<  \brief a string that points to the resource image directory 
	std::string hf_input_p; //!< \brief a path string to the algorithm's input file folder 

	win_size window_s;

	SDL_DisplayMode display; //!< contains screen information
	SDL_Event big_event; //!< queue that updates with user input

	SDL_Window* window; //!< pointer to the window object
	SDL_Renderer* renderer; //!< pointer to the renderer object

	manager tile_bag; //!< manager object that contains all the field objects
};


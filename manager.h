//! \file manager.h declares the manager class, which manipulates and stores tiles

#pragma once
#include<vector>
#include "field.h"

//! The manager is a vector+ object that handles a standard vector that contains input tiles or 'cards'
class manager{
  public:
	//! this is the constructor for the manager class, which implements some features on top of a std vector
	/*! the manager constructor doesn't do anything right now, as set up must occur after sdl_help's
	 *constructor */
	manager();
	//! destructor is left empty for now. I believe standard vectors kill themselves properly, as should these fields
	~manager();

	//! This member loads a given tile into the vector
	/*! It doesn't do anything special right now, but later it may have to
	 *implement some other book keeping, so it gets its own function 
         *\param temp is the temporary tile that is passed by value, and shoved into the tiles vector */
	void new_tile(field temp);

	/*! this init member uses fstream and regex to open and process a text file, which for now defaults
	 * to tiles.txt, in the folder tile_Input, so that information on what parameter tiles/cards need
	 *can be loaded into the program, and new tiles can be loaded without rebuilding - this work
	 *is not done in the constructor because it needs information from the sdl class, whose 
	 *constructor is called after manager's apparently*/
	void init();


	//! This runs through the vector and enacts each of the field element's print members
	/*! \param outs is the output stream that the info should be sent to */
	void print_all(std::ostream& outs);

	//! This member updates the window's dimension values win_w and win_h
	/*! This should usually only be called from a member of the sdl_help class,
	 *probably whenever the user resizes the screen, and at startup of Andiamo 
	 *\param width_in is the desired new width
	 *\param height_in is the desired new height */
	void update_win(int width_in, int height_in);

	std::vector<field> tiles;//!< vector if tile information is public for easier access from sdl_help

  private:
	std::string tile_input_p; //!< \brief a path string to the tile input file folder
	int win_w;//!< \brief keeps track of sdl window width
	int win_h;//!< \brief keeps track of sdl window height
};

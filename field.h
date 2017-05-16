//! \file field.h declares the field class, which is like a tile where users can enter parameters or use defaults

#pragma once
#include<string>
#include<iostream>
//! tile_size is a struct that contains height and width parameters for the main window
/*! These should be a fraction of the size of the window, to allow for many input cards on one screen */
struct tile_size{
	//! initialize tile size struct to -1x-1, if it isn't set this indicates bad input
	tile_size(){
		width = -1;
		height = -1;
	}

	//! this print member just prints out this tile's dimensions with no newline
	void print(std::ostream& outs){
		outs << "Tile width: " << width << " Tile height: " << height;

	}
	int width; /*!< \brief The width will be the width of an input section in the program
		    *It should be a fraction of the window size, so multiple input fields can fit
		    *in the same row. */
	int height; /*!< \brief The height will be the height of an input section in the program
		    *It should be a fraction of the window size, so multiple input fields can fit
                    *in the window. This should mean less scrolling down for the user.*/

};

//!the field class allows implementation of boxes/tiles/cards that are shown on the screen
class field{
  public:

	//! this constructor requires 2 more arguments: tile height and width
	field(std::string tile_name_in,std::string image_name_in, int width, int height);
	~field();
	
	//! this member prints a message if the user clicks on this tile
	void clicked(std::ostream& outs) const;

	//! this void member prints the field's info to a given stream
	void print(std::ostream& outs);

	tile_size get_size() { return size;}//!< getter for the private tile_size field

	std::string get_img_name(){ return image_name;}//!< getter for private image name field

	std::string tile_name; //!< the tile_name should only serve a contextual purpose like (isospin)

  private:
	std::string image_name; //!< the name of the image, so it can be found in Assets/Images

	tile_size size; //!< tile_size struct containing the dimensions for this particular card/tile/block
};

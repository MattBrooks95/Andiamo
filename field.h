//! \file field.h declares the field class, which is like a tile where users can enter parameters or use defaults

#pragma once
#include<string>
#include<vector>
#include<iostream>

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

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

	//! this the constructor that needs to be used
	/*! all of these fields should be filled by manager's init() function
	 *\param tile_name is a contextual title that will be displayed on the tile
	 *\param image_name_in is the name of the actual image used. This must be correct.
	 *\param width is the width of the image supplied for the tile (and the tile width)
	 *\param height is the height of the image supplied for the title (and the tile height) */
	field(std::string tile_name_in,std::string image_name_in, int width, int height);

	//! fields should save their render information to save time and readability in main loop and drawing
	/*! this is not done, and will likely change the structure of the program. This function now also allows
	 *field objects access to the font information through sdl_font* */
	void graphics_init(SDL_Renderer* sdl_help_renderer_in,std::string image_p_in,
			   int* xscroll_in,int* yscroll_in,TTF_Font* font_in);
	//! This is a function, called from graphics_init(), that sets up the text's surface
	/*! it should contain a logic that makes sure that the text does not run off the tile's size
	 *and it also should not overrite any text boxes in the tile, which have not yet been implemented */
	void text_init();



	//! this function is used to change this tile object's size when normal logic can't be followed
	/*! right now I believe the only case is when setting the background tile's size, because calc_corners()
	 * doesn't processes the 0th (background) tile*/
	void force_size(int width_in,int height_in);


	//! this will draw this field object's texture to the screen
	/*! this works using its known corner values offset by the current scrolling values, and
	 *the texture/surface/renderer information given to it by sdl_help's constructor calling calc_corners and
	 * give_fields_render
	 */
	void draw_me();


	//! this the destructor for the field/tile object - does nothing
	~field();
	
	//! this member prints a message if the user clicks on this tile
	/*!
	 *\param outs is the output stream that the info (if any) should be sent to */
	void clicked(std::ostream& outs) const;


	//! this void member prints the field's info to a given stream
	/*!
	 *\param outs is the output stream that the info (if any) should be sent to */
	void print(std::ostream& outs);

	//! this member returns this tiles corner location, and dimensions as an sdl rect
	/*! it's definitely weird to have them stored separately after I've decided to give field
	 * access to SDL stuff, but I'm going to leave it this way for now. Note that this
	 * does account for scrolling*/
	SDL_Rect get_rect() const;


	tile_size get_size() { return size;}//!< getter for the private tile_size field

	std::string get_img_name(){ return image_name;}//!< getter for private image name field

	std::string tile_name; //!< the tile_name should only serve a contextual purpose like (isospin)
	std::vector<std::string> descriptions; //!< input description

	int xloc; //!< the field keeps track of the xcoordinate of its upper right corner
	int yloc; //!< the field keeps track of the ycoordinate of its upper right corner

  private:
	int* sdl_xscroll;//!< this pointer allows field objects access to current x scrolling value in sdl_help
	int* sdl_yscroll;//!< this pointer allows field objects access to current y scrolling value in sdl_help
	TTF_Font* sdl_font;//!< this pointer allows field objects access to the font setting file
	SDL_Renderer* sdl_help_renderer;//!< a pointer to sdl_help's rendering context

	SDL_Surface* my_text_surf;//!< saves the surface for the text, so that it isn't recreated every frame
	SDL_Texture* my_text_tex;//!< saves the texture for the text, so that it isn't recreated every frame


	SDL_Surface* my_surf;//!< saves the surface, so that it isn't recreated every frame
	SDL_Texture* my_tex;//!< saves the texture, so that it isn't recreated every frame


	std::string image_name; //!< the name of the image, so it can be found in Assets/Images

	tile_size size; //!< tile_size struct containing the dimensions for this particular card/tile/block
};









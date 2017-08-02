//! \file manager.h declares the manager class, which manipulates and stores tiles

#pragma once
#include<vector>
#include<map>


#include "field.h"
#include "input_maker.h"

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

//! The manager is a vector+ object that handles a standard vector that contains input tiles or 'cards'
class manager{
  public:
	//! this is the constructor for the manager class, which implements some features on top of a std vector
	/*! the manager constructor doesn't do anything right now, as set up must occur after sdl_help's
	 *constructor */
	manager();

	//! destructor is left empty for now.
	~manager();

	//! This member loads a map of parameters into the map of lines
	/*! this way, when the parameters are output to the HF file,
	 *the desired line can be referenced by its name, like "5A", and then the parameter can be referenced by its name
	 *like "IENCH".
	 *\param line_name is the name of the map that will be put into the map of lines
	 *\param line_map is a map of the parameter's name, and their corresponding field*/
	void new_line(const std::string& line_name,const std::map<std::string,field>& line_map);

	//! this member is called by sdl_help's constructor, and sets up input_maker object pointer
	void set_input_maker_hook(input_maker* input_maker_hook_in);

	//! this function traverses the tile bag and sets up each object with a reference to the main renderer
	/*! it also allows fields to access sdl_help's scroll values and font pointer */
	void give_fields_renderer(SDL_Renderer* sdl_help_renderer_in,std::string image_p_in,
				  int* xscroll_in, int* yscroll_in,TTF_Font* font_in);
	//! this function walks the map, and returns the width of the widest tile
	int get_widest_tile_width();

	//! this function "fills" each non-background tile with it's default value as a string
	/* it also gives them a pointer reference to their position in input_maker's vector
	 *to save time when we have to update input_maker's values. For this reason input_maker
	 *shouldn't resize it's vector besides in the init function, because that could cause illegal reads
	 *because the field's pointer points to the vector's old location. */
	void give_fields_defaults();

	//############# GIVE_FIELDS_DEFAULTS() HELPERS ########################################################//
	void give_int4_fields_defaults();
	void give_int4_array_fields_defaults();

	void give_real8_fields_defaults();

	void give_string_fields_defaults();

	void give_r8_array_fields_defaults();

	//####################################################################################################//

	//! this function updates input_maker's vectors with the field's new values (from user)
	/*\return true means that there was no errors in string->data conversion
	 *\return false means that an stoi/stod function failed, and the file wasn't created */
	bool update_io_maker(std::vector<std::string>& bad_input_list);

	//! this member loads in tiles from the tile input file using regular expressions and file i/o
	/*! this init member uses fstream and regex to open and process a text file, which for now defaults
	 * to tiles.txt, in the folder tile_Input, so that information on what parameter tiles/cards need
	 *can be loaded into the program, and new tiles can be loaded without rebuilding - this work
	 *is not done in the constructor because it needs information from the sdl class, whose 
	 *constructor is called after manager's apparently. It also sorts the tiles by descending width for
	 *sdl_help's draw_tiles() function*/
	void init();

	//! this function runs through the 2d map, and prints all of the lines and their fields
	/* \param outs is the output stream that will be printed to */
	void print_all(std::ostream& outs);

	//! This member updates the window's dimension values win_w and win_h
	/*! This should usually only be called from a member of the sdl_help class,
	 *probably whenever the user resizes the screen, and at startup of Andiamo 
	 *\param width_in is the desired new width
	 *\param height_in is the desired new height */
	void update_win(int width_in, int height_in);

	std::vector<std::string> line_order;//!< save the order in which the lines occur, maps don't do it automatically
	std::map<std::string,std::map<std::string,field>> fields;//!< trying something new, to keep relevant tiles together
	//std::vector<field> tiles;//!< vector of tile information is public for easier access from sdl_help

  private:
	input_maker* input_maker_hook; //!< allows manager access to sdl_help's input_maker object

	std::string tile_input_p; //!< \brief a path string to the tile input file folder
	int win_w;//!< \brief keeps track of sdl window width
	int win_h;//!< \brief keeps track of sdl window height
};

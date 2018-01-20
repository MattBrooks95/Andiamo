//! \file field.h declares the field class, which is like a tile where users can enter parameters or use defaults

#pragma once
#include<string>
#include<vector>
#include<iostream>

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

#include "ftran_structs.h"
#include "string+.h"

#include "logger.h"
extern logger error_logger;

using std::string;
using std::vector;


//! this is a handy bag for the textures and surfaces necessary for text box creation. To be used in a field object
struct sdl_text_box{
	//! the constructor sets all pointers to NULL, it seems like it's the safe thing to do
	sdl_text_box();

	//! the destructor frees all of the memory
	~sdl_text_box();

	//! stores the tile_box's y offset from the top left corner of the tile on which it resides
	int y_offset; 

	SDL_Color text_color;//! keep track of the text color

	SDL_Texture* box_tex;  //!< keep track of the texture for the (usually) white background

	SDL_Surface* text_surf; //!< keep track of the surface for the current text in the text box
	SDL_Texture* text_tex;  //!< keep track of the texture for the current text in the text box

	SDL_Texture* cursor_texture;//!< save the texture for text entry indicator

};

//! tile_size is a struct that contains height and width parameters for the main window
/*! These should be a fraction of the size of the window, to allow for many input cards on one screen */
struct tile_size{
	//! initialize tile size struct to -1x-1, if it isn't set this indicates bad input
	tile_size(){
		width = -1;
		height = -1;
	}

	//! this print member just prints out this tile's dimensions with no newline
	void print(){
		error_logger.push_msg("Tile width: "+std::to_string(width)+" Tile height: "+std::to_string(height));

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
	field(string tile_name_in,string display_name_in,string image_name_in, int width, int height);

	//! copy constructor ensures that memory is not lost or double free'd
	field(const field& other);

	//! frees dynamic memory
	~field();

	//! fields should save their render information to save time and readability in main loop and drawing
	/*! this is not done, and will likely change the structure of the program. This function now also allows
	 *field objects access to the font information through sdl_font* */
	void graphics_init(SDL_Renderer* sdl_help_renderer_in,string image_p_in,
			   int* xscroll_in,int* yscroll_in/*,TTF_Font* font_in*/);
	//! This is a function, called from graphics_init(), that sets up the text's surface
	/*! it should contain a logic that makes sure that the text does not run off the tile's size
	 *and it also should not overrite any text boxes in the tile, which have not yet been implemented */
	void text_init();

	//! this function sets up the text box, which will be situated in the bottom of the tile card
	/*! it should have as much width as allowed by reason, and the height will likely be consant or based on the
	 *size of the sdl_font in use */
	void text_box_init();

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
	
	//! this member prints a message if the user clicks on this tile
	/*! this function prints to the error logger
	 *\param click_x is the xlocation relative to the top left corner of the screen, where the user left clicked
	 *\param click_y is the ylocation relative to the top left corner of the screen, where the user left clicked*/
	void clicked(SDL_Event& event, const int& click_x,const int& click_y);

	//! this function returns true if the text box was clicked, and false otherwise
	/*! it will either implement text grabbing from keyboard, or call a function that does it */
	bool text_box_clicked(const int& click_x, const int& click_y);

	//! this function changes the temp_input field as a response to typing in the text box
	/*! it should also update the surface for the text, so that the changes are reflected in the graphics when the next
	 *frame is drawn */
	void update_temp_input(SDL_Event& event);

	//! this function makes sure that the user can't type off the bounds of the text box
	bool check_text_box_bounds(SDL_Event& event) const;

	//! this function is used by manager::give_fields_defaults to set the text as read in from the HF config file
	/*! this helper will also update the text's surface dimensions, to be used when the text is drawn to the screen */
	void init_temp_input(string data);

	//! this function takes the file name of a different tile background in the images folder, and uses it instead
	/*! The tiles that have logical control over other parameters (cause locking), will be made purple
	 *to indicate that they are the reason some tiles are unavailable.*/
	void change_tile_background(string image_name);


	//! this function copies the temp value that was likely entered by the user into the output vectors
	/*! because these fields have direct pointer access to their ftran_struct in input_maker's vectors,
	 *input_maker's size should not be changed after the initial function calls when the program starts.
	 *the reason being that if it resizes, it will be re-copied to another location, and the fields pointer
	 *will go bad. This would cause illegal reads of memory, and mayhem.
	 *\return true if everything went well, false if an illegal string issue occured */
	bool update_my_value();

	//! this function is used to swap the box background texture to red
	/*! this is enacted by the manager when this tile fails to convert the user's information
	 *with stoi or stod. It serves as an error indicator for the fields that failed the test. */
	void go_red();

	//! this function returns a tile to normal after it is given proper input and "Let's Go" has been clicked
	/*! basically undos field::go_red(), which is used to signal errors */
	void go_back();

	//! this function updates the texture for the text box
	/*! this means that the information's changes by update_temp_input or back_space are reflected graphically
	 *on the next frame that occurs */
	void update_texture();

	//! this function deletes the last character, unless the string is empty, then it does nothing
	void back_space();

	//! this void member prints the field's info to a given stream
	/*! it prints to the error logger*/
	void print();

	//! this member returns this tiles corner location, and dimensions as an sdl rect
	/*! it's definitely weird to have them stored separately after I've decided to give field
	 * access to SDL stuff, but I'm going to leave it this way for now. Note that this
	 * does account for scrolling*/
	SDL_Rect get_rect() const;

	//! this member returns the boolean stored in help_mode
	bool is_help_mode() { return help_mode; }

	//! this member flips the boolean value of help_mode
	void help_toggle();

	tile_size get_size() { return size;}//!< getter for the private tile_size field

	string get_img_name(){ return image_name;}//!< getter for private image name field

	//! the tile_name should be a parameter name for an HF_input file
	/*! this name should correspond to a parameter name in the HF_config file. They are associated by 
	 *this parameter name */
	string tile_name;

	//! the display name is a name that takes the place of the variable's acronym in the graphics
	string display_name;

	vector<string> descriptions; //!< input description lines

	//! access to this tile's fortran struct in input_maker vector. Set up by manager::give_fields_defaults
	param_int4* int4_hook;

	//! access to this tile's fortran struct in input_maker vector. Set up by manager::give_fields_defaults
	param_real8* real8_hook;

	//! access to this tile's fortran struct in input_maker vector. Set up by manager::give_fields_defaults
	param_string* string_hook;

	//! access to this tile's fortran struct in input_maker vector. Set up by manager::give_fields_defaults
	param_int4_array* int4_array_hook;

	param_r8_array* r8_array_hook;

	//! stores text entered from the user overtop the default value which is loaded in with the appropriate ftran_struct hook
	string temp_input;

	//! save the text entry location
	/*! this controls the cursor when the user is typing */
	unsigned int editing_location;

	//! this function draws the indicator as to where the current editing location is
	void draw_cursor();

	//! save the dimensions of the text in the box
	/* useful when drawing the text editing cursor */
	SDL_Rect text_dims;

	bool is_red;//!< is this tile in error mode or not
	

	bool is_locked;//!< allows locking of the tile, for situation in which they are redundant or not necessary

	bool am_I_locking;//!< allows tiles to keep track of whether or not they're locking their subparameters or form buttons

	int xloc; //!< the field keeps track of the xcoordinate of its upper right corner
	int yloc; //!< the field keeps track of the ycoordinate of its upper right corner
  private:

	//! string that corresponds to the path to the image resource directory. set by graphics_init
	string image_p;
	bool help_mode;//!< this boolean variable controls whether the field will draw it's normal box or help box

	//################################### SDL_HELP HOOKS #####################################################
	//int* sdl_xscroll;//!< this pointer allows field objects access to current x scrolling value in sdl_help
	//int* sdl_yscroll;//!< this pointer allows field objects access to current y scrolling value in sdl_help
	TTF_Font* font;//!< this pointer allows field objects access to the font setting file
	//SDL_Renderer* sdl_help_renderer;//!< a pointer to sdl_help's rendering context
	//########################################################################################################

	SDL_Texture* lock_texture;//!< save the sdl texture for the lock icon

	SDL_Surface* my_text_surf;//!< saves the surface for the tile name text, so that it isn't recreated every frame
	SDL_Texture* my_text_tex;//!< saves the texture for the tile name text, so that it isn't recreated every frame

	sdl_text_box text_box; //!< struct that keeps track of the surfaces and textures required for the text box implementation

	SDL_Surface* my_help_surf;
	SDL_Texture* my_help_tex; //!< saves the texture for this tile's 'help' mode


	SDL_Texture* my_tex;//!< saves the texture, so that it isn't recreated every frame


	string image_name; //!< the name of the image, so it can be found in Assets/Images

	tile_size size; //!< tile_size struct containing the dimensions for this particular card/tile/block
};









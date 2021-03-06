/*! \brief \file field.h declares the field class, which is like a
    tile where users can enter parameters or use defaults */

#pragma once
#include<string>
#include<vector>
#include<iostream>

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

#include "text_box.h"

#include "colors.h"

#include "ftran_structs.h"
#include "string+.h"

#include "logger.h"
extern logger* output_access;;

using std::string;
using std::vector;

//! tile_size is a struct that contains height and width for this parameter
struct tile_size{
    /*! initialize tile size struct to -1x-1,
        if it isn't set this indicates bad input */
    tile_size(){
        width = -1;
        height = -1;
    }

    //! this print member just prints out this tile's dimensions with no newline
    void print(){
        output_access->push_msg("Tile width: "+std::to_string(width)+
                                " Tile height: "+std::to_string(height));
    }
    /*! \brief The width will be the width of an input section in the program
     *It should be a fraction of the window size, so multiple input
     *fields can fit in the same row. */
    int width;

    /*! \brief The height will be the height of an input section in the program
     *It should be a fraction of the window size, so multiple input
     * fields can fit in the window. This should mean less scrolling
     *down for the user.*/
    int height;

};

//! allows implementation of boxes/tiles/cards that are shown on the screen
class field{
  public:

    //! this the constructor that needs to be used
    /*! all of these fields should be filled by manager's init() function
     *\param tile_name is a contextual title that will be displayed on the tile
     *\param image_name_in is the name of the actual image used.
     *\param width is the width of the tile
     *\param height is the height of the tile */
    field(string tile_name_in,string display_name_in,string image_name_in,
            int width, int height);

    //! copy constructor ensures that memory is not lost or double free'd
    field(const field& other);

    //! frees dynamic memory
    ~field();

    //! fields should save their render information to save time
    void graphics_init(string image_p_in);

    //! called from graphics_init(), this sets up the text's surface
    void text_init();

    //! this will draw this field object's texture to the screen
    /*! this works using its known corner values offset by the current scrolling
     *values */
    void draw_me();

    //! this member prints a message if the user clicks on this tile
    /*! this function prints to the error logger
     *\param click_x is the xlocation relative to the top left corner of the screen, where the user left clicked
     *\param click_y is the ylocation relative to the top left corner of the screen, where the user left clicked*/
    void clicked(SDL_Event& event, const int& click_x,const int& click_y);

    //! this function is used by manager::give_fields_defaults to set the text as read in from the HF config file
    /*! this helper will also update the text's surface dimensions, to be used when the text is drawn to the screen */
    void init_temp_input(string data);

    //! this function takes the file name of a different tile background in the images folder, and uses it instead
    /*! The tiles that have logical control over other parameters (cause locking), will be made purple
     *to indicate that they are the reason some tiles are unavailable.*/
    void change_tile_background(string image_name);

    //! this function is used to swap the box background texture to red
    /*! this is enacted by the manager when this tile fails to convert the user's information
     *with stoi or stod. It serves as an error indicator for the fields that failed the test. */
    void set_error_state();

    //! this function returns a tile to normal after it is given proper input and "Let's Go" has been clicked
    /*! basically undos field::go_red(), which is used to signal errors */
    void cancel_error_state();

    //! this function updates the texture for the text box
    /*! this means that the information's changes by update_temp_input or back_space are reflected graphically
     *on the next frame that occurs */
    void update_texture();

    //! this void member prints the field's info to the error_logger
    void print();

    //! returns this tile's corner location, and dimensions as an sdl rect
    /*! does account for scrolling */
    SDL_Rect get_rect() const;

    //! this member returns the boolean stored in help_mode
    bool is_help_mode() { return help_mode; }

    //! this member flips the boolean value of help_mode
    void help_toggle();

    //! getter for the private tile_size field
    tile_size get_size() { return size;}

    //!< getter for private image name field
    string get_img_name(){ return image_name;}

    //! the tile_name should be a parameter name for an HF_input file
    /*! this name should correspond to a parameter name in the HF_config file.
     *They are associated by this parameter name */
    string tile_name;

    //! name that takes the place of the variable's acronym in the graphics
    string display_name;

    //! input description lines
    vector<string> descriptions;

    //! save the text entry location
    /*! this controls the cursor when the user is typing */
    unsigned int editing_location;

    //! draws the indicator as to where the current editing location is
    void draw_cursor();

    //! save the dimensions of the text in the box
    /* useful when drawing the text editing cursor */
    SDL_Rect text_dims;

    bool is_in_error;//!< is this tile in error mode or not

    /*! allows locking of the tile, for situations in which they
     * are redundant or not necessary */
    bool is_locked;

    /*! allows tiles to keep track of whether or not they're
     * locking their subparameters or form buttons */
    bool am_I_locking;

    int xloc; //!< keeps track of the xcoordinate of its upper left corner
    int yloc; //!< keeps track of the ycoordinate of its upper left corner

    //! text box object for user info entry
    text_box my_text_box;

  private:

    //! string that corresponds to the path to the image directory in assets
    string image_path;

    //!< the name of the image
    string image_name;

    /*! this boolean variable controls whether the field will draw
     *it's normal box or help box */
    bool help_mode;

    //! save the sdl texture for the lock icon
    SDL_Texture* lock_texture;

    //!< saves the surface for the tile name text
    SDL_Surface* my_text_surf;

    //!< saves the texture for the tile name text
    SDL_Texture* my_text_tex;

    //! surface for the parameter explanation
    SDL_Surface* my_help_surf;
    //! saves the texture for this tile's 'help' mode
    SDL_Texture* my_help_tex;


    SDL_Texture* my_tex;//!< saves the texture

    /*! tile_size struct containing the dimensions for this
     *particular card/tile/block */
    tile_size size;
};
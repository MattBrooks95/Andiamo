//! file cursor.h describes the class that implements the text editing cursor

//! this class allows the cursor to remember its current drawing state
/*! hopefull this will fix the issues I'm running into with figuring out
 *its location at draw time, without any context about what happened
 *previously */

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<iostream>

#include "logger.h"
extern logger error_logger;

//! \class cursor draws and keeps track of where the cursor is drawn
class cursor{
  public:

	//! cursor sets up the surface and texture for the cursor
	cursor();
	//! copy constructor ensures no double frees 
	cursor(const cursor& other);
	//! clean up memory
	~cursor();

	//! this function sets up the cursor class
	void init(SDL_Renderer* renderer,SDL_Rect* box_location_in);

	//! this function calculates the cursor's raw location and returns it
	/*! it also sets up the cursor_dest rect to later be drawn by draw_me */
	int calc_location(TTF_Font* font, const std::string& text, const unsigned int& editing_location);

	//! this function draws the cursor the passed renderer
	void draw_me(SDL_Renderer* renderer);

	//! this function prints all of the cursor's info to a passed stream
	/* or the error/message logger if no reference is given */
	void print(std::ostream& outs);
	void print();

	//! this function moves the cursor to the left
	/*! this also decrements the editing_location variable
	 *\param text is a const reference to the text from the text box
	 *\param editing_location the editing_location to be modified
	 *\param changed the boolean telling the text box to redraw  */
	void left(const std::string& text,unsigned int& editing_location,bool& changed);
	//! this function moves the cursor to the left
	/*! this also decrements the editing_location variable
	 *\param text is a const reference to the text from the text box
	 *\param editing_location the editing_location to be modified
	 *\param changed the boolean telling the text box to redraw */
	void right(const std::string& text,unsigned int& editing_location,bool& changed);

	//! grab the cursor's location
	SDL_Rect& get_cursor_dest(){ return cursor_dest;}

  private:

	SDL_Surface* my_surface;//!< surface for this object
	SDL_Texture* my_texture;//!< texture for this object

	SDL_Rect* box_location;//!< pointer to text box's location info

	//! save where the cursor should be drawn
	/*! the height and width here will be figured out by using the size
	 *of the character that the cursor is hovering over */
	SDL_Rect cursor_dest;

};


void print_sdl_rect(std::ostream& outs,const SDL_Rect& print_me);






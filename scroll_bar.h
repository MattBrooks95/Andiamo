//! \file scroll_bar.h houses the class declaration for scroll bar objects

#pragma once
#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
/*! \brief scroll_bar contains the internal bookkeeping necessary for creating scroll bars, and also 
 *references to the pertinent fields of the sdl_help object that contains these objects */
/*! it's pretty cool. The sdl_help() constructor calls this class's init() function and sets it up with
 *pointers to the sdl_help object's important information, so that when they update in the sdl_help object
 *the scroll bar gets access to that updated information instantly */ 
class scroll_bar{

  public:
	/*! the scroll_bar() constructor sets all of it's fields to bad default values,
	 * they must be init() 'd */
	scroll_bar();
	/*! I don't think we need a custom destructor, because all pointer's objects should be killed by 
         *the sdl_help object going out of scope */ 
	~scroll_bar();
	/*! \brief init takes in pointers to several fields of the sdl_help object and sets this object's 
         *fields to point to the fields in the sdl_help object */
	/*! This is pretty cool. They update in tandem with the sdl_help object's fields
	 *\param x_scroll_in integer pointer to the containing sdl_help object's x_scroll field
	 *\param y_scroll_in integer pointer to the containing sdl_help object's y_scroll field
	 *\param area_width_in integer pointer to the containing sdl_help object's area_width field
	 *\param area_height_in integer pointer to the containing sdl_help object's area_height field
	 *\param window_width_in integer pointer to the containing sdl_help object's window_s.width field
	 *\param window_height_in integer pointer to the containing sdl_help object's window_s.height field
	 *\param renderer_in SDL_Renderer pointer to the containing sdl_help object's renderer pointer */ 
	void init(int* x_scroll_in, int* y_scroll_in, const int* area_width_in, const int* area_height_in,  
		const int* window_width_in, const int* window_height_in,SDL_Renderer* renderer_in);
	/*! print() prints, to the given stream, this scroll bar's personal info, like location and image 
         *path, but also prints the values of that its pointer fields point to, to make sure they line up
         *with the sdl_help object's fields (they should point to the same location in memory
	 *\param outs is the stream the output messages should be sent to. */
	void print(std::ostream& outs);
	/*! this member should update the scroll bar's location and size in response to a window resize
	 *or scroll */
	void update();


  private:
	//**************** SCROLL BAR STUFF *********************************/
	int xloc; //!> keep track of the scroll bar's corner's x coordinate
	int yloc; //!> keep track of the scroll bar's corner's y coordinate
	int width; //!> keep track of scroll bar width
	int height; //!> keep track of the scroll bar height
	std::string image_p;
	//*******************************************************************/

	//**************** SDL STUFF ****************************************/
	int* x_scroll;
	int* y_scroll;
	const int* area_width;
	const int* area_height;
	const int* window_width;
	const int* window_height;
	SDL_Renderer* renderer;
	//*******************************************************************/



};

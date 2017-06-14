//! \file buttons.h has declarations for the buttons class, which manages static buttons that enact important functions

#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#include<iostream>
#include<string>
#include<fstream>
#include "sdl_help.h"

//! this is the base class for the static buttons that implement features
/*! some possible features are going to include graphing options, the exit dialogue opening,
 *output file name entry*/
class button{
  public:
	button(sdl_help* sdl_help_in = NULL);
	virtual ~button();

	virtual void print_me();
	virtual void draw_me();

	virtual void make_rect();


	virtual void set_corner_loc();

	//###########################################################
	virtual void handle_click(SDL_Event& mouse_event);
	virtual bool was_clicked(SDL_Event& mouse_event);
	virtual void click_helper(SDL_Event& mouse_event);
	//###########################################################
	virtual void init(std::string image_name_in, std::string image_p_in,sdl_help* sdl_help_in);
	bool shown;

  protected:
	std::string image_name;
	std::string total_image_p;

	sdl_help* sdl_helper;

	int xloc;
	int yloc;

	int width;
	int height;

	SDL_Rect my_rect;

	SDL_Surface* button_surface;
	SDL_Texture* button_texture;

};

//! contains the information needed for rectangular "click areas", like the yes/no buttons on the exit dialogue
/*! this should be useful for many different types of buttons */
struct active_area{
	//! constructor defaults stuff to 0
	active_area(){
		xloc = 0;
		yloc = 0;
		width= 0;
		height=0;
	}
	//! boolean variable to check mouse click coords against this active area's location
	bool clicked(SDL_Event& event){
		//std::cout << "MOUSE " << event.button.x << ":" << event.button.y << std::endl;
		//std::cout << "My Area: X=" << xloc << "-" << xloc + width << " Y= " << yloc << "-" 
			  //<< yloc + height << std::endl;
		if( (event.button.x > xloc && event.button.x < xloc + width) &&
		    (event.button.y > yloc && event.button.y < yloc + height) ){
			return true;
		}
		return false;
	}

	void print_me(){
		std::cout << "Printing active area" << std::endl;
		std::cout << xloc << ":" << yloc << "     " << width << ":" << height << std::endl;
	}

	int xloc;
	int yloc;
	int width;
	int height;
};








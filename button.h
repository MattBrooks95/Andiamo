//! \file buttons.h has declarations for the buttons class, which manages static buttons that enact important functions

#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#include<iostream>
#include<string>
#include<fstream>
#include "sdl_help.h"

//make this virtual?
class button{
  public:
	button(sdl_help* sdl_help_in = NULL);
	virtual ~button();

	virtual void print_me();

	virtual void set_corner_loc();

	//###########################################################
	virtual void handle_click(SDL_Event& mouse_event);
	virtual bool was_clicked(SDL_Event& mouse_event);
	virtual void click_helper(SDL_Event& mouse_event);
	//###########################################################
	void init(std::string image_name_in, std::string image_p_in,sdl_help* sdl_help_in);
	bool shown;

  protected:
	std::string image_name;
	std::string total_image_p;

	sdl_help* sdl_helper;

	int xloc;
	int yloc;

	int width;
	int height;

	SDL_Surface* button_surface;
	SDL_Texture* button_texture;

};

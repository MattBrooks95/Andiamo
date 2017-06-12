//! \file buttons.h has declarations for the buttons class, which manages static buttons that enact important functions

#pragma once

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#include<iostream>
#include<string>

#include "sdl_help.h"

struct button{
	button();
	~button();

	void init();

	std::string name;

	unsigned int width;
	unsigned int height;

	SDL_Surface* button_surface;
	SDL_Texture* button_texture;
	

};

//! buttons is a manager for the struct button, which is like a static tile, but is more graphics and option setting oriented
/*! these buttons should either stay in a box below the tiles, OR be fixed to the window kind of like a HUD,
 *It needs access to a lot of sdl_helper fields, like the renderer, font, window size... */
class buttons{
  public:
	buttons(sdl_help* sdl_helper_in);
	~buttons();




	std::string button_config_name;
	std::string button_config_path;

	sdl_help* sdl_helper;

  private:
	button exit_dialogue;
	button make_output;
	button output_file_set;

};

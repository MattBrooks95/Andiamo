//! this file describes the button_manager class that contains various derivations of class button
#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>

#include<iostream>
#include<string>
#include<fstream>
#include "sdl_help.h"

#include "button.h"
#include "derived_buttons.h"

//! button_manager is a manager for the different derivations of class button
/*! these buttons should either stay in a box below the tiles, OR be fixed to the window kind of like a HUD,
 *It needs access to a lot of sdl_helper fields, like the renderer, font, window size... */
class button_manager{
  public:
	button_manager(sdl_help* sdl_helper_in);
	~button_manager();

	void init_buttons();
	void print_buttons();

	void draw_buttons();

	void click_handling(SDL_Event& mouse_event);

	//GETTERS AND SETTERS
	exit_button& get_exit_dialogue(){ return exit_dialogue;}
	button& get_default_test(){ return default_test;}

	std::string button_image_p;

	sdl_help* sdl_helper;

  private:
	button default_test;
	exit_button exit_dialogue;
	//button make_output;
	//button output_file_set;

};

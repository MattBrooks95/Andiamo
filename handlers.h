/*! \file handlers.h \brief handlers.h contains some useful functions that help prevent main() from getting 
 *cluttered - implementations in handlers.cc
 *
 *it implements a lot of the sub switch statements, and the handlers get the SDL_Event and sdl_help
 *objects so that it can enact the functions that it needs to
 */
#pragma once
#include<string>

#include "sdl_help.h"

//void plus_two(int num_in,const sdl_help& sdl_helper);//this was used to test compilation

//! this is a filter function meant to prevent the sdl event queue from pushing mouse location updates
int filter_mouse_move(void* userdata, SDL_Event* event);


//############################## JERRY STUFF ##################################################################
//! handle_mouseb_down implements the sub-switch statement that handles mouse buttons being pressed down
void handle_mouseb_down(const SDL_Event& big_event, const sdl_help& sdl_help);

//! handle_mouseb_up implements the sub-switch statement that handles mouse buttons being released
void handle_mouseb_up(const SDL_Event& big_event, const sdl_help& sdl_help);

//! handle_mouse_wheel handles scrolling via the mousewheel
/*! will very likely behave differently than the scrolling functions found in handle_key_down. However,
 *it is the same in essence, because it just modifies the x_scroll and y_scroll members. Right now it scrolls
 *by 60 per mousewheel down or up, which is higher than the key input*/
void handle_mouse_wheel(const SDL_Event& big_event, sdl_help& sdl_help);
//############################## JERRY STUFF ##################################################################

//############################## KEYBOARD STUFF ###############################################################
//! handle_key_down impelements a sub-switch statement that handles keys being pressed down
void handle_key_down(const SDL_Event& big_event, sdl_help& sdl_help);

//! handle_key_up implements a sub-switch statement that handles keys being released
void handle_key_up(const SDL_Event& big_event, const sdl_help& sdl_help);
//#############################################################################################################



/*! \file handlers.h \brief handlers.h contains some useful functions that help prevent main() from getting 
 *cluttered - implementations in handlers.cc
 *
 *it implements a lot of the sub switch statements, and the handlers get the SDL_Event and sdl_help
 *objects so that it can enact the functions that it needs to
 */
#pragma once


#include "sdl_help.h"


//void plus_two(int num_in,const sdl_help& sdl_helper);

//! handle_mouseb_down implements the sub-switch statement that handles mouse buttons being pressed down
void handle_mouseb_down(const SDL_Event& big_event, const sdl_help& sdl_help);

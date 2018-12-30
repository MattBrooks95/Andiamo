/*! \brief \file handlers.h handlers.h contains some useful functions that
 *help prevent main() from getting cluttered */
/* \details \file handlers.h implements a lot of the sub switch statements,
 *and the handlers get the SDL_Event and sdl_help objects so that it can
 *enact the functions that it needs to */
#pragma once
#include<string>

#include "sdl_help.h"
#include "button_manager.h"


#include "logger.h"
extern logger* output_access;;

//! prevents the sdl event queue from pushing mouse location updates
int filter_mouse_move(void* userdata, SDL_Event* event);

/*! this is a filter function designed for the mini loop in scrolling_mini_loop
 *- only allows mouse motion and mouse button up events to happen
 *(only events needed in the mini loop) */
int filter_mini(void* userdata, SDL_Event* event);

//################# JERRY FUNCTIONS FOR SUBLOOPS ###############################
/*! implements a sub-switch statement that
 *handles left mouse clicks in the scrolling sub loop. */
/*! the loop processes the user dragging the mouse, and updates
 * the corresponding scroll bar, and the scroll value.
 *\param big_event the sdl event that determines behavior
 *\param which_bar and 'v' means we are working with the vertical bar,
 *'h' means the horizontal bar */
void scrolling_mini_loop(SDL_Event& big_event,char which_bar);
//##############################################################################

//##################### JERRY STUFF ############################################
//! handles mouse buttons being pressed
void handle_mouseb_down( SDL_Event& big_event);

//! handles mouse buttons being released
void handle_mouseb_up(const SDL_Event& big_event);

//! handle_mouse_wheel handles scrolling via the mousewheel
/*! will very likely behave differently than the scrolling functions
 *found in handle_key_down. However, it is the same in essence,
 *because it just modifies the x_scroll and y_scroll members.
 *Right now it scrolls by 60 per mousewheel down or up, which
 *is higher than the key input*/
void handle_mouse_wheel(const SDL_Event& big_event);
//##################### JERRY STUFF ############################################

//#################### KEYBOARD STUFF ##########################################
//! keys being pressed down
void handle_key_down(const SDL_Event& big_event);

//! handles keys being released
void handle_key_up(const SDL_Event& big_event);
//##############################################################################



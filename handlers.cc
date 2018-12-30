//! \file handlers.cc implements the functions declared in handlers.h
#include<iostream>

#include "handlers.h"

#define x_scroll_rate = 15
#define y_scroll_rate = 30

using namespace std;

int filter_mouse_move(void* userdata, SDL_Event* big_event){
	if(big_event->type == SDL_MOUSEMOTION) return 0;
	return 1;
}
int filter_mini(void* userdata, SDL_Event* big_event){
	if(big_event->type == SDL_MOUSEMOTION ||
       big_event->type == SDL_MOUSEBUTTONUP) return 1;
	return 0;
}
//############### MINI LOOP STUFF ##############################################
void scrolling_mini_loop(SDL_Event& big_event,char which_bar){

    //change to true to end the mini loop
	bool exit = false;

	//change the filter to disallow MOUSEBUTTONDOWN events and allow
	//MOUSEMOTION events that we need to know in order to scroll
	SDL_SetEventFilter(filter_mini,NULL);

    //this this necessary
	SDL_FlushEvents(0,1000);
	while(!exit){
		SDL_PollEvent(&big_event);

		switch(big_event.type){
			case SDL_MOUSEBUTTONDOWN:
                {string filt_err;
                filt_err  = "Error, MOUSEBUTTONDOWN not properly filtered";
                filt_err += "in scrolling_mini_loop.";
				output_access->push_error(filt_err);}
				break;

			case SDL_MOUSEBUTTONUP:
				switch(big_event.button.button){
					case SDL_BUTTON_LEFT:

                            //let go of left click, leave
							exit = true;
						break;
					default:
						break;
				}//end SDL_MOUSEBUTTONUP baby switch
				break;

			case SDL_MOUSEMOTION:

                //do stuff for the vertical scroll bar
				if(which_bar == 'v'){

				    //only care about y scrolling
				    if(big_event.motion.y > sdl_access->get_v_bar().get_bottom()){
						//if user has drug above the top of this bar, scroll up
						sdl_access->update_scroll(0,-45);

					} else if(big_event.motion.y < sdl_access->get_v_bar().get_top()){

						//if user has drug below the bottom of this bar, scroll down
						sdl_access->update_scroll(0,45);
					}
					sdl_access->draw();
					sdl_access->present();

					//prevent queue from getting flooded
					SDL_FlushEvent(SDL_MOUSEMOTION);

				//do stuff for the horizontal scroll bar
				//only care about x scrolling
				} else if(which_bar == 'h'){

					if(big_event.motion.x > sdl_access->get_h_bar().get_right()){

						//if user has drug right of the scroll bar, scroll right
						sdl_access->update_scroll(-45,0);

					} else if(big_event.motion.x < sdl_access->get_h_bar().get_left()){

						//if user has drug left of the scroll bar, scroll left
						sdl_access->update_scroll(45,0);
					}

					sdl_access->draw();
					sdl_access->present();
					//prevent queue from getting flooded
					SDL_FlushEvent(SDL_MOUSEMOTION);
				}
				break;

			case 1776:
				break;

			default:
				output_access->push_error("Didn't hit a case in scrolling event.");
				exit = true;
		}//end daddy switch statement
		SDL_Delay(30);
	}//end mini loop

	//put the filter back as it was.
	SDL_SetEventFilter(filter_mouse_move,NULL);
}
//##########################################################################################################

//############################# JERRY STUFF ################################################################
void handle_mouseb_down( SDL_Event& big_event){
	//-1 is dummy value, an error code of sorts
	//this is here to store the result of sdl_help.scroll_clicked() later in the
	//left mouse button click case
	int which_bar = -1;

	switch(big_event.button.button){

		case SDL_BUTTON_LEFT:

			//note that scroll bar has click priority over the tiles.
			//Clicking a scroll bar with a tile under it won't do
			//anything to that tile

			//result of scroll_clicked 'boolean' function stored here,
			//so function is only called once
			which_bar = sdl_access->scroll_clicked(big_event.button.x,big_event.button.y);

			if( which_bar == 1){

				//a return value of 1 means that the vertical scroll bar
                //was clicked make sure vals line up
				output_access->push_msg("Clicked on the vertical bar!");

				if(sdl_access->get_v_bar().is_scrolling() == false){

					sdl_access->get_v_bar().scroll_mode_change(true);
					//is this necessary?
					SDL_FlushEvents(0,1000);

				//should never hit this branch
				} else {

                    string err =
                        "V scroll bar already in scroll mode upon click!";
					output_access->push_error(err);
				}

			} else if( which_bar == 2){

				//a return value of 2 means that horizontal scroll bar
                //was clicked make sure vals line up
				output_access->push_msg("Clicked on the horizontal bar!");

				if(sdl_access->get_h_bar().is_scrolling() == false){

					sdl_access->get_h_bar().scroll_mode_change(true);
					//is this necessary?
					SDL_FlushEvents(0,1000);

				} else {

					//should likewise never hit this branch
                    string err =
                        "H scroll bar already in scroll mode upon click!";
					output_access->push_error(err);
				}

			//no scroll bar was clicked, look at other things
			} else if( which_bar == 0){
				//check buttons first
				if( !button_access->click_handling(big_event) ){

					//if click_handling returns false, then check the tiles
					sdl_access->click_detection(big_event, big_event.button.x,
												big_event.button.y);
				}

			//if which_bar is still -1, something is wrong
			} else {

                string err = "sdl_help::scroll_clicked has a bad value.";
				output_access->push_error(err);
			}
			break;

		//handle right clicks
		case SDL_BUTTON_RIGHT:
            {string msg = "\nRight clicked at location = "
                         +to_string(big_event.button.x) + ":"
                         +to_string(big_event.button.y);
			output_access->push_msg(msg);}
			break;

		//handle mousewheel clicks
		case SDL_BUTTON_MIDDLE: {
            string msg = "\nMousewheel clicked at location = "
                         + to_string(big_event.button.x)+ ": "
                         + to_string(big_event.button.y);
			output_access->push_msg(msg);
        }

		default:
			break;
	}
}

void handle_mouseb_up(const SDL_Event& big_event){
	switch(big_event.button.button){

		case SDL_BUTTON_LEFT: //handle left mouse button coming up

			break;
		case SDL_BUTTON_RIGHT: //handle right mouse button coming up

			break;
		case SDL_BUTTON_MIDDLE: //handle mousewheel coming back up
			break;

		default:
			break;
	}
}

void handle_mouse_wheel(const SDL_Event& big_event){
	sdl_access->update_scroll(big_event.wheel.x * 15,big_event.wheel.y * 60);
}
//#################### JERRY STUFF ############################################

//################# KEYBOARD STUFF ############################################
void handle_key_down(const SDL_Event& big_event){
	switch(big_event.key.keysym.sym){

		//literal down arrow key
		case SDLK_DOWN:
			sdl_access->update_scroll(0,-30); //scroll down
			break;

		case SDLK_UP:
			sdl_access->update_scroll(0,30); //scroll down
			break;

		case SDLK_RIGHT:
			sdl_access->update_scroll(-60,0); //scroll down
			break;

		case SDLK_LEFT:
			sdl_access->update_scroll(60,0); //scroll down
			break;

		case SDLK_SPACE:
			sdl_access->reset_scroll();
			break;

		case SDLK_ESCAPE:
			SDL_Event push_me;
			push_me.type = SDL_QUIT;
			SDL_PushEvent(&push_me);
			break;
		// case SDLK_ESCAPE:
		// 	exit(-1);
		// 	break;
		default:
			output_access->push_msg("Unknown key pressed down.");
			break;
	}//end switch statement

}

void handle_key_up(const SDL_Event& big_event){
	switch(big_event.key.keysym.sym){
		case SDLK_RIGHT:
			break;

		case SDLK_LEFT:
			break;

		case SDLK_DOWN:
			break;

		case SDLK_UP:
			break;
		case SDLK_SPACE:
			break;
		default:
			output_access->push_msg("Unknown key released. ");
			break;
	}//end switch statement
	SDL_FlushEvent(SDL_MOUSEBUTTONUP);
}
//#################### KEYBOARD STUFF ######################################






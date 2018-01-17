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
	if(big_event->type == SDL_MOUSEMOTION || big_event->type == SDL_MOUSEBUTTONUP) return 1;
	return 0;
}
//############################# MINI LOOP STUFF ###########################################################
void scrolling_mini_loop(SDL_Event& big_event, sdl_help& sdl_help,button_manager& b_manager,char which_bar){
	bool exit = false;//change to true to end the mini loop
	SDL_SetEventFilter(filter_mini,NULL);
		//change the filter to disallow MOUSEBUTTONDOWN events and allow
		//MOUSEMOTION events that we need to know in order to scroll 
	SDL_FlushEvents(0,1000);//this this necessary
	while(!exit){
		SDL_PollEvent(&big_event);

		switch(big_event.type){
			case SDL_MOUSEBUTTONDOWN:
				error_logger.push_error("Error, MOUSEBUTTONDOWN not properly filtered in scrolling_mini_loop");
				break;
			case SDL_MOUSEBUTTONUP:
				switch(big_event.button.button){
					case SDL_BUTTON_LEFT:
							exit = true; //let go of left click, leave
						break;
					default:
						break;
				}//end SDL_MOUSEBUTTONUP baby switch
				break;
			case SDL_MOUSEMOTION:
				if(which_bar == 'v'){ //do stuff for the vertical scroll bar
						   //only care about y scrolling

					if(big_event.motion.y > sdl_help.get_v_bar().get_bottom()){
						//if user has drug above the top of this bar, scroll up
						sdl_help.update_scroll(0,-45);
					} else if(big_event.motion.y < sdl_help.get_v_bar().get_top()){
						//if user has drug below the bottom of this bar, scroll down
						sdl_help.update_scroll(0,45);
					}
					//sdl_help.draw_tiles();
					sdl_help.draw_sbars();
					b_manager.draw_buttons();
					sdl_help.present();
					SDL_FlushEvent(SDL_MOUSEMOTION);//prevent queue from getting
										 //flooded

				} else if(which_bar == 'h'){ //do stuff for the horizontal scroll bar
							  //only care about x scrolling

					if(big_event.motion.x > sdl_help.get_h_bar().get_right()){
						//if user has drug right of the scroll bar, scroll right
						sdl_help.update_scroll(-45,0);
					} else if(big_event.motion.x < sdl_help.get_h_bar().get_left()){
						//if user has drug left of the scroll bar, scroll left
						sdl_help.update_scroll(45,0);
					}
					//sdl_help.draw_tiles();
					sdl_help.draw_sbars();	
					b_manager.draw_buttons();
					sdl_help.present();
					SDL_FlushEvent(SDL_MOUSEMOTION);//prevent queue from getting
									//flooded
				}
				break;
			case 1776:
				break;
			default:
				error_logger.push_error("Error. Didn't hit a case in scrolling event sub-loop.");
				exit = true;
		}//end daddy switch statement
		SDL_Delay(30);
	}//end mini loop
	SDL_SetEventFilter(filter_mouse_move,NULL);//put the filter back as it was.
}
//##########################################################################################################

//############################# JERRY STUFF ################################################################
void handle_mouseb_down( SDL_Event& big_event, sdl_help& sdl_help,button_manager& b_manager){
	int which_bar = -1;//-1 is dummy value, an error code of sorts
			   //this is here to store the result of sdl_help.scroll_clicked() later in the
			   //left mouse button click case

	switch(big_event.button.button){

		case SDL_BUTTON_LEFT:

			//note that scroll bar has click priority over the tiles. Clicking a scroll bar
			//with a tile under it won't do anything to that tile

			//result of scroll_clicked 'boolean' function stored here, so function is only
			//called once
			which_bar = sdl_help.scroll_clicked(big_event.button.x,big_event.button.y);

			if( which_bar == 1){
				//a return value of 1 means that the vertical scroll bar was clicked
				error_logger.push_msg("Clicked on the vertical bar!"); //make sure vals line up
				if(sdl_help.get_v_bar().is_scrolling() == false){
					sdl_help.get_v_bar().scroll_mode_change(true);
					SDL_FlushEvents(0,1000);//is this necessary?
				} else {//should never hit this branch
					error_logger.push_error("Error! V scroll bar already in scroll mode upon click!");
				}

			} else if( which_bar == 2){
				//a return value of 2 means that horizontal scroll bar was clicked
				error_logger.push_msg("Clicked on the horizontal bar!");//make sure vals line up
				if(sdl_help.get_h_bar().is_scrolling() == false){
					sdl_help.get_h_bar().scroll_mode_change(true);
					SDL_FlushEvents(0,1000);//is this necessary?
				} else { //should likewise never hit this branch

					error_logger.push_error("Error! H scroll bar already in scroll mode upon click!");
				}

			} else if( which_bar == 0){//no scroll bar was clicked, look at other things
				//check buttons first
				if( !b_manager.click_handling(big_event) ){

					//if b_manager returns false, then check the tiles
					sdl_help.click_detection(cout,big_event,&b_manager,
								 big_event.button.x,big_event.button.y);
				}

			} else {//if which_bar is still -1, something is wrong
				error_logger.push_error("Error in left mouse button case, sdl_help::scroll_clicked has a bad value.");
			}
			break;

		case SDL_BUTTON_RIGHT: //handle right clicks
			error_logger.push_msg("\nRight clicked at location = "+to_string(big_event.button.x)+":"+to_string(big_event.button.y));
			break;

		case SDL_BUTTON_MIDDLE: //handle mousewheel clicks
			error_logger.push_msg("\nMousewheel clicked at location = "+to_string(big_event.button.x)+": "+
			to_string(big_event.button.y));
		default:
			break;
	}
}

void handle_mouseb_up(const SDL_Event& big_event, const sdl_help& sdl_help){
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

void handle_mouse_wheel(const SDL_Event& big_event, sdl_help& sdl_help){
	sdl_help.update_scroll(big_event.wheel.x * 15,big_event.wheel.y * 60);
}
//############################# JERRY STUFF ################################################################

//############################# KEYBOARD STUFF #############################################################
void handle_key_down(const SDL_Event& big_event, sdl_help& sdl_help){
	switch(big_event.key.keysym.sym){
		case SDLK_DOWN: //literal down arrow key
			sdl_help.update_scroll(0,-30); //scroll down
			break;

		case SDLK_UP:
			sdl_help.update_scroll(0,30); //scroll down
			break;

		case SDLK_RIGHT:
			sdl_help.update_scroll(-60,0); //scroll down
			break;

		case SDLK_LEFT:
			sdl_help.update_scroll(60,0); //scroll down
			break; 

		case SDLK_SPACE:
			sdl_help.reset_scroll();
			break;

        case SDLK_ESCAPE:
            SDL_Event push_me;
            push_me.type = SDL_QUIT;
            SDL_PushEvent(&push_me);
			break;
		default:
			error_logger.push_msg("Unknown key pressed down.");
			break;
	}//end switch statement

}

void handle_key_up(const SDL_Event& big_event, const sdl_help& sdl_help){
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
			error_logger.push_msg("Unknown key released. ");
			break;
	}//end switch statement
	SDL_FlushEvent(SDL_MOUSEBUTTONUP);
}
//############################# KEYBOARD STUFF #############################################################






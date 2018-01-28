//! \file main.cc
//! \brief main.cc is where the big loop happens.

#include<string>
#include<vector>
#include<iostream>

#include "sdl_help.h"
#include "asset_manager.h"
#include "handlers.h"
#include "input_maker.h"
#include "button_manager.h"
#include "logger.h"
using namespace std;

//global object used for error message output
logger          error_logger;

//manages textures
asset_manager*  asset_access;

//graphics library wrapper
sdl_help*       sdl_access;

//handles HF parameter entry
manager*        tile_access;

//makes HF file
input_maker*    io_access;

//manages various buttons
button_manager* button_access;

//making this global and giving it a unique name,
//so the exit button can change it
bool main_done = false;

/*! in the event the user tries to quit andiamo without having first
 *made an output file, this function displays a message. It is passed
 *a reference to the exit button, that way the message can always be
 *in the same location relative to the button */
void no_work_done_message(exit_button& exit_dialogue);

/*! main() handles sdl events (keypress, mouse movements), instantiates
 *an sdl_help object, and calls its drawing functions per run
 *of the loop. */
int main(int argc, char *argv[]){

  if(argc == 2){
  	string argument = argv[1];
	if(argument.compare("-v") == 0){
		//if -v is appended at the command line, have the error logger also print
		//runtime debugging messages
		error_logger.verbose = true;

	//if some arg exists but is not -v, make an error message	   
	} else {
		error_logger.push_error("Supplied useless command line argument");
	}
  }
  //error_logger.push_msg("And where does the newborn go from here?"
  //					  " The net is vast and infinite.");

  //set up the sdl wrapper
  sdl_help sdl_helper("Andiamo!");
  sdl_access = &sdl_helper;

  //import all of the assets
  asset_manager assets;
  asset_access = &assets;
  asset_access->pull_assets();
  //asset_access->list_images(cout);

  //sets up the background pictures and scroll bars
  sdl_access->init();

  //make the tiles
  manager tile_bag("./Assets/Images/");
  tile_access = &tile_bag;
  tile_access->init();
  tile_access->init_fields_graphics();

  sdl_access->calc_corners();

  input_maker io_handler;
  io_access = &io_handler;
  io_access->init();

  //give fields pointers to their val in input_maker
  tile_access->give_fields_defaults();

  //set up the manager for the functional buttons
  button_manager b_manager;
  b_manager.init_tray();
  b_manager.init_buttons();
  b_manager.init_form_tray();
  button_access = &b_manager;


  //pre-loop drawing commands, so screen comes up near instantly
  sdl_access->draw();
  sdl_access->present();

  SDL_Event big_event;
  SDL_SetEventFilter(filter_mouse_move,NULL);

  while(!main_done){
	//apparently if there is no new event big_event keeps that last
	// value, so it scrolls indefinitely for example, so for now
	//I'm using 1776 as a "no operation" flag
	if(!SDL_PollEvent(&big_event)){
		big_event.type = 1776;
	}

	//if the vertical scroll bar is in "scroll mode"
	if(sdl_access->get_v_bar().is_scrolling()){
		//do a mini loop until the left mouse button is released
		scrolling_mini_loop(big_event,'v');
		//stop v scroll bar mode
		sdl_access->get_v_bar().scroll_mode_change(false);
		//is this necessary?
		SDL_FlushEvents(0,1000);


	//if the horizontal scroll bar is in "scroll mode"
	//do a mini loop until the left mouse button is released
	} else if(sdl_access->get_h_bar().is_scrolling()){

		scrolling_mini_loop(big_event,'h');

		//stop h scroll bar mode
		sdl_access->get_h_bar().scroll_mode_change(false);

		//remove all events from event queue
		SDL_FlushEvents(0,1000);

	} else

	//switch controlled by the 'type' of input given, like the mouse moving
	//or key presses
	switch(big_event.type){


		case SDL_QUIT:
			//does a mini loop that implements exit_button's functionality
			//where the user has to click yes or no for it to go away
			if(io_access->output_was_made){
				no_work_done_message(button_access->get_exit_dialogue());
			}
			button_access->get_exit_dialogue().handle_click(big_event);
			break;

		case SDL_KEYDOWN:

			handle_key_down(big_event);

			//prevents queue flooding when key is held down
			SDL_FlushEvent(SDL_KEYDOWN);
			break;

		case SDL_KEYUP:
			handle_key_up(big_event);
			SDL_FlushEvent(SDL_KEYUP);
			break;

		case SDL_MOUSEBUTTONDOWN:
			//this function handles left/right mouse button down clicks,
			//and mousewheel clicks
			handle_mouseb_down(big_event);
			break;

		case SDL_MOUSEBUTTONUP:
			handle_mouseb_up(big_event);
			break;

		case SDL_MOUSEWHEEL:
			handle_mouse_wheel(big_event);
			//make it not get flooded with scroll commands
			SDL_FlushEvent(SDL_MOUSEWHEEL);
			break;

		case SDL_WINDOWEVENT:
			//error_logger.push_msg("WINDOW EVENT ##########################");
			//error_logger.push_msg("EVENT Num:"+to_string(big_event.type));
			if(big_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
				error_logger.push_msg(to_string(big_event.window.data1)+":"+
						       to_string(big_event.window.data2));
				sdl_access->window_update(big_event.window.data1,
											big_event.window.data2);
				button_access->location_update();
			}

			break;

		//no new event this time, don't just keep repeating the last event
		case 1776:
			break;

		default:
			break;

	}//event handling switch

	sdl_access->draw();
	sdl_access->present();

	//slow down loop speed if work was done
	SDL_Delay(50);

  }//end of while loop

  //sdl_access->print_size_info();

  //SDL_Delay(5000);

  return 0;//Exit success
}



void no_work_done_message(exit_button& exit_dialogue){

	SDL_Texture* no_work_texture = NULL;

	no_work_texture = asset_access->get_texture("Assets/Images/no_work_done_msg.png");
	if(no_work_texture == NULL) error_logger.push_error(SDL_GetError());
	//plan where to draw
	SDL_Rect dest = {0,0,0,0};
	//get size of image
	SDL_QueryTexture(no_work_texture,NULL,NULL,&dest.w,&dest.h);

	//this math just "squares up" the error message nicely with the exit button
	int width_offset = (dest.w - exit_dialogue.get_width()) / 2;
	dest.x = exit_dialogue.get_xloc() - width_offset;
	dest.y = exit_dialogue.get_yloc() - dest.h;



	SDL_RenderCopy(sdl_access->renderer,no_work_texture,NULL,&dest);

}









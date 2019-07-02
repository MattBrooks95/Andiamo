//! \file main.cc
//! \brief main.cc creates the major objects, and routes user activities to them

#include <string>
#include <vector>
#include <iostream>

#include "system_wrapper.h"
#include "regex_manager.h"
#include "sdl_help.h"
#include "asset_manager.h"
#include "handlers.h"
#include "input_maker.h"
#include "fop_handler.h"
#include "button_manager.h"
#include "logger.h"
#include "helpers.h"

#include "command_args.h"

using namespace std;

// handles program environment variables like current directory,
// HOME environment and executable directory
system_wrapper* system_access;

// lazy-loads regular expressions, ensuring that unique regular expression
// patterns are only instantiated once, and used as many times as necessary
regex_manager* regex_access;

// global object used for error message output
logger* logger_access;

// global reference to asset_manager
asset_manager*  asset_access;

// global reference to graphics library wrapper
sdl_help* sdl_access;

// global reference to object that handles HF parameter entry
manager* tile_access;

// global reference to object that makes HF file
input_maker* io_access;

// global reference to object that manages various buttons
button_manager* button_access;

// global reference to object that handles creating transmission coefficients
fop_handler* FOP_access;

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
	system_wrapper system;
	system_access = &system;

	regex_manager regex_vendor;
	regex_access = &regex_vendor;

	logger error_logger;
	logger_access = &error_logger;

	//this string starts off empty. If it remains empty by
	//the time input_maker::init(string) is called, it defaults to
	//HF_Config.txt. However, if a new one is specified via the command line
	//this string is filled and used instead
	// string input_maker_config_file;

	//similarly, this can be set to cause the tile manager object
	//to use a file other than tiles.txt
	string manager_config_folder = "default";

	//used to set the show_line_guides boolean in the sdl class later
	bool line_guides = true;
	if(!process_args(argc,argv,manager_config_folder,line_guides) ){
		//if one of the arguments was malformed, or -help,
		//don't continue execution
		exit(0);
	}
	cout << "manager config folder:" << manager_config_folder << endl;
	//set up the sdl wrapper
	sdl_help sdl_helper("Andiamo!");
	sdl_access = &sdl_helper;
	if(!line_guides){
		sdl_access->show_line_guides = false;
	}

	//import all of the assets
	asset_manager assets;
	asset_access = &assets;

	// asset_access->list_images(cout);

	//sets up the background pictures and scroll bars
	sdl_access->init();

	manager tile_bag(manager_config_folder);
	tile_access = &tile_bag;

	sdl_access->calc_corners();

	input_maker io_handler;
	io_access = &io_handler;

	//set up the manager for the functional buttons
	button_manager b_manager;
	b_manager.init_everything();
	button_access = &b_manager;

	//set up the FOP handling object
	fop_handler FOP;
	// FOP.get_files_list();
	// FOP.print_file_list();
	FOP_access = &FOP;

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

		} else {

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

					if(big_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){

						logger_access->push_msg(to_string(big_event.window.data1)+":"+
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
		}

		sdl_access->draw();
		sdl_access->present();

		//slow down loop speed if work was done
		SDL_Delay(50);

	}//end of while loop

  return EXIT_SUCCESS;
}

void no_work_done_message(exit_button& exit_dialogue){
	SDL_Texture* no_work_texture = asset_access->get_texture("Assets/Images/no_work_done_msg.png");
	if(no_work_texture == NULL) logger_access->push_error(SDL_GetError());
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

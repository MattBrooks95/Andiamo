//! \file main.cc
//! \brief main.cc is where the big loop happens.

#include<string>
#include<vector>
#include<iostream>

#include "sdl_help.h"
#include "handlers.h"
#include "input_maker.h"
#include "button_manager.h"
#include "logger.h"
using namespace std;


logger error_logger; //global object used for error message output

//making this global and giving it a unique name, so the exit button can change it
bool main_done = false;

/*! in the event the user tries to quit andiamo without having first made an output file,
 *this function displays a message. It is passed a reference to the exit button,
 *that way the message can always be in the same location relative to the button */
void no_work_done_message(sdl_help& sdl_helper,exit_button& exit_dialogue);

 /*! main() handles sdl events (keypress, mouse movements), instantiates an sdl_help object,
  *and calls its drawing functions per run of the loop. It will eventually have options for resizing
  *the window, and letting the user pick the frame rate.
  */
int main(int argc, char *argv[]){

  if(argc == 2){
  	string argument = argv[1];
	if(argument.compare("-v") == 0){
		//if -v is appended at the command line, have the error logger also print
		error_logger.verbose = true; //runtime debugging messages
				   
	} else {//if some arg exists but is not -v, make an error message
		error_logger.push_error("Supplied useless command line argument");
	}
  }
  error_logger.push_msg("And where does the newborn go from here? The net is vast and infinite.");

  sdl_help sdl_helper("Andiamo!");

  button_manager b_manager(&sdl_helper);
  b_manager.init_tray();
  b_manager.init_buttons();
  b_manager.init_form_tray();
  sdl_helper.get_mgr().gain_bmanager_access(&b_manager);
  b_manager.print_buttons();
  sdl_helper.get_mgr().print_all();

  sdl_helper.print_tile_locs(cout);

  //graphics class initializations
  sdl_helper.draw_tiles();
  sdl_helper.draw_sbars();
  b_manager.draw_buttons();

  sdl_helper.present();
  
  SDL_Event big_event; //pre-loop drawing commands, so screen comes up near instantly
  SDL_SetEventFilter(filter_mouse_move,NULL);

  while(!main_done){
	//apparently if there is no new event big_event keeps that last value, so it scrolls indefinitely for
	//example, so for now I'm using 1776 as a "no operation" flag
	if(!SDL_PollEvent(&big_event)){
		big_event.type = 1776;
	}

	if(sdl_helper.get_v_bar().is_scrolling()){//if the vertical scroll bar is in "scroll mode"
		   //do a mini loop until the left mouse button is released
		scrolling_mini_loop(big_event,sdl_helper,b_manager,'v');
		sdl_helper.get_v_bar().scroll_mode_change(false);//stop v scroll bar mode
		SDL_FlushEvents(0,1000); //is this necessary?


	} else if(sdl_helper.get_h_bar().is_scrolling()){//if the horizontal scroll bar is in "scroll mode"
		     //do a mini loop until the left mouse button is released
		scrolling_mini_loop(big_event,sdl_helper,b_manager,'h');
		sdl_helper.get_h_bar().scroll_mode_change(false);//stop h scroll bar mode
		SDL_FlushEvents(0,1000);//remove all events from event queue

	} else
	switch(big_event.type){ //switch controlled by the 'type' of input given, like the mouse moving
				//or key presses


		case SDL_QUIT:
			//does a mini loop that implements exit_button's functionality
			//where the user has to click yes or no for it to go away
			if(!sdl_helper.get_io_handler().output_was_made){
				no_work_done_message(sdl_helper,b_manager.get_exit_dialogue());
			}
			b_manager.get_exit_dialogue().handle_click(big_event);
			//main_done = true;
			break;

		case SDL_KEYDOWN:
			handle_key_down(big_event,sdl_helper);
			SDL_FlushEvent(SDL_KEYDOWN);//prevents queue flooding when key is held down
			break;

		case SDL_KEYUP:
			handle_key_up(big_event,sdl_helper);
			SDL_FlushEvent(SDL_KEYUP);
			break;

		case SDL_MOUSEBUTTONDOWN:
			//this function handles left/right mouse button down clicks, and mousewheel clicks
			handle_mouseb_down(big_event,sdl_helper,b_manager);
			break;

		case SDL_MOUSEBUTTONUP:
			handle_mouseb_up(big_event,sdl_helper);
			break;

		case SDL_MOUSEWHEEL:
			handle_mouse_wheel(big_event,sdl_helper);
			SDL_FlushEvent(SDL_MOUSEWHEEL);//make it not get flooded with scroll commands
			break;

		case SDL_WINDOWEVENT:
			//error_logger.push_msg("WINDOW EVENT ####################################################");
			//error_logger.push_msg("EVENT Num:"+to_string(big_event.type));
			if(big_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
				error_logger.push_msg(to_string(big_event.window.data1)+":"+
						       to_string(big_event.window.data2));
				sdl_helper.window_update(big_event.window.data1,big_event.window.data2);
				b_manager.location_update();
			}

			break;

		case 1776: //no new event this time, don't just keep repeating the last event
			break;

		default:
			break;
	}//event handling switch

	sdl_helper.draw_tiles(); //re-draw the screen once all events have been handled
	sdl_helper.draw_sbars(); //draw the scroll bars
	b_manager.draw_buttons(); //draw visible/on buttons
	sdl_helper.present();  //and all positions have been calculated

	SDL_Delay(50);//slow down loop speed if work was done

  }//end of while loop
  //b_manager.clean_up();//have the button manager set up the necessary file paths in input_maker so
		       //update_io_maker can output/input things properly

  //b_manager.print_buttons();
  //sdl_helper.get_mgr().update_io_maker();
  sdl_helper.print_size_info();


  //SDL_Delay(5000);

  
  return 0;//Exit success
}



void no_work_done_message(sdl_help& sdl_helper,exit_button& exit_dialogue){

	SDL_Surface* no_work_surf = NULL;
	SDL_Texture* no_work_texture = NULL;

	no_work_surf = IMG_Load("Assets/Images/no_work_done_msg.png");
	if(no_work_surf == NULL) error_logger.push_error(SDL_GetError());

	no_work_texture = SDL_CreateTextureFromSurface(sdl_helper.renderer,no_work_surf);
	if(no_work_texture == NULL) error_logger.push_error(SDL_GetError());
	//plan where to draw
	SDL_Rect dest = {0,0,0,0};
	//get size of image
	SDL_QueryTexture(no_work_texture,NULL,NULL,&dest.w,&dest.h);

	//this math just "squares up" the error message nicely with the exit button
	int width_offset = (dest.w - exit_dialogue.get_width()) / 2;
	dest.x = exit_dialogue.get_xloc() - width_offset;
	dest.y = exit_dialogue.get_yloc() - dest.h;



	SDL_RenderCopy(sdl_helper.renderer,no_work_texture,NULL,&dest);


	if(no_work_surf != NULL) SDL_FreeSurface(no_work_surf);
	if(no_work_texture != NULL) SDL_DestroyTexture(no_work_texture);

}









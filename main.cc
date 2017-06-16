//! \file main.cc
//! \brief main.cc is where the big loop happens.

#include<string>
#include<vector>
#include<iostream>

#include "sdl_help.h"
#include "handlers.h"
#include "input_maker.h"
#include "button_manager.h"
using namespace std;

//making this global and giving it a unique name, so the exit button can change it
//this should have serious consequences until it's thoroughly debugged
bool main_done = false;

 /*! main() handles sdl events (keypresses, mouse movements), instantiates an sdl_help object,
  *and calls its drawing functions per run of the loop. It will eventually have options for resizing
  *the window, and letting the user pick the frame rate.
  */
int main(){

  cout << "And where does the newborn go from here? The net is vast and infinite." << endl;

  sdl_help sdl_helper("Andiamo!");
  //cout << "Now past the sdl_help constructor." << endl;

  button_manager b_manager(&sdl_helper);
  b_manager.init_tray();
  b_manager.init_buttons();
  b_manager.print_buttons();
  //sdl_helper.get_mgr().print_all(cout);

  //sdl_helper.print_tile_locs(cout);

  //graphics class initializations
  sdl_helper.draw_tiles();
  sdl_helper.draw_sbars();
  b_manager.draw_buttons();

  sdl_helper.present();

  
  SDL_Event big_event; //pre-loop drawing commands, so screen comes up near instantly
  SDL_SetEventFilter(filter_mouse_move,NULL);

  //bool done = false; //this will need to be changed to true when the user clicks on the 'x'
  while(!main_done){
	//apparently if there is no new event big_event keeps that last value, so it scrolls indefinitely for
	//example, so for now I'm using 1776 as a "no operation" flag
	if(!SDL_PollEvent(&big_event)){
		big_event.type = 1776;
	}
	//cout << "EVENT = " << big_event.type << endl;

	if(sdl_helper.get_v_bar().is_scrolling()){//if the vertical scroll bar is in "scroll mode"
		   //do a mini loop until the left mouse button is released
		scrolling_mini_loop(big_event,sdl_helper,b_manager,'v');
		sdl_helper.get_v_bar().scroll_mode_change(false);//stop v scroll bar mode
		SDL_FlushEvents(0,1000); //is this necessary?


	} else if(sdl_helper.get_h_bar().is_scrolling()){//if the horizontal scroll bar is in "scroll mode"
		     //do a mini loop until the left mouse button is released
		scrolling_mini_loop(big_event,sdl_helper,b_manager,'h');
		sdl_helper.get_h_bar().scroll_mode_change(false);//stop h scroll bar mode
		SDL_FlushEvents(0,1000);//is this necessary?

	} else
	switch(big_event.type){ //switch controlled by the 'type' of input given, like the mouse moving
				//or key presses


		case SDL_QUIT:
				//does a mini loop that implements exit_button's functionality
				//where the user has to click yes or no for it to go away
				b_manager.get_exit_dialogue().handle_click(big_event);
			break;

		//as of right now 5/11/17, this section is definitely moot because mouse motion is filtered
		//out of the input queue
		//case SDL_MOUSEMOTION: //I don't think anything will care about the mouse moving,
					//only clicks. Unless we get fancy with reactive animations
			//cout << big_event.motion.x << " " << big_event.motion.y << endl;

			//break;

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
		case 1776: //no new event this time, don't just keep repeating the last event
			break;

		default:
			break;
	}//event handling switch

	//sdl_helper.get_mgr().print_all(cout);
	sdl_helper.draw_tiles(); //re-draw the screen once all events have been handled
	sdl_helper.draw_sbars(); //draw the scroll bars
	b_manager.draw_buttons(); //draw visible/on buttons
	//sdl_helper.print_tile_locs(cout);
	sdl_helper.present();  //and all positions have been calculated

	//sdl_helper.get_win_size()->print(); //testing

	SDL_Delay(50);//this is an arbitrary number to slow down the loop speed
		     //eventually this will vary intelligently based on desired framerate
  }//end of while loop
  b_manager.print_buttons();
  sdl_helper.get_mgr().update_io_maker();
  //sdl_helper.print_size_info(cout);
  //sdl_helper.get_h_bar().print(cout);//make sure that these values are updating in the bars as
  //sdl_helper.get_v_bar().print(cout);//they are updated in the sdl_helper object

  //SDL_Delay(5000);

  
  return 0;//Exit success
}

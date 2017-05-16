//! \file main.cc
//! \brief main.cc is where the big loop happens.

#include<string>
#include<vector>
#include<iostream>

#include "sdl_help.h"

#include "handlers.h"

using namespace std;

 /*! main() handles sdl events (keypresses, mouse movements), instantiates an sdl_help object,
  *and calls its drawing functions per run of the loop. It will eventually have options for resizing
  *the window, and letting the user pick the frame rate.
  */
int main(){

  cout << "And where does the newborn go from here? The net is vast and infinite." << endl;

  sdl_help sdl_helper("Andiamo!");

  //sdl_helper.get_mgr().print_all(cout);

  sdl_helper.draw_all();
  sdl_helper.present();


  SDL_Event big_event; //pre-loop drawing commands, so screen comes up near instantly
  SDL_SetEventFilter(filter_mouse_move,NULL);



  bool done = false; //this will need to be changed to true when the user clicks on the 'x'
  while(!done){

	if(!SDL_PollEvent(&big_event));

	else
	switch(big_event.type){ //switch controlled by the 'type' of input given, like the mouse moving
				//or key presses


		case SDL_QUIT:
			cout << "quitting...." << endl;
			done = true;
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
			//SDL_FlushEvent(SDL_KEYUP);
			break;

		case SDL_MOUSEBUTTONDOWN:
			//this function handles left/right mouse button down clicks, and mousewheel clicks
			handle_mouseb_down(big_event,sdl_helper); 
			break;

		case SDL_MOUSEBUTTONUP:
			handle_mouseb_up(big_event,sdl_helper);
			break;

		case SDL_MOUSEWHEEL:
			handle_mouse_wheel(big_event,sdl_helper);
			SDL_FlushEvent(SDL_MOUSEWHEEL);//make it not get flooded with scroll commands
			break;

		default:
			break;
	}//event handling loop

	//sdl_helper.get_mgr().print_all(cout);
	sdl_helper.draw_all(); //re-draw the screen once all events have been handled
	//sdl_helper.print_tile_locs(cout);
	sdl_helper.present();  //and all positions have been calculated

	//sdl_helper.get_win_size()->print(); //testing

	SDL_Delay(50);//this is an arbitrary number to slow down the loop speed
		     //eventually this will vary intelligently based on desired framerate
  }//end of while loop

  //make sure that infinitely re-adding fields to location manager stopped happening
  //these numbers should be exactly equal
  //cout << "Tile vector size: " << sdl_helper.get_mgr().tiles.size() << " Tile location vector size: "
  //     << sdl_helper.get_locations().size() << endl;

  //sdl_helper.get_mgr().print_all(cout);

  //sdl_helper.print_tile_locs(cout);
  sdl_helper.print_size_info(cout);

  //SDL_Delay(5000);


  return 0;//Exit success
}

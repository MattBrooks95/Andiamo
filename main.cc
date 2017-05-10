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

  sdl_helper.get_mgr().print_all(cout);
  sdl_helper.draw_all();
  sdl_helper.present();


  SDL_Event big_event;
  bool done = false; //this will need to be changed to true when the user clicks on the 'x'
  while(!done){
	SDL_PollEvent(&big_event);//grab input from user

	switch(big_event.type){ //switch controlled by the 'type' of input given, like the mouse moving
				//or key presses
		case SDL_QUIT:
			cout << "quitting...." << endl;
			done = true;
			break;

		case SDL_MOUSEMOTION:
			//cout << big_event.motion.x << " " << big_event.motion.y << endl;

			break;
		case SDL_MOUSEBUTTONDOWN:
			//this function handles left/right mouse button down clicks, and mousewheel clicks
			handle_mouseb_down(big_event,sdl_helper); 
			break;

		default:
			break;
	}
	
	SDL_Delay(5);
  }


  //SDL_Delay(5000);


  return 0;//Exit success
}

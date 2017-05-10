//! \file handlers.cc implements the functions declared in handlers.h
#include<iostream>

#include "handlers.h"

using namespace std;

//example to make sure linking worked
//void plus_two(int num_in,const sdl_help& sdl_helper){
//	cout << num_in + 2 << " " << sdl_helper.window_name << endl;
//}

void handle_mouseb_down(const SDL_Event& big_event, const sdl_help& sdl_help){
	switch(big_event.button.button){

		case SDL_BUTTON_LEFT: //handle left clicks
			cout << "\nLeft clicked at location= "
			     << big_event.button.x << ":" << big_event.button.y;
			break;

		case SDL_BUTTON_RIGHT: //handle right clicks
			cout << "\nRight clicked at location= "
			     << big_event.button.x << ":" << big_event.button.y;
			break;

		case SDL_BUTTON_MIDDLE: //handle mousewheel clicks
			cout << "\nMousewheel clicked at location= "
			     << big_event.button.x << ": " << big_event.button.y;
		default:
			break;
	}
}

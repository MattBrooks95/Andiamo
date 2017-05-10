//! \file handlers.cc implements the functions declared in handlers.h
#include<iostream>

#include "handlers.h"

using namespace std;

//example to make sure linking worked
//void plus_two(int num_in,const sdl_help& sdl_helper){
//	cout << num_in + 2 << " " << sdl_helper.window_name << endl;
//}

int filter_mouse_move(void* userdata, SDL_Event* big_event){
	if(big_event->type == SDL_MOUSEMOTION) return 0;
	return 1;
}

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

void handle_mouseb_up(const SDL_Event& big_event, const sdl_help& sdl_help){
	switch(big_event.button.button){

		case SDL_BUTTON_LEFT: //handle left mouse button coming up
			cout << "\nLeft mouse button released at location= "
			     << big_event.button.x << ":" << big_event.button.y;
			break;
		case SDL_BUTTON_RIGHT: //handle right mouse button coming up
			cout << "\nRight mouse button released at location= "
			     << big_event.button.x << ":" << big_event.button.y;
			break;
		case SDL_BUTTON_MIDDLE: //handle mousewheel coming back up
			cout << "\nMousewheel released at location= "
			     << big_event.button.x << ":" << big_event.button.y;
			break;

		default:
			break;
	}
}

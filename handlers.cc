//! \file handlers.cc implements the functions declared in handlers.h
#include<iostream>

#include "handlers.h"

#define x_scroll_rate = 15
#define y_scroll_rate = 30


using namespace std;

bool handle_test = true;




//example to make sure linking worked
//void plus_two(int num_in,const sdl_help& sdl_helper){
//	cout << num_in + 2 << " " << sdl_helper.window_name << endl;
//}

int filter_mouse_move(void* userdata, SDL_Event* big_event){
	if(big_event->type == SDL_MOUSEMOTION) return 0;
	return 1;
}

//############################# JERRY STUFF ################################################################
void handle_mouseb_down(const SDL_Event& big_event, const sdl_help& sdl_help){
	switch(big_event.button.button){

		case SDL_BUTTON_LEFT: //handle left clicks
			//cout << "\nLeft clicked at location= "
			 //    << big_event.button.x << ":" << big_event.button.y << endl;

			//have sdl_help check to see if we clicked a scroll bar
			//note that scroll bar has click priority over the tiles. Clicking a scroll bar
			//with a tile under it won't do anything to that tile
			if(sdl_help.scroll_clicked(cout,big_event.button.x,big_event.button.y)){
				//there has to be some sort of mini event loop here to figure out how far
				//to scroll as a response to the scroll bar being clicked and dragged
				//the only solutions I can think of right now are ugly
				//with this member structure, how do I know if it is an hbar or vbar
				//drawing will stop if I do an event mini loop to capture the scroll
				//	amounts
				//perhaps do some sort of flag thing where big_event loop is in scrolling
				//mode, and continues to draw, but only handles mouse dragging
				//then stops when the mouse button is let go? hmmmmm
			}
			else sdl_help.click_detection(cout,big_event.button.x,big_event.button.y);

			break;

		case SDL_BUTTON_RIGHT: //handle right clicks
			if(handle_test) cout << "\nRight clicked at location= "
			     << big_event.button.x << ":" << big_event.button.y << endl;
			break;

		case SDL_BUTTON_MIDDLE: //handle mousewheel clicks
			if(handle_test) cout << "\nMousewheel clicked at location= "
			     << big_event.button.x << ": " << big_event.button.y << endl;
		default:
			break;
	}
}

void handle_mouseb_up(const SDL_Event& big_event, const sdl_help& sdl_help){
	switch(big_event.button.button){

		case SDL_BUTTON_LEFT: //handle left mouse button coming up
			//cout << "\nLeft mouse button released at location= "
			  //   << big_event.button.x << ":" << big_event.button.y << endl;
			break;
		case SDL_BUTTON_RIGHT: //handle right mouse button coming up
			//if(handle_test) cout << "\nRight mouse button released at location= "
			//     << big_event.button.x << ":" << big_event.button.y << endl;
			break;
		case SDL_BUTTON_MIDDLE: //handle mousewheel coming back up
			//if(handle_test) cout << "\nMousewheel released at location= "
			//     << big_event.button.x << ":" << big_event.button.y << endl;
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
			cout << SDL_GetKeyName(SDLK_DOWN) << " pressed down" << endl;
			sdl_help.update_scroll(0,-30); //scroll down
			break;

		case SDLK_UP:
			cout << SDL_GetKeyName(SDLK_UP) << " pressed down" << endl;
			sdl_help.update_scroll(0,30); //scroll down
			break;

		case SDLK_RIGHT:
			cout << SDL_GetKeyName(SDLK_RIGHT) << " pressed down" << endl;
			sdl_help.update_scroll(-60,0); //scroll down
			break;//-60 ^was -15

		case SDLK_LEFT:
			cout << SDL_GetKeyName(SDLK_LEFT) << " pressed down" << endl;
			sdl_help.update_scroll(60,0); //scroll down
			break; //60 ^was 15
		case SDLK_SPACE:
			cout << "Spacebar pressed, resetting scroll values." << endl;
			sdl_help.reset_scroll();
			break;
		default:
			if(handle_test) cout << "Unknown key pressed down." << endl;
			break;
	}//end switch statement

}

void handle_key_up(const SDL_Event& big_event, const sdl_help& sdl_help){
	switch(big_event.key.keysym.sym){
		case SDLK_RIGHT:
			cout << SDL_GetKeyName(SDLK_RIGHT) << " released" << endl;
			break;

		case SDLK_LEFT:
			cout << SDL_GetKeyName(SDLK_LEFT) << " released" << endl;
			break;

		case SDLK_DOWN:
			cout << SDL_GetKeyName(SDLK_DOWN) << " released." << endl;
			break;

		case SDLK_UP:
			cout << SDL_GetKeyName(SDLK_UP) << " released." << endl;
			break;
		case SDLK_SPACE:
			cout << "Spacebar released." << endl;
			break;
		default:
			if(handle_test) cout << "Unknown key released. " << endl;
			break;
	}//end switch statement

}
//############################# KEYBOARD STUFF #############################################################













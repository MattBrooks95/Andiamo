//! \file buttons.cc implements the functions declared in buttons.h

#include "buttons.h"

//###################### BUTTON STRUCT ########################################
button::button(){

	width = 0;
	height = 0;
	
	name = "No name.";

	button_surface = NULL;
	button_texture = NULL;

}

button::~button(){
	SDL_FreeSurface(button_surface);
	SDL_DestroyTexture(button_texture);
}



//#############################################################################


//####################  BUTTONS CLASS  ########################################
buttons::buttons(sdl_help* sdl_helper_in){
	sdl_helper = sdl_helper_in;


}

buttons::~buttons(){


}


//#############################################################################



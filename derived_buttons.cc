//! \file implements the class functions declared in derived_buttons.h

#include<iostream>
#include "derived_buttons.h"
using namespace std;

extern bool main_done;

//################################ EXIT ########################################################
//################################ Button ######################################################
exit_button::exit_button(){


}

void exit_button::set_corner_loc(){
	//set center of tile in the exact center of the screen 
	xloc = (sdl_helper->get_win_size()->width / 2) - (width / 2);
	yloc = (sdl_helper->get_win_size()->height / 2) - (height / 2);
}

void exit_button::handle_click(SDL_Event& mouse_event){
	int which = my_click(mouse_event);
	if(which == 0){ //neither 1 or 2 was clicked 
		
	} else if (which == 1){//return value of 1 = no was clicked

	} else if (which == 2){//return value of 2 = yes was clicked

	}
}

int exit_button::my_click(SDL_Event& mouse_event){
	if( no_button.clicked(mouse_event) ){//"no" part was clicked
		return 1;
	} else if( yes_button.clicked(mouse_event) ){//"yes" part was clicked
		return 2;
	} else {
		return 0;
	}

}

void exit_button::print_me(){

	cout << "Did my parent's print function get called too? How about now?" << endl;
	button::print_me();
}

void exit_button::my_click_helper(int which){
	if( which == 1){//don't exit
		shown = false;//re-hide this button

	} else if( which == 2 ){//exit the program
		main_done = true;
	} else {
		cout << "Error in the exit button's my_click_helper function."
		     << "Shouldn't be possible to receive a value other than 1 or 2." << endl;
		return;//do no work
	}

}

void exit_button::init(string image_name_in, string image_p_in,sdl_help* sdl_help_in){

	button::init(image_name_in,image_p_in,sdl_help_in);

	//set up corner info
	set_corner_loc();

	//exit dialogue defaults to hidden
	shown = false;

	no_button.xloc = xloc + 30;
	no_button.yloc = yloc + 30;
	no_button.width = 70;
	no_button.height = 50;


	yes_button.xloc = xloc + 130;
	yes_button.yloc = yloc + 30;
	yes_button.width = 70;
	yes_button.height = 50;


}
//##############################################################################################
//##############################################################################################


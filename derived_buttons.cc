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
	shown = true;
	bool satisfied = false;
	int which = 0;

	while(!satisfied){

		draw_me();
		sdl_helper->present();

		SDL_PollEvent(&mouse_event);
		which = my_click(mouse_event);//see if they clicked on the yes or no boxes

		my_click_helper(which,satisfied);

		SDL_Delay(50);
	}
}

int exit_button::my_click(SDL_Event& mouse_event){
	if( no_area.clicked(mouse_event) ){//"no" part was clicked
		return 1;
	} else if( yes_area.clicked(mouse_event) ){//"yes" part was clicked
		return 2;
	} else {//neither one was clicked
		return 0;
	}

}

void exit_button::my_click_helper(int which,bool& satisfied){

	if( which == 1){//don't exit
		//don't change main_done
		shown = false;//re-hide this button
		satisfied = true;

	} else if( which == 2 ){//exit the program
		main_done = true;
		satisfied = true;
	} else {
			
		return;//do no work until they hit yes or no
	}

}

void exit_button::print_me(){
	//cout << "Did my parent's print function get called too? How about now?" << endl;
	button::print_me();
	no_area.print_me();
	yes_area.print_me();
}


void exit_button::init(string image_name_in, string image_p_in,sdl_help* sdl_help_in){
	//run default procedure
	button::init(image_name_in,image_p_in,sdl_help_in);

	//change up corner info,exit button has special location
	set_corner_loc();

	//exit dialogue defaults to hidden
	shown = false;

	//configure active areas
	no_area.xloc = xloc + 30;
	no_area.yloc = yloc + 30;
	no_area.width = 70;
	no_area.height = 50;

	//this 195 number was determined by trial and error, to get the box's actual dimensions to line up
	//with the image's. I suppose measuring the pixel distance in a photo editor would have been more efficient
	yes_area.xloc = xloc + 195;
	yes_area.yloc = yloc + 30;
	yes_area.width = 70;
	yes_area.height = 50;


}
//##############################################################################################
//##############################################################################################


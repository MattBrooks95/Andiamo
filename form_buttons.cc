//! \file form_buttons.cc contains the implementations for the classes specified in form_buttons.h

#include "form_buttons.h"
using namespace std;

//######################## FORM BUTTONS #####################################
form_button::~form_button(){
	SDL_FreeSurface(lock_surface);
	SDL_DestroyTexture(lock_texture);

}

void form_button::init(sdl_help* sdl_help_in){
	sdl_helper = sdl_help_in;
	
	lock_surface = IMG_Load("Assets/Images/lock.png");
	if(lock_surface == NULL) error_logger.push_error(SDL_GetError());
	lock_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,lock_surface);
	if(lock_texture == NULL) error_logger.push_error(SDL_GetError());
	is_locked = true;


}
//should be used by the bmangers "form" init member to place each form button such that it lines up with
//the graphic for the form button tray
void form_button::set_corner_loc(int x_in, int y_in){
	xloc = x_in;
	yloc = y_in;
}

void form_button::make_rect(int width_in,int height_in){
	my_rect.x = xloc;//initialized by set_corner_loc
	my_rect.y = yloc;
	my_rect.w = width_in;
	width = width_in;//the width field is the one used to check clicks

	my_rect.h = height_in;
	height = height_in;//the height field is the one used to check clicks
}
void form_button::setup_lock(){
	lock_rect.x = my_rect.x+my_rect.w-15;//temporarily halved
	lock_rect.y = my_rect.y;
	lock_rect.w = 15;//temporarily halved
	lock_rect.h = 25;

}

void form_button::draw_lock(){
	if(is_locked){
		SDL_RenderCopy(sdl_helper->renderer,lock_texture,NULL,&lock_rect);
	}
}

void form_button::toggle_lock(){
	if(is_locked){
		is_locked = false;
	} else {
		is_locked = true;
	}

}
//#############################################################################


//####################### ICNTRL8 BUTTON ######################################
bool icntrl8_form_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		click_helper(mouse_event);
		return true;
	}
	return false;
}

void icntrl8_form_button::click_helper(SDL_Event& mouse_event){
	cout << "Clicked icntrl8/cutoff nuclei button" << endl;


}


//##############################################################################

//###################### ILV2 BUTTON ###########################################
bool ilv2_form_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		click_helper(mouse_event);
		return true;
	}
	return false;
}

void ilv2_form_button::click_helper(SDL_Event& mouse_event){
	cout << "Clicked ilv2/  button" << endl;

}


//###############################################################################

//####################### ICNTRL6 BUTTON ########################################
bool icntrl6_form_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		click_helper(mouse_event);
		return true;
	}
	return false;
}

void icntrl6_form_button::click_helper(SDL_Event& mouse_event){
	cout << "clicked the icntrl6/parameter search button" << endl;

}
//################################################################################

//####################### ICNTRL10 BUTTON ########################################
bool icntrl10_form_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		click_helper(mouse_event);
		return true;
	}
	return false;

}

void icntrl10_form_button::click_helper(SDL_Event& mouse_event){
	cout << "clicked the icntrl10/sigma info button " << endl;
}
//################################################################################


//##################### ICNTRL4 BUTTON ###########################################
void icntrl4_form_button::setup_lock(){
	lock_rect.w = 15;//temporarily halved
	lock_rect.h = 25;

	lock_rect.x = my_rect.x+my_rect.w-15;//temporarily halved
	lock_rect.y = my_rect.y + my_rect.h - lock_rect.h;

}


bool icntrl4_form_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		click_helper(mouse_event);
		return true;
	}
	return false;
}

void icntrl4_form_button::click_helper(SDL_Event& mouse_event){
	cout << "clicked the icntrl4/resolved levels info button " << endl;
}
//################################################################################










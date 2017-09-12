//! \file form_buttons.cc contains the implementations for the classes specified in form_buttons.h

#include <cmath>
#include "form_buttons.h"
using namespace std;

//######################## FORM BUTTONS #####################################
form_button::form_button(){
	lock_surface = NULL;
	lock_texture = NULL;
	is_locked = true;
	unlock_help_surface = NULL;
	unlock_help_texture = NULL;
}

form_button::~form_button(){

	//###############################################################################################
	if(lock_surface != NULL) SDL_FreeSurface(lock_surface);
	else error_logger.push_error("Lock surface for form_button NULL upon destructor being called");

	if(lock_texture != NULL) SDL_DestroyTexture(lock_texture);
	else error_logger.push_error("Lock texture for form_button NULL upon destructor being called");
	//###############################################################################################

	//###############################################################################################
	if(unlock_help_surface != NULL) SDL_FreeSurface(unlock_help_surface);
	else error_logger.push_error("Help message surface for form_button NULL upon destructor being called");

	if(unlock_help_texture != NULL) SDL_DestroyTexture(unlock_help_texture);
	else error_logger.push_error("Help message texture for form_button NULL upon destructor being called");
	//###############################################################################################

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
	lock_rect.x = my_rect.x+my_rect.w-15;
	lock_rect.y = my_rect.y;
	lock_rect.w = 15;
	lock_rect.h = 25;

}

void form_button::setup_help_msg(){

	unlock_help_surface = IMG_Load("Assets/Images/form_assets/general_form_locked_msg.png");
	if(unlock_help_surface == NULL) error_logger.push_error(SDL_GetError());

	unlock_help_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,unlock_help_surface);
	if(unlock_help_texture == NULL) error_logger.push_error(SDL_GetError());

}
void form_button::init_form(){

	my_form.init("no title","general_form_locked_msg.png",0,0,sdl_helper,sdl_helper->font);

}

void form_button::draw_help_msg(SDL_Event& big_event,SDL_Rect& destination){
	SDL_RenderCopy(sdl_helper->renderer,unlock_help_texture,NULL,&destination);
	sdl_helper->present();

	//spin until they are done reading, and they click the mouse or push a key - I can't get this to work because
	//of phantom events, for now it just stays up until they let the mouse button come up
	while(big_event.type != SDL_MOUSEBUTTONUP ){
		SDL_PollEvent(&big_event);
	}
					

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

void form_button::make_output(){




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
	error_logger.push_msg("Clicked icntrl8/cutoff nuclei button");

	//don't consider doing anything if the form is locked
	if(!is_locked){

		//in this case the form has not been previously created
		if(!my_form.prev_initiated){


			page_creation_helper();//most of this work is shared with the recreation case
					       //so it has been put into a helper function

			my_form.toggle_active();//let the form know that it is now active
			my_form.form_event_loop(mouse_event);//enter the mini loop for form entry

		//in this case the form has been previously created, but the icntrl8 value has not changed, so nothing needs to be done
		} else if(my_form.prev_init_value == stoi(sdl_helper->get_mgr().fields.at("line_6").at("ICNTRL8").temp_input) ){
			my_form.toggle_active();//let the form know that it is now active
			my_form.form_event_loop(mouse_event);//enter the mini loop for form entry

		//in this case, the form has been previously created, but the icntrl8 value has been changed, so it must be recreated
		} else {
			my_form.flush_pages();//clear out previous info


			page_creation_helper();//most of this work is shared with the 1st time creation case
					       //so it has been put into a helper function


			my_form.toggle_active();//let the form know that it is now active
			my_form.form_event_loop(mouse_event);//enter the mini loop for form entry


		}
	}
}

void icntrl8_form_button::page_creation_helper(){


			//grab val from parameter field, so the pages can be set up
			icntrl8_val = stoi(sdl_helper->get_mgr().fields.at("line_6").at("ICNTRL8").temp_input);
			error_logger.push_msg("ICNTRL8 val:" + to_string(icntrl8_val)+" when form opened");
			vector<string> pass_column_titles,pass_row_titles;
			//for icntrl 8, the labels shouldn't change
			pass_column_titles.push_back("IA8");
			pass_column_titles.push_back("IZ8");
			pass_column_titles.push_back("E8");
			//#########################################
		
			int total_height_pixels = icntrl8_val * 35;//guessing on a budget of 25 pixels per text box, and 10 padding 
			int pages_needed = ceil(total_height_pixels / 725.0); //pixels needed/pixels per page = pages needed
		

			my_form.get_pages().resize(pages_needed);
			for(unsigned int c = 0; c < my_form.get_pages().size();c++){
				my_form.get_pages()[c].page_init(3,(icntrl8_val / pages_needed),pass_column_titles,pass_row_titles,sdl_helper,sdl_helper->font);
			}

			my_form.set_page_count(pages_needed);

			my_form.prev_initiated = true;//let the form class know that it's pages have been set up
			my_form.prev_init_value = icntrl8_val;//and also what conditions caused such a creation



}

void icntrl8_form_button::init_form(){

	my_form.init("Cutoff Nuclei (ICNTRL8)","icntrl8_form_help.png",0,0,sdl_helper,sdl_helper->font);
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
	error_logger.push_msg("Clicked ilv2/  button");
	if(!is_locked){
		my_form.toggle_active();//let the form know that it is now active
		my_form.form_event_loop(mouse_event);//enter the mini loop for form entry
	}
}

void ilv2_form_button::init_form(){

	my_form.init("Level Info (ILV2)","general_form_locked_msg.png",0,0,sdl_helper,sdl_helper->font);

}


//###############################################################################

//####################### ICNTRL6 BUTTON ########################################
void icntrl6_form_button::setup_lock(){
	lock_rect.w = 15;
	lock_rect.h = 25;

	lock_rect.x = my_rect.x + my_rect.w - 15;
	lock_rect.y = my_rect.y + my_rect.h - lock_rect.h;


}

bool icntrl6_form_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		click_helper(mouse_event);
		return true;
	}
	return false;
}

void icntrl6_form_button::click_helper(SDL_Event& mouse_event){
	error_logger.push_msg("clicked the icntrl6/parameter search button");
	if(!is_locked){
		my_form.toggle_active();//let the form know that it is now active
		my_form.form_event_loop(mouse_event);//enter the mini loop for form entry
	}
}

void icntrl6_form_button::init_form(){

	my_form.init("Parameter Search (ICNTRL6)","general_form_locked_msg.png",0,0,sdl_helper,sdl_helper->font);

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
	error_logger.push_msg("clicked the icntrl10/sigma info button ");
	if(!is_locked){
		my_form.toggle_active();//let the form know that it is now active
		my_form.form_event_loop(mouse_event);//enter the mini loop for form entry
	}
}

void icntrl10_form_button::init_form(){

	my_form.init("Spin Cutoff Information (ICNTRL10)","general_form_locked_msg.png",0,0,sdl_helper,sdl_helper->font);

}

//################################################################################


//##################### ICNTRL4 BUTTON ###########################################
void icntrl4_form_button::setup_lock(){
	lock_rect.w = 15;
	lock_rect.h = 25;

	lock_rect.x = my_rect.x + my_rect.w - 15;
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
	error_logger.push_msg("clicked the icntrl4/resolved levels info button ");
	if(!is_locked){
		my_form.toggle_active();//let the form know that it is now active
		my_form.form_event_loop(mouse_event);//enter the mini loop for form entry
	}
}

void icntrl4_form_button::init_form(){

	my_form.init("Resolved Levels (ICNTRL4)","general_form_locked_msg.png",0,0,sdl_helper,sdl_helper->font);

}

//################################################################################










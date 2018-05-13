/*! \brief \file derived_buttons.cc implements the class functions
 *declared in derived_buttons.h */

#include<iostream>
#include "button_manager.h"
#include "derived_buttons.h"
using namespace std;

extern button_manager* button_access;
//################################ EXIT BUTTON #################################

exit_button::exit_button(){


}

void exit_button::set_corner_loc(){

	//set center of tile in the exact center of the screen 
	xloc = (sdl_access->get_win_size()->width / 2) - (width / 2);
	yloc = (sdl_access->get_win_size()->height / 2) - (height / 2);

    //update the SDL_Rect that is used to draw the exit button
    make_rect();

    //update the clickable regions to match the exit button's new location
    //this follows the logic from exit_button::init
    no_area.set_loc(xloc+30,yloc+30,70,50);
    yes_area.set_loc(xloc+195,yloc+30,70,50);
}

bool exit_button::handle_click(SDL_Event& mouse_event){
	shown = true;
	bool satisfied = false;
	int which = 0;
	bool did_something = false;
    draw_me();
	sdl_access->present();

	while(!satisfied){

		//draw_me();

		SDL_PollEvent(&mouse_event);

		//see if they clicked on the yes or no boxes
		which = my_click(mouse_event);

		my_click_helper(which,satisfied);
		if(which == 1 || which == 2) did_something = true;
		SDL_Delay(50);
	}
	return did_something;
}

int exit_button::my_click(SDL_Event& mouse_event){

	//"no" part was clicked
	if( no_area.clicked(mouse_event) ){
		return 1;

	//"yes" part was clicked
	} else if( yes_area.clicked(mouse_event) ){
		return 2;

	//neither one was clicked
	} else {
		return 0;
	}

}

void exit_button::my_click_helper(int which,bool& satisfied){

	//don't exit
	if( which == 1){
		//don't change main_done
		shown = false;
		//re-hide this button
		satisfied = true;

	//exit the program
	} else if( which == 2 ){
		main_done = true;
		satisfied = true;
	} else {
			
		//do no work until they hit yes or no
		return;
	}

}

void exit_button::print_me(){

	button::print_me();
	no_area.print_me();
	yes_area.print_me();
}


void exit_button::init(string image_name_in, string image_p_in){
	//run default procedure
	button::init(image_name_in,image_p_in);

	//change up corner info,exit button has special location
	set_corner_loc();

	//exit dialogue defaults to hidden
	shown = false;

	//configure active areas
	no_area.xloc = xloc + 30;
	no_area.yloc = yloc + 30;
	no_area.width = 70;
	no_area.height = 50;

	//this 195 number was determined by trial and error,
	//to get the box's actual dimensions to line up with the image's.
	//I suppose measuring the pixel distance in a photo editor would have been more efficient
	yes_area.xloc = xloc + 195;
	yes_area.yloc = yloc + 30;
	yes_area.width = 70;
	yes_area.height = 50;

}
//##############################################################################


//############################## text_box button ###############################
//TC_file_button::~TC_file_button(){

//}

void text_box_button::draw_me(){
	if(shown){

		SDL_RenderCopy(sdl_access->renderer,button_texture,NULL,&my_rect);
		my_text_box.draw_me();
	}
}
void text_box_button::handle_resize(int yloc_in){
	button::handle_resize(yloc_in);//do parent's work
	my_text_box.yloc = yloc_in + height - 25;
	my_text_box.make_rect();
}

void text_box_button::print_me(){
	button::print_me();
	my_text_box.print_me();
}
	
void text_box_button::init(const std::string& image_name_in,const std::string& image_p_in){
	button::init(image_name_in,image_p_in/*,sdl_help_in*/);

	my_text_box.init(sdl_access->font,"",xloc,yloc+height-25,width,25);
}

void text_box_button::force_corner_loc(int xloc_in, int yloc_in){

	//run default force_corner_loc
	button::force_corner_loc(xloc_in,yloc_in);

	//update the text box's location
	my_text_box.xloc = xloc_in;
	my_text_box.yloc = yloc_in + height - 25;
	my_text_box.make_rect();
}

//###############################################################################################

//###################### TRANSMISSION COEFFICIENTS FILE BUTTON ##################################
int TC_dir_button::work(){

	//set up the TC input var in the input maker
	io_access->TC_input_file_name = my_text_box.text;

	if(my_text_box.text.size() == 0 || my_text_box.text == " "){
			//return -1 to let button_manager.clean_up() know that a TC
			// input file has not been supplied. This should halt output,
			//because there's no reason to make an HF calculation without
			//transmission coefficients (I think)
			return -1;

	}
	ifstream ins_test;
	string ins_path(HOME);
	ins_path += "/Andiamo/TC_files/"+io_access->TC_input_file_name;
	ins_test.open(ins_path);
        if(ins_test.fail()){

		//return -1, failed to open file
		return -1;

	}
	ins_test.close();
	//if we make it this far, we're good. Return a good message
	return 0;
}

//##############################################################################

//######################## CREATED HF FILE OUTPUT BUTTON #######################

int output_file_button::work(/*input_maker& io_handler*/){
	if(my_text_box.text.size() == 0 || my_text_box.text == " "){
		error_logger.push_error("Output file name was not supplied, using the default \"output.txt\".");
		return -1;
	} else {
		//set up the output file name var in input maker
		io_access->output_file_name = my_text_box.text;

	}
	return 0;
}
//##############################################################################

//########################## GRAPHING OPTIONS BUTTON ###########################

/*
graphing_button::~graphing_button(){
	SDL_FreeSurface(checked_surface);
	SDL_DestroyTexture(checked_texture);
}


void graphing_button::draw_me(){
	if(shown){
		//show the version with the check box checked
		if(show_check_version){
			SDL_RenderCopy(sdl_access->renderer,checked_texture,NULL,&my_rect);
		//show the version with the check box unchecked
		} else {
			SDL_RenderCopy(sdl_access->renderer,button_texture,NULL,&my_rect);
		}
		//the graphing_button class doesn't call the text_box_button draw function because
		//it can render the wrong version of the texture. It causes a bug where the check mark
		//mode is on, and the graphing_button draws the check box, and then the text_box_button.draw_me()
		//draws on top of it
		my_text_box.draw_me();
	}

}

void graphing_button::print_me(){
	text_box_button::print_me();
	error_logger.push_msg("CHECK SURFACE: "+to_string(size_t(checked_surface))+" CHECK TEXTURE: "
                              +to_string(size_t(checked_texture)));
	error_logger.push_msg("SHOW CHECK BOOLEAN: "+to_string(show_check_version));
	check_box.print_me();
}

int graphing_button::work(){
	return 0;
}

bool graphing_button::handle_click(SDL_Event& mouse_event){
	//check or uncheck the check box
	if( check_box.clicked(mouse_event) ){

		//if already showing check, hide it
		if(show_check_version){
			error_logger.push_msg("Toggling show_check to false");
			show_check_version = false;

		} else {//if not already showing check, show it
			error_logger.push_msg("Toggling show_check to true");
			show_check_version = true;
		}
		return true;
	}
	return false;
}

void graphing_button::force_corner_loc(int xloc_in, int yloc_in){

	//fix normal x:y w:h values
	text_box_button::force_corner_loc(xloc_in,yloc_in);

	//update the check box's location
	check_box.xloc = xloc_in + 48;
	check_box.yloc = yloc_in + 35;
}

void graphing_button::init(const std::string& image_name_in,const std::string& image_p_in){

	//do the base class's setting up
	text_box_button::init(image_name_in,image_p_in);

	show_check_version = false;

	//set up the active area for the check box
	check_box.xloc = xloc + 48;
	check_box.yloc = yloc + 32;
	check_box.width = 35;
	check_box.height = 35;


	//set up the checked version texture
	checked_surface = IMG_Load( (image_p_in+"graphing_options_checked.png").c_str() );
	if(checked_surface == NULL) error_logger.push_error(string(SDL_GetError()));
	checked_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,checked_surface);
	if(checked_texture == NULL) error_logger.push_error(string(SDL_GetError()));
}
*/
//##############################################################################




//################## FOP HANDLER BUTTON ########################################
void fop_handler_button::click_helper(SDL_Event& mouse_event){
	work();

}

void fop_handler_button::work(){
	cout << "Message from the fop_button." << endl;
	FOP_access->fop_main();
}
//##############################################################################

void save_context_button::click_helper(SDL_Event& mouse_event){

    cout << "In save_context click_helper" << endl;
    work();

}

int save_context_button::work(){

    ofstream context_out;
    context_out.open(HOME+"/Andiamo/config/custom_configs/"+my_text_box.text);
    if(!context_out.fail()){
        save_fields(context_out);
        save_forms(context_out);
    }
    context_out.close();
}

void save_context_button::save_fields(ofstream& context_out){
    cout << "In save_context's save_fields() helper function" << endl;
    FIELDS_VEC* fields_ref = &tile_access->fields_order;
    for(uint line = 0; line < fields_ref->size();line++){
        for(uint param = 0; param < (*fields_ref)[line].size(); param++){
            if(((*fields_ref)[line][param])->int4_hook != NULL){
                context_out << "I4 ";
            } else if(((*fields_ref)[line][param])->real8_hook != NULL){
                context_out << "R8 ";
            } else if(((*fields_ref)[line][param])->string_hook != NULL){
                context_out << "C* ";
            } else {
                context_out << "Tile arrays should be removed" << endl;
            }
            context_out << ((*fields_ref)[line][param])->tile_name;
            if(((*fields_ref)[line][param])->string_hook != NULL){
                context_out << "|" << ((*fields_ref)[line][param])->temp_input.size()
                            << "|";
            }
            context_out << " = " << ((*fields_ref)[line][param])->temp_input << endl; 
        }

    }
}

void save_context_button::save_forms(ofstream& context_out){

    cout << "In save_context's save_forms() helper function" << endl;
    
	//save icntrl8's information to the new config file
	button_access->get_icntrl_8().save_information(context_out);
    context_out << endl;

	//save icntrl6's information to the new config file
	button_access->get_icntrl_6().save_information(context_out);
    context_out << endl;

	//save icntrl10's information to the new config file
    button_access->get_icntrl_10().save_information(context_out);

	//save icntrl4's information to the new config file
	button_access->get_icntrl_4().save_information(context_out);
    context_out << endl;

	//save ilv3/ilv5's information to the new config file
	button_access->get_ilv3_ilv5().save_information(context_out);
    context_out << endl;
}












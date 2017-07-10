//! \file implements the class functions declared in derived_buttons.h

#include<iostream>
#include "derived_buttons.h"
using namespace std;

extern bool main_done;

//################################ EXIT BUTTON ########################################################

exit_button::exit_button(){


}

void exit_button::set_corner_loc(){
	//set center of tile in the exact center of the screen 
	xloc = (sdl_helper->get_win_size()->width / 2) - (width / 2);
	yloc = (sdl_helper->get_win_size()->height / 2) - (height / 2);
}

bool exit_button::handle_click(SDL_Event& mouse_event){
	shown = true;
	bool satisfied = false;
	int which = 0;
	bool did_something = false;
	while(!satisfied){

		draw_me();
		sdl_helper->present();

		SDL_PollEvent(&mouse_event);
		which = my_click(mouse_event);//see if they clicked on the yes or no boxes

		my_click_helper(which,satisfied);
		if(which == 1 || which ==2) did_something = true;
		SDL_Delay(50);
	}
	return did_something;
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


//############################## text_box button ###############################
//TC_file_button::~TC_file_button(){

//}

void text_box_button::draw_me(){
	if(shown){
		SDL_RenderCopy(sdl_helper->renderer,button_texture,NULL,&my_rect);
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
	
void text_box_button::init(const std::string& image_name_in,const std::string& image_p_in,sdl_help* sdl_help_in){
	button::init(image_name_in,image_p_in,sdl_help_in);

	sdl_help_font = sdl_helper->font;

	my_text_box.init(sdl_helper,sdl_help_font," ",xloc,yloc+height-25,width,25);

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
	/*
	cout << "In clean up function." << endl;
	cout << "Input maker's output file name is: " << io_handler.output_file_name <<  endl;
	cout << "Input maker's TC input file name is: " << io_handler.TC_input_file_name << endl;
	cout << "Output file name button's user-entered value is " << output_fname.my_text_box.text << endl;
	cout << "TC file name button's user-entered value is " << t_coefficients.my_text_box.text << endl;
	if(output_fname.my_text_box.text.size() == 0 || output_fname.my_text_box.text == " "){
		cout << "Output file name was not supplied, using default \"output.txt\"." << endl;

	} else {

		io_handler.output_file_name = output_fname.my_text_box.text;
	}
	cout << "Input maker's output path after change: " << io_handler.output_file_name << endl;
*/
//###################### TRANSMISSION COEFFICIENTS FILE BUTTON ##################################
int TC_input_file_button::work(input_maker& io_handler){
	cout << "Hi, I'm Paul! (from TC_input_file_button.work() )" << endl;
	io_handler.TC_input_file_name = my_text_box.text;//set up the TC In put var in the
							 //input maker
	if(my_text_box.text.size() == 0 || my_text_box.text == " "){
			//return -1 to let button_manager.clean_up() know that a TC input file has not
			//been supplied. This should halt output, because there's no reason to make
			//an HF calculation without transmission coefficients (I think)
			return -1;

	}
	ifstream ins_test;
	ins_test.open("./TC_files/"+io_handler.TC_input_file_name);
        if(ins_test.fail()){

		//return -1, failed to open file
		return -1;

	}
	ins_test.close();
	//if we make it this far, we're good. Return a good message
	return 0;
}

//###############################################################################################

//######################## CREATED HF FILE OUTPUT BUTTON ########################################

int output_file_button::work(input_maker& io_handler){
	cout << "Hi, I'm Paul! (from output_file_button.work() )" << endl;
	if(my_text_box.text.size() == 0 || my_text_box.text == " "){
		cout << "Output file name was not supplied, using the default \"output.txt\"." << endl;
		return -1;
	} else {
		io_handler.output_file_name = my_text_box.text;//set up the output file name var
							       //in input maker
	}
	return 0;
}

//##############################################################################################

//########################## GRAPHING OPTIONS BUTTON ###########################################


graphing_button::~graphing_button(){
	SDL_FreeSurface(checked_surface);
	SDL_DestroyTexture(checked_texture);
}

void graphing_button::draw_me(){
	if(shown){
		//show the version with the check box checked
		if(show_check_version){
			SDL_RenderCopy(sdl_helper->renderer,checked_texture,NULL,&my_rect);

		//show the version with the check box unchecked
		} else {
			SDL_RenderCopy(sdl_helper->renderer,button_texture,NULL,&my_rect);
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
	cout << "CHECK SURFACE: " << checked_surface << " CHECK TEXTURE: " << checked_texture << endl;
	cout << "SHOW CHECK BOOLEAN: " << show_check_version << endl;
	check_box.print_me();
}

int graphing_button::work(input_maker& io_handler){
	cout << "Hi, I'm Paul! (from graphing_button.work() )" << endl;
	return 0;
}

bool graphing_button::handle_click(SDL_Event& mouse_event){
	if( check_box.clicked(mouse_event) ){//check or uncheck the check box
		//if already showing check, hide it
		if(show_check_version){
			//cout << "Toggling show_check to false" << endl;
			show_check_version = false;

		} else {//if not already showing check, show it
			//cout << "Toggling show_check to true" << endl;
			show_check_version = true;
		}
		return true;
	}
	return false;
}

void graphing_button::force_corner_loc(int xloc_in, int yloc_in){
	text_box_button::force_corner_loc(xloc_in,yloc_in);//fix normal x:y w:h values

	//update the check box's location
	check_box.xloc = xloc_in + 48;
	check_box.yloc = yloc_in + 35;
}

void graphing_button::init(const std::string& image_name_in,const std::string& image_p_in,sdl_help* sdl_help_in){
	//do the base class's setting up
	text_box_button::init(image_name_in,image_p_in,sdl_help_in);

	show_check_version = false;

	cout << "MY XLOC YLOC: " << xloc << ":" << yloc << endl;
	//set up the active area for the check box
	check_box.xloc = xloc + 48;
	check_box.yloc = yloc + 32;
	check_box.width = 35;
	check_box.height = 35;


	//set up the checked version texture
	checked_surface = IMG_Load( (image_p_in+"graphing_options_checked.png").c_str() );
	if(checked_surface == NULL) cout << SDL_GetError() << endl;
	checked_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,checked_surface);
	if(checked_texture == NULL) cout << SDL_GetError() << endl;
}

//###############################################################################################












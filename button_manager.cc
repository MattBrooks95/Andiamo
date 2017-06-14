//! \file this file implements the class described in button_manager.h

#include "button_manager.h"
using namespace std;


//####################  BUTTONS CLASS  #########################################
button_manager::button_manager(sdl_help* sdl_helper_in){
	sdl_helper = sdl_helper_in;
	button_image_p = "./Assets/Images/Buttons/";

}

button_manager::~button_manager(){

}

void button_manager::init_buttons(){
	default_test.init("default_button.png",button_image_p,sdl_helper);
	exit_dialogue.init("exit_button.png",button_image_p,sdl_helper);



}
void button_manager::print_buttons(){
	cout << "####################### PRINTING BUTTONS ############################" << endl;
	default_test.print_me();
	cout << endl;
	exit_dialogue.print_me();
	cout << "####################### DONE PRINTING BUTTONS #######################" << endl;
}

void button_manager::draw_buttons(){
	default_test.draw_me();
	//exit_dialogue.draw_me();
}

void button_manager::click_handling(SDL_Event& mouse_event){
	if(default_test.shown){
		default_test.handle_click(mouse_event);
	}
	//if( exit_dialogue.shown){

	//	exit_dialogue.handle_click(mouse_event);
	//}

}
//##############################################################################

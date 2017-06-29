//! \file this file implements the class described in button_manager.h

#include "button_manager.h"
using namespace std;


button_manager::button_manager(sdl_help* sdl_helper_in){
	sdl_helper = sdl_helper_in;

	button_image_p = "./Assets/Images/Buttons/";

	tray_image_name = "button_tray.png";

	tray_shown = true;

	tray_rect.x = 0;
	tray_rect.y = 0;
	tray_rect.w = 0;
	tray_rect.h = 0;

	button_tray_surf = NULL;
	button_tray_texture = NULL;
}

button_manager::~button_manager(){
	if(button_tray_surf == NULL || button_tray_texture == NULL){
		cout << "WEIRD, DESTRUCTOR HAS BEEN CALLED TWICE" << endl;
		char junk;
		cin >> junk;

	} //else {
		//cout << "B_MANAGER'S DESTRUCTOR RUNNING" << endl;
	//}
	SDL_FreeSurface(button_tray_surf);
	SDL_DestroyTexture(button_tray_texture);
}

void button_manager::init_tray(){
	//cout << (button_image_p+tray_image_name).c_str() << endl; 
	button_tray_surf = IMG_Load( (button_image_p+tray_image_name).c_str() );
	if(button_tray_surf == NULL) cout << SDL_GetError() << endl;
	button_tray_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,button_tray_surf);
	if(button_tray_texture == NULL) cout << SDL_GetError() << endl;

	//use query texture to get the texture's height and width
	SDL_QueryTexture(button_tray_texture,NULL,NULL,&tray_rect.w,&tray_rect.h);


	tray_rect.x = 5;//give it a little bit of distance from the right edge of the screen
	tray_rect.y = sdl_helper->get_h_bar().get_top() - (tray_rect.h + 10);//set it to be just above the bottom scroll bar

}


void button_manager::init_buttons(){
	//initialize the placeholder buttons
	default_test.init("default_button.png",button_image_p,sdl_helper);
	output_fname.init("output_name_button.png",button_image_p,sdl_helper);
	t_coefficients.init("tc_file_button.png",button_image_p,sdl_helper);
	graphing_options.init("graphing_options.png",button_image_p,sdl_helper);
	lets_go.init("lets_go.png",button_image_p,sdl_helper);

	//exit dialogue is a special snowflake, handles its own location
	exit_dialogue.init("exit_button.png",button_image_p,sdl_helper);

	int end_of_last_button = 0;//keep track of where the last button ended

	//assign their locations
	default_test.force_corner_loc( tray_rect.x+5,tray_rect.y+7 );

		//keep track of where the next button placement should start
		end_of_last_button = end_of_last_button + tray_rect.x+5 + default_test.get_width();
	
	//these two are thin enough to occupy the same horizontal space, with one above and one below
	output_fname.force_corner_loc( end_of_last_button+5, tray_rect.y + 7);
	t_coefficients.force_corner_loc( end_of_last_button+5, tray_rect.y+7 + output_fname.get_height()+10);

		end_of_last_button = end_of_last_button+5+output_fname.get_width();

	graphing_options.force_corner_loc(end_of_last_button+5,tray_rect.y + 7);

		end_of_last_button = end_of_last_button+5+graphing_options.get_width();

	lets_go.force_corner_loc( end_of_last_button+5, tray_rect.y + 7 );


}
void button_manager::print_buttons(){
	cout << "####################### PRINTING BUTTONS ############################" << endl;
	default_test.print_me();
	cout << endl;
	exit_dialogue.print_me();
	cout << endl;
	output_fname.print_me();
	cout << endl;
	t_coefficients.print_me();
	cout << endl;
	lets_go.print_me();
	cout << endl;
	graphing_options.print_me();
	cout << endl;
	cout << "####################### DONE PRINTING BUTTONS #######################" << endl;
}

void button_manager::draw_tray(){
	if(tray_shown){
		SDL_RenderCopy(sdl_helper->renderer,button_tray_texture,NULL,&tray_rect);
	}
}

void button_manager::draw_buttons(){
	draw_tray();
	default_test.draw_me();
	output_fname.draw_me();	
	t_coefficients.draw_me();
	graphing_options.draw_me();
	lets_go.draw_me();

}

void button_manager::text_box_loop(text_box_button* current_button,SDL_Event& event){
	//cout << "Text box was clicked." << endl;


	SDL_StartTextInput();//turn on the text input background functions

	//used to control text entry loop
	bool done = false;
	//int c = 0;
	bool text_was_changed = false;

	string pass_me; //string container for event text info (which is normally a c-string)
	while(!done){
		//if(c >= 10) return;
		//do stuff
		//cout << " in text input mini loop " << c << endl;

		if( !SDL_PollEvent(&event) ){
			event.type = 1776; //dummy event to stop it from printing default message every frame
					   //where no event happens
		}

		switch(event.type){
		  case SDL_MOUSEMOTION:
			cout << "Mouse motion for some reason.... " << endl;
			break;

		  case SDL_MOUSEBUTTONDOWN:
			//if the click was within the text box, move the cursor maybe
		  	if( current_button->my_text_box.was_clicked(event) ){
				//cout << "Text box click at " << event.button.x << ":" << event.button.y << endl;
		  	} else { //elsewise exit text input mode, user clicked off the text box
		  		//cout << "Clicked outside of the text box, exiting mini-loop" << endl;
				SDL_PushEvent(&event);//doing this allows the user to 'hop' to another text box
						      //directly from editing another box
				done = true;
			}
		  	break;

		  case SDL_TEXTINPUT:
		  	//cout << " I guess this was an SDL_TEXTINPUT event... " << endl;
			pass_me = event.text.text;
			current_button->my_text_box.update_text(pass_me);
			text_was_changed = true;
		  	//here this actually causes a loss of letters, so the event flooding is necessary, don't flush
			//SDL_FlushEvent(SDL_TEXTINPUT);
			break;

		  case SDL_KEYDOWN:
		  	//cout << " Key pressed: " << event.key.keysym.sym << endl;
			if(event.key.keysym.sym == SDLK_BACKSPACE){
				//they hit backspace, so delete the end character if it is non-empty
				current_button->my_text_box.back_space();
				text_was_changed = true;
			}
			SDL_FlushEvent(SDL_KEYDOWN); //prevent event flooding
		  	break;
		  case SDL_QUIT:
			//cout << "exiting from text entry" << endl;
			SDL_PushEvent(&event);//puts another sdl quit in the event queue, so program
					      //can be terminated while in "text entry" mode
			done = true;			
			break;

		  case 1776: //do nothing, event was not new
			break;

		  default:
			//outs << "Error finding case in text entry mini-loop" << endl;
			break;
		}

		//if something actually changed, re-draw
		//elsewise don't do it to try and save time
		if(text_was_changed){
			//cout << "HAVING TO REDRAW" << endl;
			//update picture
			sdl_helper->draw_tiles();
			sdl_helper->draw_sbars();
			text_was_changed = false;
			draw_buttons();
			//show updated picture
			sdl_helper->present();
		}

		//c++;
		SDL_Delay(50);
	}//end of loop
	SDL_StopTextInput();//stop text input functionality because it slows down the app

}

void button_manager::click_handling(SDL_Event& mouse_event){
	bool done_something = false;//turn off when a button has actually been clicked
			 	    //no need to check all buttons if one has already been clicked
			 	    //shouldn't be possible to click two at the same time

	cout << "HANDLING BUTTON CLICKS" << endl;
	if(!done_something && default_test.shown){
		if( default_test.handle_click(mouse_event)){
			done_something = true;
		}

	}
	if(!done_something && output_fname.shown){//needs text input handling

		if(output_fname.my_text_box.was_clicked(mouse_event) ){
			text_box_loop(&output_fname,mouse_event);
			done_something = true;
		}

	}
	if(!done_something && t_coefficients.shown){//needs text input handling

		if( t_coefficients.my_text_box.was_clicked(mouse_event) ){
			text_box_loop(&t_coefficients,mouse_event);
			done_something = true;
		}
	}
	if(!done_something && lets_go.shown){
		if( lets_go.handle_click(mouse_event) ){
			done_something = true;
		}
	}
	if(!done_something && graphing_options.shown){
		//needs text input handling and needs to do handle_click for check box
		graphing_options.handle_click(mouse_event);
		if( graphing_options.my_text_box.was_clicked(mouse_event) ) {
			text_box_loop(&graphing_options,mouse_event);
		}
	}
	cout << "DONE HANDLING BUTTON CLICKS" << endl;

}

void button_manager::clean_up(){
	input_maker& io_handler = sdl_helper->get_io_handler();
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

	if(t_coefficients.my_text_box.text == 0 || t_coefficients.my_text_box.text == 0){
	*/
	output_fname.work(io_handler); //set up input_makers output file location variable
	t_coefficients.work(io_handler);//set up input_makers transmission coefficients input file location variable

	

}












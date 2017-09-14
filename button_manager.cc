//! \file this file implements the class described in button_manager.h

#include "button_manager.h"

using namespace std;



button_manager::button_manager(sdl_help* sdl_helper_in){
	sdl_helper = sdl_helper_in;

	button_image_p = "./Assets/Images/Buttons/";

	tray_image_name = "button_tray.png";
	form_tray_image_name = "form_tray.png";


	tray_shown = true;

	tray_rect.x = 0;
	tray_rect.y = 0;
	tray_rect.w = 0;
	tray_rect.h = 0;

	form_tray_rect.x = 0;
	form_tray_rect.y = 0;
	form_tray_rect.w = 0;
	form_tray_rect.h = 0;


	button_tray_surf = NULL;
	button_tray_texture = NULL;

	form_tray_surface = NULL;
	form_tray_texture = NULL;
}

button_manager::~button_manager(){

	SDL_FreeSurface(button_tray_surf);
	SDL_DestroyTexture(button_tray_texture);

	SDL_FreeSurface(form_tray_surface);
	SDL_DestroyTexture(form_tray_texture);
}

void button_manager::init_tray(){
	button_tray_surf = IMG_Load( (button_image_p+tray_image_name).c_str() );
	if(button_tray_surf == NULL) error_logger.push_error(string(SDL_GetError()));
	button_tray_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,button_tray_surf);
	if(button_tray_texture == NULL) error_logger.push_error(string(SDL_GetError()));

	//use query texture to get the texture's height and width
	SDL_QueryTexture(button_tray_texture,NULL,NULL,&tray_rect.w,&tray_rect.h);


	tray_rect.x = 5;//give it a little bit of distance from the right edge of the screen
	tray_rect.y = sdl_helper->get_h_bar().get_top() - (tray_rect.h + 10);//set it to be just above the bottom scroll bar

}

void button_manager::init_form_tray(){

	form_tray_surface = IMG_Load( (button_image_p+form_tray_image_name).c_str() );
	if(form_tray_surface == NULL) error_logger.push_error(SDL_GetError());
	form_tray_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,form_tray_surface);
	if(form_tray_texture == NULL) error_logger.push_error(SDL_GetError());

	SDL_QueryTexture(form_tray_texture,NULL,NULL,&form_tray_rect.w,&form_tray_rect.h);

	form_tray_rect.x = 5;//start on the left edge of the screen, with some space
	form_tray_rect.y = tray_rect.y - form_tray_rect.h;//should be right on top of the button tray 

	init_form_buttons();
}

void button_manager::redo_locks(){



	icntrl_6.set_corner_loc(form_tray_rect.x + 420,form_tray_rect.y);
	icntrl_8.set_corner_loc(form_tray_rect.x + 210,form_tray_rect.y);
	ilv_2.set_corner_loc(form_tray_rect.x + 315,form_tray_rect.y);
	icntrl_10.set_corner_loc(form_tray_rect.x+105,form_tray_rect.y);
	icntrl_4.set_corner_loc(form_tray_rect.x+525,form_tray_rect.y);
	ilv3_ilv5.set_corner_loc(form_tray_rect.x+630,form_tray_rect.y);

/*
	icntrl_8.set_corner_loc(form_tray_rect.x + 105,form_tray_rect.y);
	icntrl_6.set_corner_loc(form_tray_rect.x + 210,form_tray_rect.y);
	ilv_2.set_corner_loc(form_tray_rect.x + 315,form_tray_rect.y);
	icntrl_10.set_corner_loc(form_tray_rect.x+420,form_tray_rect.y);
	icntrl_4.set_corner_loc(form_tray_rect.x+525,form_tray_rect.y);
*/

	icntrl_8.make_rect();
	icntrl_6.make_rect();
	ilv_2.make_rect();
	icntrl_10.make_rect();
	icntrl_4.make_rect();
	ilv3_ilv5.make_rect();

	icntrl_8.setup_lock();
	icntrl_6.setup_lock();
	ilv_2.setup_lock();
	icntrl_10.setup_lock();
	icntrl_4.setup_lock();
	ilv3_ilv5.setup_lock();
}

//this follows the logic used in init_buttons
//there is a lot of hard coded stuff, but I'm hoping the button manager won't need changed often
void button_manager::location_update(){
	int new_y = sdl_helper->get_h_bar().get_top() - (tray_rect.h + 10);

	//move buttons to fit the tray
	//+7 is padding from the top of the button tray

	default_test.handle_resize(new_y+7);


	output_fname.handle_resize(new_y+7);
	//make sure t_coefficients stays in the same place relative to output_fname
	t_coefficients.handle_resize(new_y+output_fname.get_height()+17);


	//graphing_options.handle_resize(new_y+7);

	lets_go.handle_resize(new_y+7);

	tray_rect.y = new_y;
	form_tray_rect.y = new_y - form_tray_rect.h;
	redo_locks();
}

void button_manager::init_buttons(){
	//initialize the placeholder buttons
	default_test.init("default_button.png",button_image_p,sdl_helper);
	output_fname.init("output_name_button.png",button_image_p,sdl_helper);
	t_coefficients.init("tc_file_button.png",button_image_p,sdl_helper);
	//graphing_options.init("graphing_options.png",button_image_p,sdl_helper);
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

	//graphing_options.force_corner_loc(end_of_last_button+5,tray_rect.y + 7);

		//end_of_last_button = end_of_last_button+5+graphing_options.get_width();

	lets_go.force_corner_loc( end_of_last_button+5, tray_rect.y + 7 );


}

void button_manager::init_form_buttons(){
	icntrl_6.init(sdl_helper);
	icntrl_8.init(sdl_helper);
	ilv_2.init(sdl_helper);
	icntrl_10.init(sdl_helper);
	icntrl_4.init(sdl_helper);
	ilv3_ilv5.init(sdl_helper);

	icntrl_6.set_corner_loc(form_tray_rect.x + 420,form_tray_rect.y);
	icntrl_8.set_corner_loc(form_tray_rect.x + 210,form_tray_rect.y);
	ilv_2.set_corner_loc(form_tray_rect.x + 315,form_tray_rect.y);
	icntrl_10.set_corner_loc(form_tray_rect.x+105,form_tray_rect.y);
	icntrl_4.set_corner_loc(form_tray_rect.x+525,form_tray_rect.y);
	ilv3_ilv5.set_corner_loc(form_tray_rect.x+630,form_tray_rect.y);

	icntrl_6.make_rect();
	icntrl_8.make_rect();
	ilv_2.make_rect();
	icntrl_10.make_rect();
	icntrl_4.make_rect();
	ilv3_ilv5.make_rect();

	icntrl_6.setup_lock();
	icntrl_8.setup_lock();
	ilv_2.setup_lock();
	icntrl_10.setup_lock();
	icntrl_4.setup_lock();
	ilv3_ilv5.setup_lock();

	icntrl_6.setup_help_msg();
	icntrl_8.setup_help_msg();
	ilv_2.setup_help_msg();
	icntrl_10.setup_help_msg();
	icntrl_4.setup_help_msg();
	ilv3_ilv5.setup_help_msg();

	icntrl_6.init_form();
	icntrl_8.init_form();
	ilv_2.init_form();
	icntrl_10.init_form();
	icntrl_4.init_form();
	ilv3_ilv5.init_form();
}

void button_manager::print_buttons(){
	error_logger.push_msg("####################### PRINTING BUTTONS ############################");
	default_test.print_me();

	exit_dialogue.print_me();

	output_fname.print_me();

	t_coefficients.print_me();

	lets_go.print_me();

	//graphing_options.print_me();

	error_logger.push_msg("####################### DONE PRINTING BUTTONS #######################");
}

void button_manager::draw_tray(){
	if(tray_shown){
		SDL_RenderCopy(sdl_helper->renderer,button_tray_texture,NULL,&tray_rect);
	}
}

void button_manager::draw_form_tray(){
	SDL_RenderCopy(sdl_helper->renderer,form_tray_texture,NULL,&form_tray_rect);
	icntrl_8.draw_lock();
	icntrl_6.draw_lock();
	ilv_2.draw_lock();
	icntrl_10.draw_lock();
	icntrl_4.draw_lock();
	ilv3_ilv5.draw_lock();
	
}

void button_manager::draw_buttons(){
	draw_tray();
	draw_form_tray();
	default_test.draw_me();
	output_fname.draw_me();	
	t_coefficients.draw_me();
	//graphing_options.draw_me();
	lets_go.draw_me();

}

void button_manager::text_box_loop(text_box_button* current_button,SDL_Event& event){


	SDL_StartTextInput();//turn on the text input background functions

	//used to control text entry loop
	bool done = false;
	//int c = 0;
	bool text_was_changed = false;

	string pass_me; //string container for event text info (which is normally a c-string)
	while(!done){
		//if(c >= 10) return;
		//do stuff

		if( !SDL_PollEvent(&event) ){
			event.type = 1776; //dummy event to stop it from printing default message every frame
					   //where no event happens
		}

		switch(event.type){
		  case SDL_MOUSEMOTION:
			break;

		  case SDL_MOUSEBUTTONDOWN:
			//if the click was within the text box, move the cursor maybe
		  	if( current_button->my_text_box.was_clicked(event) ){
				error_logger.push_msg("Text box click at "+to_string(event.button.x)+":"+to_string(event.button.y));
		  	} else { //elsewise exit text input mode, user clicked off the text box
		  		error_logger.push_msg("Clicked outside of the text box, exiting mini-loop");
				SDL_PushEvent(&event);//doing this allows the user to 'hop' to another text box
						      //directly from editing another box
				done = true;
			}
		  	break;

		  case SDL_TEXTINPUT:
			pass_me = event.text.text;
			current_button->my_text_box.update_text(pass_me);
			text_was_changed = true;
		  	//here this actually causes a loss of letters, so the event flooding is necessary, don't flush
			//SDL_FlushEvent(SDL_TEXTINPUT);
			break;

		  case SDL_KEYDOWN:
			
			if(event.key.keysym.sym == SDLK_BACKSPACE){
				//they hit backspace, so delete the end character if it is non-empty
				current_button->my_text_box.back_space();
				text_was_changed = true;
			} else if(event.key.keysym.sym == SDLK_LEFT){
				if(current_button->my_text_box.editing_location > 0){
					current_button->my_text_box.editing_location--;
					text_was_changed = true;
				}
			} else if(event.key.keysym.sym == SDLK_RIGHT){
				if(current_button->my_text_box.editing_location < current_button->my_text_box.text.size()){
					current_button->my_text_box.editing_location++;
					text_was_changed = true;
				}
			}
				
			SDL_FlushEvent(SDL_KEYDOWN); //prevent event flooding
		  	break;
		  case SDL_QUIT:
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
			//update picture
			sdl_helper->draw_tiles();
			sdl_helper->draw_sbars();
			text_was_changed = false;
			draw_buttons();
			//show updated picture
			sdl_helper->present();
		}

		//c++;
		//SDL_Delay(50);
	}//end of loop
	SDL_StopTextInput();//stop text input functionality because it slows down the app

}

bool button_manager::click_handling(SDL_Event& mouse_event){
	bool done_something = false;//turn off when a button has actually been clicked
			 	    //no need to check all buttons if one has already been clicked
			 	    //shouldn't be possible to click two at the same time

	error_logger.push_msg("HANDLING BUTTON CLICKS");
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
			vector<string> bad_input_list;//filled up by update_io_maker if there are issues
						      //that way user can be given a list of errors
			//if everything is in place, go ahead and make the file
			if( clean_up() == 0){

				//update input_maker's info from the tiles
				if( !sdl_helper->get_mgr().update_io_maker(bad_input_list) ){
					//if something went wrong, this code is executed
					bad_tile_input_warnings(bad_input_list);
				} else { //if there were no errors, this is ran
					//have input_maker output to the file
					sdl_helper->get_io_handler().output();
				}
			}
			done_something = true; //don't consider the other cases, this one has been hit
		}
	}
	/*if(!done_something && graphing_options.shown){
		//needs text input handling and needs to do handle_click for check box
		graphing_options.handle_click(mouse_event);
		if( graphing_options.my_text_box.was_clicked(mouse_event) ) {
			text_box_loop(&graphing_options,mouse_event);
			done_something = true;
		}
	}*/


	//############### FORM BUTTON CHECKS #########################################
	if(!done_something){
		SDL_Rect msg_dest = {250,form_tray_rect.y-500,500,500};
		if( icntrl_8.handle_click(mouse_event) ){
			if(icntrl_8.get_is_locked()){
				icntrl_8.draw_help_msg(mouse_event,msg_dest);
			} else {

			}
			done_something = true;
		} else if( icntrl_6.handle_click(mouse_event) ){
			if(icntrl_6.get_is_locked()){
				icntrl_6.draw_help_msg(mouse_event,msg_dest);
			} else {

			}
			done_something = true;

		} else if( ilv_2.handle_click(mouse_event) ){
			if(ilv_2.get_is_locked()){
				ilv_2.draw_help_msg(mouse_event,msg_dest);
			} else {

			}
			done_something = true;

		} else if( icntrl_10.handle_click(mouse_event) ) {
			if(icntrl_10.get_is_locked()){
				icntrl_10.draw_help_msg(mouse_event,msg_dest);
			} else {

			}
			done_something = true;

		} else if( icntrl_4.handle_click(mouse_event)  ) {
			if(icntrl_4.get_is_locked()){
				icntrl_4.draw_help_msg(mouse_event,msg_dest);
			} else {

			}
			done_something = true;

		} else if( ilv3_ilv5.handle_click(mouse_event) ) {
			if(icntrl_4.get_is_locked()){
				icntrl_4.draw_help_msg(mouse_event,msg_dest);
			} else {

			}
			done_something = true;
		}
	}


	//#############################################################################


	error_logger.push_msg("DONE HANDLING BUTTON CLICKS");
	return done_something;//let main know if it should check tiles or not
}

int button_manager::clean_up(){
	input_maker& io_handler = sdl_helper->get_io_handler();
	bool bad_output_fname = false;
	bool bad_tc_input_fname = false;

	//set up input_makers output file location variable
	if(output_fname.work(io_handler) != 0){
		bad_output_fname = true;
	}

	//set up input_makers transmission coefficients input file location variable
	if(t_coefficients.work(io_handler) != 0){
		//exit if t_coefficients.work returns a bad number
		//but it defaults to "output.txt", so it should just run 
		bad_tc_input_fname = true;
	}

	//if either 'bad' flag is true, make the warnings
	if(bad_output_fname || bad_tc_input_fname){
		clean_up_warnings(bad_output_fname,bad_tc_input_fname);
	}

	if(bad_tc_input_fname){
		return -1;//return with error, TC input file must be given
	}
	//elsewise, exit normally
	return 0;//successful exit
}
void button_manager::bad_tile_input_warnings(vector<string>& bad_input_list){
	//sdl_helper
	SDL_Surface* bad_input_msg_surface = IMG_Load("Assets/Images/bad_input_message.png");
	if(bad_input_msg_surface == NULL) error_logger.push_error(string(SDL_GetError()));
	SDL_Texture* bad_input_msg_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,bad_input_msg_surface);
	if(bad_input_msg_surface == NULL) error_logger.push_error(string(SDL_GetError()));
	
	SDL_Rect msg_dest;//calculate where to put the error message
	SDL_QueryTexture(bad_input_msg_texture,NULL,NULL,&msg_dest.w,&msg_dest.h);
	msg_dest.x = (sdl_helper->get_win_size()->width / 2 ) - (.5 * msg_dest.w);
	msg_dest.y = (sdl_helper->get_win_size()->height / 2) - (.5 * msg_dest.h);
	SDL_RenderCopy(sdl_helper->renderer,bad_input_msg_texture,NULL,&msg_dest);
	
	sdl_helper->present();//update the screen to show the message
	SDL_Delay(5000);//delay for 3 seconds so they can read the message




	SDL_FreeSurface(bad_input_msg_surface);
	SDL_DestroyTexture(bad_input_msg_texture);//free memory back up

}
void button_manager::clean_up_warnings(bool bad_output_fname,bool bad_tc_input_fname){
	SDL_Surface* tc_input_error_surf = NULL;
	SDL_Texture* tc_input_error_texture = NULL;

	SDL_Surface* output_fname_error_surf = NULL;
	SDL_Texture* output_fname_error_texture = NULL;




	//make the error message for the output file name
	if(bad_output_fname){

		output_fname_error_surf = IMG_Load("Assets/Images/Buttons/output_fname_err.png");
		if(output_fname_error_surf == NULL) error_logger.push_error(string(SDL_GetError()));

		output_fname_error_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,output_fname_error_surf);
		if(output_fname_error_texture == NULL) error_logger.push_error(string(SDL_GetError()));

		//plan where to draw image
		SDL_Rect dest = {0,0,0,0};
		//get image dimensions
		SDL_QueryTexture(output_fname_error_texture,NULL,NULL,&dest.w,&dest.h);
		
		//yloc should stay the same
		dest.y = sdl_helper->get_win_size()->height/2 - dest.h/2;
		//if we are also going to make an error message box for the transmission coefficient input file
		//we will have to draw to the right of the center of the window
		if(bad_tc_input_fname){
			dest.x = sdl_helper->get_win_size()->width/2 + 5;//constant 5 is padding

		} else {
			//if this is the only error messaeg being printed, draw it dead center
			dest.x = sdl_helper->get_win_size()->width/2 - (dest.w/2);
		}

		SDL_RenderCopy(sdl_helper->renderer,output_fname_error_texture,NULL,&dest);

	}

	//make the error message for the transmission coefficient input file name
	if(bad_tc_input_fname){

		tc_input_error_surf = IMG_Load("Assets/Images/Buttons/TC_input_err.png");
		if(tc_input_error_surf == NULL) cout << SDL_GetError() << endl;
		tc_input_error_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,tc_input_error_surf);
		if(tc_input_error_texture == NULL) cout << SDL_GetError() << endl;

		SDL_Rect dest = {0,0,0,0};

		//plan out where to draw the error message

		//this gets us the texture's width and height
		SDL_QueryTexture(tc_input_error_texture,NULL,NULL,&dest.w,&dest.h);

		//height should stay the same no matter how many error messages are made
		dest.y = sdl_helper->get_win_size()->height/2-dest.h/2;

		//if both output messages have been made, this one will need to the left of the center
		if(bad_output_fname){
			dest.x = sdl_helper->get_win_size()->width/2 - (dest.w + 5);//constant 5 is padding

		} else {
			//if this is the only error message, then it can be exactly centered
			dest.x = sdl_helper->get_win_size()->width/2 - dest.w/2;

		}

		SDL_RenderCopy(sdl_helper->renderer,tc_input_error_texture,NULL,&dest);
	}


	sdl_helper->present();//show the error messages to the screen
	SDL_Delay(5000);//delay for 5 seconds, so they can read the messages

	//free up memory, this stuff is temporary
	if(output_fname_error_surf != NULL) SDL_FreeSurface(output_fname_error_surf);
	if(output_fname_error_texture != NULL) SDL_DestroyTexture(output_fname_error_texture);

	if(tc_input_error_surf != NULL) SDL_FreeSurface(tc_input_error_surf);
	if(tc_input_error_texture != NULL) SDL_DestroyTexture(tc_input_error_texture);
}















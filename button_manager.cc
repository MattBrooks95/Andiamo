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


	graphing_options.handle_resize(new_y+7);

	lets_go.handle_resize(new_y+7);

	tray_rect.y = new_y;

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
		//SDL_Delay(50);
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
			//if everything is in place, go ahead and make the file
			if( clean_up() == 0){

				//update input_maker's info from the tiles
				sdl_helper->get_mgr().update_io_maker();
				//have input_maker output to the file
				sdl_helper->get_io_handler().output();
				done_something = true;
			}
			done_something = true; //don't consider the other cases, this one has been hit
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

void button_manager::clean_up_warnings(bool bad_output_fname,bool bad_tc_input_fname){
	SDL_Surface* tc_input_error_surf = NULL;
	SDL_Texture* tc_input_error_texture = NULL;

	SDL_Surface* output_fname_error_surf = NULL;
	SDL_Texture* output_fname_error_texture = NULL;




	//make the error message for the output file name
	if(bad_output_fname){

		output_fname_error_surf = IMG_Load("Assets/Images/Buttons/output_fname_err.png");
		if(output_fname_error_surf == NULL) cout << SDL_GetError() << endl;

		output_fname_error_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,output_fname_error_surf);
		if(output_fname_error_texture == NULL) cout << SDL_GetError() << endl;

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















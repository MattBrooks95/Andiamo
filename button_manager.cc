//! \file button_manager.cc implements the class described in button_manager.h

#include "system_wrapper.h"
#include "button_manager.h"
#include "asset_manager.h"

#include "regex_patterns.h"

using namespace std;


extern asset_manager* asset_access;
extern manager* tile_access;
extern system_wrapper* system_access;

#define BUTTON_IMAGE_PATH "Images/Buttons/"

button_manager::button_manager(){
	button_image_p       = BUTTON_IMAGE_PATH;
	tray_image_name      = "button_tray.png";
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

	button_tray_texture = NULL;
	form_tray_texture   = NULL;
}

button_manager::~button_manager(){

}

void button_manager::init_everything(){
	init_tray();
	init_buttons();
	init_form_tray();
}

void button_manager::init_tray(){
	button_tray_texture = asset_access->get_texture(button_image_p +
													tray_image_name);

	if(button_tray_texture == NULL){
		logger_access->push_error(string(SDL_GetError()));
	}

	//use query texture to get the texture's height and width
	SDL_QueryTexture(button_tray_texture,NULL,NULL,&tray_rect.w,&tray_rect.h);

	//give it a little bit of distance from the right edge of the screen
	tray_rect.x = 5;

	//set it to be just above the bottom scroll bar
	tray_rect.y = sdl_access->get_h_bar().get_top() - (tray_rect.h + 10);
}

void button_manager::init_form_tray(){

	string form_tray_arg;
	form_tray_arg = button_image_p+form_tray_image_name;
	form_tray_texture = asset_access->get_texture(form_tray_arg);
	if(form_tray_texture == NULL) logger_access->push_error(SDL_GetError());

	SDL_QueryTexture(form_tray_texture,NULL,NULL,
					 &form_tray_rect.w,&form_tray_rect.h);

	//start on the left edge of the screen, with some space
	form_tray_rect.x = 5;

	//should be right on top of the button tray
	form_tray_rect.y = tray_rect.y - form_tray_rect.h;

	init_form_buttons();
}

void button_manager::redo_locks(){

	icntrl_6.set_corner_loc(form_tray_rect.x + 315,form_tray_rect.y);
	icntrl_8.set_corner_loc(form_tray_rect.x + 210,form_tray_rect.y);
	icntrl_10.set_corner_loc(form_tray_rect.x+105,form_tray_rect.y);
	icntrl_4.set_corner_loc(form_tray_rect.x+420,form_tray_rect.y);
	ilv3_ilv5.set_corner_loc(form_tray_rect.x+525,form_tray_rect.y);

	icntrl_8.make_rect();
	icntrl_6.make_rect();
	icntrl_10.make_rect();
	icntrl_4.make_rect();
	ilv3_ilv5.make_rect();

	icntrl_8.setup_lock();
	icntrl_6.setup_lock();
	icntrl_10.setup_lock();
	icntrl_4.setup_lock();
	ilv3_ilv5.setup_lock();
}

//this follows the logic used in init_buttons
//there is a lot of hard coded stuff, but I'm hoping the
//button manager won't need changed often

void button_manager::location_update(){
	int new_y = sdl_access->get_h_bar().get_top() - (tray_rect.h + 10);

	//move buttons to fit the tray
	//+7 is padding from the top of the button tray

	fop_button.handle_resize(new_y+7);


	output_fname.handle_resize(new_y+7);
	//make sure tc_dir stays in the same place relative to output_fname
	tc_dir.handle_resize(new_y+output_fname.get_height()+17);


	//graphing_options.handle_resize(new_y+7);

	lets_go.handle_resize(new_y+7);
	save_context.handle_resize(new_y+7);


	//update the form tray
	tray_rect.y = new_y;
	form_tray_rect.y = new_y - form_tray_rect.h;
	redo_locks();
	icntrl_6.update_landing();

	//update the landing button's window dimension variables
	exit_dialogue.set_corner_loc();

}

void button_manager::init_buttons(){

	//initialize the placeholder buttons
	fop_button.init(button_image_p + "fop_button.png");
	output_fname.init(button_image_p + "output_name_button.png");
	tc_dir.init(button_image_p + "tc_file_button.png");
	//graphing_options.init("graphing_options.png",button_image_p);
	lets_go.init(button_image_p + "lets_go.png");
	save_context.init(button_image_p + "save_context.png");
	save_context.init_confirmation(button_image_p + "save_context_confirm.png");

	//exit dialogue is a special snowflake, handles its own location
	exit_dialogue.init(button_image_p + "exit_button.png");

	int end_of_last_button = 0;//keep track of where the last button ended

	//assign their locations
	fop_button.force_corner_loc( tray_rect.x+5,tray_rect.y+7 );

	//keep track of where the next button placement should start
	end_of_last_button = end_of_last_button + tray_rect.x + 5 +
						 fop_button.get_width();

	//these two are thin enough to occupy the same horizontal space,
	//with one above and one below
	output_fname.force_corner_loc( end_of_last_button+5, tray_rect.y + 7);
	tc_dir.force_corner_loc( end_of_last_button+5,
								tray_rect.y+7 + output_fname.get_height()+10);

	end_of_last_button = end_of_last_button+5+output_fname.get_width();

	//graphing_options.force_corner_loc(end_of_last_button+5,tray_rect.y + 7);

	//end_of_last_button = end_of_last_button+5+graphing_options.get_width();

	//lets_go.force_corner_loc( end_of_last_button+5, tray_rect.y + 7 );
	save_context.force_corner_loc( end_of_last_button+5,tray_rect.y + 7);
	end_of_last_button = end_of_last_button+5+save_context.get_width();
	lets_go.force_corner_loc( end_of_last_button+5, tray_rect.y + 7 );

}

void button_manager::init_form_buttons(){

	icntrl_6.init();

	icntrl_8.init();
	icntrl_10.init();
	icntrl_4.init();
	ilv3_ilv5.init();

	icntrl_6.set_corner_loc(form_tray_rect.x + 315,form_tray_rect.y);
	icntrl_8.set_corner_loc(form_tray_rect.x + 210,form_tray_rect.y);
	icntrl_10.set_corner_loc(form_tray_rect.x+105,form_tray_rect.y);
	icntrl_4.set_corner_loc(form_tray_rect.x+420,form_tray_rect.y);
	ilv3_ilv5.set_corner_loc(form_tray_rect.x+525,form_tray_rect.y);

	icntrl_6.make_rect();
	icntrl_8.make_rect();
	icntrl_10.make_rect();
	icntrl_4.make_rect();
	ilv3_ilv5.make_rect();

	icntrl_6.setup_lock();
	icntrl_8.setup_lock();
	icntrl_10.setup_lock();
	icntrl_4.setup_lock();
	ilv3_ilv5.setup_lock();

	icntrl_6.setup_help_msg();
	icntrl_8.setup_help_msg();
	//icntrl_10.setup_help_msg();
	icntrl_4.setup_help_msg();
	ilv3_ilv5.setup_help_msg();

	vector<regex*> icntrl_6_patterns;
	vector<regex*> icntrl_8_patterns;
	// vector<regex> ilv_2_patterns;
	vector<regex*> icntrl_4_patterns;
	vector<regex*> ilv3_ilv5_patterns;
	fill_regex_vectors(icntrl_6_patterns,icntrl_8_patterns, 
						icntrl_4_patterns, ilv3_ilv5_patterns);

	icntrl_6.init_form(icntrl_6_patterns);
	icntrl_8.init_form(icntrl_8_patterns);
	icntrl_4.init_form(icntrl_4_patterns);
	ilv3_ilv5.init_form(ilv3_ilv5_patterns);

}

void button_manager::fill_regex_vectors(vector<regex*>& icntrl_6_patterns,
										vector<regex*>& icntrl_8_patterns,
										vector<regex*>& icntrl_4_patterns,
										vector<regex*>& ilv3_ilv5_patterns){

	//decimals as field width, but fortran only requires there
	//to be a decimal, so something like 1776. would be acceptable
	//if precision is too big, it will work, but it'll get cut off
	//need - in there if negative

	//this is passed for columns that don't care about regular
	//expressions, like the column of row labels in the icntrl6
	//parity form
	//regex dummy(".*");
	regex* int5  = regex_access->get_regular_expression(RE_INT5);
	regex* f10_3 = regex_access->get_regular_expression(RE_F10_3);
	regex* f8_4  = regex_access->get_regular_expression(RE_F8_4);
	regex* f10_4 = regex_access->get_regular_expression(RE_F10_4);
	regex* f5_2  = regex_access->get_regular_expression(RE_F5_2);
	regex* f5_1  = regex_access->get_regular_expression(RE_F5_1);
	regex* f7_3  = regex_access->get_regular_expression(RE_F7_3);


	//set up icntrl_6 tests
	//for the inm1 form
	icntrl_6_patterns.push_back(int5); //0
	//1 2 3 4 5 6 7 8 9
	for(int c = 0; c < 9; c++){ //this line has many columns
		icntrl_6_patterns.push_back(f8_4);
	}
	//for the inm2 form
	icntrl_6_patterns.push_back(int5);  //10
	icntrl_6_patterns.push_back(int5);  //11
	icntrl_6_patterns.push_back(f10_4); //12
	icntrl_6_patterns.push_back(f10_4); //13

	//for the parity form
	icntrl_6_patterns.push_back(f8_4);  //14
	icntrl_6_patterns.push_back(int5);  //15

	//set up icntrl_8 tests
	icntrl_8_patterns.push_back(int5);
	icntrl_8_patterns.push_back(int5);
	icntrl_8_patterns.push_back(f10_3);

	//set up icntrl_4 tests################################################
	//line 8
	icntrl_4_patterns.push_back(int5);
	icntrl_4_patterns.push_back(int5);

	//line 9
	icntrl_4_patterns.push_back(f5_2);
	icntrl_4_patterns.push_back(f5_1);
	icntrl_4_patterns.push_back(int5);
	icntrl_4_patterns.push_back(f5_1);
	//#####################################################################

	//set up ilv3_ilv5 tests
	ilv3_ilv5_patterns.push_back(int5);
	ilv3_ilv5_patterns.push_back(int5);
	ilv3_ilv5_patterns.push_back(f7_3);
	ilv3_ilv5_patterns.push_back(f7_3);

}

void button_manager::print_buttons(){

	string print_msg;
	print_msg = "#################### PRINTING BUTTONS #####################";
	logger_access->push_msg(print_msg);
	fop_button.print_me();

	exit_dialogue.print_me();

	output_fname.print_me();

	tc_dir.print_me();

	lets_go.print_me();

	save_context.print_me();

	//graphing_options.print_me();
	string done_print_msg;
	done_print_msg = "############# DONE PRINTING BUTTONS ####################";
	logger_access->push_msg(done_print_msg);
}

void button_manager::draw_all(){

	draw_tray();
	draw_form_tray();
	draw_buttons();
	draw_form_buttons();
}

void button_manager::draw_tray(){
	if(tray_shown){
		SDL_RenderCopy(sdl_access->renderer,button_tray_texture,NULL,&tray_rect);
	}
}

void button_manager::draw_form_tray(){

	//draw the tray first
	SDL_RenderCopy(sdl_access->renderer,form_tray_texture,NULL,&form_tray_rect);

	icntrl_8.draw_lock();
	icntrl_6.draw_lock();
	icntrl_10.draw_lock();
	icntrl_4.draw_lock();
	ilv3_ilv5.draw_lock();

}

void button_manager::draw_form_buttons(){
	icntrl_8.draw_me();
	icntrl_6.draw_me();
	icntrl_10.draw_me();
	icntrl_4.draw_me();
	ilv3_ilv5.draw_me();
}

void button_manager::draw_buttons(){

	draw_tray();
	draw_form_tray();
	fop_button.draw_me();
	output_fname.draw_me();
	tc_dir.draw_me();
	//graphing_options.draw_me();
	lets_go.draw_me();
	save_context.draw_me();

	//these are drawn last, so that if they are open
	//they can't be overdrawn by anything else
	draw_form_buttons();
}

bool button_manager::click_handling(SDL_Event& mouse_event){
	//turn true when a button has actually been clicked
	//no need to check all buttons if one has already been clicked
	//shouldn't be possible to click two at the same time
	bool done_something = false;

	logger_access->push_msg("HANDLING BUTTON CLICKS");
	if(fop_button.shown){
		if( fop_button.handle_click(mouse_event)){
			done_something = true;
		}

	}
	if(!done_something && output_fname.shown){

		if(output_fname.my_text_box.was_clicked(mouse_event) ){
			string command;
			output_fname.my_text_box.edit_loop(mouse_event,command);
			done_something = true;
		}

	}
	if(!done_something && tc_dir.shown){

		if( tc_dir.my_text_box.was_clicked(mouse_event) ){
			string command;
			tc_dir.my_text_box.edit_loop(mouse_event,command);
			done_something = true;
		}

	}
	if(!done_something && lets_go.shown){
		if( lets_go.handle_click(mouse_event) ){

			//filled up by update_io_maker if there are issues
			//that way user can be given a list of errors
			vector<string> bad_input_list;

			//if everything is in place, go ahead and make the file
			if( clean_up(mouse_event) == 0){

				//update input_maker's info from the tiles
				if( /*!io_access->grab_values(bad_input_list) &&*/
					bad_input_list.size() != 0 ){
					//if something went wrong, this code is executed
					bad_tile_input_warnings(bad_input_list);
				} else {
					//if there were no errors, this is ran
					//have input_maker output to the file
					vector<string> form_bad_inputs;

					if(!io_access->output(form_bad_inputs)){
						//set up the texture to draw the error message
						SDL_Texture* error_message = NULL;
						SDL_Rect destination;
						make_form_error_message(form_bad_inputs,
												error_message,destination);
						if(error_message == NULL){
							logger_access->push_error("Error message failure.");
						} else {
							form_error_message_loop(mouse_event,error_message,
													destination);
						}
					}
				}
			}

			//don't consider the other cases, this one has been hit
			done_something = true;
		}
	}
	if(!done_something && save_context.shown){

		if(save_context.my_text_box.was_clicked(mouse_event)){

			string command;
			save_context.my_text_box.edit_loop(mouse_event,command);
			done_something = true;

		} else if(save_context.handle_click(mouse_event)){

			//cout << "Clicked on the actual button, not the text box." << endl;
			done_something = true;
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


	//############### FORM BUTTON CHECKS ######################################
	if(!done_something){
		//SDL_Rect msg_dest = {250,form_tray_rect.y-500,500,500};
		SDL_Rect msg_dest = {form_tray_rect.x+250,form_tray_rect.y-500,500,500};

		//if cutoff_portion is greater than 0, then we need to
		//"shove" the help message down, so it isn't drawn off the
		//top of the screen
		int cutoff_portion = 0 - (form_tray_rect.y - 500);
		if(cutoff_portion > 0){
			//this shifts the message's drawn destination down to compensate
			//for a window that doesn't have much height to it
			msg_dest.y += cutoff_portion;
		}
		if( icntrl_8.handle_click(mouse_event) ){

			if(icntrl_8.get_is_locked()){

				icntrl_8.draw_help_msg(mouse_event,msg_dest);

			} else {

				//incase a resize happened within the form functions,
				//update the button_manager's location
				location_update();
			}
			done_something = true;

		} else if( icntrl_6.handle_click(mouse_event) ){

			if(icntrl_6.get_is_locked()){

				icntrl_6.draw_help_msg(mouse_event,msg_dest);

			} else {

				location_update();
			}
			done_something = true;

		} else if( icntrl_10.handle_click(mouse_event) ) {

			if(icntrl_10.get_is_locked()){

				icntrl_10.draw_help_msg(mouse_event,msg_dest);

			} else {

				location_update();
			}
			done_something = true;

		} else if( icntrl_4.handle_click(mouse_event)  ) {

			if(icntrl_4.get_is_locked()){

				icntrl_4.draw_help_msg(mouse_event,msg_dest);

			} else {

				location_update();
			}
			done_something = true;

		} else if( ilv3_ilv5.handle_click(mouse_event) ) {

			if(ilv3_ilv5.get_is_locked()){

				icntrl_4.draw_help_msg(mouse_event,msg_dest);

			} else {

				location_update();
			}
			done_something = true;
		}
	}


	//########################################################################


	logger_access->push_msg("DONE HANDLING BUTTON CLICKS");
	//let main know if it should check tiles or not
	return done_something;
}

void button_manager::form_error_message_loop(SDL_Event& event,
		SDL_Texture* message_texture, SDL_Rect& destination){

	//save the destination parameter so it can be reset
	//by the user hitting space
	SDL_Rect dest_copy = destination;

	bool changed = false;
	bool leave   = false;

	bool scrolled = false;

	SDL_RenderCopy(sdl_access->renderer,message_texture,NULL,&destination);
	sdl_access->present();

	while(!leave){

		if( SDL_PollEvent(&event) == 0){
			//if there was no event to pull, this code prevents
			//the loop from continuing to use the old event
			//this stops it from scrolling indefinitely if you only
			//move the mouse wheel once
			scrolled = false;
			SDL_Event dummy;
			dummy.type = 1776;
			SDL_PushEvent(&dummy);
		}

		switch(event.type){

			case SDL_QUIT:
				//leave message and prompt to exit Andiamo!
				//on SDL_Quit signal
				SDL_Event push_me;
				push_me.type = SDL_QUIT;
				SDL_PushEvent(&push_me);
				return;

			case SDL_KEYDOWN:
				scrolled = false;
				switch(event.key.keysym.sym){

					//literal down arrow key
					case SDLK_DOWN:
						destination.y -= 5;
						changed = true;
						break;

					//scroll down
					case SDLK_UP:
						destination.y += 5;
						changed = true;
						break;

					//scroll right
					case SDLK_RIGHT:
						destination.x -= 5;
						changed = true;
						break;

					//scroll left
					case SDLK_LEFT:
						destination.x += 5;
						changed = true;
						break;

					//reset the scrolling to how it was when
					//this loop function was called
					case SDLK_SPACE:
						destination = dest_copy;
						changed = true;
						break;

					//leave message if they hit q or escape
					case SDLK_q:
					case SDLK_ESCAPE:
						leave = true;
						break;
				}
				break;

			case SDL_MOUSEWHEEL:
				if(!scrolled){
					destination.x += event.wheel.x * 3;
					destination.y += event.wheel.y * 5;
					changed  = true;
					scrolled = true;
				}
				//make it not get flooded with scroll commands
				SDL_FlushEvent(SDL_MOUSEWHEEL);
				break;

			default:
				scrolled = false;

		}

		if(changed && !leave){
			changed = false;

			SDL_RenderClear(sdl_access->renderer);
			SDL_RenderCopy(sdl_access->renderer,message_texture,NULL,
								&destination);

			sdl_access->present();
		}

	}

}

void button_manager::make_form_error_message(const vector<string>& form_bad_inputs,
											 SDL_Texture*& drawing_info,
											 SDL_Rect& destination){

	if(form_bad_inputs.size() == 0){
		string err = "make_form_error_message was called with an empty list.";
		logger_access->push_error(err);
		return;
	}

	//set up the surface's pixel masks. I don't fully understand this but it's from
	//the sdl documentation  https://wiki.libsdl.org/SDL_CreateRGBSurface
	Uint32 red,green,blue,alpha;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	red   = 0xff000000;
	green = 0x00ff0000;
	blue  = 0x0000ff00;
	alpha = 0x000000ff;
	#else
	red   = 0x000000ff;
	green = 0x0000ff00;
	blue  = 0x00ff0000;
	alpha = 0xff000000;
	#endif

	int max_message_width = 0;
	int message_height    = 0;
	//it's unfortunate to have to loop over the inputs here to figure how big
	//of a background to make, but this is crucial to the scrolling being a boon
	//to the visualization of the errors
	for(unsigned int c = 0; c < form_bad_inputs.size();c++){
		int temp_width;
		int temp_height;
		TTF_SizeText(sdl_access->font,form_bad_inputs[c].c_str(),&temp_width,
					 &temp_height);
		if(temp_width > max_message_width){
			max_message_width = temp_width;
		}
		message_height += temp_height;
	}


	//get window info to figure out what we're drawing to
	int window_h = sdl_access->get_win_size()->height;
	int window_w = sdl_access->get_win_size()->width;

	if(max_message_width < window_w){
		max_message_width = window_w;
	}
	if(message_height < window_h){
		message_height = window_h;
	}
	destination = {0,0,max_message_width,message_height};



	//create a surface w/o a source image that can be blittted to for making error messages
	SDL_Surface* message_surf = SDL_CreateRGBSurface(0,max_message_width,message_height,32,red,green,blue,alpha);

	//draw in the created surface with the background color
	SDL_FillRect(message_surf,NULL,SDL_MapRGBA(message_surf->format,OU_GREEN));

	//define the color for the text to be plastered onto the error message
	SDL_Color text_color = {BLACK};

	//keep track of where to draw the next line
	SDL_Rect line_destination = {0,0,0,0};

	//loop over lines that need rendered to the screen
	for(unsigned int c = 0; c < form_bad_inputs.size();c++){
		SDL_Surface* text_surface;

		text_surface = TTF_RenderUTF8_Blended(sdl_access->font,form_bad_inputs[c].c_str(),text_color);
		TTF_SizeText(sdl_access->font,form_bad_inputs[c].c_str(),&line_destination.w,
					 &line_destination.h);
		SDL_BlitSurface(text_surface,NULL,message_surf,&line_destination);
		line_destination.y += line_destination.h;

		//give memory for this line back
		SDL_FreeSurface(text_surface);
	}

	//turn the surface we've created and drawn to into a texture
	drawing_info = SDL_CreateTextureFromSurface(sdl_access->renderer,message_surf);

	//give back memory
	if(message_surf != NULL){
		SDL_FreeSurface(message_surf);
	}

}

int button_manager::clean_up(SDL_Event& big_event){

	//start off assuming the user has not specified
	//an alternative output file name
	bool bad_output_fname = false;

	//check the fop handler to see if the transmission coefficients
	//have been made yet
	bool tc_not_ready = !FOP_access->ready;

	//set up input_makers output file location variable
	//if(output_fname.work(io_handler) != 0){
	if(output_fname.work() != 0){
		bad_output_fname = true;
	}

	//if either 'bad' flag is true, make the warnings
	if(bad_output_fname || tc_not_ready){
		clean_up_warnings(big_event,bad_output_fname,tc_not_ready);
	}

	/* //disabling this until the FOP wrapper is done, so other
	   //parts of the code can be checked
	if(tc_not_ready){
		return -1;//return with error, TC input file must be given
	}*/

	//elsewise, exit normally
	return 0;//successful exit
}
void button_manager::bad_tile_input_warnings(vector<string>& bad_input_list){

	string msg_target(system_access->get_home());
	msg_target += "/Andiamo/Assets/Images/bad_input_message.png";
	SDL_Texture* bad_input_msg_texture = asset_access->get_texture(msg_target);
	if(bad_input_msg_texture == NULL) logger_access->push_error(string(SDL_GetError()));

	//calculate where to put the error message
	SDL_Rect msg_dest;
	SDL_QueryTexture(bad_input_msg_texture,NULL,NULL,&msg_dest.w,&msg_dest.h);

	msg_dest.x = (sdl_access->get_win_size()->width / 2 ) - (.5 * msg_dest.w);
	msg_dest.y = (sdl_access->get_win_size()->height / 2) - (.5 * msg_dest.h);
	SDL_RenderCopy(sdl_access->renderer,bad_input_msg_texture,NULL,&msg_dest);

	//update the screen to show the message
	sdl_access->present();

	//delay for 5 seconds so they can read the message
	SDL_Delay(5000);

	//free memory
	SDL_DestroyTexture(bad_input_msg_texture);

}
void button_manager::clean_up_warnings(SDL_Event& big_event,
								bool bad_output_fname,bool bad_tc_input_fname){

	SDL_Texture* tc_input_error_texture = NULL;

	SDL_Texture* output_fname_error_texture = NULL;

	//make the error message for the output file name
	if(bad_output_fname){

		string out_fname_err_target = "Images/Buttons/output_fname_err.png";
		output_fname_error_texture =
			asset_access->get_texture(out_fname_err_target);
		if(output_fname_error_texture == NULL) logger_access->push_error(string(SDL_GetError()));

		//plan where to draw image
		SDL_Rect dest = {0,0,0,0};
		//get image dimensions
		SDL_QueryTexture(output_fname_error_texture,NULL,NULL,&dest.w,&dest.h);

		//yloc should stay the same
		dest.y = sdl_access->get_win_size()->height/2 - dest.h/2;

		//if we are also going to make an error message box for the
		//transmission coefficient input file we will have to draw to the right
		// of the center of the window
		if(bad_tc_input_fname){
			//constant 5 is padding
			dest.x = sdl_access->get_win_size()->width/2 + 5;
		} else {
			//if this is the only error messaeg being printed, draw it
			//dead center
			dest.x = sdl_access->get_win_size()->width/2 - (dest.w/2);
		}

		SDL_RenderCopy(sdl_access->renderer,output_fname_error_texture,
						NULL,&dest);
	}

	//make the error message for the transmission coefficient input file name
	if(bad_tc_input_fname){

		string bad_tc_input_target(system_access->get_home());
		bad_tc_input_target +=
							"/Andiamo/Assets/Images/Buttons/TC_input_err.png";
		tc_input_error_texture = asset_access->get_texture(bad_tc_input_target);
		if(tc_input_error_texture == NULL) logger_access->push_error(SDL_GetError());

		SDL_Rect dest = {0,0,0,0};

		//plan out where to draw the error message

		//this gets us the texture's width and height
		SDL_QueryTexture(tc_input_error_texture,NULL,NULL,&dest.w,&dest.h);

		//height should stay the same no matter how many error messages are made
		dest.y = sdl_access->get_win_size()->height/2-dest.h/2;

		//if both output messages have been made, this one will need to the left of the center
		if(bad_output_fname){
			//constant 5 is padding
			dest.x = sdl_access->get_win_size()->width/2 - (dest.w + 5);

		} else {
			//if this is the only error message, then it can be exactly
			//centered
			dest.x = sdl_access->get_win_size()->width/2 - dest.w/2;
		}

		SDL_RenderCopy(sdl_access->renderer,tc_input_error_texture,NULL,&dest);
	}


	//show the error messages to the screen
	sdl_access->present();

	//spin until they hit a key, or click somewhere on the screen
	while(SDL_PollEvent(&big_event) == 0 || big_event.type == SDL_MOUSEBUTTONUP);
}

//! \file form_buttons.cc contains the implementations for the classes specified in form_buttons.h

#include <cmath>
#include<iomanip>//for setting precision and field width
#include "form_buttons.h"
#include "button_manager.h"
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
	my_form.init("no title","default_form_help.png",0,0,sdl_helper,sdl_helper->font);

}

void form_button::screen_size(){
	bool update = false;
	int old_width = sdl_helper->get_win_size()->width;
	int old_height = sdl_helper->get_win_size()->height;
	if(old_width < 800){
		old_width = 800; 
		update = true;
	}

	if(sdl_helper->get_win_size()->width < 800){
		old_height = 800;
		update = true;
	}

	if(update){
		SDL_SetWindowSize(sdl_helper->get_window(),old_width,old_height);
		sdl_helper->window_update(old_width,old_height);

		cout << "Updated screen size upon opening a form." << endl;
		error_logger.push_msg("Updated screen size upon opening a form.");
	}
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
	error_logger.push_error("A form button has called the base classes output creation member. Each form button is",
				" likely to have a special format, so you should customize this function in the derived class.");
}

//#############################################################################


//####################### ICNTRL8 BUTTON ######################################

void icntrl8_form_button::setup_help_msg(){
	unlock_help_surface = IMG_Load("Assets/Images/form_assets/icntrl8_form_locked_msg.png");
	if(unlock_help_surface == NULL) error_logger.push_error(SDL_GetError());

	unlock_help_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,unlock_help_surface);
	if(unlock_help_texture == NULL) error_logger.push_error(SDL_GetError());
}

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
		screen_size();

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

void icntrl8_form_button::make_output(){
/*
	vector<page>* pages_ptr = &my_form.get_pages(); //saves calls to the getter, and space in this function

	for(unsigned int c = 0; c < pages_ptr->size();c++){
		unsigned int columns = pages_ptr->at(c).get_columns();

		for(unsigned int d = 0; d < ( pages_ptr->at(c).get_text_boxes().size() / columns) ;d += columns){

			cout << setw(5);
			//cout << pages_ptr->at(c).get_text_boxes().at(d) << pages_ptr->at(c).get_text_boxes().at(d+1);
			cout << pages_ptr->at(c).get_text_boxes()[d];
			cout << setw(10);
			cout << setprecision(3);
			//cout << pages_ptr->at(c).get_text_boxes()[d+2] << endl;


		}

	}
*/
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
		screen_size();
		my_form.toggle_active();//let the form know that it is now active
		my_form.form_event_loop(mouse_event);//enter the mini loop for form entry
	}
}

void ilv2_form_button::init_form(){

	my_form.init("Level Info (ILV2)","default_form_help.png",0,0,sdl_helper,sdl_helper->font);

}


//###############################################################################

//####################### ICNTRL6 BUTTON ########################################
icntrl6_form_button::~icntrl6_form_button(){
	if(landing_surface != NULL){
		SDL_FreeSurface(landing_surface);
	}
	if(landing_texture != NULL){
		SDL_DestroyTexture(landing_texture);
	}


}

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

		//resize the window if necessary
		screen_size();

		//draw the form selection landing 
		show_landing();

		//have sdl_helper update the display
		sdl_helper->present();

		//if we enter a form loop, we should ignore whatever event is in the queue when
		//the form loop exits. So, this flag should be set to true if a form loop is entered,
		//that way the do-while loop will know to not worry about where the most recent click was
		bool did_something = false;

		//enter this loop unconditionally at first, because the user had to have clicked on the form_button
		//to get here
		do{
			//reset did_something flag
			did_something = false;

			//read from queue until a click event happens
			while( !(SDL_PollEvent(&mouse_event) == 1 &&
				(mouse_event.type == SDL_MOUSEBUTTONDOWN || mouse_event.type == SDL_QUIT) ) );

			if(mouse_event.type == SDL_QUIT){
				SDL_PushEvent(&mouse_event);//putting the same event back in the queue
							    //makes it possible to exit andiamo directly
				return;
			}


			if( parity_area.clicked(mouse_event) ){

				parity_page_creation();
				my_form.toggle_active();//let the form know that it is now active
				my_form.form_event_loop(mouse_event);//enter the mini loop for form entry
				did_something = true;

			} else if( spectra_area.clicked(mouse_event) ){

				search_spectra_page_creation();
				search_spectra.toggle_active();
				search_spectra.form_event_loop(mouse_event);
				did_something = true;

			} else if( xsections_area.clicked(mouse_event) ){

				cross_sections_page_creation();
				cross_sections.toggle_active();
				cross_sections.form_event_loop(mouse_event);
				did_something = true;

			}
			SDL_RenderClear(sdl_helper->renderer);//clear off the screen

			sdl_helper->draw_tiles();//redraw the tile field (in the background)
			sdl_helper->draw_sbars();//likewise, redraw the scroll bars

			b_manager->draw_tray(); //redraw the button tray
			b_manager->draw_form_tray(); //redraw the form button tray
			b_manager->draw_buttons();//redraw the buttons themselves
			show_landing();  //redraw the form selection area

			sdl_helper->present();

		//loop until the user clicks off the landing screen
		} while( did_something || landing_was_clicked(mouse_event) );


	}
}

bool icntrl6_form_button::landing_was_clicked(SDL_Event& mouse_event){

		if( (mouse_event.button.x > landing_rect.x && mouse_event.button.x < landing_rect.x + landing_rect.w) &&
		    (mouse_event.button.y > landing_rect.y && mouse_event.button.y < landing_rect.y + landing_rect.h) ){
			return true;
		}
		return false;

}



void icntrl6_form_button::init_form(){

	//set up the image that lets the user switch between this button's different forms
	setup_landing();


	//my_form (from default form_button class) will be the parity info
	my_form.init("Parameter Search (ICNTRL6)","icntrl6_parity_help.png",0,0,sdl_helper,sdl_helper->font);

	//INM1 form
	search_spectra.init("# of Search Spectra","icntrl6_spectra_help.png",0,0,sdl_helper,sdl_helper->font);

	//INM2 form
	cross_sections.init("# of Search Cross Sections","icntrl6_xsections_help.png",0,0,sdl_helper,sdl_helper->font);

}

/*	void page_init(unsigned int num_columns_in, unsigned int num_rows_in,const std::vector<std::string>& column_labels_in,
	     std::vector<std::string>& row_labels_in,sdl_help* sdl_helper_in,TTF_Font* sdl_font_in);

	void column_logic(const std::vector<std::string>& types);
*/

void icntrl6_form_button::parity_page_creation(){
	

}

void icntrl6_form_button::search_spectra_page_creation(){


}

void icntrl6_form_button::cross_sections_page_creation(){


}

void icntrl6_form_button::setup_landing(){

	//setup the graphics pointers
	landing_surface = IMG_Load("./Assets/Images/form_assets/icntrl6_landing.png");
	if(landing_surface == NULL){
		error_logger.push_error("Could not init the icntrl6 form selection surface.",
				        SDL_GetError());
	}
	landing_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,landing_surface);
	if(landing_texture == NULL){
		error_logger.push_error("Could not init the the icntrl6 form selection texture.",
					SDL_GetError());
	}

	//figure out where to place the landing image
	//as it stands right now, the form selection image is 312x70

	//center of landing page should line up w/center of button
	landing_rect.x = xloc + (width * .5) - 156;

	//70 pixels higher
	landing_rect.y = yloc - 70;

	landing_rect.w = 312;
	landing_rect.h = 70;

	//configure the click detection areas for the different form openers
	parity_area.set_loc(landing_rect.x,landing_rect.y,100,70);
	spectra_area.set_loc(landing_rect.x+106,landing_rect.y,100,70);
	xsections_area.set_loc(landing_rect.x+212,landing_rect.y,100,70);

}

void icntrl6_form_button::show_landing(){

	SDL_RenderCopy(sdl_helper->renderer,landing_texture,NULL,&landing_rect);


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
		screen_size();
		my_form.toggle_active();//let the form know that it is now active
		my_form.form_event_loop(mouse_event);//enter the mini loop for form entry
	}
}

void icntrl10_form_button::init_form(){

	my_form.init("Spin Cutoff Information (ICNTRL10)","default_form_help.png",0,0,sdl_helper,sdl_helper->font);

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
		screen_size();
		my_form.toggle_active();//let the form know that it is now active
		my_form.form_event_loop(mouse_event);//enter the mini loop for form entry
	}
}

void icntrl4_form_button::init_form(){

	my_form.init("Resolved Levels (ICNTRL4)","default_form_help.png",0,0,sdl_helper,sdl_helper->font);

}

//################################################################################

//################## IVL4 || ILV5 BUTTON #########################################

bool ilv3_ilv5_form_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		click_helper(mouse_event);
		return true;
	}
	return false;
}

void ilv3_ilv5_form_button::click_helper(SDL_Event& mouse_event){
	error_logger.push_msg("clicked the icntrl4/resolved levels info button ");
	if(!is_locked){
		screen_size();
		my_form.toggle_active();//let the form know that it is now active
		my_form.form_event_loop(mouse_event);//enter the mini loop for form entry
	}
}

void ilv3_ilv5_form_button::init_form(){

	my_form.init("Resolved Levels (ICNTRL4)","default_form_help.png",0,0,sdl_helper,sdl_helper->font);

}


//#################################################################################










//! \file form_buttons.cc contains the implementations for the classes specified in form_buttons.h

#include <cmath>
#include<iomanip>//for setting precision and field width
#include "form_buttons.h"
#include "button_manager.h"
using namespace std;

//copied over from input_maker
#define F << setw(8) <<
#define F10 << setw(10) <<
#define F5 << setw(5) <<
#define I << setw(5) <<
#define I10 << setw(10) <<



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

void form_button::make_output(ofstream& outs){
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
	try{
	  icntrl8_val = stoi(sdl_helper->get_mgr().fields.at("line_6").at("ICNTRL8").temp_input);
	} catch (out_of_range& range_error){
	  error_logger.push_error("ICNTRL8 could not be found in the field map",
				  range_error.what());
	  icntrl8_val = 0;
	} catch (invalid_argument& arg_error){
	  error_logger.push_error("ICNTRL8 has been given an invalid (non-numerical?) argument.",
				  arg_error.what());
	  icntrl8_val = 0;
	}



	error_logger.push_msg("ICNTRL8 val:" + to_string(icntrl8_val)+" when form opened");
	vector<string> pass_column_titles,pass_row_titles;
	//for icntrl 8, the labels shouldn't change
	pass_column_titles.push_back("A to Drop");
	pass_column_titles.push_back("Z to Drop");
	pass_column_titles.push_back("Excitation Energy Cutoff");
	//#########################################

	int rows_per_page = floor(725.0 / 35);
	int rows_needed   = icntrl8_val;
	unsigned int vector_size = ceil((icntrl8_val * 35) / 725.0);//calculate how many pages are needed
	unsigned int pages_made = 0;

	vector<page>& pages = my_form.get_pages();//saves space later
	pages.resize(vector_size);

	vector<int> column_spaces;
	column_spaces.push_back(0);
	column_spaces.push_back(150);
	column_spaces.push_back(150);


	for(unsigned int c = 0; c < pages.size();c++){

		if(rows_per_page >= rows_needed){
			pages[c].page_init(3,rows_needed,pass_column_titles,pass_row_titles,
								      sdl_helper, sdl_helper->font,column_spaces);
			rows_needed = 0;
		} else {

			pages[c].page_init(3,rows_per_page,pass_column_titles,pass_row_titles,
								      sdl_helper, sdl_helper->font,column_spaces);
			rows_needed = rows_needed - rows_per_page;
		}
		pages_made++;//we made a page, so increase the counter
	}
	if(pages_made != vector_size) {
		error_logger.push_error("Error in icntrl8 page_creation_helper, # of created pages does not match",
				       "expected value.");
	}

	my_form.set_page_count(pages_made);

	my_form.prev_initiated = true;//let the form class know that it's pages have been set up
	my_form.prev_init_value = icntrl8_val;//and also what conditions caused such a creation


}

void icntrl8_form_button::init_form(){
	my_form.init("Cutoff Nuclei (ICNTRL8)","icntrl8_form_help.png",0,0,sdl_helper,sdl_helper->font);
}

void icntrl8_form_button::make_output(ofstream& outs){
	if(outs.fail()){
		error_logger.push_error("icntrl8_form_button::make_output was not given a valid output file stream.",
					"exiting.");
		return;
	}


	//outs << "TESTING ICNTRL8'S OUTPUT" << endl;
	vector<page>* pages_ptr = &my_form.get_pages(); //saves calls to the getter, and space in this function

	//loop over each page
	for(unsigned int c = 0; c < pages_ptr->size();c++){

		unsigned int columns = pages_ptr->at(c).get_columns();

		//loop over each row
		for(unsigned int d = 0; d < pages_ptr->at(c).get_text_boxes().size() ;d += columns){

			outs I pages_ptr->at(c).get_text_boxes().at(d).text
			     I pages_ptr->at(c).get_text_boxes().at(d+1).text;
			outs << setw(10);
			outs << setprecision(3);
			outs << pages_ptr->at(c).get_text_boxes()[d+2].text << endl;


		}

	}

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

//this one is kind of unique because the form itself should not change, it has a fixed size in the input manual
void icntrl6_form_button::parity_page_creation(){

	if( !my_form.prev_initiated ){

		my_form.prev_initiated = true;
		my_form.prev_init_value = 18;

		my_form.get_pages().resize(1);//make room for the first and only page

		vector<string> column_labels, row_labels;
		fill_parity_labels(row_labels, column_labels);

		vector<int> column_spaces;
		column_spaces.push_back(0);
		column_spaces.push_back(250);
		column_spaces.push_back(100);

		my_form.get_pages()[0].page_init( 3, 18, column_labels, row_labels, sdl_helper,sdl_helper->font,column_spaces);
		
	}


}

void icntrl6_form_button::fill_parity_labels(vector<string>& row_labels,vector<string>& column_labels){

	//########## ROW LABELS (PARAMETER NAMES) ################################
	row_labels.push_back("α");   //1
	row_labels.push_back("γ");   //2
	row_labels.push_back("F1");  //3
	row_labels.push_back("F2");  //4
	row_labels.push_back("b0");  //5
	row_labels.push_back("b1");  //6
	row_labels.push_back("b2");  //7
	row_labels.push_back("b3");  //8
	row_labels.push_back("b4");  //9
	row_labels.push_back("ECON");//10
	row_labels.push_back("C0");  //11
	row_labels.push_back("C10"); //12
	row_labels.push_back("C11"); //13
	row_labels.push_back("C12"); //14
	row_labels.push_back("C3");  //15
	row_labels.push_back("FF");  //16
	row_labels.push_back("MU");  //17
	row_labels.push_back("18");  //18
	//#################################################################################

	//################# COLUMN LABELS #################################################
	column_labels.push_back("Al-Quraishi Parameter");
	column_labels.push_back("value");
	column_labels.push_back("Vary?");

	//#################################################################################
}

void icntrl6_form_button::search_spectra_page_creation(){

	int current_INM1_val;
	try{
 		current_INM1_val = stoi(sdl_helper->get_mgr().fields.at("line_10").at("INM1").temp_input); 
	} catch(invalid_argument& arg_error){
		error_logger.push_error("Error reading current INM1/#Search Spectra value for page creation",
					" logics.");
	}

	if( !search_spectra.prev_initiated ){

		search_spectra_page_helper();

	//case where form has been previously created, but INM1's val has not changed, so it does not need to be remade
	} else if( search_spectra.prev_initiated && search_spectra.prev_init_value == current_INM1_val ){
		return;

	//case where form has been previously created, and the value of INM1 has been changed
	} else {

		search_spectra.flush_pages();
		search_spectra_page_helper();
	}//*/


}

void icntrl6_form_button::search_spectra_page_helper(){

	try{
	  INM1_val = stoi(sdl_helper->get_mgr().fields.at("line_10").at("INM1").temp_input);
	} catch ( out_of_range& range_error ){
	  error_logger.push_error("ICNTRL6-INM1 could not be found in the field map.",
					  range_error.what());
	  INM1_val = 0;
	} catch ( invalid_argument& arg_error ){
	  error_logger.push_error("ICNTRL6-INM1 has been supplied an illegal (non-numerical?) argument.",
				  arg_error.what());
	  INM1_val = 0;
  	}

	search_spectra.prev_initiated = true;
	search_spectra.prev_init_value = INM1_val;

	vector<string> pass_column_labels,pass_row_labels;
	vector<int> column_spaces;
	fill_spectra_vectors(pass_column_labels,column_spaces);


	int rows_per_page = floor(725.0 / 35);
	int rows_needed   = INM1_val;
	unsigned int vector_size = ceil((INM1_val * 35) / 725.0);//calculate how many pages are needed
	unsigned int pages_made = 0;

	vector<page>& pages = search_spectra.get_pages();//saves space later
	pages.resize(vector_size);


	for(unsigned int c = 0; c < pages.size();c++){

		if(rows_per_page >= rows_needed){
			pages[c].page_init(9,rows_needed,pass_column_labels,pass_row_labels,
								      sdl_helper, sdl_helper->font,column_spaces);
			rows_needed = 0;
		} else {

			pages[c].page_init(9,rows_per_page,pass_column_labels,pass_row_labels,
								      sdl_helper, sdl_helper->font,column_spaces);
			rows_needed = rows_needed - rows_per_page;
		}
		pages_made++;//we made a page, so increase the counter
	}
	if(pages_made != vector_size) {
		error_logger.push_error("Error in icntrl6/search spectra page_creation_helper, # of created pages does not match",
				       "expected value.");
	}

	search_spectra.set_page_count(pages_made);

	search_spectra.prev_initiated = true;//let the form class know that it's pages have been set up
	search_spectra.prev_init_value = INM1_val;//and also what conditions caused such a creation


}

void icntrl6_form_button::fill_spectra_vectors(vector<string>& pass_column_labels,vector<int>& column_spaces){
	//fill column labels
	pass_column_labels.push_back("IFIT");
	pass_column_labels.push_back("SIGFIT");
	pass_column_labels.push_back("DSIGFIT");
	pass_column_labels.push_back("ESIG");
	pass_column_labels.push_back("DESIG");
	pass_column_labels.push_back("E2SIG");
	pass_column_labels.push_back("DE2SIG");
	pass_column_labels.push_back("ELIML");
	pass_column_labels.push_back("ELIMU");

	//fill column spacing info
	column_spaces.push_back(0);
	column_spaces.push_back(75);
	column_spaces.push_back(85);
	column_spaces.push_back(100);
	column_spaces.push_back(85);
	column_spaces.push_back(85);
	column_spaces.push_back(85);
	column_spaces.push_back(85);
	column_spaces.push_back(85);

}

void icntrl6_form_button::cross_sections_page_creation(){


	int current_INM2_val;
	try{
 		current_INM2_val = stoi(sdl_helper->get_mgr().fields.at("line_10").at("INM2").temp_input); 
	} catch(invalid_argument& arg_error){
		error_logger.push_error("Error reading current INM2/cross sections value for page creation",
					" logics.");
	}

	if( !cross_sections.prev_initiated ){

		cross_sections_helper();

	//case where form has been previously created, but INM2's val has not changed, so it does not need to be remade
	} else if( cross_sections.prev_initiated && cross_sections.prev_init_value == current_INM2_val ){
		return;

	//case where form has been previously created, and the value of INM2 has been changed
	} else {

		cross_sections.flush_pages();
		cross_sections_helper();
	}//*/

}

void icntrl6_form_button::cross_sections_helper(){

	try{
	  INM2_val = stoi(sdl_helper->get_mgr().fields.at("line_10").at("INM2").temp_input);
	} catch ( out_of_range& range_error ){
	  error_logger.push_error("ICNTRL6-INM2 could not be found in the field map.",
					  range_error.what());
	  INM2_val = 0;
	} catch ( invalid_argument& arg_error ){
	  error_logger.push_error("ICNTRL6-INM2 has been supplied an illegal (non-numerical?) argument.",
				  arg_error.what());
	  INM2_val = 0;
  	}

	cross_sections.prev_initiated = true;
	cross_sections.prev_init_value = INM2_val;

	//fill in column labels
	vector<string> pass_column_labels,pass_row_labels;
	pass_column_labels.push_back("# p evaporated");
	pass_column_labels.push_back("# n evaporated");
	pass_column_labels.push_back("Integrated Cross Section");
	pass_column_labels.push_back("Integrated CS Error");

	//fill in spacing info
	vector<int> column_spaces;
	column_spaces.push_back(0);
	column_spaces.push_back(140); column_spaces.push_back(140); column_spaces.push_back(220);

	int rows_per_page = floor(725.0 / 35);
	int rows_needed   = INM2_val;
	unsigned int vector_size = ceil((INM2_val * 35) / 725.0);//calculate how many pages are needed
	unsigned int pages_made = 0;

	vector<page>& pages = cross_sections.get_pages();//saves space later
	pages.resize(vector_size);

	for(unsigned int c = 0; c < pages.size();c++){

		if(rows_per_page >= rows_needed){
			//pages[c].page_init(4,rows_needed,pass_column_labels,pass_row_labels,
								     // sdl_helper, sdl_helper->font,120);//prev 15

			pages[c].page_init(4,rows_needed,pass_column_labels,pass_row_labels,
								     sdl_helper, sdl_helper->font,column_spaces);
			rows_needed = 0;
		} else {

			//pages[c].page_init(4,rows_per_page,pass_column_labels,pass_row_labels,
								      //sdl_helper, sdl_helper->font,120);//prev 15
			pages[c].page_init(4,rows_per_page,pass_column_labels,pass_row_labels,
								     sdl_helper, sdl_helper->font,column_spaces);


			rows_needed = rows_needed - rows_per_page;
		}
		pages_made++;//we made a page, so increase the counter
	}
	if(pages_made != vector_size) {
		error_logger.push_error("Error in icntrl6/cross sections page creation_helper, # of created pages does not match",
				       "expected value.");
	}

	cross_sections.set_page_count(pages_made);

	search_spectra.prev_initiated = true;//let the form class know that it's pages have been set up
	search_spectra.prev_init_value = INM2_val;//and also what conditions caused such a creation



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

void icntrl6_form_button::update_landing(){

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

void icntrl6_form_button::make_output(ofstream& outs){
	if(outs.fail()){
		error_logger.push_error("Icntrl6_form_button::make_output was not given a valid output file stream.",
					"exiting.");
		return;
	}	

	//outs << "TESTING ICNTRL6'S OUTPUT" << endl;
	vector<page>& parity_ref = my_form.get_pages();         //handle for accessing parity form's data
	vector<page>& search_ref = search_spectra.get_pages();  //handle for accessing search_spectra form's data
	vector<page>& cross_ref  = cross_sections.get_pages();  //handle for accessing cross_section form's data 

	

	//INM1 is up first
	if(search_ref.size() != 0){
		//loop over search spectra's pages
		for(unsigned int c = 0; c < search_ref.size();c++){
			unsigned int columns = search_ref[c].get_columns();
			outs << "PAGE " << c << endl;
			//loop over each row
			for(unsigned int d = 0; d < search_ref.at(c).get_text_boxes().size(); d += columns){
				outs I search_ref[c].get_text_boxes()[d].text;
				outs << setprecision(4);  //set precision for float numbers
				outs F search_ref[c].get_text_boxes()[d+1].text F search_ref[c].get_text_boxes()[d+2].text;
				outs F search_ref[c].get_text_boxes()[d+3].text F search_ref[c].get_text_boxes()[d+4].text;
				outs F search_ref[c].get_text_boxes()[d+5].text F search_ref[c].get_text_boxes()[d+6].text;
				outs F search_ref[c].get_text_boxes()[d+7].text F search_ref[c].get_text_boxes()[d+8].text;
				outs F search_ref[c].get_text_boxes()[d+9].text << endl;

			}
		}
	}
	//INM2 is up next
	if(cross_ref.size() != 0){
		//loop over search spectra's pages
		for(unsigned int c = 0;c < cross_ref.size();c++){
			outs << "PAGE " << c << endl;
			//loop over each row
			for(unsigned int d = 0; c < cross_ref.size();c++){
				outs I cross_ref[c].get_text_boxes()[d].text I cross_ref[c].get_text_boxes()[d+1].text;
				outs << setprecision(4);
				outs F10 cross_ref[c].get_text_boxes()[d+2].text F10 cross_ref[c].get_text_boxes()[d+3].text << endl;

			}

		}


	}

	//and finally, parity


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

void icntrl4_form_button::init_form(){

	my_form.init("Resolved Levels (ICNTRL4)","icntrl4_form_help.png",0,0,sdl_helper,sdl_helper->font);

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
		} else if(my_form.prev_init_value == stoi(sdl_helper->get_mgr().fields.at("line_8").at("NCH4").temp_input) ){
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
void icntrl4_form_button::page_creation_helper(){

	//grab val from parameter field, so the pages can be set up
	try{
	  nch4_val = stoi(sdl_helper->get_mgr().fields.at("line_8").at("NCH4").temp_input);
	} catch (out_of_range& range_error){
	  error_logger.push_error("NCH4 could not be found in the field map",
				  range_error.what());
	  nch4_val = 0;
	} catch (invalid_argument& arg_error){
	  error_logger.push_error("NCH4 has been given an invalid (non-numerical?) argument.",
				  arg_error.what());
	  nch4_val = 0;
	}



	error_logger.push_msg("NCH4 val:" + to_string(nch4_val)+" when form opened");
	vector<string> pass_column_titles,pass_row_titles;
	//for icntrl 4, the labels shouldn't change
	pass_column_titles.push_back("RL Energy");
	pass_column_titles.push_back("RL Spin");
	pass_column_titles.push_back("RL Parity");
    pass_column_titles.push_back("RL Isospin");
	//#########################################

	int rows_per_page = floor(725.0 / 35);
	int rows_needed   = nch4_val;
	unsigned int vector_size = ceil((nch4_val * 35) / 725.0);//calculate how many pages are needed
	unsigned int pages_made = 0;

	vector<page>& pages = my_form.get_pages();//saves space later
	pages.resize(vector_size);

	vector<int> column_spaces;
	column_spaces.push_back(0);
	column_spaces.push_back(208);
	column_spaces.push_back(184);
    column_spaces.push_back(201);


	for(unsigned int c = 0; c < pages.size();c++){

		if(rows_per_page >= rows_needed){
			pages[c].page_init(4,rows_needed,pass_column_titles,pass_row_titles,
								      sdl_helper, sdl_helper->font,column_spaces);
			rows_needed = 0;
		} else {

			pages[c].page_init(4,rows_per_page,pass_column_titles,pass_row_titles,
								      sdl_helper, sdl_helper->font,column_spaces);
			rows_needed = rows_needed - rows_per_page;
		}
		pages_made++;//we made a page, so increase the counter
	}
	if(pages_made != vector_size) {
		error_logger.push_error("Error in icntrl8 page_creation_helper, # of created pages does not match",
				       "expected value.");
	}

	my_form.set_page_count(pages_made);

	my_form.prev_initiated = true;//let the form class know that it's pages have been set up
	my_form.prev_init_value = nch4_val;//and also what conditions caused such a creation


}

void icntrl4_form_button::make_output(ostream& outs){
    vector<text_box>* boxes = &my_form.get_pages()[0].get_text_boxes();
    string spaces = "     ";
    //we do 4 prints a loop, so c should go up by four each time
	for(unsigned int c = 0; c < boxes->size();c += 4){
        //output the line as declared by the input manual and as expected by HF
        //note here that I'm using the string 'spaces' to approximate the fortran 5x formatting
        //tag
        outs << setprecision(2) F5 boxes->at(c).text << spaces;
        outs << setprecision(1) F5 boxes->at(c+1).text << spaces I boxes->at(c+2).text F5 boxes->at(c+3).text << endl; 

    }
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

	my_form.init("Resolved Levels (ICNTRL4)","icntrl4_form_help.png",0,0,sdl_helper,sdl_helper->font);

}


//#################################################################################










//! \file form_buttons.cc contains the implementations for the classes specified in form_buttons.h

#include <cmath>
#include<iomanip>//for setting precision and field width
#include "form_buttons.h"
#include "button_manager.h"
#include "define.h"
using namespace std;

//copied over from input_maker

//! print floating point number with field width 8 (Fortran style)
#define F << setw(8) <<
//! print floating point number with field width 10 (Fortran style)
#define F10 << setw(10) <<
//! print floating point number with field width 5 (Fortran style)
#define F5 << setw(5) <<
//! print floating point number with field width 7 (Fortran style)
#define F7 << setw(7) <<
//! print floating point number with field width 8 (Fortran style)
#define F8 << setw(8) <<
//! print integer with field width 5 (Fortran style)
#define I << setw(5) <<
//! print integer with field width 10 (Fortran style)
#define I10 << setw(10) <<

extern asset_manager* asset_access;
extern manager* tile_access;

//######################## FORM BUTTONS #####################################
form_button::form_button(){
	lock_texture = NULL;
	is_locked = true;
	unlock_help_texture = NULL;
}

form_button::~form_button(){

}

void form_button::init(){	

	string lock_target = HOME+"/Andiamo/Assets/Images/lock.png";
	lock_texture = asset_access->get_texture(lock_target);
	if(lock_texture == NULL) error_logger.push_error(SDL_GetError());
	is_locked  = true;
    init_array = NULL;
    pre_config = false;
}
//should be used by the bmangers "form" init member to place each form button such that it lines up with
//the graphic for the form button tray
void form_button::set_corner_loc(int x_in, int y_in){
	xloc = x_in;
	yloc = y_in;
}

void form_button::make_rect(int width_in,int height_in){

	//initialized by set_corner_loc
	my_rect.x = xloc;
	my_rect.y = yloc;
	my_rect.w = width_in;
	//the width field is the one used to check clicks
	width = width_in;

	my_rect.h = height_in;
	//the height field is the one used to check clicks
	height = height_in;
}
void form_button::setup_lock(){
	lock_rect.x = my_rect.x+my_rect.w-15;
	lock_rect.y = my_rect.y;
	lock_rect.w = 15;
	lock_rect.h = 25;

}

void form_button::setup_help_msg(){

	string unlock_target = "Assets/Images/form_assets/general_form_locked_msg.png";
	unlock_help_texture = asset_access->get_texture(unlock_target);
	if(unlock_help_texture == NULL) error_logger.push_error(SDL_GetError());

}
void form_button::init_form(const vector<regex>& pattern_tests){
	my_form.init("no title","default_form_help.png",0,0,pattern_tests);

}

void form_button::screen_size(){
	bool update = false;

	int old_width = sdl_access->get_win_size()->width;
	int old_height = sdl_access->get_win_size()->height;
	if(old_width < 800){
		old_width = 800; 
		update = true;
	}

	if(sdl_access->get_win_size()->width < 800){
		old_height = 800;
		update = true;
	}

	if(update){

		SDL_SetWindowSize(sdl_access->get_window(),old_width,old_height);
		sdl_access->window_update(old_width,old_height);

		error_logger.push_msg("Updated screen size upon opening a form.");
	}
}

void form_button::draw_help_msg(SDL_Event& big_event,SDL_Rect& destination){

    sdl_access->draw();
	SDL_RenderCopy(sdl_access->renderer,unlock_help_texture,NULL,&destination);
	sdl_access->present();

    //spin until they hit a key, or click somewhere on the screen
    while(SDL_PollEvent(&big_event) == 0 || big_event.type == SDL_MOUSEBUTTONUP);				

}

void form_button::draw_lock(){
	if(is_locked){
		SDL_RenderCopy(sdl_access->renderer,lock_texture,NULL,&lock_rect);
	}
}

void form_button::toggle_lock(){
	if(is_locked){
		is_locked = false;
	} else {
		is_locked = true;
	}

}

bool form_button::make_output(ofstream& outs,
                              vector<index_value>& bad_input_list){
    string err = "A form button has called the base classes output creation ";
    err       += "member. Each form button is likely to have a special format,";
    err       += "so you should customize this function in the derived class.";
	error_logger.push_error(err);
	return false;
}

bool form_button::check_values(vector<index_value>& error_details){

    string err = "A form button has had check_values() called upon it, but";
    err       += " it hasn't been overloaded to fit a form. This could be";
    err       += " unintended behavior";
	error_logger.push_error(err);
	return false;
}

void form_button::init_values_helper(){


    //get reference to this form's pages
    vector<page>& pages = my_form.get_pages();   

    //if default values exist, and the there is at least one page made
    if(init_array != NULL && pages.size() != 0){

        //keep track of which default value needs to be used
        UINT init_val_index = 0;
        for(UINT page = 0; page < pages.size(); page++){

            //get a reference to this page's array of text boxes
            vector<text_box>& boxes = pages[page].get_text_boxes();

            for(UINT box = 0; box < boxes.size(); box++){

                //make sure we don't go out of the bounds of the default
                //value array
                if(init_val_index == (*init_array).size() ){
                    break;
                } else {
                    //use update_text to fill in the box with default values
                    //this handles the drawing, and moves the cursor
                    //to the end of the new default value
                    boxes[box].update_text((*init_array)[init_val_index]);

                    //move to the next init value
                    init_val_index++;
                }//text setting if/else block

            }//text box for loop

        }//page for loop

    }//outer if


}

void form_button::init_form_with_vec(form& fill_me,vector<string>& use_me){

    //get reference to this form's pages
    vector<page>& pages = fill_me.get_pages();    

    //if default values exist, and the there is at least one page made
    if(!use_me.empty() && pages.size() != 0){

        //keep track of which default value needs to be used
        UINT init_val_index = 0;
        for(UINT page = 0; page < pages.size(); page++){

            //get a reference to this page's array of text boxes
            vector<text_box>& boxes = pages[page].get_text_boxes();

            for(UINT box = 0; box < boxes.size(); box++){

                //make sure we don't go out of the bounds of the default
                //value array
                if(init_val_index == use_me.size() ){
                    break;
                } else {
                    //use update_text to fill in the box with default values
                    //this handles the drawing, and moves the cursor
                    //to the end of the new default value
                    boxes[box].update_text(use_me[init_val_index]);

                    //move to the next init value
                    init_val_index++;
                }//text setting if/else block

            }//text box for loop

        }//page for loop

    }//outer if

}

void form_button::save_information(ofstream& context_out){ 

    //get a reference to all of the pages in this form
    vector<page>& pages_ref = my_form.get_pages();

    //iterate over all of the pages
    for(uint page = 0; page < pages_ref.size(); page++){

        //create a reference to this page's text_box array
        vector<text_box>& tb_array = pages_ref[page].get_text_boxes();

        //loop over all of the text boxes, saving their current
        //information to the new config file
        for(uint text_box = 0; text_box < tb_array.size(); text_box++){
            context_out << tb_array[text_box].text;
            if(text_box != tb_array.size()-1) context_out << " ";     
        }

    }

}

void form_button::save_information(ofstream& context_out,form& this_form){

    //get a reference to all of the pages in this form
    vector<page>& pages_ref = this_form.get_pages();

    //iterate over all of the pages
    for(uint page = 0; page < pages_ref.size(); page++){

        //create a reference to this page's text_box array
        vector<text_box>& tb_array = pages_ref[page].get_text_boxes();

        //loop over all of the text boxes, saving their current
        //information to the new config file
        for(uint text_box = 0; text_box < tb_array.size(); text_box++){
            context_out << tb_array[text_box].text;
            if(text_box != tb_array.size()-1) context_out << " ";     
        }

    }
    

}
//#############################################################################


//####################### ICNTRL8 BUTTON ######################################

void icntrl8_form_button::setup_help_msg(){

	string unlock_target = "Assets/Images/form_assets/icntrl8_form_locked_msg.png";
	unlock_help_texture = asset_access->get_texture(unlock_target);	
	if(unlock_help_texture == NULL) error_logger.push_error(SDL_GetError());
}

bool icntrl8_form_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		SDL_RenderClear(sdl_access->renderer);
		click_helper(mouse_event);
		return true;
	}
	return false;
}

void icntrl8_form_button::click_helper(SDL_Event& mouse_event){
	error_logger.push_msg("Clicked icntrl8/cutoff nuclei button");

    //grab the value for icntrl8 as it exists with the GUI right now
    int curr_val =
        stoi(tile_access->fields.at("line_6").at("ICNTRL8")->temp_input);

	//don't consider doing anything if the form is locked
	if(!is_locked){
		screen_size();

		//in this case the form has not been previously created
		if(!my_form.prev_initialized){


            //make the blank form
            page_creation_helper();

			my_form.toggle_active();//let the form know that it is now active
			//enter the mini loop for form entry
			my_form.form_event_loop(mouse_event);

		} else if( my_form.prev_init_value == curr_val ){

			//let the form know that it is now active
			my_form.toggle_active();

			//enter the mini loop for form entry
			my_form.form_event_loop(mouse_event);

		//in this case, the form has been previously created, but
        //the icntrl8 value has been changed, so it must be recreated
		} else {
			my_form.flush_pages();//clear out previous info

			//most of this work is shared with the 1st time creation case
			//so it has been put into a helper function
			page_creation_helper();

			//let the form know that it is now active
			my_form.toggle_active();
			//enter the mini loop for form entry
			my_form.form_event_loop(mouse_event);
		}
	}
}

void icntrl8_form_button::page_creation_helper(){


	//grab val from parameter field, so the pages can be set up
	try{

	  icntrl8_val = stoi(tile_access->fields.at("line_6").at("ICNTRL8")->temp_input);

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
	//calculate how many pages are needed
	uint vector_size = ceil((icntrl8_val * 35) / 725.0);
	uint pages_made = 0;

	//saves space later
	vector<page>& pages = my_form.get_pages();
	pages.resize(vector_size);

	vector<int> column_spaces;
	column_spaces.push_back(0);
	column_spaces.push_back(150);
	column_spaces.push_back(150);


	for(uint c = 0; c < pages.size();c++){

		if(rows_per_page >= rows_needed){
			pages[c].page_init(3,rows_needed,pass_column_titles,pass_row_titles,
								      column_spaces);
			rows_needed = 0;
		} else {

			pages[c].page_init(3,rows_per_page,pass_column_titles,pass_row_titles,
								      column_spaces);
			rows_needed = rows_needed - rows_per_page;
		}
		//we made a page, so increase the counter
		pages_made++;
	}
	if(pages_made != vector_size) {
		error_logger.push_error("Error in icntrl8 page_creation_helper, # of created pages does not match",
				       "expected value.");
	}

	my_form.set_page_count(pages_made);

    init_values_helper();

	//let the form class know that it's pages have been set up
	my_form.prev_initialized = true;
	//and also what conditions caused such a creation
	my_form.prev_init_value = icntrl8_val;
}

void icntrl8_form_button::init_form(const vector<regex>& pattern_tests){
	my_form.init("Cutoff Nuclei (ICNTRL8)","icntrl8_form_help.png",0,0,
                 pattern_tests);


    try{
        init_array = &io_access->form_init_arrays.at("ICNTRL8");
    } catch(out_of_range& not_found){

    }
}

bool icntrl8_form_button::make_output(ofstream& outs,
		vector<index_value>& bad_input_list){

	if(outs.fail()){
		error_logger.push_error("icntrl8_form_button::make_output was not given a ",
					"valid output file stream. exiting.");
		return false;
	}

	if(!check_values(bad_input_list)){
		return false;
	}




    outs << "ICNTRL 8 OUTPUT" << endl;

	//outs << "TESTING ICNTRL8'S OUTPUT" << endl;
	//saves calls to the getter, and space in this function
	vector<page>* pages_ptr = &my_form.get_pages();

	//loop over each page
	for(uint c = 0; c < pages_ptr->size();c++){

		uint columns = pages_ptr->at(c).get_columns();

		//loop over each row
		for(uint d = 0; d < pages_ptr->at(c).get_text_boxes().size() ;d += columns){

			outs I pages_ptr->at(c).get_text_boxes().at(d).text
			     I pages_ptr->at(c).get_text_boxes().at(d+1).text;
			outs << setw(10);
			outs << setprecision(3);
			outs << pages_ptr->at(c).get_text_boxes()[d+2].text << endl;
		}

	}
	return true;
}

bool icntrl8_form_button::check_values(vector<index_value>& error_details){

	return my_form.check_values(error_details);

}

void icntrl8_form_button::save_information(ofstream& context_out){

    //this is the case that the user has opened & interacted with
    //the cutoff nuclei form, and should take precedence over the
    //initialization values
    if(my_form.prev_initialized && my_form.get_pages().size() != 0 ){
        context_out << "FORM:ICNTRL8 ";
        form_button::save_information(context_out);

    //this is the case that the custom config file has provided icntrl8 values
    //but the user may or may not have clicked on the icntrl8 form
    //because the user hasn't clicked on it, it hasn't been created,
    //and the original default values stored in the input_maker must be used
    } else if(init_array != NULL){

        context_out << "FORM:ICNTRL8 ";
        for(UINT c = 0; c < (*init_array).size();c++){
            context_out << (*init_array)[c];
            if(c != (*init_array).size() - 1){
                context_out << " ";
            }
        }
    }
}
//##############################################################################


//####################### ICNTRL6 BUTTON ########################################
icntrl6_form_button::~icntrl6_form_button(){

}

void icntrl6_form_button::setup_lock(){
	lock_rect.w = 15;
	lock_rect.h = 25;

	lock_rect.x = my_rect.x + my_rect.w - 15;
	lock_rect.y = my_rect.y + my_rect.h - lock_rect.h;


}

bool icntrl6_form_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		SDL_RenderClear(sdl_access->renderer);
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

        //draw the main context
        sdl_access->draw();

		//draw the form selection landing 
		show_landing();

		//have sdl_helper update the display
		sdl_access->present();

		//if we enter a form loop, we should ignore whatever event is in the
        //queue when the form loop exits. So, this flag should be set to
        //true if a form loop is entered, that way the do-while loop
        //will know to not worry about where the most recent click was
		bool did_something = false;

		//enter this loop unconditionally at first, because the user had
		//to have clicked on the form_button to get here
		do{
			//reset did_something flag
			did_something = false;

			//read from queue until a click event happens
			while( !(SDL_PollEvent(&mouse_event) == 1 &&
				(mouse_event.type == SDL_MOUSEBUTTONDOWN ||
                 mouse_event.type == SDL_QUIT) ) );

			if(mouse_event.type == SDL_QUIT){
				//putting the same event back in the queue
				//makes it possible to exit andiamo directly
				SDL_PushEvent(&mouse_event);
				return;
			}


			if( parity_area.clicked(mouse_event) ){
                SDL_RenderClear(sdl_access->renderer);
				parity_page_creation();

                //this will set up the 'my_form' object just fine
                init_values_helper();

				//let the form know that it is now active
				my_form.toggle_active();

				//enter the mini loop for form entry
				my_form.form_event_loop(mouse_event);
				did_something = true;

			} else if( spectra_area.clicked(mouse_event) ){
                SDL_RenderClear(sdl_access->renderer);
				search_spectra_page_creation();
                if(!io_access->icntrl6_extra_init_arrays.empty()){
                    init_form_with_vec(search_spectra,
                                   io_access->icntrl6_extra_init_arrays[0]);
                }
				search_spectra.toggle_active();
				search_spectra.form_event_loop(mouse_event);
				did_something = true;

			} else if( xsections_area.clicked(mouse_event) ){
                SDL_RenderClear(sdl_access->renderer);
				cross_sections_page_creation();
                if(!io_access->icntrl6_extra_init_arrays.empty()){
                    init_form_with_vec(cross_sections,
                                    io_access->icntrl6_extra_init_arrays[1]);
                }
				cross_sections.toggle_active();
				cross_sections.form_event_loop(mouse_event);
				did_something = true;

			}
            //clear off the screen
			SDL_RenderClear(sdl_access->renderer);

            sdl_access->draw();

            //redraw the button tray
			//button_access->draw_tray();
            //redraw the form button tray
			//button_access->draw_form_tray();
            //redraw the buttons themselves
			//button_access->draw_buttons();
            //redraw the form selection area
			show_landing();

			sdl_access->present();

		//loop until the user clicks off the landing screen
		} while( did_something || landing_was_clicked(mouse_event) );


	}
}

bool icntrl6_form_button::landing_was_clicked(SDL_Event& mouse_event){

		bool in_x = (mouse_event.button.x > landing_rect.x &&
						 mouse_event.button.x < landing_rect.x + landing_rect.w);
		bool in_y = (mouse_event.button.y > landing_rect.y &&
						 mouse_event.button.y < landing_rect.y + landing_rect.h);

		if( in_x && in_y ){
			return true;
		}
		return false;

}



void icntrl6_form_button::init_form(const vector<regex>& pattern_tests){

	//set up the image that lets the user switch between this
    //button's different forms
	setup_landing();

    vector<regex> inm1_patterns;
    inm1_patterns.push_back(pattern_tests[0]);
    //this line has many columns 9 floating point #'s with field width
	// of 8 and precision of 4
    //for(int c = 0; c < 9; c++){
	for(int c = 1; c < 10; c++){
        inm1_patterns.push_back(pattern_tests[c]);;
    }
    vector<regex> inm2_patterns;
    inm2_patterns.push_back(pattern_tests[10]);
    inm2_patterns.push_back(pattern_tests[11]);
    inm2_patterns.push_back(pattern_tests[12]);
    inm2_patterns.push_back(pattern_tests[13]);

    vector<regex> parity_patterns;
    parity_patterns.push_back(pattern_tests[14]);
    parity_patterns.push_back(pattern_tests[15]);

	//my_form (from default form_button class) will be the parity info
	my_form.init("Parameter Search (ICNTRL6)","icntrl6_parity_help.png",0,0,
                  parity_patterns);

	//INM1 form
	search_spectra.init("# of Search Spectra","icntrl6_spectra_help.png",0,0,
                        inm1_patterns);

	//INM2 form
	cross_sections.init("# of Search Cross Sections","icntrl6_xsections_help.png",0,0,
                        inm2_patterns);


    try{
        init_array = &io_access->form_init_arrays.at("ICNTRL6");
        cout << "Default ICNTRL6 values received." << endl;
    } catch(out_of_range& not_found){

    }

}

//this one is kind of unique because the form itself should not change, it has a fixed size in the input manual
void icntrl6_form_button::parity_page_creation(){

	if( !my_form.prev_initialized ){

		my_form.prev_initialized = true;
		my_form.prev_init_value = 18;

		//make room for the first and only page
		my_form.get_pages().resize(1);

		vector<string> column_labels, row_labels;
		fill_parity_labels(row_labels, column_labels);

		vector<int> column_spaces;
		column_spaces.push_back(0);
		column_spaces.push_back(250);
		column_spaces.push_back(100);

		my_form.get_pages()[0].page_init( 3, 18, column_labels,
											row_labels,column_spaces);
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
	//#########################################################################

	//################# COLUMN LABELS #########################################
	column_labels.push_back("Al-Quraishi Parameter");
	column_labels.push_back("value");
	column_labels.push_back("Vary?");

	//#########################################################################
}

void icntrl6_form_button::search_spectra_page_creation(){

	int current_INM1_val;
	try{

 		current_INM1_val = stoi(tile_access->fields.at("line_10").at("INM1")->temp_input); 
	} catch(invalid_argument& arg_error){
		error_logger.push_error("Error reading current INM1/#Search Spectra value for page creation",
					" logics.");
	}

	if( !search_spectra.prev_initialized ){

		search_spectra_page_helper();

	//case where form has been previously created, but INM1's val has not changed, so it does not need to be remade
	} else if( search_spectra.prev_initialized && search_spectra.prev_init_value == current_INM1_val ){
		return;

	//case where form has been previously created, and the value of INM1 has been changed
	} else {

		search_spectra.flush_pages();
		search_spectra_page_helper();
	}//*/


}

void icntrl6_form_button::search_spectra_page_helper(){

	try{
	  INM1_val = stoi(tile_access->fields.at("line_10").at("INM1")->temp_input);

	} catch ( out_of_range& range_error ){
	  error_logger.push_error("ICNTRL6-INM1 could not be found in the field map.",
					  range_error.what());
	  INM1_val = 0;
	} catch ( invalid_argument& arg_error ){
	  error_logger.push_error("ICNTRL6-INM1 has been supplied an illegal (non-numerical?) argument.",
				  arg_error.what());
	  INM1_val = 0;
  	}

	search_spectra.prev_initialized = true;
	search_spectra.prev_init_value = INM1_val;

	vector<string> pass_column_labels,pass_row_labels;
	vector<int> column_spaces;
	fill_spectra_vectors(pass_column_labels,column_spaces);


	int rows_per_page = floor(725.0 / 35);
	int rows_needed   = INM1_val;
	//calculate how many pages are needed
	uint vector_size = ceil((INM1_val * 35) / 725.0);
	uint pages_made = 0;

    //saves space later
	vector<page>& pages = search_spectra.get_pages();
	pages.resize(vector_size);


	for(uint c = 0; c < pages.size();c++){

		if(rows_per_page >= rows_needed){
			pages[c].page_init(9,rows_needed,pass_column_labels,pass_row_labels,
								      column_spaces);
			rows_needed = 0;
		} else {

			pages[c].page_init(9,rows_per_page,pass_column_labels,pass_row_labels,
								      column_spaces);
			rows_needed = rows_needed - rows_per_page;
		}
		//we made a page, so increase the counter
		pages_made++;
	}
	if(pages_made != vector_size) {
        string err = "Error in icntrl6/search spectra page_creation_helper, ";
        err       += "# of created pages does not match, expected value.";
		error_logger.push_error(err);
	}

	search_spectra.set_page_count(pages_made);

	//let the form class know that it's pages have been set up
	search_spectra.prev_initialized = true;
	//and also what conditions caused such a creation
	search_spectra.prev_init_value = INM1_val;


}


void icntrl6_form_button::fill_spectra_vectors(vector<string>& pass_column_labels,
                                               vector<int>& column_spaces){
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
		current_INM2_val = stoi(tile_access->fields.at("line_10").at("INM2")->temp_input); 
	} catch(invalid_argument& arg_error){
		error_logger.push_error("Error reading current INM2/cross sections value for page creation",
					" logics.");
	}

	if( !cross_sections.prev_initialized ){

		cross_sections_helper();

	//case where form has been previously created, but INM2's val has not changed, so it does not need to be remade
	} else if( cross_sections.prev_initialized && cross_sections.prev_init_value == current_INM2_val ){
		return;

	//case where form has been previously created, and the value of INM2 has been changed
	} else {

		cross_sections.flush_pages();
		cross_sections_helper();
	}//*/

}

void icntrl6_form_button::cross_sections_helper(){

	try{
	  INM2_val = stoi(tile_access->fields.at("line_10").at("INM2")->temp_input);

	} catch ( out_of_range& range_error ){
	  error_logger.push_error("ICNTRL6-INM2 could not be found in the field map.",
					  range_error.what());
	  INM2_val = 0;
	} catch ( invalid_argument& arg_error ){
	  error_logger.push_error("ICNTRL6-INM2 has been supplied an illegal (non-numerical?) argument.",
				  arg_error.what());
	  INM2_val = 0;
  	}

	cross_sections.prev_initialized = true;
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
	uint vector_size = ceil((INM2_val * 35) / 725.0);//calculate how many pages are needed
	uint pages_made = 0;

	vector<page>& pages = cross_sections.get_pages();//saves space later
	pages.resize(vector_size);

	for(uint c = 0; c < pages.size();c++){

		if(rows_per_page >= rows_needed){
			pages[c].page_init(4,rows_needed,pass_column_labels,pass_row_labels,
								     column_spaces);
			rows_needed = 0;
		} else {

			pages[c].page_init(4,rows_per_page,pass_column_labels,pass_row_labels,
								     column_spaces);


			rows_needed = rows_needed - rows_per_page;
		}
		//we made a page, so increase the counter
		pages_made++;
	}
	if(pages_made != vector_size) {
		error_logger.push_error("Error in icntrl6/cross sections page creation_helper, # of created pages does not match",
				       "expected value.");
	}

	cross_sections.set_page_count(pages_made);

	//let the form class know that it's pages have been set up
	search_spectra.prev_initialized = true;
	//and also what conditions caused such a creation
	search_spectra.prev_init_value = INM2_val;

}

void icntrl6_form_button::setup_landing(){

	//setup the graphics pointers
	string landing_target(HOME);
	landing_target += "/Andiamo/Assets/Images/form_assets/icntrl6_landing.png";
	landing_texture = asset_access->get_texture(landing_target);
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

	SDL_RenderCopy(sdl_access->renderer,landing_texture,NULL,&landing_rect);

}

bool icntrl6_form_button::make_output(ofstream& outs,vector<index_value>& bad_input_list){
	if(outs.fail()){
		error_logger.push_error("Icntrl6_form_button::make_output was not given a valid output file stream.",
					"exiting.");
		return false;
	}	
    outs << "ICNTRL6 OUTPUT" << endl;

	if(!check_values(bad_input_list)){
		return false;
	}


	//outs << "TESTING ICNTRL6'S OUTPUT" << endl;
	//handle for accessing parity form's data
	vector<page>& parity_ref = my_form.get_pages();
	//handle for accessing search_spectra form's data
	vector<page>& search_ref = search_spectra.get_pages();
	//handle for accessing cross_section form's data   
	vector<page>& cross_ref  = cross_sections.get_pages();

	

	//INM1 is up first
	if(search_ref.size() != 0){
		//loop over search spectra's pages
		for(uint c = 0; c < search_ref.size();c++){
			uint columns = search_ref[c].get_columns();
			outs << "PAGE " << c << endl;
			//loop over each row
			for(uint d = 0; d < search_ref.at(c).get_text_boxes().size(); d += columns){
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
		for(uint c = 0;c < cross_ref.size();c++){
			outs << "PAGE " << c << endl;
			//loop over each row
			for(uint d = 0; c < cross_ref.size();c++){
				outs I cross_ref[c].get_text_boxes()[d].text I cross_ref[c].get_text_boxes()[d+1].text;
				outs << setprecision(4);
				outs F10 cross_ref[c].get_text_boxes()[d+2].text F10 cross_ref[c].get_text_boxes()[d+3].text << endl;

			}
		}
	}

	//and finally, parity
	if(parity_ref.size() != 0){
    	for(uint c = 0; c < parity_ref[0].get_text_boxes().size();c += 2){
        	outs << setprecision(4);
        	outs F8 parity_ref[0].get_text_boxes()[c].text
				 I parity_ref[0].get_text_boxes()[c+1].text << endl;
    	}
	}

	return true;
}

bool icntrl6_form_button::check_values(vector<index_value>& error_details){

	bool return_me = true;

	vector<index_value> parity_errors;
	vector<index_value> search_errors;
	vector<index_value> cross_errors;

	if(!my_form.check_values(parity_errors)){
		return_me = false;
	}

	if(!search_spectra.check_values(search_errors)){
		return_me = false;
	}

	if(!cross_sections.check_values(cross_errors)){
		return_me = false;
	}

	for(uint c = 0; c < parity_errors.size(); c++){
		error_details.push_back(parity_errors[c]);
	}

	for(uint c = 0; c < search_errors.size(); c++){
		error_details.push_back(search_errors[c]);
	}

	for(uint c = 0; c < cross_errors.size(); c++){
		error_details.push_back(cross_errors[c]);
	}

	return return_me;
}

void icntrl6_form_button::save_information(ofstream& context_out){
    if(my_form.prev_initialized && my_form.get_pages().size() != 0){
        context_out << "FORM:ICNTRL6 ";
        //this part outputs the Parity Info Entry form
        //(the leftmost one that you can choose after clicking "Parameter Search"

        //use pipe characters as a seperator for the information that corresponds
        //to the three forms
        form_button::save_information(context_out);
        context_out << "|";
        form_button::save_information(context_out,search_spectra);
        context_out << "|";
        form_button::save_information(context_out,cross_sections);
    }    

}
//################################################################################

//####################### ICNTRL10 BUTTON ########################################

void icntrl10_button::init(){


	string lock_target = HOME+"/Andiamo/Assets/Images/lock.png";
	lock_texture = asset_access->get_texture(lock_target);
	if(lock_texture == NULL) error_logger.push_error(SDL_GetError());
	is_locked         = true;
    active            = false;
    current_context   = 0;

    string back_target = HOME + "/Andiamo/Assets/Images/";
    back_target       += "form_assets/icntrl10_backdrop.png";
    icntrl10_backdrop = asset_access->get_texture(back_target);
    if(icntrl10_backdrop == NULL){
        error_logger.push_error("Couldn't create texture for icntrl10 'form' ",
                                SDL_GetError());
        cout << SDL_GetError() << endl;
    }

    bd_dest = {0,0,800,800};
    exit.set_loc(0,0,50,50);
    left_arrow.set_loc(700,0,25,50);
    right_arrow.set_loc(775,0,25,50);


    //set up the empty overlay surface

    Uint32 red,green,blue,alpha;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        red   = 0xff000000;
        green = 0x00ff0000;
        blue  = 0x0000ff00;
        alpha = 0x000000ff;
    #else
        red   = 0x000000ff;
        green = 0x0000ff00;
        blue  =	0x00ff0000;
        alpha = 0xff000000;
    #endif
    over_surface = 
        SDL_CreateRGBSurface(0,800,800,32,red,green,blue,alpha);


    //set up the context number indicator in the top right
    string number_path(HOME);
    number_path += "/Andiamo/Assets/Images/form_assets/number_sprites.png";
	number_sprites = IMG_Load(number_path.c_str());
	if(number_sprites == NULL) error_logger.push_error(SDL_GetError());



    int offset = 20;
    SDL_Rect source = {20,0,offset,offset};
    SDL_Rect destination = {750,26,20,20};
    
    //draw max page # in top right
    SDL_BlitSurface(number_sprites,&source,over_surface,&destination);

    source = {offset+current_context*offset,0,offset,offset};
    destination = {725,0,20,20};

    //draw current page # (0) in rop right
    SDL_BlitSurface(number_sprites,&source,over_surface,&destination);

    over_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,over_surface);

    //patern for sub-line 1 of line 11 (page 12 in manual)
    my_patterns.emplace_back(regex("(\\s*-?\\s*[0-9]{1,5}\\s*){1}\\s(\\s*-?\\s*[0-9]{1,3}\\s*\\.\\s*[0-9]{0,2}\\s*){1}"));
    //pattern for sub-line 2 of line 11
    my_patterns.emplace_back(regex("(\\s*-?\\s*[0-9]{1,4}\\s*\\.\\s*[0-9]{0,2}\\s*){6}"));
    //pattern for sub-line 3 of line 11
    my_patterns.emplace_back(regex("(\\s*-?\\s*[0-9]{1,4}\\s*\\.\\s*[0-9]{0,2}\\s*){6}"));

    string unlock_path(HOME);
    unlock_path +=
        "/Andiamo/Assets/Images/form_assets/general_form_locked_msg.png";
    unlock_help_texture = asset_access->get_texture(unlock_path);

}

void icntrl10_button::set_corner_loc(int x, int y){

    xloc = x;
    yloc = y;


}

void icntrl10_button::make_rect(int width_in,int height_in){

	//initialized by set_corner_loc
	my_rect.x = xloc;
	my_rect.y = yloc;

    width = width_in;
    height = height_in;

	my_rect.w = width;
	my_rect.h = height;

}

void icntrl10_button::setup_lock(){

	lock_rect.w = 15;
	lock_rect.h = 25;

	lock_rect.x = my_rect.x + my_rect.w - 15;
	lock_rect.y = my_rect.y;



}

void icntrl10_button::draw_lock(){

	if(is_locked){
		SDL_RenderCopy(sdl_access->renderer,lock_texture,NULL,&lock_rect);
	}

}

void icntrl10_button::toggle_lock(){

	if(is_locked){
		is_locked = false;
	} else {
		is_locked = true;
	}

}


icntrl10_button::icntrl10_button(){

    is_locked = true;

}


bool icntrl10_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		SDL_RenderClear(sdl_access->renderer);
		click_helper(mouse_event);
		return true;
	}
	return false;

}

void icntrl10_button::click_helper(SDL_Event& mouse_event){
	error_logger.push_msg("clicked the icntrl10/sigma info button ");



	if(!is_locked){


        string NNSIG_str;
        NNSIG_str = tile_access->fields.at("line_11").at("NNSIG")->temp_input;

        unsigned int current_NNSIG;
        current_NNSIG = stoi(NNSIG_str);

        if( current_NNSIG != prev_NNSIG){

            prev_NNSIG = current_NNSIG;
            init_data(prev_NNSIG);
            init_values_helper();

            event_loop(mouse_event);

        } else {

            event_loop(mouse_event);
            init_values_helper();
        }


	}

}

void icntrl10_button::event_loop(SDL_Event& big_event){

    // toggle to true to end the loop
    bool done = false;

    // prevents one click causing multiple events to happen
    bool click_lock = false;

    while(!done){

    	if( !SDL_PollEvent(&big_event) ){
            //arbitrary do-nothing event pushed onto queue,
            //so it doesn't hit any cases
    		big_event.type = 1776;
    	}

        switch(big_event.type){

		    case SDL_QUIT:
			    toggle_active();
			    done = true;
			    SDL_PushEvent(&big_event);
			    break;
		    case SDL_KEYDOWN:
                if(big_event.key.keysym.sym == SDLK_ESCAPE){
                    toggle_active();
                    done = true;
                }
			    break;
		    case SDL_KEYUP:
			    break;
		    case SDL_MOUSEBUTTONDOWN:
			    event_loop_click(big_event,done,click_lock);
			    break;
		    case SDL_MOUSEBUTTONUP:
			    //reset click lock, they let go finally
			    click_lock = false;
			    break;
		    case SDL_MOUSEWHEEL:
			    break;
		    case SDL_WINDOWEVENT:
			    break;
		
		    //nop
		    case 1776:
			    break;

		    default:
			    break;

        }

    	draw_me();
    	sdl_access->present();
    	SDL_Delay(50);

    }

}

void icntrl10_button::event_loop_click(SDL_Event& mouse_event,bool& done,
                                       bool& click_lock){

	if(!click_lock){

		//consider if the exit button was clicked
		if(exit.clicked(mouse_event)){
			error_logger.push_msg("Clicked the exit button.");
			toggle_active();
			done = true;//end mini loop

		//consider if the right arrow was clicked
		} else if(right_arrow.clicked(mouse_event) ){
			error_logger.push_msg("clicked the page right button");
            page_right();      
			//current_context++;

		//consider if the left arrow was clicked
		} else if(left_arrow.clicked(mouse_event) ){
			error_logger.push_msg("clicked the page left button");
            page_left();
			//current_context--;

        } else {

    	//used to kick out of the loop after the text box that
		//was clicked has been found
		bool found = false;
		if(!data.size() == 0){

			//filled by text_box_loop to tell this loop to do things
			string command;

            icntrl10_data&  current = data[current_context];

			for(unsigned int c = 0; c < 3 && !found; c++){

				/*enter text box loop for the matching text box, where
                 *the current text box was either clicked, or our index,
                 *'c', was set for us by command being equal to "TAB" */
    			if(current.line_entries[c].was_clicked(mouse_event) ||
                   command == "TAB" ){

						//reset command container if it was set
						if(command == "TAB") command = "";

						text_entry(current.line_entries[c],mouse_event,done,
                                   command,c);

						if(command == "TAB" &&  c < 3){
							//redo this step, but act on the next text box
							continue;
						} else found = true;
					}
				}
			}
		}
		click_lock = true;
	}
}

void icntrl10_button::page_right(){

    if(current_context < data.size() - 1){
        current_context++;
        update_page_indicator();
    }


}

void icntrl10_button::page_left(){

    if(current_context > 0){
        current_context--;
        update_page_indicator();
    }

}

void icntrl10_button::update_page_indicator(){

    //set up the area we are going to draw to
    SDL_Rect destination = {725,0,20,20};

    //overwrite the old number with white
    SDL_PixelFormat* format = over_surface->format;
    if(SDL_FillRect(over_surface,&destination,SDL_MapRGBA(format,WHITE)) != 0){
        error_logger.push_error(SDL_GetError());
    }

    //draw the new number over it
    SDL_Rect source = {20+current_context*20,0,20,20};
    if(SDL_BlitSurface(number_sprites,&source,over_surface,&destination) != 0){
        cout << "COULDNT CHANGE NUMBER" << endl;
        error_logger.push_error(SDL_GetError());
    }


    over_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,over_surface);



}

void icntrl10_button::text_entry(text_box& curr_tb,SDL_Event& event,
                                 bool& done,string& command, unsigned int which_box){

	//turn on the text input background functions
	SDL_StartTextInput();

	//used to control text entry loop
	bool text_done = false;
	//int c = 0;
	bool text_was_changed = false;

	//string container for event text info (which is normally a c-string)
	string pass_me;
	while(!text_done){
		//if(c >= 10) return;
		//do stuff

		if( !SDL_PollEvent(&event) ){
			//dummy event to stop it from printing default message every frame
			//where no event happens
			event.type = 1776;
		}
		/*if(event.type != 1776) cout << "Text box loop type:"
                                      << event.type << endl; */
		switch(event.type){
		  case SDL_MOUSEMOTION:
			break;

		  case SDL_MOUSEBUTTONDOWN:
			//if the click was within the text box, move the cursor maybe
		  	if( curr_tb.was_clicked(event) ){
                string msg = "Text box click at "+to_string(event.button.x);
                msg += ":"+to_string(event.button.y);
				error_logger.push_msg(msg);

			//elsewise exit text input mode, user clicked off the text box
		  	} else {
                string msg = "Clicked outside of the text box,";
                msg       += " exiting mini-loop";
		  		error_logger.push_msg(msg);

				SDL_Event keyup_event;
				//putting in this key up removes the click locking
				keyup_event.type = SDL_MOUSEBUTTONUP;
				//for the loop in form_event_loop
				SDL_PushEvent(&keyup_event);

				//doing this allows the user to 'hop' to another text box
				//directly from editing another box
				SDL_PushEvent(&event);

				text_done = true;
			}
		  	break;

		  case SDL_TEXTINPUT:
			pass_me = event.text.text;
			curr_tb.update_text(pass_me,my_patterns[which_box]);
			text_was_changed = true;
		  	//here this actually causes a loss of letters, so the event
            //flooding is necessary, don't flush
			//SDL_FlushEvent(SDL_TEXTINPUT);
			break;

		  case SDL_KEYDOWN:
			
			if(event.key.keysym.sym == SDLK_BACKSPACE){
				//they hit backspace, so delete the end character if
                //it is non-empty
				curr_tb.back_space(my_patterns[which_box]);
				text_was_changed = true;

			} else if(event.key.keysym.sym == SDLK_LEFT){

                curr_tb.dec_cursor(text_was_changed);

			} else if(event.key.keysym.sym == SDLK_RIGHT){

                curr_tb.inc_cursor(text_was_changed);
			

            //tab over to next text box
			} else if(event.key.keysym.sym == SDLK_TAB){
				command = "TAB";
				return;
			}
				

            //prevent event flooding
			SDL_FlushEvent(SDL_KEYDOWN);
		  	break;
		  case SDL_QUIT:
			//puts another sdl quit in the event queue, so program
			//can be terminated while in "text entry" mode
			SDL_PushEvent(&event);
			text_done = true;			
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
			draw_me();
			text_was_changed = false;

			//show updated picture
			sdl_access->present();
		}

		//c++;
		//SDL_Delay(50);
	}//end of loop

    //stop text input functionality because it slows down the app
	SDL_StopTextInput();

}

void icntrl10_button::draw_me(){

    SDL_RenderCopy(sdl_access->renderer,icntrl10_backdrop,NULL,&bd_dest);
    SDL_RenderCopy(sdl_access->renderer,over_texture,NULL,&bd_dest);
    for(unsigned int c  = 0; c < 3; c++){
        data[current_context].line_entries[c].draw_me();
    }
}

void icntrl10_button::save_information(ofstream& context_out){

    if(!data.empty()){
        stringstream output_line;
        output_line << "FORM:ICNTRL10 ";
        for(UINT data_obj = 0; data_obj < data.size(); data_obj++){

            vector<text_box>& box_array = data[data_obj].line_entries;

            for(UINT text_b = 0; text_b < box_array.size();text_b++){

                if(text_b == 0){
                    output_line << box_array[text_b].text;
                } else {
                    output_line  << "," << box_array[text_b].text;
                }
            }
            if(data_obj+1 != data.size()){
                output_line << "|";
            }

        }

        cout << output_line.str() << endl;
        context_out << output_line.str() << endl;
    }

}

void icntrl10_button::init_values_helper(){

    if(init_array != NULL && !data.empty()){

        //there should only be one string in the string array
        //and that is the entire line from the config file minus
        //the FORM:ICNTRL10 part, so it needs parsed
        vector<string> text_box_info;
        string init_list = (*init_array)[0];

        cout << init_list << endl;
        //splitting on '|' gives us a string where
        //the 3 text boxes that each page of icntrl10 form needs
        //are separated by ','
        vector<string> each_page = split(init_list,'|');
        for(UINT page_info = 0; page_info < each_page.size();page_info++){

          //after splitting on ',', we have an array
          //of the text strings that need to be inserted
          //into the text boxes
          vector<string> each_box = split(each_page[page_info],',');
          for(UINT box_line = 0; box_line < each_box.size();box_line++){
          
            //insert this array of actual information into the array
            //that will be used to fill in values later
            text_box_info.push_back(each_box[box_line]);  

          }
        }

        int init_val_index = 0;

        //loop over each data object that has been created
        for(UINT data_obj = 0; data_obj < data.size(); data_obj++){

            //loop over each text box in that object
            vector<text_box>& boxes = data[data_obj].line_entries;
            for(UINT box = 0; box < boxes.size(); box++){

                //initializing its graphics and data, using the
                //array that was made previously
                if(init_val_index != text_box_info.size()){
                    boxes[box].update_text(text_box_info[init_val_index]);
                    init_val_index++;
                }
            }

        }

    } 

}

void icntrl10_button::init_data(unsigned int num_contexts){


    try{
        init_array = &io_access->form_init_arrays.at("ICNTRL10");
        cout << "ICNTRL10 default values received" << endl;
    } catch(out_of_range& not_found){

    }   

    //resize the vector to contain the correct number of input screens
    data.resize(num_contexts);

    TTF_Font* font = sdl_access->font;

    //loop over them and initialize them
    for(uint context = 0; context < num_contexts; context++){

        //these values make sure that the text box goes where
        //there is space for them
        data[context].line_entries[0].init(font,"",0,153,800,25);
        data[context].line_entries[1].init(font,"",0,305,800,25);
        data[context].line_entries[2].init(font,"",0,369,800,25);

    }   

    //set up the area we are going to draw to
    SDL_Rect destination = {752,27,20,20};

    //overwrite the old number with white
    SDL_PixelFormat* format = over_surface->format;
    if(SDL_FillRect(over_surface,&destination,SDL_MapRGBA(format,WHITE)) != 0){
        error_logger.push_error(SDL_GetError());
    }

    //draw the new number over it
    SDL_Rect source = {20+((int)num_contexts-1)*20,0,20,20};
    if(SDL_BlitSurface(number_sprites,&source,over_surface,&destination) != 0){
        cout << "COULDNT CHANGE NUMBER" << endl;
        error_logger.push_error(SDL_GetError());
    }


    over_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,over_surface);

    //we remade the form, so start back at page one
    current_context = 0;

    update_page_indicator();
}

void icntrl10_button::draw_help_msg(SDL_Event& big_event,SDL_Rect& destination){

    sdl_access->draw();
	SDL_RenderCopy(sdl_access->renderer,unlock_help_texture,NULL,&destination);
	sdl_access->present();

    //spin until they hit a key, or click somewhere on the screen
    while(SDL_PollEvent(&big_event) == 0 || big_event.type == SDL_MOUSEBUTTONUP);				

}


bool icntrl10_button::make_output(ofstream& outs,vector<index_value>& icntrl10_errors){

    //code stub just to make this thing seem satisfied, icntrl10 isn't actually
    //implemented yet
    //if(my_form.get_pages().size() == 0) return true;
    //else return false;
    
    bool return_value = true;

    cout << "Hello from icntrl10::make_output" << endl;
    for(unsigned int c = 0; c < data.size();c++){

        for(unsigned int d = 0; d < 3;d++){
            if(data[c].line_entries[d].bad_input ){

                index_value temp_bag(data[c].line_entries[d].text,c*3 + d);                
                icntrl10_errors.push_back(temp_bag);
                return_value = false;
            } else {

                outs << data[c].line_entries[d].text << endl;

            }
        }

    }

    return return_value;
}


//################################################################################


//##################### ICNTRL4 BUTTON ###########################################
void icntrl4_form_button::setup_lock(){
	lock_rect.w = 15;
	lock_rect.h = 25;

	lock_rect.x = my_rect.x + my_rect.w - 15;
	lock_rect.y = my_rect.y + my_rect.h - lock_rect.h;

}

void icntrl4_form_button::init_form(const vector<regex>& pattern_tests){

	my_form.init("Resolved Levels (ICNTRL4)","icntrl4_form_help.png",0,0,
                 pattern_tests);


    try{
        init_array = &io_access->form_init_arrays.at("ICNTRL4");
        cout << "Do stuff with ICNTRL4 init list" << endl;
    } catch(out_of_range& not_found){

    }
}

bool icntrl4_form_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		SDL_RenderClear(sdl_access->renderer);
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
		if(!my_form.prev_initialized){


			//most of this work is shared with the recreation case
			//so it has been put into a helper function
			page_creation_helper();

			my_form.toggle_active();//let the form know that it is now active
			my_form.form_event_loop(mouse_event);//enter the mini loop for form entry

		//in this case the form has been previously created, but the icntrl8 value has not changed, so nothing needs to be done
		} else if(my_form.prev_init_value == stoi(tile_access->fields.at("line_8").at("NCH4")->temp_input) ){
		//let the form know that it is now active
		my_form.toggle_active();
		//enter the mini loop for form entry
		my_form.form_event_loop(mouse_event);

		//in this case, the form has been previously created, but the icntrl8 value has been changed, so it must be recreated
		} else {

			//clear out previous info
			my_form.flush_pages();

			//most of this work is shared with the 1st time creation case
			//so it has been put into a helper function
			page_creation_helper();

			//let the form know that it is now active
			my_form.toggle_active();
			//enter the mini loop for form entry
			my_form.form_event_loop(mouse_event);
		}
	}

}
void icntrl4_form_button::page_creation_helper(){

	//grab val from parameter field, so the pages can be set up
	try{
	  nch4_val = stoi(tile_access->fields.at("line_8").at("NCH4")->temp_input);
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
	//calculate how many pages are needed
	uint vector_size = ceil((nch4_val * 35) / 725.0);
	uint pages_made = 0;

	vector<page>& pages = my_form.get_pages();//saves space later
	pages.resize(vector_size);

	vector<int> column_spaces;
	column_spaces.push_back(0);
	column_spaces.push_back(208);
	column_spaces.push_back(184);
    column_spaces.push_back(201);


	for(uint c = 0; c < pages.size();c++){

		if(rows_per_page >= rows_needed){
			pages[c].page_init(4,rows_needed,pass_column_titles,pass_row_titles,
								      column_spaces);
			rows_needed = 0;
		} else {

			pages[c].page_init(4,rows_per_page,pass_column_titles,pass_row_titles,
								      column_spaces);
			rows_needed = rows_needed - rows_per_page;
		}
		//we made a page, so increase the counter
		pages_made++;
	}
	if(pages_made != vector_size) {
		error_logger.push_error("Error in icntrl8 page_creation_helper, # of created pages does not match",
				       "expected value.");
	}

	my_form.set_page_count(pages_made);

    form_button::init_values_helper();

	//let the form class know that it's pages have been set up
	my_form.prev_initialized = true;
	//and also what conditions caused such a creation
	my_form.prev_init_value = nch4_val;


}

bool icntrl4_form_button::make_output(ostream& outs,vector<index_value>& bad_input_list){


	std::vector<index_value> icntrl4_errors;
	if(!check_values(icntrl4_errors)){
		return false;
	}


    vector<text_box>* boxes = &my_form.get_pages()[0].get_text_boxes();
    string spaces = "     ";
    outs << "ICNTRL4 OUTPUT" << endl;
    //we do 4 prints a loop, so c should go up by four each time
	for(uint c = 0; c < boxes->size();c += 4){
        //output the line as declared by the input manual and as expected by HF
        //note here that I'm using the string 'spaces' to
		// approximate the fortran 5x formatting tag
        outs << setprecision(2) F5 boxes->at(c).text << spaces;
        outs << setprecision(1) F5 boxes->at(c+1).text << spaces I boxes->at(c+2).text F5 boxes->at(c+3).text << endl; 

    }
	return true;
}

bool icntrl4_form_button::check_values(vector<index_value>& error_details){

	return my_form.check_values(error_details);

}

void icntrl4_form_button::save_information(ofstream& context_out){

    //icntrl8_form_button::save_information has explanations for these
    //cases
    if(my_form.prev_initialized && my_form.get_pages().size() != 0){

        context_out << "FORM:ICNTRL4 ";
        form_button::save_information(context_out);

    } else if(init_array != NULL){

        context_out << "FORM:ICNTRL4 ";
        for(UINT c = 0; c < (*init_array).size();c++){
            context_out << (*init_array)[c];
            if(c != (*init_array).size() - 1){
                context_out << " ";
            }
        }

    }
}
//################################################################################

//################## IVL4 || ILV5 BUTTON #########################################

bool ilv3_ilv5_form_button::handle_click(SDL_Event& mouse_event){
	if(button::was_clicked(mouse_event)){
		SDL_RenderClear(sdl_access->renderer);
		click_helper(mouse_event);
		return true;
	}
	return false;
}

void ilv3_ilv5_form_button::click_helper(SDL_Event& mouse_event){
	error_logger.push_msg("clicked the icntrl4/resolved levels info button ");

        int curr_ilv3 = stoi(tile_access->fields.at("line_5").at("ILV3")->temp_input);
        int curr_ilv5 = stoi(tile_access->fields.at("line_5").at("ILV5")->temp_input);

	//don't consider doing anything if the form is locked
	if(!is_locked){
		screen_size();

		//in this case the form has not been previously created
		if(!my_form.prev_initialized){


			//most of this work is shared with the recreation case
			//so it has been put into a helper function
			page_creation_helper();

			//let the form know that it is now active
			my_form.toggle_active();
			//enter the mini loop for form entry
			my_form.form_event_loop(mouse_event);

		//in this case the form has to be remade if the user has
		//switched from ilv3 to ilv5 or vice versa
		} else if(my_form.prev_init_value == curr_ilv3 || my_form.prev_init_value == curr_ilv5 ){

			if(my_form.form_title.compare("Distinct Residual Level Density") == 0 && curr_ilv5 > curr_ilv3 ){
			        my_form.flush_pages();
					my_form.set_form_title("Distinct Level Density Model");
			        page_creation_helper();

			} else if(my_form.form_title.compare("Distinct Level Density Model") == 0 && curr_ilv3 > curr_ilv5){
			        my_form.flush_pages();
					my_form.set_form_title("Distinct Residual Level Density");
			        page_creation_helper();

			}

			//let the form know that it is now active
			my_form.toggle_active();
			//enter the mini loop for form entry
			my_form.form_event_loop(mouse_event);

		//in this case, the form has been previously created, but the ilv3 or ilv5 value has been changed,
        //so it must be recreated
		} else {
			//clear out previous info
			my_form.flush_pages();

			//most of this work is shared with the 1st time creation case
			//so it has been put into a helper function
			page_creation_helper();

			my_form.toggle_active();//let the form know that it is now active
			my_form.form_event_loop(mouse_event);//enter the mini loop for form entry
		}
	}

}

void ilv3_ilv5_form_button::init_form(const vector<regex>& pattern_tests){

	my_form.init("Distinct Residual Level Density","ilv3_form_help.png",0,0,
					pattern_tests);



    try{
        init_array = &io_access->form_init_arrays.at("ILV3_ILV5");
        cout << "Do stuff with ILV3/ILV5 init list" << endl;
    } catch(out_of_range& not_found){

    }
}
void ilv3_ilv5_form_button::page_creation_helper(){
	int ilv3_val;
	int ilv5_val;
	try{

		ilv3_val = stoi(tile_access->fields.at("line_5").at("ILV3")->temp_input);
		ilv5_val = stoi(tile_access->fields.at("line_5").at("ILV5")->temp_input);

	} catch(invalid_argument& bad_arg){
		error_logger.push_error("Ilv3 or ilv5's value failed to conver to int in page_creation_helper.",
	bad_arg.what());
	}

	//let the form know that it has been initiated
	my_form.prev_initialized = true;

	//should be set to ilv3 or ilv5, whichever caused this form to be opened
	int rows_needed;
	vector<string> pass_column_titles;

	//this needs to exist, but does nothing here
	vector<string> pass_row_titles;

	//column labels are the same for 3 of 4
	pass_column_titles.push_back("A for custom LD");
	pass_column_titles.push_back("Z for custom LD");
	//set the title to reflect which mode is being used
	if( ilv3_val > ilv5_val){
		my_form.set_form_title("Distinct Residual Level Density");
		my_form.prev_init_value = ilv3_val;
		rows_needed = ilv3_val;
	//the third column label depends on what mode the form is in
	pass_column_titles.push_back("Little a for A,Z");
	} else {
		my_form.set_form_title("Distinct Level Density Model");
		my_form.prev_init_value = ilv5_val;
		rows_needed = ilv5_val;
		//the third column label depends on what mode the form is in
		pass_column_titles.push_back("Temp for A,Z");
	}
	//the fourth column label is the same for both cases
	pass_column_titles.push_back("Delta for A, Z");
      
	int rows_per_page = floor(725.0 / 35);

	//calculate how many pages are needed
	uint vector_size = ceil((rows_needed * 35) / 725.0);
	uint pages_made = 0;

	vector<page>& pages = my_form.get_pages();//saves space later
	pages.resize(vector_size);

	vector<int> column_spaces;
	column_spaces.push_back(0);
	column_spaces.push_back(180);
	column_spaces.push_back(180);
	column_spaces.push_back(180);


	for(uint c = 0; c < pages.size();c++){

		if(rows_per_page >= rows_needed){
			pages[c].page_init(4,rows_needed,pass_column_titles,pass_row_titles,
			column_spaces);
			rows_needed = 0;
		} else {

			pages[c].page_init(4,rows_per_page,pass_column_titles,pass_row_titles,
						      column_spaces);
			rows_needed = rows_needed - rows_per_page;
		}
		//we made a page, so increase the counter
		pages_made++;
	}
	if(pages_made != vector_size) {
	error_logger.push_error("Error in icntrl8 page_creation_helper, # of created pages does not match",
    					    "expected value.");
	}

	my_form.set_page_count(pages_made);

    form_button::init_values_helper();
}

bool ilv3_ilv5_form_button::make_output(ofstream& outs,vector<index_value>& bad_input_list){
	if(outs.fail()){
		error_logger.push_error("ilv3_ilv5_form_button::make_output was not given a valid output file stream.",
					"exiting.");
		return false;
	}
    outs << "ILV3 ILV5 OUTPUT" << endl;

	if(!check_values(bad_input_list)){
		return false;
	}


    vector<page>& pages = my_form.get_pages();

    for(uint c = 0; c < pages.size(); c++){
        for(uint d = 0; d < pages[c].get_text_boxes().size(); d += 4){
            outs I pages[c].get_text_boxes()[d].text I pages[c].get_text_boxes()[d+1].text;
            outs F7 pages[c].get_text_boxes()[d+2].text F7 pages[c].get_text_boxes()[d+3].text;
            outs << endl;
        }
    }

	return true;
}

bool ilv3_ilv5_form_button::check_values(vector<index_value>& error_details){

	return my_form.check_values(error_details);

}

void ilv3_ilv5_form_button::save_information(ofstream& context_out){

    //icntrl8_form_button::save_information has explanations for these
    //cases
    if(my_form.prev_initialized && my_form.get_pages().size() != 0){
        context_out << "FORM:ILV3_ILV5 ";
        form_button::save_information(context_out);
    } else if(init_array != NULL){

        context_out << "FORM:ILV3_ILV5 ";
        for(UINT c = 0; c < (*init_array).size();c++){
            context_out << (*init_array)[c];
            if(c != (*init_array).size() - 1){
                context_out << " ";
            }
        }

    }
}
//#################################################################################










//! \file form.cc implements the descriptions of the form class, which is found in form.h

#include "form.h"
//! message for when Andiamo prevents a double free error
#define FORM_ERROR "Attempted double free in ~form"
//! default text box width for form
#define TEXT_BOX_W 60
//! default horizontal distance between text boxes
#define TEXT_BOX_HORIZ_PADDING 10
//! how far right you need to progress to get the next number from the sprite
#define SPRITE_OFFSET 20
using namespace std;

extern asset_manager* asset_access;
extern string HOME;
//############### FORM CLASS ################################################
form::form(){
	form_title = "no title";

	form_title_surface = NULL;

	form_surface = NULL;
	form_texture = NULL;
	number_sprites = NULL;

	form_area = {0,0,0,0};

	active = false;

	help_shown = false;

	prev_initialized = false;
	prev_init_value = 0;

	page_count = 0;
	current_page = 0;

	form_texture = NULL;
}

form::~form(){
	if(form_title_surface != NULL){
		SDL_FreeSurface(form_title_surface);
	} else error_logger.push_error(FORM_ERROR);

	if(form_surface != NULL && form_texture != NULL){
		SDL_FreeSurface(form_surface);
		SDL_DestroyTexture(form_texture);
	} else error_logger.push_error(FORM_ERROR);

	if(help_texture != NULL){
		SDL_DestroyTexture(help_texture);
	} else error_logger.push_error(FORM_ERROR);

	if(number_sprites != NULL){
		SDL_FreeSurface(number_sprites);
	} else error_logger.push_error(FORM_ERROR);
}

void form::init(string form_title_in,string help_msg_image_name,int xloc_in,
				 int yloc_in, const vector<regex>& pattern_tests){

	//set up state variables
	form_title = form_title_in;
	
    //set up the patterns for user type checking
    my_patterns = pattern_tests;

	//initialize the page number spritesheet
	//these numbers are blitted onto the form surface,
	//to indicate which page the user is on
	string help_path(HOME);
    help_path += "/Andiamo/Assets/Images/form_assets/" + help_msg_image_name;

    string number_path(HOME);
    number_path += "Andiamo/Assets/Images/form_assets/number_sprites.png";
	number_sprites = IMG_Load(number_path.c_str());
	if(number_sprites == NULL) error_logger.push_error(SDL_GetError());
	//##########################################################################

	//set up the corner
	form_area.x = xloc_in;
	form_area.y = yloc_in;

	//initialize the form texture from its asset file
    string form_surf_path(HOME);
    form_surf_path += "/Andiamo/Assets/Images/form_assets/form.png";
	form_surface = IMG_Load(form_surf_path.c_str());

	if(form_surface == NULL){
		error_logger.push_error(SDL_GetError());
	} else {
		int offset = SPRITE_OFFSET;
		SDL_Rect source = {offset+page_count*offset,0,offset,offset};
		SDL_Rect destination = {750,26,20,20};

		//draw max page # in top right
		SDL_BlitSurface(number_sprites,&source,form_surface,&destination);

		source = {offset+current_page*offset,0,offset,offset};
		destination = {725,0,20,20};

		//draw current page # (0) in rop right
		SDL_BlitSurface(number_sprites,&source,form_surface,&destination);

		//make the form title surface
		SDL_Color black = {0,0,0,0};

		//make this font a little bit bigger than the others
		string title_path(HOME);
		title_path += "/Andiamo/Assets/fonts/LiberationSerif-Regular.ttf";
		TTF_Font* title_font = TTF_OpenFont( title_path.c_str(), 28);
		form_title_surface = TTF_RenderUTF8_Blended(title_font,form_title.c_str(),black);
		if(form_title_surface == NULL) error_logger.push_error(SDL_GetError());
		else {
			TTF_SizeText(title_font,form_title.c_str(),&source.w,&source.h);
			source.x = 0; source.y = 0;
			destination.w = source.w; destination.h = source.h;
			destination.x = form_area.x + 400 - (source.w / 2);
			destination.y = form_area.y + 25 - (source.h / 2);
			if(SDL_BlitSurface(form_title_surface,&source,form_surface,&destination) != 0){
				error_logger.push_error(SDL_GetError());
			}
			
		}
		TTF_CloseFont(title_font);
	}
	
	form_texture =
		SDL_CreateTextureFromSurface(sdl_access->renderer,form_surface);

	if(form_texture == NULL) error_logger.push_error(SDL_GetError());
	//######################################################################

	//initialize the help message that is shown upon the question mark being clicked
	string help_target(HOME);
    help_target += "/Andiamo/Assets/Images/form_assets/"+help_msg_image_name;
	help_texture = asset_access->get_texture(help_target);
	if(help_texture == NULL) error_logger.push_error(SDL_GetError());
	//######################################################################


	//query the texture to figure out its height and width dimensions
	SDL_QueryTexture(form_texture,NULL,NULL,&form_area.w,&form_area.h);

	//set up active area locations
	exit.set_loc(0,0,25,25);
	help.set_loc(0,25,25,25);
	right_arrow.set_loc(775,0,25,50);
	left_arrow.set_loc(700,0,25,50);
}

void form::set_form_title(string new_title){
	//the form title should have been set by the init function,
	//if it wasn't, something is wrong
	if(form_title_surface == NULL){
        string err;
        err = "Couldn't switch the form's title, it was previously null.";
		error_logger.push_error(err);
	}
	//destroy the old form title surface
	SDL_FreeSurface(form_title_surface);
        
	//save the new title surface
	form_title = new_title;

	//make this font a little bit bigger than the others
	string font_target(HOME);
	font_target += "/Andiamo/Assets/fonts/LiberationSerif-Regular.ttf";
	TTF_Font* title_font = TTF_OpenFont( font_target.c_str(), 28);

	//render the text as a graphic

	//black text
	SDL_Color black = {0,0,0,0};
	form_title_surface =
                    TTF_RenderUTF8_Blended(title_font,form_title.c_str(),black);

    //before we can draw the form title to the surface, 
    //we must first fill over the old form title.
	//Make sure the color is the same as the form's
    SDL_Rect source, destination;

	//set destination to the region we have to draw over
    destination = {30,0,650,50};

    //draw a rectangle over that area, with the gray color used in the form's asset
    //change the SDL_MapRGBA call to match the new color if the form is changed, thought it
    //is possible to query the surface directly and get the color info that way....
    SDL_FillRect(form_surface,&destination,
                        SDL_MapRGBA(form_surface->format,119,111,103,255));

    //get its size, and then calculate where it should be positioned on the form
	TTF_SizeText(title_font,form_title.c_str(),&source.w,&source.h);
	source.x = 0; source.y = 0;
	destination.w = source.w;
    destination.h = source.h;
	destination.x = form_area.x + 400 - (source.w / 2);
	destination.y = form_area.y + 25  - (source.h / 2);

    //blit the new title surface to the form's surface
	if(SDL_BlitSurface(form_title_surface,&source,form_surface,&destination) != 0){
		error_logger.push_error(SDL_GetError());
	}
	//close the font
	TTF_CloseFont(title_font);

	if(form_texture != NULL){
		SDL_DestroyTexture(form_texture);
	}
	form_texture =
                SDL_CreateTextureFromSurface(sdl_access->renderer,form_surface);
}

void form::form_event_loop(SDL_Event& big_event){

    //turn on text input
	SDL_StartTextInput();

	//toggle to true to end the loop
	bool done = false;

	//used to prevent one click causing multiple things to happen,
	//like repeatedly
	bool click_lock = false;

	while(!done){
	//cout << "IN FORM MINI LOOP" << endl;
	if( !SDL_PollEvent(&big_event) ){
        //arbitrary do-nothing event pushed onto queue,
        //so it doesn't hit any cases
		big_event.type = 1776;
	}
	/*if(big_event.type != 1776){
        cout << "BIG EVENT TYPE:" << big_event.type << endl;
    */
	switch(big_event.type){
		case SDL_QUIT:
			toggle_active();
			done = true;
			SDL_PushEvent(&big_event);
			break;
		case SDL_KEYDOWN:
			break;
		case SDL_KEYUP:
			break;
		case SDL_MOUSEBUTTONDOWN:
			handle_click(big_event,done,click_lock);
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

	//turn off text input, it hurts performance
	SDL_StopTextInput();
}




void form::handle_click(SDL_Event& mouse_event,bool& done,bool& click_lock){

	if(!click_lock){

		//consider if the exit button was clicked
		if(exit.clicked(mouse_event)){
			error_logger.push_msg("Clicked the exit button.");
			toggle_active();
			done = true;//end mini loop

		//consider if the help button was clicked
		} else if(help.clicked(mouse_event) ){
			error_logger.push_msg("clicked the help button.");
			if(!help_shown){
				help_shown = true;
			} else {
				help_shown = false;
			}
		//consider if the right arrow was clicked
		} else if(right_arrow.clicked(mouse_event) ){
			error_logger.push_msg("clicked the page right button");
			next_page();

		//consider if the left arrow was clicked
		} else if(left_arrow.clicked(mouse_event) ){
			error_logger.push_msg("clicked the page left button");
			prev_page();

		//consider the text boxes on the currently displayed page,
        //so long as it is not currently displaying the help message
		} else if(!help_shown){

			//used to kick out of the loop after the text box that
			//was clicked has been found
			bool found = false;
			if(!pages.size() == 0){

				//filled by text_box_loop to tell this loop to do things
				string command;

                page&  current = pages[current_page];

				for(unsigned int c = 0;
					c < current.get_const_text_boxes().size() && !found;
					c++){

					/*enter text box loop for the matching text box, where
                     *the current text box was either clicked, or our index,
                     *'c', was set for us by command being equal to "TAB" */
					if(current.get_text_boxes()[c].was_clicked(mouse_event) ||
					   command == "TAB" ){

						//reset command container if it was set
						if(command == "TAB") command = "";

                        //the columns should line up with the supplied
                        //vector of regular expressions
                        int pattern_index = 0;
						if(current.get_row_labels().size() == 0){						
                        	pattern_index =  c % current.get_columns();
						//if there is a column of row labels for this page,
						//mod by the number of actual input columns
						} else {
							pattern_index  = c % (current.get_columns() - 1);
						}
                        //cout << "NUM COLUMNS: " << pattern_index << endl;
                        //cout << "PATTERN INDEX: " << pattern_index << endl;
                        //cout << "MY PATTERNS SIZE: " << my_patterns.size() << endl;
						text_box_loop(current.get_text_boxes()[c],mouse_event,command,
                                      my_patterns[pattern_index]);

						if(command == "TAB" &&  c < current.get_text_boxes().size()){
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

void form::draw_me(){

	if(active){
		if(!help_shown){
			SDL_RenderCopy(sdl_access->renderer,form_texture,NULL,&form_area);
			if(!pages.empty()){
				pages[current_page].draw_me();
			}
		} else {
			SDL_RenderCopy(sdl_access->renderer,form_texture,NULL,&form_area);
			SDL_Rect help_area = form_area;
			help_area.y = 50;

			//the texture is actually shorter, it replaces the text box area
			help_area.h = 725;
			SDL_RenderCopy(sdl_access->renderer,help_texture,NULL,&help_area);
		}

	} else return;

}

void form::toggle_active(){
	if(active){
		active = false;	

	} else {
		active = true;

	}
}


void form::next_page(){
	if(current_page < page_count){
		current_page++;
		update_page_indicator();
	}
}

void form::prev_page(){
	if(current_page > 0){
		current_page--;
		update_page_indicator();
	}
}

bool form::check_values(vector<index_value>& error_details){

	if(!prev_initialized){
		string push_me = "Form: " + form_title;
        push_me       += " has not been filled it at all: ";
		error_details.push_back(push_me);
        push_me  = "it can be filled in by clicking";
        push_me += " its corresponding button on the toolbar.";
		error_details.push_back(push_me);

		return false;
	}

	//it should be assumed that inputs are correct,
    //but if one is found not to be, change this to false
	bool return_me = true;

	//loop over each page in this form
	for(unsigned int c = 0; c < pages.size(); c++){

		//get the number of columns for this page
		unsigned int num_columns = pages[c].get_columns();

		/*if this page happens to have a column of row labels, decrement
         *the # of columns by one to ensure that only columns that have a
         *logical meaning are considered in the evaluation */
		if(pages[c].get_const_row_labels().size() != 0){
			num_columns = num_columns - 1;
		}

		//then make sure each column has its own regular
        //expression to verify inputs with
		if(num_columns != my_patterns.size()){
            string err = "From form::check_values() A form doesn't";
            err += " have a regular expression to check each";
            err += "of its column's inputs against to ensure input integrity.";
			error_logger.push_error(err);
		}

		//save a reference to this page's vector in a temporary variable
		//so I can use it instead of the getter function
		const vector<text_box>& box_ref = pages[c].get_const_text_boxes();

		//go through and check each text box against the appropriate
        //regular expression using the mod operator
		for(unsigned int d = 0; d < box_ref.size(); d++){
			if(!regex_match(box_ref[d].text,my_patterns[d % num_columns]) ||
				box_ref[d].text.compare(" ") || box_ref[d].text.length() == 0){
				index_value temp_tuple(box_ref[d].text,d);
				error_details.push_back(temp_tuple);
				return_me = false;
			}

		}

	}

	return return_me;
}

void form::set_page_count(int page_count_in){

	page_count = page_count_in - 1;
	if(page_count > 10){
        string err = "page count greater than 10, form construction";
        err += " may not work properly";
		error_logger.push_error(err);
	}

	SDL_Rect source = {20+page_count*20,0,20,20};

	//destination is shifted right and down a bit, so fill doesn't
	//mess with "/" in page display
	SDL_Rect destination = {751,27,20,20};

	//"erase" previous page # by filling with white
    SDL_PixelFormat* format = form_surface->format;
	if(SDL_FillRect(form_surface,&destination,SDL_MapRGBA(format,WHITE)) != 0){
		error_logger.push_error(SDL_GetError());
	}

	//put the destination back how it was, so numbers are drawn
	//to the same location
	destination = {750,26,20,20};

	//"print" new max page # to surface
	SDL_BlitSurface(number_sprites,&source,form_surface,&destination);

	if(form_texture != NULL){
		SDL_DestroyTexture(form_texture);

        //update the texture
		form_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,form_surface);

	} else {

		error_logger.push_error("Error in set_page_count, previous form texture was NULL");

	}
}

void form::update_page_indicator(){

	SDL_Rect source = {20+current_page*20,0,20,20};
	SDL_Rect destination = {725,0,20,20};

	//write over old number, kind of like erasing just that part of the surface
    SDL_PixelFormat* format = form_surface->format;
	if(SDL_FillRect(form_surface,&destination,SDL_MapRGBA(format,WHITE)) != 0){
		error_logger.push_error(SDL_GetError());
	}

	//draw in new number
	SDL_BlitSurface(number_sprites,&source,form_surface,&destination);

	if(form_texture != NULL){

		SDL_DestroyTexture(form_texture);
		form_texture =
                SDL_CreateTextureFromSurface(sdl_access->renderer,form_surface);

	} else {
        string err = "Error in update_page_indicator,";
        err += " previous form texture was NULL";
		error_logger.push_error(err);
	}


}

void form::flush_pages(){
	//clear out the vector
	pages.clear();
	page_count = 0;
	current_page = 0;
	update_page_indicator();
}

void form::text_box_loop(text_box& current_box,SDL_Event& event,string& command,
                         const regex& pattern){

	//turn on the text input background functions
	SDL_StartTextInput();

	//used to control text entry loop
	bool done = false;
	//int c = 0;
	bool text_was_changed = false;

	//string container for event text info (which is normally a c-string)
	string pass_me;
	while(!done){
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
		  	if( current_box.was_clicked(event) ){
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

				done = true;
			}
		  	break;

		  case SDL_TEXTINPUT:
			pass_me = event.text.text;
			current_box.update_text(pass_me,pattern);
			text_was_changed = true;
		  	//here this actually causes a loss of letters, so the event
            //flooding is necessary, don't flush
			//SDL_FlushEvent(SDL_TEXTINPUT);
			break;

		  case SDL_KEYDOWN:
			
			if(event.key.keysym.sym == SDLK_BACKSPACE){
				//they hit backspace, so delete the end character if
                //it is non-empty
				current_box.back_space(pattern);
				text_was_changed = true;

			} else if(event.key.keysym.sym == SDLK_LEFT){

                current_box.dec_cursor(text_was_changed);

			} else if(event.key.keysym.sym == SDLK_RIGHT){

                current_box.inc_cursor(text_was_changed);
			

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

//###################### PAGE CLASS BELOW ######################################
page::page(){
	num_columns = 0;
	num_rows = 0;
}

page::page(const page& other){
	num_columns = other.num_columns;
	num_rows = other.num_rows;
	column_labels = other.column_labels;
	row_labels = other.row_labels;
}


page::~page(){

	if(column_label_textures.size() == 0) return;

	for(unsigned int c = (column_label_textures.size() - 1) ; c > 0;c--){
		if(column_label_textures[c] != NULL){
			SDL_DestroyTexture(column_label_textures[c]);
			column_label_textures.pop_back();
		}
	}

}

void page::page_init(unsigned int num_columns_in, unsigned int rows_needed,
						const vector<string>& column_labels_in
                        ,vector<string>& row_labels_in,
						const vector<int>& column_spacings){

	//abstracts some of the assignments to save space here
	init_local_var(num_columns_in, rows_needed,
                            column_labels_in, row_labels_in);

	//turn true to make room for row labels
	bool row_labels_exist = false;

	//used to control where the text boxes start to be drawn
	int x_start_point = 0;


	if(num_columns_in != column_spacings.size()){
		error_logger.push_msg("Not been enough spacing values supplied to page init.");
		error_logger.push_msg("Column_spacings should have one less element than num_columns_in.");
		error_logger.push_msg("This could be by design, as is the case with ICNTRL6's parity form.");
	}


	//implement logics for setting row labels here
	//how does this affect the placing of the actual text boxes?
	if(row_labels_in.size() != 0){
		set_row_labels(row_labels_in,x_start_point);
		row_labels_exist = true;
	}


	set_text_boxes(x_start_point,column_spacings,row_labels_exist);

	init_column_labels(column_spacings,x_start_point,row_labels_exist);


}
void page::init_local_var(uint num_columns_in, uint rows_needed,
                        const vector<string>& column_labels_in,
                        vector<string>& row_labels_in){
	num_columns = num_columns_in;
	num_rows = rows_needed;
	column_labels = column_labels_in;
	row_labels = row_labels_in;
}

void page::set_row_labels(const vector<string>& row_labels_in,
                            int& x_start_point){


	SDL_Color black = {0,0,0,0};
	SDL_Surface* temp_surface = NULL;
	SDL_Texture* temp_texture = NULL;


	for(unsigned int c = 0; c < row_labels_in.size(); c++){

		//shove in null pointer
		row_label_textures.push_back(temp_texture);

		//create surface from the text
		temp_surface = 
            TTF_RenderUTF8_Blended(sdl_access->font,row_labels[c].c_str(),black);

		int width;
		int height;

			
		if(TTF_SizeText(sdl_access->font,row_labels[c].c_str(),&width,&height) != 0){
            string err = "Error while making row labels in page,";
            err += " TTF_SizeText failure." + string(TTF_GetError());
			error_logger.push_error(err);
        }

			
			//make the previously shoved null pointer now point
            //to a texture created from the surface
			row_label_textures.back() =
                SDL_CreateTextureFromSurface(sdl_access->renderer,temp_surface);

			//calc location & use the size
			//explicit cast of c to int stops compiler warnings
			SDL_Rect temp_rect = 
                 {TEXT_BOX_HORIZ_PADDING,80+25*int(c)+10*int(c), width, height};

			if(width > x_start_point){
				//update the starting point tracker
				x_start_point = width;
			}

			//push that info into the row label vector
			row_label_rects.push_back(temp_rect);
		}
}

void page::set_text_boxes(int& x_start_point,
                    const vector<int>& column_spacings,bool& row_labels_exist){

    //give it some wiggle room
	if(x_start_point != 0) x_start_point += 10;

	//this variable declared here, because if row labels exist, the inner loop
	//should start at 1 instead of at 0
	unsigned int j = 0;
	if(row_labels_exist){
		j = 1;
	}

	//had a hard fight with the Rule of Three here.... 
	//implementing a copy constructor stopped the double free()
	// crash at the text_boxes.push_back
	int x_offset = 0;
	for(unsigned int i = 0; i < num_rows; i++){
		x_offset = 0;
		for( ; j < num_columns; j++){
			text_box new_text_box;
			int x_val = x_start_point+x_offset+column_spacings[j];
			x_offset += column_spacings[j];
			int y_val = 80+25*i+10*i;
			new_text_box.init(sdl_access->font,"",x_val,y_val,60,25);
			text_boxes.push_back(new_text_box);
		}
		//reset the j variable, because the for loop will no longer do it
		//since it was declared at the start of the function and not in the loop
		if(row_labels_exist) j = 1;
		else j = 0;
	}

	//but, I think another solution could have been to tell the vector
    //make 'x' text_boxes, and then init the copies that exist inside the vector

}

void page::init_column_labels(const vector<int>& column_spacings,
                         int& x_start_point,bool& row_labels_exist){


    //shorter reference to font
    TTF_Font* font = sdl_access->font;

	int x_offset = 0;
	//set up column labels
	SDL_Color black = {0,0,0,0};
	SDL_Surface* temp_surf = NULL;
	SDL_Texture* temp_texture = NULL;
	for(unsigned int c = 0; c < column_labels.size();c++){
		//shove in a null pointer
		column_label_textures.push_back(temp_texture);

        //render the text
		temp_surf = TTF_RenderUTF8_Blended(font,column_labels[c].c_str(),black);

		//make the pointer we shoved point at the desired texture
		column_label_textures.back() =
            SDL_CreateTextureFromSurface(sdl_access->renderer,temp_surf);

		//calculate drawing info for the column label
		SDL_Rect temp_rect = {x_start_point+x_offset+column_spacings[c],50,0,0};
		x_offset += column_spacings[c];

		//size the text and shove it into the vector
		TTF_SizeText(font,column_labels[c].c_str(),&temp_rect.w,&temp_rect.h);

		//shove it into the drawing info vector
		column_label_rects.push_back(temp_rect);

		//give memory back
		SDL_FreeSurface(temp_surf);
		//reset surf pointer for next item
		temp_surf = NULL;
	}

}

void page::draw_me(){


    SDL_Renderer* renderer = sdl_access->renderer;

	if( column_label_textures.size() != column_label_rects.size() ){
        string err = "The vector that saves drawing location for page";
        err += " column labels does not have the same size as";
        err += " the vector that contains the textures. Aborting drawing.";
   		error_logger.push_error(err);
		return;
	}

	for(unsigned int c = 0; c < column_label_textures.size();c++){

		if(SDL_RenderCopy(renderer,column_label_textures[c],NULL,&column_label_rects[c]) != 0){
			error_logger.push_error("Could not draw column title.");
		}
	}

	if( row_label_textures.size() != row_label_rects.size() ){
        string err = "The vector that saves the drawing location for";
        err += " the page row labels does not have the same size";
        err += " as the vector that contains the textures. Aborting drawing.";
		error_logger.push_error(err);
		return;
	}

	for(unsigned int c = 0; c < row_label_textures.size();c++){

		if( SDL_RenderCopy(renderer,row_label_textures[c],NULL,&row_label_rects[c]) != 0 ){

		}

	}

	for(unsigned int c = 0; c < text_boxes.size();c++){
		text_boxes[c].draw_me();
	}

}






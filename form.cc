//! \file form.cc implements the descriptions of the form class, which is found in form.h

#include "form.h"
#define FORM_ERROR "Attempted double free in ~form"
#define TEXT_BOX_W 60
#define TEXT_BOX_HORIZ_PADDING 10
#define SPRITE_OFFSET 20
using namespace std;


//######################## FORM CLASS #########################################################################
form::form(){
	form_title = "no title";

	form_title_surface = NULL;

	form_surface = NULL;
	form_texture = NULL;
	number_sprites = NULL;

	form_area = {0,0,0,0};

	active = false;

	help_shown = false;

	prev_initiated = false;
	prev_init_value = 0;

	page_count = 0;
	current_page = 0;

	sdl_helper = NULL;
	sdl_font = NULL;

	help_surface = NULL;
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

	if(help_surface != NULL && help_texture != NULL){
		SDL_FreeSurface(help_surface);
		SDL_DestroyTexture(help_texture);
	} else error_logger.push_error(FORM_ERROR);

	if(number_sprites != NULL){
		SDL_FreeSurface(number_sprites);
	} else error_logger.push_error(FORM_ERROR);
}

void form::init(string form_title_in,string help_msg_image_name,int xloc_in, int yloc_in,
		     sdl_help* sdl_helper_in,TTF_Font* sdl_font_in,const vector<regex>& pattern_tests){

	//set up state variables
	form_title = form_title_in;
	
    //set up the patterns for user type checking
    my_patterns = pattern_tests;

	//set up pointers to graphics class
	sdl_helper = sdl_helper_in;
	sdl_font = sdl_font_in;	

	//initialize the page number spritesheet
	//these numbers are blitted onto the form surface, to indicate which page the user is on
	string help_path = "Assets/Images/form_assets/" + help_msg_image_name;
	number_sprites = IMG_Load("Assets/Images/form_assets/number_sprites.png");
	if(number_sprites == NULL) error_logger.push_error(SDL_GetError());
	//###############################################################################################

	//set up the corner
	form_area.x = xloc_in;
	form_area.y = yloc_in;

	//initialize the form texture from its asset file
	form_surface = IMG_Load("Assets/Images/form_assets/form.png");
	if(form_surface == NULL){
		error_logger.push_error(SDL_GetError());
	} else {
		int offset = SPRITE_OFFSET;
		SDL_Rect source = {offset+page_count*offset,0,offset,offset};
		SDL_Rect destination = {750,26,20,20};
		SDL_BlitSurface(number_sprites,&source,form_surface,&destination);//draw max page # in top right

		source = {offset+current_page*offset,0,offset,offset};
		destination = {725,0,20,20};
		SDL_BlitSurface(number_sprites,&source,form_surface,&destination);//draw current page # (0) in rop right

		//make the form title surface
		SDL_Color black = {0,0,0,0};

		//make this font a little bit bigger than the others
		TTF_Font* title_font = TTF_OpenFont( "./Assets/fonts/LiberationSerif-Regular.ttf", 28);
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
	
	form_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,form_surface);
	if(form_texture == NULL) error_logger.push_error(SDL_GetError());
	//################################################################################################

	//initialize the help message that is shown upon the question mark being clicked
	string help_target = "Assets/Images/form_assets/"+help_msg_image_name;
	help_surface = IMG_Load(help_target.c_str());
	if(help_surface == NULL) error_logger.push_error(SDL_GetError());
	help_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,help_surface);
	if(help_texture == NULL) error_logger.push_error(SDL_GetError());
	//################################################################################################


	//query the texture to figure out its height and width dimensions
	SDL_QueryTexture(form_texture,NULL,NULL,&form_area.w,&form_area.h);

	//set up active area locations
	exit.set_loc(0,0,25,25);
	help.set_loc(0,25,25,25);
	right_arrow.set_loc(775,0,25,50);
	left_arrow.set_loc(700,0,25,50);
}

void form::set_form_title(std::string new_title){
	//the form title should have been set by the init function, if it wasn't, something is wrong
	if(form_title_surface == NULL){
		error_logger.push_error("Couldn't switch the form's title, it was previously null.");
	}
	//destroy the old form title surface
	SDL_FreeSurface(form_title_surface);
        
	//save the new title surface
	form_title = new_title;

	//make this font a little bit bigger than the others
	TTF_Font* title_font = TTF_OpenFont( "./Assets/fonts/LiberationSerif-Regular.ttf", 28);
	//render the text as a graphic
	SDL_Color black = {0,0,0,0};//black text
	form_title_surface = TTF_RenderUTF8_Blended(title_font,form_title.c_str(),black);

    //before we can draw the form title to the surface, 
    //we must first fill over the old form title. Make sure the color is the same as the form's
    SDL_Rect source, destination;
    destination = {30,0,650,50};//set destination to the region we have to draw over

    //draw a rectangle over that area, with the gray color used in the form's asset
    //change the SDL_MapRGBA call to match the new color if the form is changed, thought it
    //is possible to query the surface directly and get the color info that way....
    SDL_FillRect(form_surface,&destination,SDL_MapRGBA(form_surface->format,119,111,103,255));

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
	form_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,form_surface);
}

void form::form_event_loop(SDL_Event& big_event){

	SDL_StartTextInput();//turn on text input
	//sdl_helper->toggle_resizable();//forbid resizing the window

	//toggle to true to end the loop
	bool done = false;

	//used to prevent one click causing multiple things to happen, like repeatedly
	bool click_lock = false;

	while(!done){
	//cout << "IN FORM MINI LOOP" << endl;
	if( !SDL_PollEvent(&big_event) ){
		big_event.type = 1776;//arbitrary do-nothing event pushed onto queue, so it doesn't hit any cases
	}
	//if(big_event.type != 1776) cout << "BIG EVENT TYPE:" << big_event.type << endl;
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
			click_lock = false;//reset click lock, they let go finally
			break;
		case SDL_MOUSEWHEEL:
			break;
		case SDL_WINDOWEVENT:
			break;
		case 1776://nop
			break;

		default:
			break;

	}
	draw_me();
	sdl_helper->present();
	SDL_Delay(50);

 }

	//sdl_helper->toggle_resizable();//allow resizing the window
	SDL_StopTextInput();//turn off text input, it hurts performance to leave it on when it's not needed
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

		//consider the text boxes on the currently displayed page, so long as it is not currently displaying the help message
		} else if(!help_shown){

			bool found = false;//used to kick out of the loop after the text box that was clicked has been found
			if(!pages.size() == 0){

				string command;//filled by text_box_loop to tell this loop to do things

				for(unsigned int c = 0; c < pages[current_page].get_const_text_boxes().size() && !found ;c++){

					//enter text box loop for the matching text box, where the current text box was
					//either clicked, or our index, 'c', was set for us by command being equal to "TAB"
					if(pages[current_page].get_text_boxes()[c].was_clicked(mouse_event) ||
					   command == "TAB" ){
						if(command == "TAB") command = "";//reset command container if it was set

                        //the columns should line up with the supplied vector of regular expressions
                        int pattern_index = 0;
						if(pages[current_page].get_row_labels().size() == 0){						
                        	pattern_index =  c % pages[current_page].get_columns();
						//if there is a column of row labels for this page,
						//mod by the number of actual input columns
						} else {
							pattern_index  = c % (pages[current_page].get_columns() - 1);
						}
                        //cout << "NUM COLUMNS: " << pattern_index << endl;
                        //cout << "PATTERN INDEX: " << pattern_index << endl;
                        //cout << "MY PATTERNS SIZE: " << my_patterns.size() << endl;
						text_box_loop(pages[current_page].get_text_boxes()[c],mouse_event,command,
                                      my_patterns[pattern_index]);

						if(command == "TAB" &&  c < pages[current_page].get_text_boxes().size()){
							continue;//redo this step, but act on the next text box
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
			SDL_RenderCopy(sdl_helper->renderer,form_texture,NULL,&form_area);
			if(!pages.empty()){
				pages[current_page].draw_me();
			}
		} else {
			SDL_RenderCopy(sdl_helper->renderer,form_texture,NULL,&form_area);
			SDL_Rect help_area = form_area;
			help_area.y = 50;
			help_area.h = 725;//the texture is actually shorter, it replaces the text box area
			SDL_RenderCopy(sdl_helper->renderer,help_texture,NULL,&help_area);
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

	//it should be assumed that inputs are correct, but if one is found not to be
	//change this to false
	bool return_me = true;

	//loop over each page in this form
	for(unsigned int c = 0; c < pages.size(); c++){

		//get the number of columns for this page
		unsigned int num_columns = pages[c].get_columns();

		//if this page happens to have a column of row labels, decrement the # of columns
		//by one to ensure that only columns that have a logical meaning are considered
		//in the evaluation
		if(pages[c].get_const_row_labels().size() != 0){
			num_columns = num_columns - 1;
		}

		//then make sure each column has its own regular expression to verify inputs with
		if(num_columns != my_patterns.size()){
			error_logger.push_error("From form::check_values() A form doesn't have a regular expression to check each",
				"of its column's inputs against to ensure input integrity.");
		}

		//save a reference to this page's vector in a temporary variable
		//so I can use it instead of the getter function
		const vector<text_box>& box_ref = pages[c].get_const_text_boxes();

		//go through and check each text box against the appropriate regular expression
		//using the mod operator
		for(unsigned int d = 0; d < box_ref.size(); d++){
			if(!regex_match(box_ref[d].text,my_patterns[d % num_columns])){
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
		error_logger.push_error("page count greater than 10, form construction may not work properly");
	}

	SDL_Rect source = {20+page_count*20,0,20,20};
	SDL_Rect destination = {751,27,20,20};//destination is shifted right and down a bit, so fill doesn't mess with "/" in page display

	//"erase" previous page # by filling with white
	if(SDL_FillRect(form_surface,&destination,SDL_MapRGBA(form_surface->format,255,255,255,255)) != 0){
		error_logger.push_error(SDL_GetError());
	}

	destination = {750,26,20,20};//put the destination back how it was, so numbers are drawn to the same location
	SDL_BlitSurface(number_sprites,&source,form_surface,&destination);//"print" new max page # to surface

	if(form_texture != NULL){
		SDL_DestroyTexture(form_texture);	
		form_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,form_surface);//update the texture
	} else {
		error_logger.push_error("Error in set_page_count, previous form texture was NULL");
	}
}

void form::update_page_indicator(){

	SDL_Rect source = {20+current_page*20,0,20,20};
	SDL_Rect destination = {725,0,20,20};

	//write over old number, kind of like erasing just that part of the surface
	if(SDL_FillRect(form_surface,&destination,SDL_MapRGBA(form_surface->format,255,255,255,255)) != 0){
		error_logger.push_error(SDL_GetError());
	}

	SDL_BlitSurface(number_sprites,&source,form_surface,&destination);//draw in new number

	if(form_texture != NULL){
		SDL_DestroyTexture(form_texture);
		form_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,form_surface);
	} else {
		error_logger.push_error("Error in update_page_indicator, previous form texture was NULL");
	}


}

void form::flush_pages(){
	pages.clear();//clear out the vector
	page_count = 0;
	current_page = 0;
	update_page_indicator();
}

void form::text_box_loop(text_box& current_box,SDL_Event& event,string& command,
                         const regex& pattern){

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
		//if(event.type != 1776) cout << "Text box loop type:" << event.type << endl;
		switch(event.type){
		  case SDL_MOUSEMOTION:
			break;

		  case SDL_MOUSEBUTTONDOWN:
			//if the click was within the text box, move the cursor maybe
		  	if( current_box.was_clicked(event) ){
				error_logger.push_msg("Text box click at "+to_string(event.button.x)+":"+to_string(event.button.y));
		  	} else { //elsewise exit text input mode, user clicked off the text box
		  		error_logger.push_msg("Clicked outside of the text box, exiting mini-loop");

				SDL_Event keyup_event;
				keyup_event.type = SDL_MOUSEBUTTONUP;//putting in this key up removes the click locking
				SDL_PushEvent(&keyup_event);	     //for the loop in form_event_loop


				SDL_PushEvent(&event);//doing this allows the user to 'hop' to another text box
						      //directly from editing another box

				done = true;
			}
		  	break;

		  case SDL_TEXTINPUT:
			pass_me = event.text.text;
			current_box.update_text(pass_me,pattern);
			text_was_changed = true;
		  	//here this actually causes a loss of letters, so the event flooding is necessary, don't flush
			//SDL_FlushEvent(SDL_TEXTINPUT);
			break;

		  case SDL_KEYDOWN:
			
			if(event.key.keysym.sym == SDLK_BACKSPACE){
				//they hit backspace, so delete the end character if it is non-empty
				current_box.back_space(pattern);
				text_was_changed = true;
			} else if(event.key.keysym.sym == SDLK_LEFT){

                current_box.dec_cursor(text_was_changed);

			} else if(event.key.keysym.sym == SDLK_RIGHT){

                current_box.inc_cursor(text_was_changed);
			
			} else if(event.key.keysym.sym == SDLK_TAB){//tab over to next text box
				command = "TAB";
				return;
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
			draw_me();
			text_was_changed = false;
			//show updated picture
			sdl_helper->present();
		}

		//c++;
		//SDL_Delay(50);
	}//end of loop
	SDL_StopTextInput();//stop text input functionality because it slows down the app

}

//###################### PAGE CLASS BELOW #############################################################
page::page(){
	num_columns = 0;
	num_rows = 0;

	sdl_helper = NULL;
	sdl_font = NULL;
}

page::page(const page& other){
	num_columns = other.num_columns;
	num_rows = other.num_rows;
	column_labels = other.column_labels;
	row_labels = other.row_labels;
	sdl_helper = other.sdl_helper;
	sdl_font = sdl_font;
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
/*
void page::page_init(unsigned int num_columns_in, unsigned int num_rows_in,const vector<string>& column_labels_in,
	   vector<string>& row_labels_in,sdl_help* sdl_helper_in,TTF_Font* sdl_font_in,int additional_spacing){
*/
/*
void page::page_init(unsigned int num_columns_in, unsigned int rows_needed,
			const vector<string>& column_labels_in, vector<string>& row_labels_in,
			sdl_help* sdl_helper_in,TTF_Font* sdl_font_in,int additional_spacing){
*/
void page::page_init(unsigned int num_columns_in, unsigned int rows_needed,
		     const vector<string>& column_labels_in, vector<string>& row_labels_in,
		     sdl_help* sdl_helper_in,TTF_Font* sdl_font_in,const vector<int>& column_spacings){

	//abstracts some of the assignments to save space here
	page_init_local_var(num_columns_in, rows_needed, column_labels_in, row_labels_in, sdl_helper_in, sdl_font_in);

	bool row_labels_exist = false;//turn true to make room for row labels
	int x_start_point = 0;//used to control where the text boxes start to be drawn


	if(num_columns_in != column_spacings.size()){
		cout << num_columns_in << ":" << column_spacings.size() << endl;
		cout << "There has not been enough spacing values supplied to page init."
		     << "Column_spacings should have one less element than num_columns_in." << endl;
		error_logger.push_msg("There has not been enough spacing values supplied to page init.");
		error_logger.push_msg("Column_spacings should have one less element than num_columns_in.");
		error_logger.push_msg("This could be by design, as is the case with ICNTRL6's parity form.");
	}


	//implement logics for setting row labels here
	//how does this affect the placing of the actual text boxes?
	if(row_labels_in.size() != 0){
		page_init_set_row_labels(row_labels_in,x_start_point);
		row_labels_exist = true;
	}


	page_init_set_text_boxes(x_start_point,column_spacings,row_labels_exist);

	page_init_column_labels(column_spacings,x_start_point,row_labels_exist);


}
void page::page_init_local_var(unsigned int num_columns_in, unsigned int rows_needed, const vector<string>& column_labels_in,
			       vector<string>& row_labels_in, sdl_help* sdl_helper_in,TTF_Font* sdl_font_in){
	num_columns = num_columns_in;
	num_rows = rows_needed;
	column_labels = column_labels_in;
	row_labels = row_labels_in;
	sdl_helper = sdl_helper_in;
	sdl_font = sdl_font_in;
}

void page::page_init_set_row_labels(const std::vector<std::string>& row_labels_in,int& x_start_point){


		SDL_Color black = {0,0,0,0};
		SDL_Surface* temp_surface = NULL;
		SDL_Texture* temp_texture = NULL;


		for(unsigned int c = 0; c < row_labels_in.size(); c++){
			row_label_textures.push_back(temp_texture);//shove in null pointer

			//create surface from the text
			temp_surface = TTF_RenderUTF8_Blended(sdl_font,row_labels[c].c_str(),black);

			int width;
			int height;

			
			if(TTF_SizeText(sdl_font,row_labels[c].c_str(),&width,&height) != 0){
				error_logger.push_error("Error while making row labels in page, TTF_SizeText failure.",
							TTF_GetError());
			}

			
			//make the previously shoved null pointer now point to a texture created from the surface
			row_label_textures.back() = SDL_CreateTextureFromSurface(sdl_helper->renderer,temp_surface);

			//calc location & use the size
			//explicit cast of c to int stops compiler warnings
			SDL_Rect temp_rect = {TEXT_BOX_HORIZ_PADDING,80+25*int(c)+10*int(c), width, height};
			if(width > x_start_point){
				x_start_point = width;//update the starting point tracker
			}


			row_label_rects.push_back(temp_rect); //push that info into the row label vector
		}


}

void page::page_init_set_text_boxes(int& x_start_point,const vector<int>& column_spacings,bool& row_labels_exist){
	if(x_start_point != 0) x_start_point += 10;//give it some wiggle room

	
	unsigned int j = 0;  //this variable declared here, because if row labels exist, the inner loop
			     //should start at 1 instead of at 0
	if(row_labels_exist){
		j = 1;
	}

	//had a hard fight with the Rule of Three here.... 
	//implementing a copy constructor stopped the double free() crash at the text_boxes.push_back
	int x_offset = 0;
	for(unsigned int i = 0; i < num_rows; i++){
		x_offset = 0;
		for( ; j < num_columns; j++){
			text_box new_text_box;
			//int x_val = x_start_point+TEXT_BOX_W*i+TEXT_BOX_HORIZ_PADDING*i+column_spacings[i];
			int x_val = x_start_point+x_offset+column_spacings[j];
			x_offset += column_spacings[j];
			int y_val = 80+25*i+10*i;
			new_text_box.init(sdl_helper,sdl_font,"",x_val,y_val,60,25);
			text_boxes.push_back(new_text_box);
		}
		//reset the j variable, because the for loop will no longer do it
		//since it was declared at the start of the function and not in the loop
		if(row_labels_exist) j = 1;
		else j = 0;
	}

	//but, I think another solution could have been to tell the vector make 'x' text_boxes, and then init the copies
	//that exist inside the vector

}

void page::page_init_column_labels(const vector<int>& column_spacings,int& x_start_point,bool& row_labels_exist){


	int x_offset = 0;
	//set up column labels
	SDL_Color black = {0,0,0,0};
	SDL_Surface* temp_surf = NULL;
	SDL_Texture* temp_texture = NULL;
	for(unsigned int c = 0; c < column_labels.size();c++){
		column_label_textures.push_back(temp_texture);//shove in a null pointer

		temp_surf = TTF_RenderUTF8_Blended(sdl_font,column_labels[c].c_str(),black);//render the text

		//make the pointer we shoved point at the desired texture
		column_label_textures.back() = SDL_CreateTextureFromSurface(sdl_helper->renderer,temp_surf);

		//calculate drawing info for the column label
		SDL_Rect temp_rect = {x_start_point+x_offset+column_spacings[c],50,0,0};
		x_offset += column_spacings[c];
		//size the text and shove it into the vector
		TTF_SizeText(sdl_font,column_labels[c].c_str(),&temp_rect.w,&temp_rect.h);
		column_label_rects.push_back(temp_rect);//shove it into the drawing info vector

		SDL_FreeSurface(temp_surf);//give memory back
		temp_surf = NULL;//reset surf pointer for next item
	}

}

void page::draw_me(){

	if( column_label_textures.size() != column_label_rects.size() ){
		error_logger.push_error("The vector that saves drawing location for page column labels does not have",
					" the same size as the vector that contains the textures. Aborting drawing.");
		return;
	}

	for(unsigned int c = 0; c < column_label_textures.size();c++){
		if(SDL_RenderCopy(sdl_helper->renderer,column_label_textures[c],NULL,&column_label_rects[c]) != 0){
			error_logger.push_error("Could not draw column title.");
		}
	}

	if( row_label_textures.size() != row_label_rects.size() ){
		error_logger.push_error("The vector that saves the drawing location for the page row labels does not",
					" have the same size as the vector that contains the textures. Aborting drawing.");
		return;
	}

	for(unsigned int c = 0; c < row_label_textures.size();c++){
		if( SDL_RenderCopy(sdl_helper->renderer,row_label_textures[c],NULL,&row_label_rects[c]) != 0 ){

		}

	}

	//cout << "Text boxes:" << text_boxes.size() << endl;
	for(unsigned int c = 0; c < text_boxes.size();c++){
		text_boxes[c].draw_me();
	}

}







































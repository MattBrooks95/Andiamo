//! \file text_box.cc implements the functions declared in text_box.h
#include "text_box.h"
#include "sdl_help.h"

using namespace std;

extern asset_manager* asset_access;

text_box::text_box(TTF_Font* font_in, string text_in, int xloc_in, int
					yloc_in,int width_in, int height_in){

	font = font_in;

	//set up the text to be black
	text_color.r = 0;
	text_color.g = 0;
	text_color.b = 0;
	text_color.a = 0;


	xloc = xloc_in;
	yloc = yloc_in;

	width = width_in;
	height = height_in;

	x_scroll = NULL;
	y_scroll = NULL;

	input_test_regex = NULL;

	bad_input = false;
	editing_location = 0;
	shown_area       = {0,0,0,0};

	text_box_texture = NULL;

	text_surface = NULL;
	text_texture = NULL;

	bad_texture  = NULL;
}

text_box::text_box(const text_box& other){
	my_rect = other.my_rect;

	text_color = other.text_color;

	xloc = other.xloc;
	yloc = other.yloc;

	width  = other.width;
	height = other.height;

	x_scroll = other.x_scroll;
	y_scroll = other.y_scroll;

	text      = other.text;
	bad_input = other.bad_input;

	text_dims  = other.text_dims;
	shown_area = other.shown_area;


	font = other.font;

	editing_location = other.editing_location;

	//init the cursor, this needs to be called or else cursor
	//won't have a reference to sdl class, or this class's location
	//and may cause a seg fault
	my_cursor.init(&my_rect);

	string text_b_target = "Images/text_box.png";
	text_box_texture     = asset_access->get_texture(text_b_target);

	string bad_target = "Images/bad_tile.png";
	bad_texture = asset_access->get_texture(bad_target);

	if(sdl_access != NULL && font != NULL){
		text_surface = TTF_RenderUTF8_Blended(font,text.c_str(),text_color);
		text_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,
														text_surface);
	}
}

// text_box::~text_box(){
// }

void text_box::init(TTF_Font* font_in, string text_in, int xloc_in, int yloc_in,
					int width_in, int height_in){

	font = font_in;
	text = text_in;

	//set up location
	xloc = xloc_in;
	yloc = yloc_in;

	width = width_in;
	height = height_in;

	//set up rect version of the location
	my_rect.x = xloc;
	my_rect.y = yloc;
	my_rect.w = width;
	my_rect.h = height;

	//call the cursor's set up function
	my_cursor.init(&my_rect);

	//load the same text box image used by the tiles
	string texture_path = "Images/text_box.png";
	text_box_texture = asset_access->get_texture(texture_path);
	if(text_box_texture == NULL) output_access->push_error(SDL_GetError());

	string bad_path = "Images/bad_tile.png";
	bad_texture = asset_access->get_texture(bad_path);
	if(bad_texture == NULL) output_access->push_error(SDL_GetError());

	text_surface = TTF_RenderUTF8_Blended(font,text.c_str(),text_color);

	if(text_surface == NULL) output_access->push_error(SDL_GetError());
	text_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,text_surface);
	if(text_texture == NULL) output_access->push_error(SDL_GetError());

	TTF_SizeText(font,text.c_str(),&text_dims.w,&text_dims.h);

	//x source should start at 0, but be shifted as needed to control what
	//parts of the text are shown, y should likely always stay 0
	shown_area.x = 0;
	shown_area.y = 0;
	shown_area.w = my_rect.w;
	shown_area.h = my_rect.h;

}

void text_box::set_regular_expression(regex* test_regex){

	if(test_regex == NULL){
		output_access->push_msg("Textbox set_regular_expression given a NULL pointer.");
	}

	input_test_regex = test_regex;

}

void text_box::set_scrolling(){

	x_scroll = sdl_access->get_xscroll_ptr();
	y_scroll = sdl_access->get_yscroll_ptr();

}

void text_box::print_me(){

	output_access->push_msg("Printing text box.");
	output_access->push_msg("My location: "+to_string(xloc)+":"+to_string(yloc)+" "+to_string(width)+":"+to_string(height));
	output_access->push_msg("My_rect:"+to_string(my_rect.x)+":"+to_string(my_rect.y)+" "+to_string(my_rect.w)+":"
						  +to_string(my_rect.h));
	output_access->push_msg("Color: "+to_string(text_color.r)+":"+to_string(text_color.g)+":"+to_string(text_color.b)
							  +":"+to_string(text_color.a));
	output_access->push_msg("Text: "+text);
	output_access->push_msg("sdl_access ptr: "+to_string(size_t(sdl_access)));
	output_access->push_msg("font: "+to_string(size_t(font)));

	output_access->push_msg("text box texture "+
							to_string(size_t(text_box_texture)));

	output_access->push_msg("Text surface: "+to_string(size_t(text_surface))+" text texture "
				  +to_string(size_t(text_texture)));

	output_access->push_msg("bad box texture " + to_string(size_t(bad_texture)));
}

void text_box::draw_me(){

	SDL_Rect modified_rect = my_rect;
	if(x_scroll != NULL && y_scroll != NULL){
		modified_rect.x += *x_scroll;
		modified_rect.y += *y_scroll;
	}

	//if the text box hasn't been given bad input, or this text box
	//just "doesn't care" then draw the normal white box
	if(!bad_input){

		//render the white box
		SDL_RenderCopy(sdl_access->renderer,text_box_texture,NULL,&modified_rect);

	//elsewise, draw the text box as being red, to indicate that it has been given bad input
	} else {

		SDL_RenderCopy(sdl_access->renderer,bad_texture,NULL,&modified_rect);
	}

	if(text != " " && !text.empty() ){

		//start off by figuring out where the cursor would be drawn if we didn't care
		//about it going off the end of the text box
		int raw_cursor_location;

		//my_cursor.calc_location returns what it's x value would be if
		// it didn' care about the constraining text box
		raw_cursor_location = my_cursor.calc_location(sdl_access->font,
														text,editing_location);

		if(x_scroll == NULL && y_scroll == NULL){
			my_cursor.draw_me();
		} else {
			my_cursor.draw_me(*x_scroll,*y_scroll);
		}

		//use the entire text texture if it is smaller than the
		//text box's width this is the simplest case
		if(text_dims.w < my_rect.w){

			SDL_Rect mod_text_rect;
			//half of the info will be the same
			mod_text_rect   = modified_rect;
			//width and height should match text exactly
			mod_text_rect.w = text_dims.w;
			mod_text_rect.h = text_dims.h;
			SDL_RenderCopy(sdl_access->renderer,text_texture,NULL,&mod_text_rect);

		//if the text is bigger than the text box, use the shown_area as
		//source info things get a bit more complicated here
		} else {
			//calculate which sub-bin of the text texture to show based on
			//the return value of the previously called my_cursor.calc_location
			//this math gives an integer that tells us how many text boxes
			//of space exist, and which one the cursor is in
			int sub_area_selector = floor(raw_cursor_location / modified_rect.w);

			//make the start area for the source be the beginning of
			//the appropriate bin
			shown_area.x = 0 + modified_rect.w * sub_area_selector;

			//figure out if the text destination must be made smaller to avoid
			//the stretching that results from assuming that the last sub
			//bin will have the same width as the text box
			int dist_from_end = (shown_area.x + my_rect.w) - text_dims.w;
			SDL_Rect mod_dest = modified_rect;
			SDL_Rect mod_src  = shown_area;


			// is the cursor in the last bin?

			//calculate the number of bins before the last bin
			int number_of_bins = (floor(text_dims.w / modified_rect.w));

			//find the beginning of the last bin
			int last_bin_begin = modified_rect.w * number_of_bins;

			//is the last bin's text shorter than the bin?
			bool partial_bin = (dist_from_end < modified_rect.w);

			//adjust the source dimensions to match text exactly
			if(raw_cursor_location > last_bin_begin && partial_bin){
				mod_dest.w = modified_rect.w - dist_from_end;
				mod_src.w  = modified_rect.w - dist_from_end;
			}

			//at this point, mod_src and mod_dest have either been modified
			//to match the text in the last sub bin, or they haven't
			//been touched and using the whole sub-bin as a source and
			//the whole text box as a destination is fine
			SDL_RenderCopy(sdl_access->renderer,text_texture,&mod_src,&mod_dest);
		}
	}
}

void text_box::make_rect(){

	my_rect.x = xloc;
	my_rect.y = yloc;
	my_rect.w = width;
	my_rect.h = height;

}

void text_box::update_text(const string& new_text){

	text.insert(editing_location,new_text);
	if(input_test_regex != NULL){
		check_text();
	}

	editing_location += strlen(new_text.c_str());
	TTF_SizeText(sdl_access->font,text.c_str(),&text_dims.w,&text_dims.h);
	shown_area.h = text_dims.h;

	//update the texture for the text
	update_texture();
}

void text_box::check_text(){

	if (input_test_regex == NULL){
		return;
	}

	bool test_result = regex_match(text,*input_test_regex);
	if(!bad_input){
		if(text != " " && !test_result){
			set_error_state();
		}
	} else{
		if(text == " " || test_result){
			cancel_error_state();
		}
	}

}

void text_box::set_error_state(){
	bad_input = true;
}

void text_box::cancel_error_state(){
	bad_input = false;
}

void text_box::inc_cursor(bool& text_was_changed){
	if(editing_location == text.size()) return;
	my_cursor.right(text,editing_location,text_was_changed);
}

void text_box::dec_cursor(bool& text_was_changed){
	if(editing_location == 0) return;
	my_cursor.left(text,editing_location,text_was_changed);

}

void text_box::update_texture(){

	if(text_surface != NULL){
		SDL_FreeSurface(text_surface);
	}

	if(text_texture != NULL){
		SDL_DestroyTexture(text_texture);
	}

	if(font != NULL){

		text_surface = TTF_RenderUTF8_Blended(font,text.c_str(),text_color);

		if(text_surface == NULL){
			output_access->push_error(SDL_GetError());
			text_surface = TTF_RenderUTF8_Blended(font," ",text_color);
		}

		text_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,text_surface);

		if(text_texture == NULL){
			output_access->push_error(SDL_GetError());
		}

	} else {
		output_access->push_error("In text_box::update_texture, font is NULL.");
	}

}

void text_box::back_space(){

	if(editing_location == 0) return;

	//erase from current editing location
	text.erase(editing_location-1,1);
	check_text();

	//decrement editing location
	editing_location--;

	//update text size information
	TTF_SizeText(font,text.c_str(),&text_dims.w,&text_dims.h);

	update_texture();

}

bool text_box::was_clicked(SDL_Event& event){

	int real_xloc = xloc;
	int real_yloc = yloc;

	if(x_scroll != NULL && y_scroll != NULL){
		real_xloc += *x_scroll;
		real_yloc += *y_scroll;
	}

	bool below_top    = (event.button.x >= real_xloc);
	bool above_bottom = (event.button.x <= real_xloc+width);

	bool left_of_right = (event.button.y <= real_yloc + height);
	bool right_of_left = (event.button.y >= real_yloc);
	if( (below_top && above_bottom) && (left_of_right && right_of_left)){
		return true;
	}
	return false;
}

/*thanks to
*http://lazyfoo.net/tutorials/SDL/32_text_input_and_clipboard_handling/index.php
which was used as a reference */
void text_box::edit_loop(SDL_Event& event,string& command){

	//turn on the text input background functions
	SDL_StartTextInput();

	//used to control text entry loop
	bool done = false;

	//int c = 0;
	bool text_was_changed = false;

	//passable storage for event.text.text
	string pass_me;

	while(!done){

		if( !SDL_PollEvent(&event) ){
			//dummy event to stop it from printing default message every frame
			//where no event happens
			event.type = 1776;
		}

		switch(event.type){
			case SDL_MOUSEMOTION:
			break;

			case SDL_MOUSEBUTTONDOWN:
				//if the click was within the text box, move the cursor maybe
				//if( current_tile->my_text_box.was_clicked(event) ){
				if( was_clicked(event) ){
					string msg = "Text box click at " + to_string(event.button.x);
					msg       += ":" + to_string(event.button.y);
					output_access->push_msg(msg);

					//elsewise exit text input mode, user clicked off the text box
				} else {
					//doing this allows the user to 'hop' to another text box
					//directly from editing another box
					SDL_PushEvent(&event);
					done = true;
				}
				break;

			case SDL_TEXTINPUT:
				pass_me = event.text.text;
				update_text(pass_me);
				text_was_changed = true;
				//here event flooding is necessary, don't flush
				break;

			case SDL_KEYDOWN:

				edit_key_helper(event.key.keysym,text_was_changed,command);

				//prevent event flooding
				SDL_FlushEvent(SDL_KEYDOWN);
				break;

			case SDL_QUIT:
				//puts another sdl quit in the event queue, so program
				//can be terminated while in "text entry" mode
				SDL_PushEvent(&event);
				done = true;
			break;

			//do nothing, event was not new
			case 1776:
			break;

			default:
			output_access->push_msg("Error finding case in text entry mini-loop");
			break;
		}

		if(command.compare("TAB") == 0){
			return;
		}

		//update picture
		sdl_access->draw();
		sdl_access->present();

		text_was_changed = false;
	}//end of loop

	//stop text input functionality because it slows down the app
	SDL_StopTextInput();

}

void text_box::edit_key_helper(SDL_Keysym& key,bool& text_was_changed,
								string& command){

	switch( key.sym ){
		case SDLK_BACKSPACE:
			//delete last character, unless it's empty already than do nothing
			if( text.size() > 0 ){
				//delete a character, update text's graphics
				back_space();
				text_was_changed = true;
			}
			break;

		case SDLK_LEFT:
			//if we are not already at the very left of the text,
			//move the editing position one to the left
			dec_cursor(text_was_changed);
			break;
		case SDLK_RIGHT:
			//if we are not already at the very end of the text,
			//move the editing position one to the right
			inc_cursor(text_was_changed);
			break;

		case SDLK_TAB:
			//tell the loops calling this function that the user
			//hit tab, so we can enter the text box loop for
			//the next parameter over
			command = "TAB";
			break;
	  default:
		break;
	}

}

















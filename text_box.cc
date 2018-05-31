//! \file text_box.cc implements the functions declared in text_box.h
#include "text_box.h"
#include "sdl_help.h"
using namespace std;

extern asset_manager* asset_access;
extern string HOME;

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

	string text_b_target(HOME);
	text_b_target += "/Andiamo/Assets/Images/text_box.png";
	text_box_texture = asset_access->get_texture(text_b_target);

	string bad_target(HOME);
	bad_target += "/Andiamo/Assets/Images/bad_tile.png";
	bad_texture = asset_access->get_texture(bad_target);

	text_surface = TTF_RenderUTF8_Blended(font,text.c_str(),text_color);

	text_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,
												text_surface);

	if(sdl_access != NULL && font != NULL){
		text_surface = TTF_RenderUTF8_Blended(font,text.c_str(),text_color);
		text_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,
													text_surface);
	}
}

text_box::~text_box(){
}

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
	string texture_path(HOME);
	texture_path += "/Andiamo/Assets/Images/text_box.png";
	text_box_texture = asset_access->get_texture(texture_path);
	if(text_box_texture == NULL) error_logger.push_error(SDL_GetError());

	string bad_path(HOME);
	bad_path += "/Andiamo/Assets/Images/bad_tile.png";
	bad_texture = asset_access->get_texture(bad_path);
	if(bad_texture == NULL) error_logger.push_error(SDL_GetError());

	text_surface = TTF_RenderUTF8_Blended(font,text.c_str(),text_color);

	if(text_surface == NULL) error_logger.push_error(SDL_GetError());
	text_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,text_surface);
	if(text_texture == NULL) error_logger.push_error(SDL_GetError());

	TTF_SizeText(font,text.c_str(),&text_dims.w,&text_dims.h);

    //x source should start at 0, but be shifted as needed to control what
    //parts of the text are shown, y should likely always stay 0
    shown_area.x = 0;
    shown_area.y = 0;
    shown_area.w = my_rect.w;
    shown_area.h = my_rect.h;

}

void text_box::set_scrolling(){

	x_scroll = sdl_access->get_xscroll_ptr();
	y_scroll = sdl_access->get_yscroll_ptr();

}

void text_box::print_me(){

	error_logger.push_msg("Printing text box.");
	error_logger.push_msg("My location: "+to_string(xloc)+":"+to_string(yloc)+" "+to_string(width)+":"+to_string(height));
	error_logger.push_msg("My_rect:"+to_string(my_rect.x)+":"+to_string(my_rect.y)+" "+to_string(my_rect.w)+":"
			      +to_string(my_rect.h));
	error_logger.push_msg("Color: "+to_string(text_color.r)+":"+to_string(text_color.g)+":"+to_string(text_color.b)
	     		      +":"+to_string(text_color.a)); 
	error_logger.push_msg("Text: "+text);
	error_logger.push_msg("sdl_access ptr: "+to_string(size_t(sdl_access)));
	error_logger.push_msg("font: "+to_string(size_t(font)));

	error_logger.push_msg("text box texture "+
							to_string(size_t(text_box_texture)));

	error_logger.push_msg("Text surface: "+to_string(size_t(text_surface))+" text texture "
			      +to_string(size_t(text_texture)));
	
	error_logger.push_msg("bad box texture "
					      +to_string(size_t(bad_texture)));
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
		my_cursor.print();


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

			//do some modification to make the source dimensions
			//match the text in the last bin, if the cursor is in the last bin
			//the first clause in english is "Am I (the cursor) in the last sub bin?"
			if( raw_cursor_location > modified_rect.w * (floor(text_dims.w / modified_rect.w))
				//this second clause ensures that we only 'crop' the source box
				//when the text in the last sub-bin doesn't use the
				//whole sub-bin
				&& dist_from_end < modified_rect.w){

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
	editing_location += strlen(new_text.c_str());
	TTF_SizeText(sdl_access->font,text.c_str(),&text_dims.w,&text_dims.h);
	shown_area.h = text_dims.h;

	//update the texture for the text
	update_texture();
}

void text_box::update_text(string& new_text,const regex& test){

	text.insert(editing_location,new_text);
	check_text(test);
	editing_location += strlen(new_text.c_str());
	TTF_SizeText(sdl_access->font,text.c_str(),&text_dims.w,&text_dims.h);
	shown_area.h = text_dims.h;

	//update the texture for the text
	update_texture();
}

void text_box::check_text(const regex& test){
	bool test_result = regex_match(text,test);
	if(!bad_input){
		if(text != " " && !test_result){
			toggle_red();
		}
	} else {
		if(text == " " || test_result){
			toggle_red();
		}
	}


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
			error_logger.push_error(SDL_GetError());
				text_surface = TTF_RenderUTF8_Blended(font," ",text_color);
		}
		text_texture = SDL_CreateTextureFromSurface(sdl_access->renderer,text_surface);
		if(text_texture == NULL){
			error_logger.push_error(SDL_GetError());
		}

	} else {
		error_logger.push_error("In text_box::update_texture, font is NULL.");
	}

}

void text_box::toggle_red(){
	if(bad_input){
		bad_input = false;
	} else {
		bad_input = true;
	}
}

void text_box::back_space(){

	if(editing_location <= 0) return;

	//erase from current editing location
	text.erase(editing_location-1,1);

	//decrement editing location
	editing_location--;

	//update text size information
	TTF_SizeText(font,text.c_str(),&text_dims.w,&text_dims.h);
	update_texture();

}

void text_box::back_space(const regex& test){

	if(editing_location <= 0) return;

	//erase from current editing location
	text.erase(editing_location-1,1);
	check_text(test);
	//decrement editing location
	editing_location--;

	//update text size information
	TTF_SizeText(font,text.c_str(),&text_dims.w,&text_dims.h);

	check_text(test);
	update_texture();

}

//################# CLICK FUNCTIONS ##########################################

bool text_box::was_clicked(SDL_Event& event){
	int real_xloc = xloc;
	int real_yloc = yloc;

	if(x_scroll != NULL && y_scroll != NULL){
		real_xloc += *x_scroll;
		real_yloc += *y_scroll;
	}
	if( (event.button.x >= real_xloc && event.button.x <= real_xloc+width) &&
	    (event.button.y >= real_yloc && event.button.y <= real_yloc + height)){
	    return true;
	}
	return false;
}
//############################################################################




















//! \file text_box.cc this file implements the functions declared in text_box.h
#include "text_box.h"

using namespace std;

text_box::text_box(sdl_help* sdl_help_in,TTF_Font* font_in, string text_in, int xloc_in, int 
		   yloc_in,int width_in, int height_in){


	sdl_helper = sdl_help_in;
	sdl_help_font = font_in;

	//set up the text to be black
	text_color.r = 0;
	text_color.g = 0;
	text_color.b = 0;
	text_color.a = 0;


	xloc = xloc_in;	
	yloc = yloc_in;

	width = width_in;
	height = height_in; 

	editing_location = 0;
	shown_area       = {0,0,0,0};

	text_box_surface = NULL;
	text_box_texture = NULL;

	text_surface = NULL;
	text_texture = NULL;

}

text_box::text_box(const text_box& other){
	my_rect = other.my_rect;

	text_color = other.text_color;

	xloc = other.xloc;
	yloc = other.yloc;

	width  = other.width;
	height = other.height;

	text        = other.text;
	text_dims   = other.text_dims;
    shown_area = other.shown_area;


	sdl_helper = other.sdl_helper;
	sdl_help_font = other.sdl_help_font;

	editing_location = other.editing_location;

	text_box_surface = IMG_Load("./Assets/Images/text_box.png");;
	text_box_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,text_box_surface);

	text_surface = TTF_RenderUTF8_Blended(sdl_help_font,text.c_str(),text_color);
	text_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,text_surface);


}

text_box::~text_box(){
	if(text_box_surface == NULL || text_box_texture == NULL) {
		error_logger.push_error("Attempted double free in text_box deconstructor");
	} else {
		SDL_FreeSurface(text_box_surface);
		SDL_DestroyTexture(text_box_texture);
	}

	if(text_surface == NULL || text_texture == NULL){
		error_logger.push_error("Attempted double free in text_box deconstructor");
	} else {
		SDL_FreeSurface(text_surface);
		SDL_DestroyTexture(text_texture);
	}

}

void text_box::init(sdl_help* sdl_help_in,TTF_Font* font_in, string text_in, int xloc_in, int yloc_in,
		    int width_in, int height_in){

	sdl_helper = sdl_help_in;
	sdl_help_font = font_in;
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
	my_cursor.init(sdl_helper->renderer,&my_rect);


	//load the same text box image used by the tiles
	text_box_surface = IMG_Load("./Assets/Images/text_box.png");
	if(text_box_surface == NULL) error_logger.push_error(SDL_GetError());
	text_box_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,text_box_surface);
	if(text_box_texture == NULL) error_logger.push_error(SDL_GetError());

	text_surface = TTF_RenderUTF8_Blended(sdl_help_font,text.c_str(),text_color);
	if(text_surface == NULL) error_logger.push_error(SDL_GetError());
	text_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,text_surface);
	if(text_texture == NULL) error_logger.push_error(SDL_GetError());

	TTF_SizeText(sdl_help_font,text.c_str(),&text_dims.w,&text_dims.h);

    //x source should start at 0, but be shifted as needed to control what
    //parts of the text are shown, y should likely always stay 0
    shown_area.x = 0;
    shown_area.y = 0;
    shown_area.w = my_rect.w;
    shown_area.h = my_rect.h;

}

void text_box::print_me(){

	error_logger.push_msg("Printing text box.");
	error_logger.push_msg("My location: "+to_string(xloc)+":"+to_string(yloc)+" "+to_string(width)+":"+to_string(height));
	error_logger.push_msg("My_rect:"+to_string(my_rect.x)+":"+to_string(my_rect.y)+" "+to_string(my_rect.w)+":"
			      +to_string(my_rect.h));
	error_logger.push_msg("Color: "+to_string(text_color.r)+":"+to_string(text_color.g)+":"+to_string(text_color.b)
	     		      +":"+to_string(text_color.a)); 
	error_logger.push_msg("Text: "+text);
	error_logger.push_msg("sdl_help ptr: "+to_string(size_t(sdl_helper)));
	error_logger.push_msg("sdl_help_font: "+to_string(size_t(sdl_help_font)));

	error_logger.push_msg("Text box surface: "+to_string(size_t(text_box_surface))+" text box texture "
			      +to_string(size_t(text_box_texture)));
	error_logger.push_msg("Text surface: "+to_string(size_t(text_box_surface))+" text texture "
			      +to_string(size_t(text_texture)));
}

void text_box::draw_me(){
	SDL_RenderCopy(sdl_helper->renderer,text_box_texture,NULL,&my_rect);//render the white box
	if(text != " " && !text.empty() ){

		int raw_cursor_location;
		raw_cursor_location = my_cursor.calc_location(sdl_helper->font,text,editing_location);
		my_cursor.draw_me(sdl_helper->renderer);
		my_cursor.print(cout);


		//use the entire text texture if it is smaller than the text box's width
		if(text_dims.w < my_rect.w){
			SDL_Rect mod_rect;
			mod_rect   = my_rect; //half of the info will be the same 
			mod_rect.w = text_dims.w;  //width and height should match text exactly
			mod_rect.h = text_dims.h;
			SDL_RenderCopy(sdl_helper->renderer,text_texture,NULL,&mod_rect);

		//if the text is bigger than the text box, use the shown_area as source info
		} else {
			//calculate which sub-bin of the text texture to show based on the return value
			//of the previously called my_cursor.calc_location
			int sub_area_selector = floor(raw_cursor_location / my_rect.w);

			//make the start area for the source be the beginning of the appropriate bin
			shown_area.x = 0 + my_rect.w * sub_area_selector;

			//figure out if the text destination must be made smaller to avoid the stretching
			//that results from assuming that the last sub bin will have the same width as
			//the text box
			int dist_from_end = text_dims.w - raw_cursor_location;
			SDL_Rect mod_rect = my_rect;
			if(dist_from_end < my_rect.w){
				mod_rect.w = my_rect.w - dist_from_end;
			}
			SDL_RenderCopy(sdl_helper->renderer,text_texture,&shown_area,&mod_rect);
		}
	}
}

void text_box::make_rect(){
	my_rect.x = xloc;
	my_rect.y = yloc;
	my_rect.w = width;
	my_rect.h = height;

}

void text_box::update_text(string& new_text){

     //update_text_bounds_check(new_text);
	//add to the string
	text.insert(editing_location,new_text);
	editing_location += strlen(new_text.c_str());
	TTF_SizeText(sdl_helper->font,text.c_str(),&text_dims.w,&text_dims.h);
	shown_area.h = text_dims.h;

	//update the texture for the text
	update_texture();

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

	text_surface = TTF_RenderUTF8_Blended(sdl_help_font,text.c_str(),text_color);
	if(text_surface == NULL){
		error_logger.push_error(SDL_GetError());
		text_surface = TTF_RenderUTF8_Blended(sdl_help_font," ",text_color);
	}
	text_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,text_surface);
	if(text_texture == NULL){
		error_logger.push_error(SDL_GetError());
	}
}

void text_box::back_space(){
	if(editing_location <= 0) return;

	text.erase(editing_location-1,1);//erase from current editing location
	editing_location--;//decrement editing location

	//update text size information
	TTF_SizeText(sdl_help_font,text.c_str(),&text_dims.w,&text_dims.h);


	update_texture();

}

//##################### CLICK FUNCTIONS ############################################################

bool text_box::was_clicked(SDL_Event& event){
	if( (event.button.x >= xloc && event.button.x <= xloc+width) &&
	    (event.button.y >= yloc && event.button.y <= yloc + height) ) return true;
	return false;
}
//##################################################################################################




















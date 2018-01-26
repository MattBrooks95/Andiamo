//! \file scroll_bar.cc implements the functions defined in scroll_bar.h
#include<fstream>

#include "scroll_bar.h"

using namespace std;

extern asset_manager* asset_access;

//###################### CONSTRUCTORS/DESTRUCTORS #################################################
scroll_bar::scroll_bar(){

	//dummy initial values to hint if things weren't initialized properly by init()
	xloc = -1;
	yloc = -1;
	width = 0;
	height = 0;
	image_p = "notset";
	my_tex = NULL;

	//do not start out in scrolling mode
	scrolling_mode = false;

	//null out references to sdl_help members until the init function is ran
	x_scroll = NULL;
	y_scroll = NULL;
	window_width = NULL;
	window_height = NULL;
	renderer = NULL;
}
//#############################################################################

//############## GETTERS AND SETTERS ##########################################
bool scroll_bar::is_scrolling(){
	// return whether or not we are in scrolling mode
	return scrolling_mode;
}

void scroll_bar::scroll_mode_change(bool bool_in){
	scrolling_mode = bool_in;
	if(scrolling_mode){
		error_logger.push_msg("Scrolling mode changed to: true");
	} else {
		error_logger.push_msg("Scrolling mode changed to: false");
	}
}
//#############################################################################

///helper for init(), calcs corner location and sets up texture 
void scroll_bar::init_corner_texture(){

	my_tex = asset_access->get_texture(image_p);
	if(my_tex == NULL){
		//something went wrong, print error to the screen
		error_logger.push_error(string(SDL_GetError()));
		return;
	}
	//fills in width and height fields
	SDL_QueryTexture(my_tex,NULL,NULL,&width,&height);
	
	//we are dealing with a horizontal scroll bar
	if(width > height){
		//start off on the left of the screen 
		xloc = 0;
		if(window_height == NULL){
			error_logger.push_error("ERROR in init, null ptr window_height!");
			return;
		}
		//texture's bottom needs to be inline with window's bottom,
		//so its top right corner needs to be
		//window.height-texture.height 
		yloc = *window_height - height;

	//we are dealing with a vertical scroll bar
	} else {
		if(window_width == NULL){
			error_logger.push_error("ERROR in init, null ptr window_width!");
			return;
		}

		//similarly, texture's right side needs to be inline with the
		//window's right side, so top right corner's xval is
		//window.height - texture.height
		xloc = *window_width - width;
		//similarly start at the top of the screen
		yloc = 0;
	}
}
void scroll_bar::init(int* x_scroll_in, int* y_scroll_in,
				const int* window_width_in, const int* window_height_in,
				SDL_Renderer* renderer_in,string image_p_in){

	//initialize members that point to sdl_help object's members
	x_scroll = x_scroll_in;
	y_scroll = y_scroll_in;
	window_width = window_width_in;
	window_height = window_height_in;
	renderer = renderer_in;

	image_p = "./Assets/scroll_bars/" + image_p_in;
	init_corner_texture();

}

void scroll_bar::draw_me(){
	SDL_Rect dest = {xloc,yloc,width,height};
	SDL_RenderCopy(renderer,my_tex,NULL,&dest);
}

void scroll_bar::update(){
	if( x_scroll == NULL      || y_scroll == NULL || 
		window_height == NULL || window_width == NULL){

		error_logger.push_error("Error in scroll_bar::update(), x_scroll, y_scroll, window_width, or window_height pointers are null");
		return;
	}
	if(width > height){//logic for horizontal bar
		xloc = int(-(*x_scroll));

		if(xloc + width > *window_width){
			//don't go past right side of the screen
			xloc = *window_width - width;
		} else if(xloc < 0){
			//don't go past left side of the screen
			xloc = 0;
		}

	//logic for vertical bar
	} else {

		//move as a function of screen size vs scrollable size
		yloc =  int( -(*y_scroll)); 
		//don't go above the top of the window
		if(yloc < 0){
			yloc = 0;

		//don't go below bottom of the window
		} else if(yloc + height > (*window_height)){
			yloc = (*window_height) - height;
		}
	}
}

bool scroll_bar::in(int click_x,int click_y) const{
	if( (click_x > xloc && click_x < xloc+width) &&
	    (click_y > yloc && click_y < yloc+height) ) return true;
	return false;
}
bool scroll_bar::clicked(int click_x, int click_y) const{
	bool was_clicked = false;
	if( in(click_x,click_y)) was_clicked = true;
	return was_clicked;
}

void scroll_bar::handle_resize(){

	//dealing with horizontal scroll bar, because it is wider than it is tall
	if(width > height){
		//situate self at very bottom of the window
		//xloc = 0;//start on the very leftmost edge
		yloc = *window_height - height;

	//dealing with vertical scroll bar, because it is taller than it is wide
	//yloc = 0;//situate self at top of screen
	} else if(height > width) {
		//situate self at very right of window
		xloc = *window_width - width;
	}

}



















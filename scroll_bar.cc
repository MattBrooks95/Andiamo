//! \file scroll_bar.cc implements the functions defined in scroll_bar.h
#include<fstream>

#include "scroll_bar.h"

using namespace std;
//###################### CONSTRUCTORS/DESTRUCTORS #################################################
scroll_bar::scroll_bar(){
	xloc = -1;//dummy initial values to hint if things weren't initialized properly by init()
	yloc = -1;
	width = 0;
	height = 0;
	image_p = "notset";
	my_tex = NULL;
	my_surf = NULL;

	scrolling_mode = false;//do not start out in scrolling mode

	//null out references to sdl_help members until the init function is ran
	x_scroll = NULL;
	y_scroll = NULL;
	window_width = NULL;
	window_height = NULL;
	renderer = NULL;
}

scroll_bar::~scroll_bar(){
	SDL_FreeSurface(my_surf);
	SDL_DestroyTexture(my_tex);
}
//################################################################################################

//######################### GETTERS AND SETTERS ##################################################
bool scroll_bar::is_scrolling(){
	return scrolling_mode; // return whether or not we are in scrolling mode
}
void scroll_bar::scroll_mode_change(bool bool_in){
	scrolling_mode = bool_in;
	cout << "Scrolling mode changed to: " << scrolling_mode << endl;
}
//################################################################################################

///helper for init(), calcs corner location and sets up texture 
void scroll_bar::init_corner_texture(){
	my_surf = IMG_Load(image_p.c_str());
	if(my_surf == NULL){
		cout << SDL_GetError() << endl; //something went wrong, print error to screen
		return;
	}

	my_tex = SDL_CreateTextureFromSurface(renderer,my_surf); //turn surface into a texture
	if(my_tex == NULL){
		cout << SDL_GetError() << endl; //something went wrong, print error to the screen
		return;
	}
	SDL_QueryTexture(my_tex,NULL,NULL,&width,&height);//fills in width and height fields
	
	if(width > height){ //we are dealing with a horizontal scroll bar
		xloc = 0;//start off on the left of the screen 
		if(window_height == NULL){
			cout << "ERROR in init, null ptr window_height!" << endl;
			return;
		}
		yloc = *window_height - height;//texture's bottom needs to be inline with window's bottom,
					      //so its top right corner needs to be
					      //window.height-texture.height 

	} else { //we are dealing with a vertical scroll bar
		if(window_width == NULL){
			cout << "ERROR in init, null ptr window_width!" << endl;
			return;
		}
		xloc = *window_width - width; //similarly, texture's right side needs to be inline with the
					     //window's right side, so top right corner's xval is
					     //window.height - texture.height
		yloc = 0;//similarly start at the top of the screen
	}
}
void scroll_bar::init(int* x_scroll_in, int* y_scroll_in, const int* window_width_in, const int* window_height_in,
		      SDL_Renderer* renderer_in,string image_p_in){
	x_scroll = x_scroll_in; //initialize members that point to sdl_help object's members
	y_scroll = y_scroll_in;
	window_width = window_width_in;
	window_height = window_height_in;
	renderer = renderer_in;

	image_p = "./Assets/scroll_bars/" + image_p_in;
	init_corner_texture();

}
void scroll_bar::print(ostream& outs){
	if( x_scroll == NULL || y_scroll == NULL || window_width == NULL || window_height == NULL || renderer == NULL) {
		cout << "ERROR! One of the scrollbar's pointers to sdl_help fields is NULL."
		     << "Exiting scroll_bar::print() early " << endl;
		delete x_scroll; delete y_scroll;
		delete window_width; delete window_height;
		return;//get outta here
	}

}

void scroll_bar::draw_me(){
	SDL_Rect dest = {xloc,yloc,width,height};
	SDL_RenderCopy(renderer,my_tex,NULL,&dest);
}

void scroll_bar::update(){
	if(x_scroll == NULL || y_scroll == NULL || window_height == NULL || window_width == NULL){
		cout << "Error in scroll_bar::update(), x_scroll, y_scroll,"
		     << " window_width, or window_height pointers are null" << endl;
		return;
	}
	if(width > height){//logic for horizontal bar
		xloc = int(-(*x_scroll));

		if(xloc + width > *window_width){
			xloc = *window_width - width;//don't go past right side of the screen
		} else if(xloc < 0){
			xloc = 0;//don't go past left side of the screen
		}
	} else {//logic for vertical bar

		//move as a function of screen size vs scrollable size
		//cout << (*y_scroll) << " " << (*window_height) << ":" << (*area_height) << endl;
		yloc =  int( -(*y_scroll)); 
		if(yloc < 0){
			yloc = 0;//don't go above the top of the window
		} else if(yloc + height > (*window_height)){ //don't go below bottom of the window
			yloc = (*window_height) - height;
		}
	}
	cout << xloc << ":" <<  yloc << endl;
}

bool scroll_bar::in(int click_x,int click_y) const{
	if( (click_x > xloc && click_x < xloc+width) &&
	    (click_y > yloc && click_y < yloc+height) ) return true;
	return false;
}
bool scroll_bar::clicked(ostream& outs, int click_x, int click_y) const{
	bool was_clicked = false;
	if( in(click_x,click_y)) was_clicked = true;
	return was_clicked;
}

void scroll_bar::handle_resize(){

	if(width > height){//dealing with horizontal scroll bar, because it is wider than it is tall
		yloc = *window_height - height;//situate self at very bottom of the window
		//xloc = 0;//start on the very leftmost edge


	} else if(height > width) {//dealing with vertical scroll bar, because it is taller than it is wide
		//yloc = 0;//situate self at top of screen
		xloc = *window_width - width;//situate self at very right of window


	}

}



















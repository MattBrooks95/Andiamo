//! \file scroll_bar.cc implements the functions defined in scroll_bar.h

#include "scroll_bar.h"

using namespace std;

scroll_bar::scroll_bar(){
	xloc = -1;//dummy initial values to hint if things weren't initialized properly by init()
	yloc = -1;
	width = 0;
	height = 0;

	//null out references to sdl_help members until the init function is ran
	x_scroll = NULL;
	y_scroll = NULL;
	area_width = NULL;
	area_height = NULL;
	window_width = NULL;
	window_height = NULL;
	renderer = NULL;
}

scroll_bar::~scroll_bar(){
}
void scroll_bar::init(int* x_scroll_in, int* y_scroll_in, const int* area_width_in,const int* area_height_in,
		      const int* window_width_in, const int* window_height_in, SDL_Renderer* renderer_in){
	x_scroll = x_scroll_in; //initialize members that point to sdl_help object's members
	y_scroll = y_scroll_in;
	area_width = area_width_in;
	area_height = area_height_in;
	window_width = window_width_in;
	window_height = window_height_in;
	renderer = renderer_in;
}
void scroll_bar::print(ostream& outs){
	if( x_scroll == NULL || y_scroll == NULL || area_width == NULL || area_height == NULL ||
	    window_width == NULL || window_height == NULL || renderer == NULL) {
		cout << "ERROR! One of the scrollbar's pointers to sdl_help fields is NULL."
		     << "Exiting scroll_bar::print() early " << endl;
		delete x_scroll; delete y_scroll; delete area_width; delete area_height;
		delete window_width; delete window_height;
		return;//get outta here
	}
	cout << "x_scroll = " << *x_scroll << " | " << "y_scroll = " << *y_scroll << endl;
	cout << "area_width = " << *area_width << " | " << "area_height = " << *area_height << endl;
	cout << "window_width = " << *window_width << " | " << "window_height = " << *window_height << endl;
	cout << "renderer = " << renderer << endl;
}
//it doesn't actually do anything yet LOL
void update(){
}


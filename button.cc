//! \file buttons.cc implements the functions declared in buttons.h

#include "button.h"
using namespace std;

//###################### BUTTON class ########################################
button::button(sdl_help* sdl_help_in){

	width = 0;
	height = 0;
	xloc = 0;
	yloc = 0;

	my_rect = {0,0,0,0};

	shown = true;

	image_name = "No name.";
	total_image_p = "illegal image path";

	button_surface = NULL;
	button_texture = NULL;

	sdl_helper = sdl_help_in;
}

button::~button(){
	SDL_FreeSurface(button_surface);
	SDL_DestroyTexture(button_texture);
}

void button::init(const string& image_name_in,const string& image_p_in,sdl_help* sdl_help_in){

	sdl_helper = sdl_help_in;


	image_name = image_name_in;

	total_image_p = image_p_in + image_name;

	button_surface = IMG_Load(total_image_p.c_str());
	if(button_surface == NULL) cout << SDL_GetError() << endl;
	button_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,button_surface);
	if(button_texture == NULL) cout << SDL_GetError() << endl;

	SDL_QueryTexture(button_texture,NULL,NULL,&width,&height);

	set_corner_loc();
	make_rect();
}

//virtual
void button::print_me(){
	cout << "IMAGE NAME: " << image_name << " TOTAL IMAGE PATH: " << total_image_p << endl;
	cout << "SDL_HELP HOOK: " << sdl_helper << " XLOC:YLOC = " << xloc << ":" << yloc << endl;
	cout << "WIDTH = " << width << " HEIGHT = " << height << endl;
	cout << "SURFACE PTR: " << button_surface << " TEXTURE PTR: " << button_texture << endl;
	cout << "SHOWN? = " << shown << endl;
}

//virtual
void button::make_rect(){
	my_rect.x = xloc;
	my_rect.y = yloc;
	my_rect.w = width;
	my_rect.h = height;
}

//virtual
void button::draw_me(){
	if(shown){
		SDL_RenderCopy(sdl_helper->renderer,button_texture,NULL,&my_rect);
	}
}

//virtual 
void button::set_corner_loc(){
	xloc = 0;
	//this puts the tile in a fixed position just above the horizonal scroll bar
	//this is what the HUD style of buttons could look like
	yloc = sdl_helper->get_h_bar().get_top() - height;
}

//virtual
void button::handle_resize(int yloc_in){
	yloc = yloc_in;
	make_rect();
}



//virtual
void button::force_corner_loc(int xloc_in, int yloc_in){
	cout << "DEFAULT BUTTON OLD: " << xloc << ":" << yloc << endl;
	xloc = xloc_in;
	yloc = yloc_in;
	cout << "DEFAULT BUTTON NEW: " << xloc << ":" << yloc << endl;
	make_rect();
}

//################## virtual click members #############
bool button::handle_click(SDL_Event& mouse_event){
	if( was_clicked(mouse_event) ){
		click_helper(mouse_event);
		return true;
	}
	return false;
}

bool button::was_clicked(SDL_Event& mouse_event){
	if( (mouse_event.button.x > xloc && mouse_event.button.x < xloc + width) &&
	    (mouse_event.button.y > yloc && mouse_event.button.x < yloc + height) ){
		return true;
	}
	return false;
}

void button::click_helper(SDL_Event& mouse_event){
	cout << "default button click_helper" << endl;


}
//################# virtual click members ###############

//#############################################################################






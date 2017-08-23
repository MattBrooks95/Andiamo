//! \file form.cc implements the descriptions of the form class, which is found in form.h

#include "form.h"

using namespace std;


//######################## FORM CLASS #########################################################################
form::form(){
	form_title = "no title";

	form_title_surface = NULL;
	form_title_texture = NULL;

	form_surface = NULL;
	form_texture = NULL;

	form_area = {0,0,0,0};

	active = false;

	page_count = 0;
	current_page = 0;

	sdl_helper = NULL;
	sdl_font = NULL;

}

form::~form(){
	SDL_FreeSurface(form_title_surface);
	SDL_DestroyTexture(form_title_texture);

	

}

void form::form_init(string form_title_in,unsigned int page_count_in,SDL_Rect form_area_in,
		     sdl_help* sdl_helper_in,TTF_Font* sdl_font_in){
	form_title = form_title_in;
	page_count = page_count_in;
	form_area = form_area_in;
	
	sdl_helper = sdl_helper_in;
	sdl_font = sdl_font_in;	

}
void form::draw_me(){
	if(active){





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



}

void prev_page(){




}













//###################### PAGE CLASS BELOW #############################################################

page::page(unsigned int num_columns_in, unsigned int num_rows_in,vector<string>& column_labels_in,
	   vector<string>& row_labels_in,sdl_help* sdl_helper_in){
	num_columns = num_columns_in;
	num_rows = num_rows_in;
	column_labels = column_labels_in;
	row_labels = row_labels_in;
	sdl_helper = sdl_helper_in;
}

page::~page(){


}





















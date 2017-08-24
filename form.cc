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

	SDL_FreeSurface(form_surface);
	SDL_DestroyTexture(form_texture);

}

void form::form_init(string form_title_in,unsigned int page_count_in,int xloc_in, int yloc_in,
		     sdl_help* sdl_helper_in,TTF_Font* sdl_font_in){

	//set up state variables
	form_title = form_title_in;
	page_count = page_count_in;
	
	//set up pointers to graphics class
	sdl_helper = sdl_helper_in;
	sdl_font = sdl_font_in;	

	//initialize the form texture from it's asset file
	form_surface = IMG_Load("Assets/Images/form_assets/form.png");
	if(form_surface == NULL) error_logger.push_error(SDL_GetError());
	form_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,form_surface);
	if(form_texture == NULL) error_logger.push_error(SDL_GetError());

	//set the corner location as specified by xloc_in and yloc_in, then query the texture
	//to figure out its height and width dimensions
	form_area.x = xloc_in;
	form_area.y = yloc_in;
	SDL_QueryTexture(form_texture,NULL,NULL,&form_area.w,&form_area.h);


	//set up active area locations
	exit.set_loc(0,0,25,25);
	help.set_loc(0,25,25,25);
	right_arrow.set_loc(775,0,25,50);
	left_arrow.set_loc(700,0,25,50);
}

void form::handle_click(SDL_Event& mouse_event){
	if(exit.clicked(mouse_event)){
		cout << "Clicked the exit button." << endl;
	} else if(help.clicked(mouse_event) ){
		cout << "clicked the help button." << endl;
	} else if(right_arrow.clicked(mouse_event) ){
		cout << "clicked the page right button" << endl;
	} else if(left_arrow.clicked(mouse_event) ){
		cout << "clicked the page left button" << endl;
	}


}

void form::draw_me(){
	if(active){
		SDL_RenderCopy(sdl_helper->renderer,form_texture,NULL,&form_area);
		if(!pages.empty()){
			pages[current_page].draw_me();
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
	if(current_page < (pages.size() - 1) ) current_page++;

}

void form::prev_page(){
	if(current_page > 0) current_page--;

}






//###################### PAGE CLASS BELOW #############################################################

page::page(unsigned int num_columns_in, unsigned int num_rows_in,vector<string>& column_labels_in,
	   vector<string>& row_labels_in,sdl_help* sdl_helper_in,TTF_Font* sdl_font_in){
	num_columns = num_columns_in;
	num_rows = num_rows_in;
	column_labels = column_labels_in;
	row_labels = row_labels_in;
	sdl_helper = sdl_helper_in;
	sdl_font = sdl_font_in;
}

page::~page(){


}

void page::draw_me(){






}








































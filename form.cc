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
	number_sprites = NULL;

	form_area = {0,0,0,0};

	active = false;

	help_shown = false;
	page_count = 0;
	current_page = 0;

	sdl_helper = NULL;
	sdl_font = NULL;

	help_surface = NULL;
	form_texture = NULL;

}

form::~form(){
	SDL_FreeSurface(form_title_surface);
	SDL_DestroyTexture(form_title_texture);

	SDL_FreeSurface(form_surface);
	SDL_DestroyTexture(form_texture);

	SDL_FreeSurface(help_surface);
	SDL_DestroyTexture(help_texture);

	SDL_FreeSurface(number_sprites);
}

void form::init(string form_title_in,unsigned int page_count_in,int xloc_in, int yloc_in,
		     sdl_help* sdl_helper_in,TTF_Font* sdl_font_in){

	//set up state variables
	form_title = form_title_in;
	page_count = page_count_in;
	
	//set up pointers to graphics class
	sdl_helper = sdl_helper_in;
	sdl_font = sdl_font_in;	

	//initialize the page number spritesheet
	//these numbers are blitted onto the form surface, to indicate which page the user is on
	number_sprites = IMG_Load("Assets/Images/form_assets/number_sprites.png");
	if(number_sprites == NULL) error_logger.push_error(SDL_GetError());
	//###############################################################################################



	//initialize the form texture from its asset file
	form_surface = IMG_Load("Assets/Images/form_assets/form.png");
	if(form_surface == NULL){
		error_logger.push_error(SDL_GetError());
	} else {
		SDL_Rect source = {page_count*20,0,20,20};
		SDL_Rect destination = {750,26,20,20};
		SDL_BlitSurface(number_sprites,&source,form_surface,&destination);//draw max page # in top right

		source = {current_page*20,0,20,20};
		destination = {725,0,20,20};
		SDL_BlitSurface(number_sprites,&source,form_surface,&destination);//draw current page # (0) in rop right
	}
	form_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,form_surface);
	if(form_texture == NULL) error_logger.push_error(SDL_GetError());
	//################################################################################################

	//initialize the help message that is shown upon the question mark being clicked
	help_surface = IMG_Load("Assets/Images/form_assets/default_form_help.png");
	if(help_surface == NULL) error_logger.push_error(SDL_GetError());
	help_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,help_surface);
	if(help_texture == NULL) error_logger.push_error(SDL_GetError());
	//################################################################################################


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

void form::form_event_loop(SDL_Event& big_event){

	SDL_StartTextInput();//turn on text input
	//sdl_helper->toggle_resizable();//forbid resizing the window

	bool done = false;//toggle to true to end the loop
	bool click_lock = false;//used to prevent one click causing multiple things to happen, like repeatedly
				//opening and closing the help dialogue
 while(!done){
	//cout << "IN FORM MINI LOOP" << endl;
	SDL_PollEvent(&big_event);

	switch(big_event.type){
		case SDL_QUIT:
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

		if(exit.clicked(mouse_event)){
			cout << "Clicked the exit button." << endl;
			toggle_active();
			done = true;//end mini loop
		} else if(help.clicked(mouse_event) ){
			cout << "clicked the help button." << endl;
			if(!help_shown){
				help_shown = true;
			} else {
				help_shown = false;
			}

		} else if(right_arrow.clicked(mouse_event) ){
			cout << "clicked the page right button" << endl;
			if(current_page < pages.size()){
				current_page++;
			} else {
				cout << "can't page right" << endl;
			}
		} else if(left_arrow.clicked(mouse_event) ){
			cout << "clicked the page left button" << endl;
			if(current_page > 0){
				current_page--;
			} else {
				cout << "can't page left" << endl;
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
			help_area.y = 75;//start lower than the starting point for the form + the column headers
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
	if(current_page < (pages.size() - 1) ) current_page++;

}

void form::prev_page(){
	if(current_page > 0) current_page--;

}






//###################### PAGE CLASS BELOW #############################################################

page::page(unsigned int num_columns_in, unsigned int num_rows_in,const vector<string>& column_labels_in,
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

void page::set_parameters(const vector<string>& int4_list,const vector<string>& real8_list,
			  const vector<string>& string_list){

}

void page::set_parameters(const vector<string>& int4_list,const vector<string>& real8_list){
	set_parameters(int4_list);
	
}
	
void page::set_parameters(const vector<string>& int4_list){
	for(unsigned int c = 0; c < int4_list.size();c++){
	;		

	}
}


void page::draw_me(){






}








































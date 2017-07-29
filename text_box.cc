//! this file implements the functions declared in text_box.h
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


	cursor_surface = NULL;
	cursor_texture = NULL;
	editing_location = 0;

}


text_box::~text_box(){
	SDL_FreeSurface(text_box_surface);
	SDL_DestroyTexture(text_box_texture);

	SDL_FreeSurface(text_surface);
	SDL_DestroyTexture(text_texture);

	SDL_FreeSurface(cursor_surface);
	SDL_DestroyTexture(cursor_texture);
}

void text_box::init(sdl_help* sdl_help_in,TTF_Font* font_in, string text_in, int xloc_in, int yloc_in,
		    int width_in, int height_in){

	sdl_helper = sdl_help_in;
	sdl_help_font = font_in;


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


	//load the same text box image used by the tiles
	text_box_surface = IMG_Load("./Assets/Images/text_box.png");
	if(text_box_surface == NULL) cout << SDL_GetError() << endl;
	text_box_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,text_box_surface);
	if(text_box_texture == NULL) cout << SDL_GetError() << endl;

	text_surface = TTF_RenderUTF8_Blended(sdl_help_font,text.c_str(),text_color);
	if(text_surface == NULL) cout << SDL_GetError() << endl;
	text_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,text_surface);
	if(text_texture == NULL) cout << SDL_GetError() << endl;

	TTF_SizeText(sdl_help_font,text.c_str(),&text_dims.w,&text_dims.h);

	cursor_surface = IMG_Load("Assets/Images/cursor.png");
	if(cursor_surface == NULL) cout << SDL_GetError() << endl;
	cursor_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,cursor_surface);
	if(cursor_texture == NULL) cout << SDL_GetError() << endl;
}

void text_box::print_me(){
	cout << "Printing text box." << endl;
	cout << "My location: " << xloc << ":" << yloc << " " << width << ":" << height << endl;
	cout << "My_rect:" <<  my_rect.x << ":" << my_rect.y << " " << my_rect.w << ":" << my_rect.h << endl;
	cout << "Color: " << text_color.r << ":" << text_color.g << ":" << text_color.b
	     << ":" << text_color.a << endl;
	cout << "Text: " << text << endl;
	cout << "sdl_help ptr: " << sdl_helper << endl;
	cout << "sdl_help_font: " << sdl_help_font << endl;

	cout << "Text box surface: " << text_box_surface << " text box texture " << text_box_texture << endl;
	cout << "Text surface: " << text_box_surface << " text texture " << text_texture << endl;
}

void text_box::draw_me(){
	SDL_RenderCopy(sdl_helper->renderer,text_box_texture,NULL,&my_rect);//render the white box
	if(text != " " && !text.empty() ){
		//the following four lines make sure that the letters are not blurred or stretched
		SDL_Rect text_source = {0,0,0,0};
		TTF_SizeText(sdl_helper->font,text.c_str(),&text_source.w,&text_source.h);

		SDL_Rect text_destination = my_rect;
		text_destination.w = text_source.w; text_destination.h = text_source.h;

		SDL_RenderCopy(sdl_helper->renderer,text_texture,&text_source,&text_destination);//render the text
	}
	draw_cursor();
}

void text_box::draw_cursor(){

	int start_to_edit, no_point;//start_to_edit is length of string to edit point
				    //no point is a dummy, needed by SizeText
	TTF_SizeText(sdl_helper->font, text.substr(0,editing_location).c_str(),
				&start_to_edit,&no_point);

	int char_width;//grab the cursor's width
	TTF_SizeText(sdl_helper->font,(text.substr(editing_location,1)).c_str(),
				&char_width,&no_point);

	SDL_Rect cursor_dest;//save the coordinates and dimensions for the cursor
	if(editing_location != text.size()){
		cursor_dest = {xloc + start_to_edit, yloc,char_width, text_dims.h};
	} else {
		cursor_dest = {xloc+text_dims.w, yloc, 6, text_dims.h};
	}

	if( SDL_RenderCopy(sdl_helper->renderer,cursor_texture,NULL,&cursor_dest) ){
		cout << SDL_GetError() << endl;
	}
}

void text_box::make_rect(){
	my_rect.x = xloc;
	my_rect.y = yloc;
	my_rect.w = width;
	my_rect.h = height;

}

void text_box::update_text(string& new_text){
	//add to the string
	text.insert(editing_location,new_text);
	editing_location += strlen(new_text.c_str());
	TTF_SizeText(sdl_helper->font,text.c_str(),&text_dims.w,&text_dims.h);
	//update the texture for the text
	update_texture();
}

void text_box::update_texture(){
	SDL_FreeSurface(text_surface);
	SDL_DestroyTexture(text_texture);

	text_surface = TTF_RenderUTF8_Blended(sdl_help_font,text.c_str(),text_color);
	if(text_surface == NULL) cout << SDL_GetError() << endl;
	text_texture = SDL_CreateTextureFromSurface(sdl_helper->renderer,text_surface);
	if(text_texture == NULL) cout << SDL_GetError() << endl;
}

void text_box::back_space(){
	if(editing_location > 0){
		text.erase(editing_location-1,1);//erase from current editing location
		editing_location--;//decrement editing location
	}
	//update cursor size information
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




















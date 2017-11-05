//! file cursor.cc implements the class members found in cursor.h

#include "cursor.h"

using namespace std;

//#################### RULE OF THREE ######################################
cursor::cursor(){
	my_surface    = NULL;
	my_texture    = NULL;
	box_location  = NULL;
	cursor_dest   = {0,0,0,0};	

}

 
cursor::cursor(const cursor& other){

}

cursor::~cursor(){

}
//#########################################################################
void cursor::init(SDL_Renderer* renderer, SDL_Rect* box_location_in){
	string cursor_p = "./Assets/Images/cursor.png";

	my_surface = IMG_Load(cursor_p.c_str());
	if(my_surface == NULL){
		error_logger.push_error("Failed to load image: " + cursor_p); 
	}
	my_texture = SDL_CreateTextureFromSurface(renderer,my_surface);
	if(my_texture == NULL){
		error_logger.push_error("Failure to create cursor texture, surface was NULL");
	}

	//set up pointer to containing box's info
	box_location = box_location_in;
	cursor_dest.h = box_location->h;
}

int cursor::calc_location(TTF_Font* font,const string& text, const unsigned int& editing_location){

	//no point is a dummy height argument for TTF_SizeText to fill in
	//this is because the height of the cursor should never change,
	//but this function needs the parameter anyway
	int no_point;


	//start off by placing the cursor dest at the beginning of the containing
	//text box
	cursor_dest.x = box_location->x;
	cursor_dest.y = box_location->y;

	//if the editing location is at the very end of the string, give
	//it a constant width of 6, because it isn't hovering over a character
	if(editing_location == text.size()){
		cursor_dest.w   = 6;

	//elsewise, the cursor should resize itself based on the character it's hovering
	//over
	} else {

		TTF_SizeText(font,text.substr(editing_location,1).c_str(),
					 &cursor_dest.w,&no_point);
	}

	int start_to_edit;//calculate the x offset for cursor placement
	TTF_SizeText(font,text.substr(0,editing_location).c_str(),
				 &start_to_edit,&no_point);

	//calculate the actual spot where the cursor will be drawn
	cursor_dest.x += start_to_edit % box_location->w;
	
	//let text_box::draw_me determine which section of the text surface to use
	return start_to_edit;

}

void cursor::draw_me(SDL_Renderer* renderer){

	if( SDL_RenderCopy(renderer,my_texture,NULL,&cursor_dest) != 0){
		error_logger.push_error(SDL_GetError());
	}

}

void cursor::print(std::ostream& outs){
	outs << "-----------------------------------------------------------------------\n";
	outs << "Surface ptr: " << my_surface << " Texture ptr: " << my_texture << endl;
	outs << "Containing box's rectangle: "; print_sdl_rect(outs,*box_location);
	outs << "Destination rectangle: "; print_sdl_rect(outs,cursor_dest);
	outs << "-----------------------------------------------------------------------\n";
}

void cursor::print(){
	print(error_logger.get_stream());
}

void cursor::left(const string& text,unsigned int& editing_location,bool& changed){
	//this logic ensures the editing location is never negative
	if(editing_location > 0){
		editing_location--;
	    changed = true;
	}
}

void cursor::right(const string& text,unsigned int& editing_location,bool& changed){
    if(editing_location < text.length()){
        editing_location++;
        changed = true;
    }
}

void print_sdl_rect(ostream& outs,const SDL_Rect& print_me){
	outs << print_me.x << ":" << print_me.y << ":" << print_me.w << ":" << print_me.h
		 << endl;
}









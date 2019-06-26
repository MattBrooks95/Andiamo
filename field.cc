//! \file field.cc implements the functions declared in field.h
#include "field.h"
#include <regex>

#include "sdl_help.h"
#include "asset_manager.h"

using namespace std;

extern sdl_help* sdl_access;
extern asset_manager* asset_access;

field::field(string tile_name_in,string display_name_in,string image_name_in,
			 int width,int height,vector<string>* descriptions_in){
	tile_name    = tile_name_in;
	display_name = display_name_in;

	image_name = image_name_in;

	size.width  = width;
	size.height = height;

	descriptions = descriptions_in;

	//start off in normal mode
	help_mode = false;

	//not in locking mode at Default
	am_I_locking = false;

	//these will be taken care of by calc_corners()
	xloc = 0;
	yloc = 0;

	my_tex  = NULL;

	my_help_tex  = NULL;

	//LOCKING VARIABLES
	lock_texture = NULL;
	is_locked = false;
}

field::field(const field& other){
	//cout << "Copy constructor for fields called!" << endl;

	tile_name    = other.tile_name;
	display_name = other.display_name;

	//copy over the description lines, if any
	for(unsigned int c = 0; c < other.descriptions->size(); c++){
		descriptions->push_back(other.descriptions->at(c));
	}

	my_text_box.set_text(other.my_text_box.get_text());

	editing_location = other.editing_location;

	text_dims.w = other.text_dims.w;
	text_dims.h = other.text_dims.h;

	text_dims.x = other.text_dims.x;
	text_dims.y = other.text_dims.y;

	is_locked = other.is_locked;

	am_I_locking = other.am_I_locking;//not in locking mode at default

	xloc = other.xloc;
	yloc = other.yloc;

	image_path = other.image_path;

	image_name = other.image_name;

	help_mode = other.help_mode; //start off in normal mode

	lock_texture = NULL;

	my_tex       = NULL;
	my_help_tex  = NULL;

	descriptions = NULL;

	size.width  = other.size.width;
	size.height = other.size.height;
}

field::~field(){

	if(my_help_tex != NULL){
		SDL_DestroyTexture(my_help_tex);
		my_help_tex = NULL;
	} else {
		logger_access->push_error("Attempted double free in ~field!");
	}

	if(descriptions != NULL){
		delete(descriptions);
	}
}

SDL_Rect field::get_rect() const{

	int corner_x_location = xloc + sdl_access->get_xscroll();
	int corner_y_location = yloc+sdl_access->get_yscroll();
	int width             = size.width;
	int height            = size.height;
	SDL_Rect return_me    = {corner_x_location, corner_y_location,width,height};
	return return_me;
}

void field::graphics_init(){
	string assets_image_path = image_path + image_name;

	//load in tile background
	my_tex = asset_access->get_texture(assets_image_path);
	if(my_tex == NULL){
		cout << "NO TEXTURE FOR FIELD BACKGROUND TILE NAME" << tile_name << endl;
		cout << "PATH:" << image_path << endl;
		string error = "Error in field.cc's graphics init() function: ";
		error       +=  SDL_GetError();
		logger_access->push_error(error);
	}

	string lock_target = image_path + "lock.png";
	lock_texture       = asset_access->get_texture(lock_target);
	if(lock_texture == NULL) logger_access->push_error(string(SDL_GetError()));

	//get a more shorthand reference to the SDL class's font pointer
	TTF_Font* font = sdl_access->font;
	if(font == NULL){
		logger_access->push_msg("NULL font in text init()!");
	}

	SDL_Renderer* renderer = sdl_access->renderer;
	if(renderer == NULL){
		logger_access->push_msg("NULL renderer in text init()!");
	}

	SDL_Color color = {BLACK};

	set_up_tile_title(font,renderer,color);

	//this part sets up this tile's help box - if it has been provided
	set_up_description(font, renderer, color);
}

void field::set_up_tile_title(TTF_Font* font, SDL_Renderer* renderer,const SDL_Color& color){
	my_text_surf = TTF_RenderUTF8_Blended(font,(display_name).c_str(),color);
	if(my_text_surf == NULL){
		string error = "Error in field.cc's graphics init() function: ";
		error       += SDL_GetError();
		logger_access->push_error(error);
	}

	my_text_tex = SDL_CreateTextureFromSurface(renderer,my_text_surf);

	if(my_text_tex == NULL){
		string error = "Error in field.cc's graphics init() function: ";
		error       += SDL_GetError();
		logger_access->push_error(error);
	}
}

void field::set_up_description(TTF_Font* font, SDL_Renderer* renderer, const SDL_Color& color){
	//don't need to do anything if this parameter didn't have a description specified in the config file
	if(descriptions == NULL || descriptions->size() == 0){
		return;
	}

	//find widest description line
	unsigned int max_width = 0;
	int max_w_index        = 0;
	int total_h            = 0;
	int word_h             = 0;
	int word_w             = 0;

	//# of pixels between each line of text
	int vert_offset = 1;

	for(unsigned int c = 0; c < descriptions->size();c++){
		//if this string is the longest we've seen
		if(descriptions[c].size() > max_width){
			max_width   = descriptions[c].size();
			max_w_index = c;
		}
	}
	TTF_SizeText(font,descriptions->at(max_w_index).c_str(),&word_w,&word_h);

	//now have help box's width
	max_width = word_w;

	//now have help box's height
	total_h = descriptions->size() * (word_h + vert_offset);

	//set up the surface's pixel masks. I don't fully understand this
	//but it's from the sdl documentation
	//https://wiki.libsdl.org/SDL_CreateRGBSurface
	//should do this in an object once and store it, then use it for everything
	//instead of redoing this per tile
	Uint32 red,green,blue,alpha;
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		red   = 0xff000000;
		green = 0x00ff0000;
		blue  = 0x0000ff00;
		alpha = 0x000000ff;
	#else
		red   = 0x000000ff;
		green = 0x0000ff00;
		blue  = 0x00ff0000;
		alpha = 0xff000000;
	#endif

	my_help_surf = SDL_CreateRGBSurface(0,max_width,total_h,32,red,green,blue,alpha);

	if(my_help_surf == NULL){
		string error = "Error making ";
		error       += tile_name + "'s help box.";
		error       += SDL_GetError();
		logger_access->push_error(error);
	}
	//color in the help background
	//OU green from colors.h
	SDL_FillRect(my_help_surf,NULL,SDL_MapRGBA(my_help_surf->format,OU_GREEN));

	int new_row_height = 0;
	for(unsigned int c = 0; c < descriptions->size();c++){
		//used to tell it where to draw each line
		SDL_Rect word_dest = {0,0,0,0};

		SDL_Surface* temp_line = TTF_RenderUTF8_Blended(font,descriptions->at(c).c_str(),color);

		//account for height of previous lines
		word_dest.y    = new_row_height + vert_offset;
		new_row_height = word_dest.y + word_h;

		//draw words atop the help surface
		if(SDL_BlitSurface(temp_line,NULL,my_help_surf,&word_dest) != 0){
			string error = SDL_GetError();
			logger_access->push_error("Error in help blit."+error);
		}

		//free memory, this pointer will be used again
		SDL_FreeSurface(temp_line);
	}

	my_help_tex = SDL_CreateTextureFromSurface(renderer,my_help_surf);
	if(my_help_tex == NULL){
		string error = "Error in creating help box texture. ";
		error       += SDL_GetError();
		logger_access->push_error(error);
	}
}

void field::draw_me(){
	//get a space saving reference to the sdl renderer
	SDL_Renderer* renderer = sdl_access->renderer;

	//space saving reference to x and y scroll from sdl class
	int x_scroll = sdl_access->get_xscroll();
	int y_scroll = sdl_access->get_yscroll();

	//space saving reference to tile dimensions
	int width  = size.width;
	int height = size.height;

	//this part draws the "normal box"
	if(!help_mode || my_help_surf == NULL || my_help_tex == NULL){

		//############## Draw name and tile background #########################
		SDL_Rect dest_temp = {xloc+x_scroll,yloc+y_scroll,width,height};
		SDL_RenderCopy(renderer,my_tex,NULL,&dest_temp);

		//this part draws the text in the box
		SDL_Rect text_dest_temp = {xloc+x_scroll,yloc+y_scroll,0,0};

		//get text surface info
		SDL_QueryTexture(my_text_tex,NULL,NULL,&text_dest_temp.w,&text_dest_temp.h);
		SDL_RenderCopy(renderer,my_text_tex,NULL,&text_dest_temp);
		//######################################################################

		my_text_box.draw_me();
	}

	//handle the lock
	if(is_locked){
		SDL_Rect lock_dest = {xloc+x_scroll+size.width-15,
							  yloc+y_scroll+25,15,25};
		SDL_RenderCopy(renderer,lock_texture,NULL,&lock_dest);
	}

	//this part draws the "help box" in its place
	if(help_mode && my_help_surf != NULL && my_help_tex != NULL){
		/*draw normal box boundaries - note that this is implemented
		 *in both if cases here, because later we may want the help box to
		 *have a different background color or image*/
		SDL_Rect dest_temp = {xloc+x_scroll,yloc+y_scroll,width,height};
		SDL_RenderCopy(renderer,my_tex,NULL,&dest_temp);

		//draw help text instead
		SDL_Rect help_dest_temp = {xloc+x_scroll,yloc+y_scroll,0,0};
		SDL_QueryTexture(my_help_tex,NULL,NULL,&help_dest_temp.w,
						 &help_dest_temp.h);
		SDL_RenderCopy(renderer,my_help_tex,NULL,&help_dest_temp);
	}

}

void field::help_toggle(){
	//don't do anything if this tile doesn't have a help box
	if(descriptions == NULL || descriptions->size() == 0) return;

	//if it's true, make it false
	if(help_mode){
		help_mode = false;
		return;
	}

	//elsewise, it is false and needs to be made true
	help_mode = true;
}

string field::get_string(){

	ostringstream field_info_as_string;

	field_info_as_string << "Tile name: "         << tile_name
						 << " Tile Image Name: "  << image_name << "\n"
						 << " CORNER x:y = "      << to_string(xloc)
						 << ":"                   << to_string(yloc);

	if(descriptions != NULL){
		for(unsigned int c = 0; c < descriptions->size(); c++){
			field_info_as_string << descriptions->at(c) << "\n";
		}
	}

	// return_string >> "Help mode?:" >> help_mode;

	return field_info_as_string.str();
}

void field::print(){
	string field_as_string = get_string();
	logger_access->push_msg(field_as_string);
}

void field::clicked(SDL_Event& event, const int& click_x,const int& click_y){
	help_toggle();
}

void field::change_tile_background(string image_name_in){
	my_tex = NULL;
	string assets_full_path = image_path + image_name_in;
	// cout << "change tile background: " << display_name << endl;
	my_tex = asset_access->get_texture(assets_full_path);
	if(my_tex == NULL) logger_access->push_error(SDL_GetError());
}

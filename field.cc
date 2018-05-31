//! \file field.cc implements the functions declared in field.h
#include "field.h"
#include <regex>

#include "sdl_help.h"
#include "asset_manager.h"

using namespace std;

extern sdl_help* sdl_access;
extern asset_manager* asset_access;

field::field(string tile_name_in,string display_name_in,string image_name_in,
			 int width, int height){
	tile_name = tile_name_in;
	display_name = display_name_in;


	image_name = image_name_in;

	size.width = width;
    size.height = height;

    //start off in normal mode
	help_mode = false;

    //not in error mode at default
	is_red = false;

    //not in locking mode at Default
	am_I_locking = false;

	//start off input blank. Default value loaded in by input
	//manager, overridden by user
	temp_input = "temp_input -> default failure";

    /*
	editing_location = 0;
	text_dims.w = 0;
	text_dims.h = 0;

	text_dims.x = 0;
	text_dims.y = 0;
    */

    //these will be taken care of by calc_corners()
	xloc = 0;
	yloc = 0;

    /*
	//these need to start off null, then be created later
	my_text_surf = NULL;
	my_text_tex  = NULL;
    */

	my_tex  = NULL;

	my_help_tex  = NULL;

	//LOCKING VARIABLES
	lock_texture = NULL;
	is_locked = false;

	//this will allow the field to change values in the input manager's vectors
	//they will be set up by give_defaults. Only one should be used
	int4_hook       = NULL;
	real8_hook      = NULL;
	string_hook     = NULL;
	int4_array_hook = NULL;
	r8_array_hook   = NULL;
}

field::field(const field& other){
	//cout << "Copy constructor for fields called!" << endl;

	tile_name = other.tile_name;
	display_name = other.display_name;

	//copy over the description lines, if any
	for(unsigned int c = 0; c < other.descriptions.size(); c++){
		descriptions.push_back(other.descriptions[c]);
	}

	int4_hook       = other.int4_hook;
	real8_hook      = other.real8_hook;
	string_hook     = other.string_hook;
	int4_array_hook = other.int4_array_hook;
	r8_array_hook   = other.r8_array_hook;

	temp_input = other.temp_input; 

	editing_location = other.editing_location;

	text_dims.w = other.text_dims.w;
	text_dims.h = other.text_dims.h;

	text_dims.x = other.text_dims.x;
	text_dims.y = other.text_dims.y;

	is_red = other.is_red;

	is_locked = other.is_locked;

	am_I_locking = other.am_I_locking;//not in locking mode at default

	xloc = other.xloc;
	yloc = other.yloc;

	image_p = other.image_p;

	image_name = other.image_name;

	help_mode = other.help_mode; //start off in normal mode

	lock_texture = NULL;

	my_tex       = NULL;
	my_help_tex  = NULL;

	size.width  = other.size.width;
    size.height = other.size.height;
}

field::~field(){

	if(my_help_tex != NULL){
		SDL_DestroyTexture(my_help_tex);
		my_help_tex = NULL;
	} else {
		error_logger.push_error("Attempted double free in ~field!");
	}

}

SDL_Rect field::get_rect() const{
	SDL_Rect return_me = {xloc+ sdl_access->get_xscroll(),
				yloc+sdl_access->get_yscroll(),size.width,size.height};

	return return_me;
}

void field::graphics_init(string image_p_in){
	image_p = image_p_in;

	//get asset directory from sdl_help, and change image name
	//to the full path
	image_name = image_p_in + image_name;

	//load in tile background
	my_tex = asset_access->get_texture(image_name);
	if(my_tex == NULL){
		string error = "Error in field.cc's graphics init() function: ";
		error       +=  SDL_GetError();
		error_logger.push_error(error);
	}

	string lock_target = image_p+"lock.png";
	lock_texture = asset_access->get_texture(lock_target);
	if(lock_texture == NULL) error_logger.push_error(string(SDL_GetError()));

	text_init();
    my_text_box.init(sdl_access->font,"",xloc,yloc+25,size.width,25);
}

void field::text_init(){

	//thanks to 
	//http://headerphile.blogspot.com/2014/07/sdl2-part-10-text-rendering.html
	//for the tutorial I used. Also thanks to
	//http://gigi.nullneuron.net/gigilabs/displaying-text-in-sdl2-with-sdl_ttf/

	//get a more shorthand reference to the SDL class's font pointer
	TTF_Font* font = sdl_access->font;
	if(font == NULL){
		error_logger.push_msg("NULL font in text init()!");
	}

	SDL_Renderer* renderer = sdl_access->renderer;
	if(renderer == NULL){
		error_logger.push_msg("NULL renderer in text init()!");
	}

	//this part sets up the tile title surface #################################
	//black text
	SDL_Color color= {BLACK};
	my_text_surf = TTF_RenderUTF8_Blended(font,(display_name).c_str(),color);
	if(my_text_surf == NULL){
		string error = "Error in field.cc's graphics init() function: ";
		error       += SDL_GetError();
		error_logger.push_error(error);
	}

	my_text_tex = SDL_CreateTextureFromSurface(renderer,my_text_surf);

	if(my_text_tex == NULL){
		string error = "Error in field.cc's graphics init() function: ";
		error += SDL_GetError();
		error_logger.push_error(error);	
	}
	//##########################################################################

	//this part sets up this tile's help box####################################
	if(descriptions.size() > 0){

		//find widest description line
		unsigned int max_width = 0;
		int max_w_index;
		int total_h = 0;
		int word_h, word_w;

		//# of pixels between each line of text
		int vert_offset = 1;

		for(unsigned int c = 0; c < descriptions.size();c++){
			//if this string is the longest we've seen
			if(descriptions[c].size() > max_width){
				max_width = descriptions[c].size(); //save its length
				max_w_index = c; //save a reference to the winning string
			}
		}
		TTF_SizeText(font,descriptions[max_w_index].c_str(),&word_w,&word_h);

		//now have help box's width
		max_width = word_w;

		//now have help box's height
		total_h = descriptions.size() * (word_h + vert_offset);
		
		//set up the surface's pixel masks. I don't fully understand this
		//but it's from the sdl documentation
		//https://wiki.libsdl.org/SDL_CreateRGBSurface
		Uint32 red,green,blue,alpha;
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			red   = 0xff000000;
			green = 0x00ff0000;
			blue  = 0x0000ff00;
			alpha = 0x000000ff;
		#else
			red   = 0x000000ff;
			green = 0x0000ff00;
			blue  =	0x00ff0000;
			alpha = 0xff000000;
		#endif

		my_help_surf = 
			SDL_CreateRGBSurface(0,max_width,total_h,32,red,green,blue,alpha);

		if(my_help_surf == NULL){
			string error = "Error making ";
			error       += tile_name + "'s help box.";
			error       += SDL_GetError();
			error_logger.push_error(error);

		}
		//color in the help background
		//OU green from colors.h
		SDL_FillRect(my_help_surf,NULL,
									SDL_MapRGBA(my_help_surf->format,OU_GREEN));


		int new_row_height = 0;
		for(unsigned int c = 0; c < descriptions.size();c++){

			//used to tell it where to draw each line
			SDL_Rect word_dest = {0,0,0,0};

			SDL_Surface* temp_line =
					TTF_RenderUTF8_Blended(font,descriptions[c].c_str(),color);

			//account for height of previous lines
			word_dest.y = new_row_height + vert_offset;
			new_row_height = word_dest.y + word_h;

			//draw words atop the help surface
			if(SDL_BlitSurface(temp_line,NULL,my_help_surf,&word_dest) != 0){
				string error = SDL_GetError();
				error_logger.push_error("Error in help blit."+error);
			} 

			//free memory, this pointer will be used again
			SDL_FreeSurface(temp_line);
		}

		my_help_tex = SDL_CreateTextureFromSurface(renderer,my_help_surf);
		if(my_help_tex == NULL){
			string error = "Error in creating help box texture. ";
			error += SDL_GetError();
			error_logger.push_error(error);
		}
	}
}
void field::draw_me(){

	//get a space saving reference to the sdl renderer
	SDL_Renderer* renderer = sdl_access->renderer;

	//space saving reference to x and y scroll from sdl class
	int x_scroll = sdl_access->get_xscroll();
	int y_scroll = sdl_access->get_yscroll();

	//space saving reference to tile dimensions
	int width = size.width;
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
//		SDL_Rect lock_dest = {xloc+x_scroll+size.width-15,
//									yloc+y_scroll+text_box.y_offset,15,25};
//		SDL_RenderCopy(renderer,lock_texture,NULL,&lock_dest);
	}


	//this part draws the "help box" in its place
	if( help_mode && my_help_surf != NULL && my_help_tex != NULL){

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
	if(descriptions.size() == 0) return;

	//if it's true, make it false
	if(help_mode){
		help_mode = false;
		return;
	}

	//elsewise, it is false and needs to be made true
	help_mode = true;

}
void field::print(){

	error_logger.push_msg("Tile name: "+tile_name+
						  " Tile Image Name: "+image_name);
	error_logger.push_msg("CORNER x:y = "+to_string(xloc)+":"+to_string(yloc));

	if(sdl_access != NULL){
		error_logger.push_msg("SDL pointers texture:renderer = "+
								to_string(size_t(&my_tex))+":"+
								to_string(size_t(sdl_access->renderer)));
	} else {
		string msg = "Couldn't print texture or renderer pointers,";
		msg += " sdl_access is NULL."; 
		error_logger.push_msg(msg);
	}
	error_logger.push_msg("font hook: "+to_string(size_t(sdl_access->font)) );
	error_logger.push_msg("Description lines:");

	for(unsigned int c = 0; c < descriptions.size();c++){
		error_logger.push_msg(descriptions[c]);
	}
	string yay_or_nay;
	if(help_mode) yay_or_nay = "Yes";
	else yay_or_nay = "No";
	error_logger.push_msg("Help mode: "+yay_or_nay);
	size.print();
}

void field::clicked(SDL_Event& event, const int& click_x,const int& click_y){
	help_toggle();
}

void field::change_tile_background(string image_name){
	string full_path = image_p + image_name;

	my_tex = NULL;

	my_tex = asset_access->get_texture(full_path);
	if(my_tex == NULL) error_logger.push_error(SDL_GetError());
}

//this function updates this fields ftran_struct in the input_maker vectors
bool field::update_my_value(){

	regex bad_int("\\D");
	regex bad_real8("[A-Za-z _]");

	regex good_string("\".*?\"");

	//set to false to flag that stoi/stod failed
	bool success = true;


	if(int4_hook == NULL && real8_hook == NULL &&
	   string_hook == NULL && int4_array_hook == NULL &&
	   r8_array_hook == NULL){
		error_logger.push_error("ERROR! Tile "+tile_name
			 +" has no association with a fortran struct"
		     +" in input_maker's vectors. Please check"
			 +" that the tile's name in the tiles.txt"
		     +" and HF_config.txt match each other.\n\n");
		return false;
	}

	if(int4_hook != NULL){
		if( !temp_input.empty() ){
		  try{
			if( !regex_search(temp_input,bad_int) ){
				int4_hook->value = stoi(temp_input);
			} else {
				error_logger.push_error(display_name+" has an illegal string:");
				success = false;
			}
		  } catch( invalid_argument& error ){
			string err_msg = "Error! int4 parameter provided with";
			err_msg += " an illegal string. Tile name:";
			error_logger.push_error(err_msg+tile_name); 
			int4_hook->value = -1804;
			success = false;
		  }
		}
	} else if(real8_hook != NULL){
		if( !temp_input.empty() ){
			try{
				if( !regex_search(temp_input,bad_real8) ){
					real8_hook->value = stod(temp_input);
				} else {
					string err;
					err = display_name + " has an illegal string:" + temp_input;
					error_logger.push_error(err);
					success = false;
				}
			} catch(invalid_argument& error){
			  string err;
			  err = "Error in field::update_my_value(), illegal value entered:"
					+ temp_input;
			  error_logger.push_error(err);
			  real8_hook->value = -180.4;
			  success = false;
			}
		}
	} else if(string_hook != NULL){

		if( regex_match(temp_input,good_string) ){
			string temp_string = temp_input;
			unsigned int balance_factor;
			balance_factor = temp_string.length() - string_hook->value.length();
			trim(temp_string,balance_factor);
			string_hook->value = temp_string;
			string msg;
			msg = "Ftran String value after: "+string_hook->value;
			error_logger.push_msg(msg);
		} else {
			string err;
			err = "Error! String " + display_name
				  +" is likely missing quotation marks.";
			error_logger.push_error(err);
			success = false;

		}

	} else if(int4_array_hook != NULL){
		//split the user's string across commas
		vector<string> user_entered_values = split(temp_input,',');

		//replace the default numbers in input_maker with the
		//ones entered by the user

		int num_values = user_entered_values.size();

		for(unsigned int c = 0;
			c < (unsigned int)num_values && c < int4_array_hook->values.size();c++){

			try{
			  int4_array_hook->values[c] = stoi(user_entered_values[c]);
		  	} catch (invalid_argument& error){

			  string err_msg = "Error! Int4 array value is an illegal string.";
			  err_msg = "Tile name:"+tile_name + " Array index:";
			  error_logger.push_error( err_msg+to_string(c));
			  success = false;
			}
		}

	} else if(r8_array_hook != NULL){

		vector<string> user_entered_values = split(temp_input,',');

		int num_values = user_entered_values.size();

		for(unsigned int c = 0;
			c < (unsigned int)num_values && c < r8_array_hook->values.size();
			c++){
			try {
			  r8_array_hook->values[c] = stod(user_entered_values[c]);
			} catch(invalid_argument& error){
			  string err;	

			  err = "Error! real 8 array given illegal value:"
					+ user_entered_values[c]
			  	    +" at index:"+to_string(c);

			  error_logger.push_error(err);

			  success = false;
			}
		}
	}

	//let manager know if it worked or not
	return success;
}

void field::go_red(){
	my_tex = NULL;

	my_tex = asset_access->get_texture(image_p+"bad_tile.png");
	if(my_tex == NULL){
		string error = SDL_GetError();
		error_logger.push_error("Error changing tile:"+display_name\
								+" to red."+error);
	}
	is_red = true;
}

void field::go_back(){

	my_tex = NULL;

	my_tex = asset_access->get_texture(image_name);
	if(my_tex == NULL){
		string error = SDL_GetError();
		error_logger.push_error("Error in field::go_back() function: "+error);
	}
}



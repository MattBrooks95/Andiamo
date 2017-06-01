//! \file field.cc implements the functions declared in field.h
#include "field.h"

using namespace std;

field::field(string tile_name_in,string image_name_in, int width, int height){
	tile_name = tile_name_in;
	image_name = image_name_in;
	//description = description_in;
	//cout << "DESCRIPTION IN: " << description << endl;

	size.width = width;
        size.height = height;

	help_mode = false; //start off in normal mode

	
	xloc = 0;//these will be taken care of by calc_corners()
	yloc = 0;

	sdl_xscroll = NULL;//these will be set up by manager's give_renderer function
	sdl_yscroll = NULL;

	sdl_font = NULL;

	my_text_surf = NULL;
	my_text_tex = NULL;//these need to start off null, then be created later

	my_surf = NULL; //these will be taken care of by graphics_init() later
	my_tex = NULL;

	my_help_surf = NULL; //these will be set up by graphics_init's helper text_init
	my_help_tex = NULL;

	sdl_help_renderer = NULL;
}
SDL_Rect field::get_rect() const{
	SDL_Rect return_me = {xloc+ (*sdl_xscroll),yloc+ (*sdl_yscroll),size.width,size.height};

	return return_me;

}

void field::force_size(int width_in,int height_in){
	size.width = width_in;
	size.height = height_in;
}

void field::graphics_init(SDL_Renderer* sdl_help_renderer_in,string image_p_in,
			  int* xscroll_in, int* yscroll_in,TTF_Font* font_in){
	sdl_help_renderer = sdl_help_renderer_in;//set up "hook" to sdl_help's renderer
	image_p = image_p_in;
	image_name = image_p_in + image_name;//get asset directory from sdl_help, and change image name to the
					     //full path

	sdl_font = font_in;//sets up the "hook" to true text font information pointer

	my_surf = IMG_Load(image_name.c_str());
	if(my_surf == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	my_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_surf);
	if(my_tex == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	/*
	//thanks to http://headerphile.blogspot.com/2014/07/sdl2-part-10-text-rendering.html for the tutorial I used
	//also http://gigi.nullneuron.net/gigilabs/displaying-text-in-sdl2-with-sdl_ttf/
	SDL_Color color= {0,0,0,0}; //black text
	my_text_surf = TTF_RenderText_Solid(sdl_font,(tile_name+" = ").c_str(),color);
	if(my_text_surf == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	my_text_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_text_surf);
	if(my_text_tex == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	*/
	text_init();


	sdl_xscroll = xscroll_in;
	sdl_yscroll = yscroll_in;

}
void field::text_init(){
	//thanks to http://headerphile.blogspot.com/2014/07/sdl2-part-10-text-rendering.html for the tutorial I used
	//also http://gigi.nullneuron.net/gigilabs/displaying-text-in-sdl2-with-sdl_ttf/

	//this part sets up the tile title surface
	SDL_Color color= {0,0,0,0}; //black text
	my_text_surf = TTF_RenderText_Solid(sdl_font,(tile_name+" = ").c_str(),color);
	if(my_text_surf == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	my_text_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_text_surf);
	if(my_text_tex == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	


	//this part sets up this tile's dialogue box


	my_help_surf = TTF_RenderText_Solid(sdl_font,"string here", color); 
	if(my_help_surf == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;

	my_help_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_help_surf);
	if(my_help_tex == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	/*

	//this part sets up the help_mode's text

	/*
	int longest_line = 0; //figures out how wide the help box needs to be
	int index; //save loc of winning string

	for(unsigned int c = 0; c < descriptions.size();c++){
		if(descriptions[c].size() > longest_line){
			longest_line = descriptions[c].size();//save max value
			index = c;
		}
	}
	//trying to make background for help mode be OU green, which is 0,105, 78 (00694E)
	Uint32 red = 0xFF000000; Uint32 green = 0x00FF0000; Uint32 blue = 0x0000FF00; Uint32 alpha = 0x000000FF;
	SDL_Surface* temp_surf = SDL_CreateRGBSurface(0,size.width,size.height,32,red,green,blue,alpha);
	if(temp_surf == NULL) cout << "Error in text_init()! " << SDL_GetError() << endl;
	my_help_surf = temp_surf;
	my_help_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_help_surf);
	*/











	/* //this method tried to imagine splitting lines up so that they fit in that tile's box, but it seems
	   //that allowing the box to scale based on the lines is the only way to go. Cramming everything in either
	   //harms readability, or makes life harder for the person configuring the tiles
	int text_total_height = 0; //updates whenever a new row is created, that way the lines don't overlap and
				   //sit nicely beneath the rows above them
	int text_w; int text_h; //integers to store current string dimensions
	SDL_Surface* temp_surf;
	my_help_surf = IMG_Load(image_name.c_str());

	string desc_temp;
	for(unsigned int c = 0; c < descriptions.size();c++){
		
		desc_temp.append(" " + descriptions[c] );
		
		TTF_SizeText(sdl_font,desc_temp.c_str(),&text_w,&text_h);
		cout << "TEXTW: " << text_w << " SIZEW: " << size.width << endl;
		if(text_w > size.width || c == descriptions.size()-1){
			cout << "CALCULATING LONG STRINGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG" << endl;
			SDL_Rect dest_rect = {0,text_total_height,text_w,text_h};

			temp_surf = TTF_RenderText_Solid(sdl_font,desc_temp.c_str(),color);//put text up until here
											   //on temp surface
			SDL_BlitSurface(temp_surf,NULL,my_help_surf,&dest_rect); //put temp surface onto 
			//the surface that will be saved in the field object's my_help_surf

			text_total_height = text_total_height +text_h + 4;
			//keep track of this row's bottom, with a four pixel buffer
			desc_temp = ""; //reset string accumulator
		}
		SDL_FreeSurface(temp_surf);
	}

	TTF_SizeText(sdl_font,desc_temp.c_str(),&text_w,&text_h);
	//my_help_surf = TTF_RenderText_Solid(sdl_font,(desc_temp+to_string(text_w)).c_str(),color);
	if(my_help_surf == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;

	my_help_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_help_surf);
	if(my_help_tex == NULL)  cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	*/



}
void field::draw_me(){
	if(tile_name == "background"){ //if this is the background field, it gets drawn with no text
		SDL_RenderCopy(sdl_help_renderer,my_tex,NULL,NULL); //and covers the entire environment
		return;
	}
	//normal tiles are drawn below

	//this part draws the "normal box"
	if(!help_mode){
		SDL_Rect dest_temp = {xloc+ (*sdl_xscroll),yloc+ (*sdl_yscroll),size.width,size.height};
		SDL_RenderCopy(sdl_help_renderer,my_tex,NULL,&dest_temp);

		//this part draws the text in the box
		SDL_Rect text_dest_temp = {xloc+(*sdl_xscroll),yloc+ (*sdl_yscroll),0,0};
		SDL_QueryTexture(my_text_tex,NULL,NULL,&text_dest_temp.w,&text_dest_temp.h); //get text surface info
		SDL_RenderCopy(sdl_help_renderer,my_text_tex,NULL,&text_dest_temp);

	} else { //this part draws the "help box" in its place

		//draw normal box boundaries - note that this is implemented in both if cases here, because
		//later we may want the help box to have a different background color or image
		SDL_Rect dest_temp = {xloc+ (*sdl_xscroll),yloc+ (*sdl_yscroll),size.width,size.height};
		SDL_RenderCopy(sdl_help_renderer,my_tex,NULL,&dest_temp);

		//draw help text instead
		SDL_Rect help_dest_temp = {xloc+ (*sdl_xscroll),yloc+ (*sdl_yscroll),0,0};
		SDL_QueryTexture(my_help_tex,NULL,NULL,&help_dest_temp.w,&help_dest_temp.h);
		SDL_RenderCopy(sdl_help_renderer,my_help_tex,NULL,&help_dest_temp);
	}

}
void field::help_toggle(){
	if(help_mode){ //if it's true, make it false
		help_mode = false;
		return;
	} //elsewise, it is false and needs to be made true
	help_mode = true;

}
void field::print(ostream& outs){
	outs << "Tile name: " << tile_name << " Tile Image Name: "
	     << image_name << endl;
	outs << "CORNER x:y = " << xloc << ":" << yloc << endl;
	outs << "scroll value hooks x_ptr:y_ptr = " << sdl_xscroll << ":" << sdl_yscroll << endl; 
	outs << "SDL pointers surface:texture:renderer = " << my_surf << ":" << my_tex << ":"
	     << sdl_help_renderer << endl;
	outs << "SDL font hook: " << sdl_font << endl;
	outs << "Description lines:" << endl;

	for(unsigned int c = 0; c < descriptions.size();c++){
		outs << descriptions[c] << endl;
	}
	
	outs << "Help mode: ";
	if(help_mode) cout << "Yes" << endl;
	else cout << "No" << endl;
	size.print(outs);
	cout << "\n\n" << endl;
}
void field::clicked(ostream& outs){
	outs << "Tile " << tile_name << " says: That tickles!" << endl;
	help_toggle();
}
field::~field(){
	SDL_FreeSurface(my_text_surf);//give back memory
	SDL_DestroyTexture(my_text_tex);//give back memory


	SDL_FreeSurface(my_surf); //free memory
	SDL_DestroyTexture(my_tex); //free memory
}








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
	
	xloc = 0;//these will be taken care of by calc_corners()
	yloc = 0;

	sdl_xscroll = NULL;//these will be set up by manager's give_renderer function
	sdl_yscroll = NULL;

	sdl_font = NULL;

	my_text_surf = NULL;
	my_text_tex = NULL;//these need to start off null, then be created later

	my_surf = NULL; //these will be taken care of by graphics_init() later
	my_tex = NULL;
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
	SDL_Color color= {0,0,0,0}; //black text
	
	string desc_lines; //sum of all description lines
	for(unsigned int c = 0; c < descriptions.size();c++){
		desc_lines.append((" " + descriptions[c]));
		cout << "Current string total: " << desc_lines << endl;
	}

	my_text_surf = TTF_RenderText_Solid(sdl_font,(tile_name+desc_lines).c_str(),color);
	if(my_text_surf == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	my_text_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_text_surf);
	if(my_text_tex == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;

}
void field::draw_me(){
	//normal tiles are drawn here
	if(tile_name != "background") {
		//this part draws the "box"
		SDL_Rect dest_temp = {xloc+ (*sdl_xscroll),yloc+ (*sdl_yscroll),size.width,size.height};
		SDL_RenderCopy(sdl_help_renderer,my_tex,NULL,&dest_temp);

		//this part draws the text in the box
		SDL_Rect text_dest_temp = {xloc+(*sdl_xscroll),yloc+ (*sdl_yscroll),0,0};
		SDL_QueryTexture(my_text_tex,NULL,NULL,&text_dest_temp.w,&text_dest_temp.h); //get text surface info
		SDL_RenderCopy(sdl_help_renderer,my_text_tex,NULL,&text_dest_temp);
	
	} else { //background tile gets drawn to cover the entire environment
		SDL_RenderCopy(sdl_help_renderer,my_tex,NULL,NULL);
	}

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
	size.print(outs);
	cout << "\n\n" << endl;
}
void field::clicked(ostream& outs) const{
	outs << "Tile " << tile_name << " says: That tickles!" << endl;
}
field::~field(){
	SDL_FreeSurface(my_text_surf);//give back memory
	SDL_DestroyTexture(my_text_tex);//give back memory


	SDL_FreeSurface(my_surf); //free memory
	SDL_DestroyTexture(my_tex); //free memory
}








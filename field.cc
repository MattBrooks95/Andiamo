//! \file field.cc implements the functions declared in field.h
#include "field.h"

using namespace std;

field::field(string tile_name_in,string image_name_in, int width, int height){
	tile_name = tile_name_in;
	image_name = image_name_in;

	size.width = width;
        size.height = height;
	
	xloc = 0;//these will be taken care of by calc_corners()
	yloc = 0;

	sdl_xscroll = NULL;//these will be set up by manager's give_renderer function
	sdl_yscroll = NULL;

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
			  int* xscroll_in, int* yscroll_in){
	sdl_help_renderer = sdl_help_renderer_in;//set up "hook" to sdl_help's renderer
	image_name = image_p_in + image_name;//get asset directory from sdl_help, and change image name to the
					  //full path

	my_surf = IMG_Load(image_name.c_str());
	if(my_surf == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	my_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_surf);
	if(my_tex == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;

	sdl_xscroll = xscroll_in;
	sdl_yscroll = yscroll_in;
}

void field::draw_me(){
	SDL_Rect dest_temp = {xloc+ (*sdl_xscroll),yloc+ (*sdl_yscroll),size.width,size.height};
	SDL_RenderCopy(sdl_help_renderer,my_tex,NULL,&dest_temp);
}

void field::print(ostream& outs){
	outs << "Tile name: " << tile_name << " Tile Image Name: "
	     << image_name << endl;
	outs << "CORNER x:y = " << xloc << ":" << yloc << endl;
	outs << "scroll value hooks x_ptr:y_ptr = " << sdl_xscroll << ":" << sdl_yscroll << endl; 
	outs << "SDL pointers surface:texture:renderer = " << my_surf << ":" << my_tex << ":"
	     << sdl_help_renderer << endl;
	size.print(outs);
	cout << "\n\n" << endl;
}
void field::clicked(ostream& outs) const{
	outs << "Tile " << tile_name << " says: That tickles!" << endl;
}
field::~field(){
	SDL_FreeSurface(my_surf); //free memory
	SDL_DestroyTexture(my_tex); //free memory
}

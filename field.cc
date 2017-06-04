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

	input = ""; //start off input blank. Default value loaded in by input
		    //manager, overridden by user
	
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
	if(descriptions.size() > 0 ) text_init();



	sdl_xscroll = xscroll_in;
	sdl_yscroll = yscroll_in;

}
void field::text_init(){
	//thanks to http://headerphile.blogspot.com/2014/07/sdl2-part-10-text-rendering.html for the tutorial I used
	//also http://gigi.nullneuron.net/gigilabs/displaying-text-in-sdl2-with-sdl_ttf/

	//this part sets up the tile title surface
	SDL_Color color= {0,0,0,0}; //black text
	my_text_surf = TTF_RenderUTF8_Blended(sdl_font,(tile_name+" = ").c_str(),color);
	if(my_text_surf == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	my_text_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_text_surf);
	if(my_text_tex == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	


	//this part sets up this tile's dialogue box

		//find widest description line
		unsigned int max_width = 0;
		int max_w_index;
		int total_height = 0;
		int word_height; int word_width;
		int vert_offset = 1; //# of pixels between each line of text
		for(unsigned int c = 0; c < descriptions.size();c++){
			if(descriptions[c].size() > max_width){ //if this string is the longest we've seen
				max_width = descriptions[c].size(); //save its length
				max_w_index = c; //save a reference to the winning string
			}
		}
		TTF_SizeText(sdl_font,descriptions[max_w_index].c_str(),&word_width,&word_height);
		max_width = word_width; //now have help box's width
		total_height = descriptions.size() * (word_height + vert_offset); //now have help box's height
		
		//set up the surface's pixel masks. I don't fully understand this but it's from
		//the sdl documentation  https://wiki.libsdl.org/SDL_CreateRGBSurface
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

		//make help background 
		//cout << "SURFACE DIMS: " << max_width << ":" << total_height << endl;
		my_help_surf = SDL_CreateRGBSurface(0,max_width,total_height,32,red,green,blue,alpha);
		if(my_help_surf == NULL) cout << "Error making " << tile_name << "'s help box."
					      << SDL_GetError() << endl;
		//color in the help background
		SDL_FillRect(my_help_surf,NULL,SDL_MapRGBA(my_help_surf->format,0,105,78,255));//OU green?


		int new_row_height = 0;
		for(unsigned int c = 0; c < descriptions.size();c++){
			SDL_Rect word_dest = {0,0,0,0}; //used to tell it where to draw each line

			SDL_Surface* temp_line = TTF_RenderUTF8_Blended(sdl_font,descriptions[c].c_str(),color);
			word_dest.y = new_row_height + vert_offset;//account for height of previous lines
			new_row_height = word_dest.y + word_height;
			//cout << "Word_dest.h: " << word_dest.h <<  " Word_height: " << word_height << " C: " << c << endl;
			if(SDL_BlitSurface(temp_line,NULL,my_help_surf,&word_dest) != 0){
				cout << "Error in help blit." << " " << SDL_GetError() << endl;
			} //draw words atop the help surface
			SDL_FreeSurface(temp_line);//free memory, this pointer will be used again
		}
	my_help_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_help_surf);
	if(my_help_tex == NULL) cout << "Error in creating help box texture. " << SDL_GetError() << endl;
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








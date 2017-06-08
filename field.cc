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

	temp_input = "temp_input -> default failure"; //start off input blank. Default value loaded in by input
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

	//this will allow the field to change values in the input manager's vectors
	//they will be set up by give_defaults. Note that it's likely that any one tile will only have one
	//of these not be null
	int4_hook = NULL;
	real8_hook = NULL;
	string_hook = NULL;

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

	//load in tile background
	my_surf = IMG_Load(image_name.c_str());
	if(my_surf == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;
	my_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_surf);
	if(my_tex == NULL) cout << "Error in field.cc's graphics init() function: " << SDL_GetError() << endl;

	text_init();

	//note that text_box_init() is called from manager::give_fields_defaults because of a timing issue where this code would run
	//before it was given access to the info in the input_maker

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
	

	if(descriptions.size() > 0){
	//this part sets up this tile's help box

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
		SDL_FillRect(my_help_surf,NULL,SDL_MapRGBA(my_help_surf->format,0,105,78,255));//OU green


		int new_row_height = 0;
		for(unsigned int c = 0; c < descriptions.size();c++){
			SDL_Rect word_dest = {0,0,0,0}; //used to tell it where to draw each line

			SDL_Surface* temp_line = TTF_RenderUTF8_Blended(sdl_font,descriptions[c].c_str(),color);
			word_dest.y = new_row_height + vert_offset;//account for height of previous lines
			new_row_height = word_dest.y + word_height;

			if(SDL_BlitSurface(temp_line,NULL,my_help_surf,&word_dest) != 0){
				cout << "Error in help blit." << " " << SDL_GetError() << endl;
			} //draw words atop the help surface
			SDL_FreeSurface(temp_line);//free memory, this pointer will be used again
		}

		my_help_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,my_help_surf);
		if(my_help_tex == NULL) cout << "Error in creating help box texture. " << SDL_GetError() << endl;
	}
}
void field::draw_me(){
	if(tile_name == "background"){ //if this is the background field, it gets drawn with no text
		SDL_RenderCopy(sdl_help_renderer,my_tex,NULL,NULL); //and covers the entire environment
		return;//get out before any unecessary things are drawn
	}
	//normal tiles are drawn below

	//this part draws the "normal box"
	if(!help_mode || my_help_surf == NULL || my_help_tex == NULL){
		//####################### Draw name and tile background #####################################
		SDL_Rect dest_temp = {xloc+ (*sdl_xscroll),yloc+ (*sdl_yscroll),size.width,size.height};
		SDL_RenderCopy(sdl_help_renderer,my_tex,NULL,&dest_temp);

		//this part draws the text in the box
		SDL_Rect text_dest_temp = {xloc+(*sdl_xscroll),yloc+ (*sdl_yscroll),0,0};
		SDL_QueryTexture(my_text_tex,NULL,NULL,&text_dest_temp.w,&text_dest_temp.h); //get text surface info
		SDL_RenderCopy(sdl_help_renderer,my_text_tex,NULL,&text_dest_temp);
		//###########################################################################################

		//####################### Do the drawing for the text box ###################################
		//text box xcoord will line up with tile's x coord
		//y corner will be tile ycorner + scroll + calculated offset (by text_box_init(), same width
		//height of 15 is line with the constant value in text_box_init()
		SDL_Rect text_box_dest = {xloc+(*sdl_xscroll), yloc + text_box.y_offset + (*sdl_yscroll),size.width,25};
		SDL_Rect text_box_src = {0,0,size.width,25};
		SDL_RenderCopy(sdl_help_renderer,text_box.box_tex,&text_box_src,&text_box_dest);


		//text was too big in cases, now text won't show at all. I'm pretty sure it's a texture source_rect issue
		//but who knows...

		SDL_Rect text_box_text_src = {0,0,0,0};
		TTF_SizeText(sdl_font,temp_input.c_str(),&text_box_text_src.w,&text_box_text_src.h);
		SDL_Rect text_box_text_dest = {xloc+(*sdl_xscroll), yloc + text_box.y_offset + (*sdl_yscroll),0,0};

		//I had a "funny" bug here where I was drawing the text box's text using the dimensions of the 
		//TILE NAME'S TEXTURE INSTEAD OF THE TEXT BOXE'S AHOFEHF*($H(HFO(HEDASUIOFHW(((((DFH(H&*()Q
		//causing blurry text and such...changing it to text_box.text_text fixed it. Unfortunate naming scheme
		SDL_QueryTexture(text_box.text_tex,NULL,NULL,&text_box_text_dest.w,&text_box_text_dest.h);
		SDL_RenderCopy(sdl_help_renderer,text_box.text_tex,&text_box_text_src,&text_box_text_dest);
		//###########################################################################################


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
	if(descriptions.size() == 0) return; //don't do anything if this tile doesn't have a help box
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

void field::clicked(ostream& outs,SDL_Event& event, const int& click_x,const int& click_y){
	outs << "Tile " << tile_name << " says: That tickles!" << endl;
	help_toggle();
}

bool field::text_box_clicked(std::ostream& outs, const int& click_x, const int& click_y){
	//note that in order to be in this function, the tile MUST have been clicked in the text boxes x values,
	//as the text boxes are the same width as the tiles they sit on, so only the height needs checked
	//atleast until it's fancy enough to place the cursor right or left based on how far to the right or left
	//the user clicked
	if( int4_hook == NULL && real8_hook == NULL && string_hook == NULL){
		//return false because there is no field to input_manager connection for the user to modify
		return false;
	}
	if( (click_y >  yloc + (*sdl_yscroll) + size.height - 25  && click_y < yloc + (*sdl_yscroll) + size.height ) ){
		return true;
	}
	return false;

}
void field::back_space(){
	if(temp_input.length() > 0){
		cout << "BEFORE DELETE: " << temp_input << endl;
		temp_input.pop_back();
		cout << "AFTER DELETE: " << temp_input << endl;
	}
	update_texture();
}
void field::update_temp_input(SDL_Event& event){
	
	cout << "Stuff to change text and update surfaces here" << endl;
	cout << "OLD LINE: " << temp_input << endl;
	temp_input.append( event.text.text );
	cout << "AFTER APPEND: " << temp_input << endl;
	update_texture();
}

void field::update_texture(){
		cout << "Texture has been updated, but can you draw it?" << endl;
		SDL_FreeSurface(text_box.text_surf);//prevent memory loss
		SDL_DestroyTexture(text_box.text_tex);//prevent memory loss

		text_box.text_surf = TTF_RenderUTF8_Blended(sdl_font,temp_input.c_str(),text_box.text_color);
		text_box.text_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,text_box.text_surf);
}

field::~field(){
	SDL_FreeSurface(my_text_surf);//give back memory
	SDL_DestroyTexture(my_text_tex);//give back memory


	SDL_FreeSurface(my_surf); //free memory
	SDL_DestroyTexture(my_tex); //free memory

	SDL_FreeSurface(my_help_surf);
	SDL_DestroyTexture(my_help_tex);
}


void field::text_box_init(){

	//      v distance below tile =     v tile height - v constant pixel height
	text_box.y_offset =                 size.height   - 25;


	SDL_Color color = {0,0,0,0};//text black as davy jones's heart

	//set up text box background
	text_box.box_surf = IMG_Load((image_p+"text_box.png").c_str());
	if(text_box.box_surf == NULL) cout << "Error in text_box_init! " << SDL_GetError() << endl;
	text_box.box_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,text_box.box_surf);
	if(text_box.box_tex == NULL) cout << "Error in text_box_init! " << SDL_GetError() << endl;


	//set up text box text
	text_box.text_surf = TTF_RenderUTF8_Blended(sdl_font,temp_input.c_str(),color);

	SDL_Rect delete_me = {0,0,0,0};
	TTF_SizeText(sdl_font,temp_input.c_str(),&delete_me.w,&delete_me.h);
	//cout << "Text:" << temp_input << " Size: " << delete_me.w << ":" << delete_me.h << endl;
	if(text_box.text_surf == NULL) cout << "Error in text_box_init! " << SDL_GetError() << endl;
	text_box.text_tex = SDL_CreateTextureFromSurface(sdl_help_renderer,text_box.text_surf);
	if(text_box.text_tex == NULL) cout << "Error in text_box_init! " << SDL_GetError() << endl;


}


sdl_text_box::sdl_text_box(){

	box_surf = NULL;
	box_tex = NULL;

	text_surf = NULL;
	text_tex = NULL;

	text_color = {0,0,0,0};

	y_offset = 0;

}

sdl_text_box::~sdl_text_box(){
	SDL_FreeSurface(box_surf);
	SDL_FreeSurface(text_surf);
	SDL_DestroyTexture(box_tex);
	SDL_DestroyTexture(text_tex);
}









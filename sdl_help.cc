//! \file sdl_help.cc implements the functions declared in sdl_help.h
#include "sdl_help.h"
#include<iostream>
#include<string>

using namespace std;

bool sdl_test = false;


//######################### WIN SIZE STRUCT ################################################################
win_size::win_size(){//initialize window dimensions to bad values so they must be initialized elsewhere
	width = -1;
	height = -1;
}
void win_size::print(ostream& outs){
	outs << width << "x" << height << endl;
}

win_size* sdl_help::get_win_size(){
	win_size* return_me = &window_s;
	return return_me;
}
//######################### WIN SIZE STRUCT ################################################################

//######################### SDL_HELP CONSTRUCTORS/DESTRUCTORS ##############################################
sdl_help::sdl_help(string name_in){
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS|SDL_INIT_VIDEO);//for now, timer,video and keyboard
	IMG_Init(IMG_INIT_PNG);//allows use of .png files

	window_name = name_in; //set window name
	image_p = "./Assets/Images/";
	hf_input_p = "./HF_Input/";

	if(SDL_GetCurrentDisplayMode(0,&display) < 0){
		cout << "Get current display mode error" << endl;
		cout << SDL_GetError();
        };

	//cout << display.w << " " << display.h << endl;;
	window = SDL_CreateWindow(window_name.c_str(), 0, 0, display.w / 2, display.h * .75, 0);
	renderer = SDL_CreateRenderer(window,-1,0);
        if(sdl_test) cout << "Enacting tile_bag update with values: " << display.w / 2 << " "
                          << display.h << endl;

	window_update(display.w/2,display.h * .75);//this call updates sdl_help and manager's
					           // dimension window fields
	x_scroll = 0; y_scroll = 0; //set scrolling variables to 0

	area_size_set = false; //this should be changed to run after the first inner loop run in draw_all()

	tile_bag.init();
}

sdl_help::sdl_help(std::string name_in, int width, int height){
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS|SDL_INIT_VIDEO);//for now, timer and keyboard
	IMG_Init(IMG_INIT_PNG);//allows use of .png files

	window_name = name_in;
	image_p = "./Assets/Images/";
	hf_input_p = "./HF_Input/";



	//this call is to make sure refresh_rate and driverdata are handled
	if(SDL_GetCurrentDisplayMode(0,&display) < 0){
		cout << "Get current display mode error" << endl;
		cout << SDL_GetError();
	}
  //initializing them twice is an obvious waste of time, consider a more elegant solution

  //init displaymode values
  display.w = width;
  display.h = height; //overrite previous height and width values to the user's preference

  window_update(display.w/2,display.h);//this call updates sdl_help and manager's window dimension fields
  //tile_bag.update_win(display.w,display.h);

  window = SDL_CreateWindow(window_name.c_str(), 0, 0, display.w, display.h, 0);
  renderer = SDL_CreateRenderer(window,-1,0);

  x_scroll = 0; y_scroll = 0; //set scrolling values to 0

  area_size_set = false; //this should be changed to run after the first inner loop run in draw_all()

  tile_bag.init();
}

sdl_help::~sdl_help(){
	SDL_DestroyRenderer(renderer);//stops memory leaks
	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();
}

void sdl_help::quit(){
	this->~sdl_help();  
}
//######################### SDL_HELP CONSTRUCTORS/DESTRUCTORS ##############################################
void sdl_help::window_update(int width_in, int height_in){
	window_s.width = width_in; //update sdl class's window size variables
	window_s.height = height_in;

	tile_bag.update_win(width_in,height_in);
}
//prints area window size and display 
void sdl_help::print_size_info(std::ostream& outs){

	outs << "Printing window size: "; window_s.print(outs);
	outs << "Printing actual size: "; area.print(outs);
	outs << "Printing display info: " << display.w << "x" << display.h << endl;
}

void sdl_help::present(){
	SDL_RenderPresent(renderer);
}
//arrange the tiles and draw their textures, for now just doing two tiles per row, with their locations
//being just a fraction of the screen size for testing
void sdl_help::draw_all(){
	SDL_Texture* tex = NULL;//seeing if putting these up here will help stop the memory leak
	SDL_Surface* surf = NULL;//it seems to have worked. It runs better and valgrind says hardly any
			  //memory leaks

	int max_y = -1; //keep track of highest y+texture height value we've seen
				   //so that sdl_help can keep track of what can't fit on the screen
	int max_x = -1; //similarly keep track of highest x+texture width value we've seen


	int row = 0; //keep track of what row we're on
	for(unsigned int c = 0; c < get_mgr().tiles.size();c++){
		string path = image_p + tile_bag.tiles[c].get_img_name(); //combine folder and image path

		surf = IMG_Load(path.c_str()); //load the image to a sdl surface


		if(surf == NULL) cout << SDL_GetError() << endl; //get useful error messages



		tex = SDL_CreateTextureFromSurface(renderer,surf); //change the surface to a
										//texture
		if(tex == NULL) cout <<  SDL_GetError() << endl; //get useful sdl error messages

		//calculate where this tile's base location (upper left corner) is
		int xloc = 0, yloc = 0;//default, should be changed in all cases except background tile


		if(c % 2 == 1){ //for now we're assuming only 2 tiles per row
			xloc = ((display.w / 2) / 4) - tile_bag.tiles[c].get_size().width / 2;
											//if odd place in
			yloc = 30 + 30 * row + row * tile_bag.tiles[c].get_size().height;//column 1

		} else if(c == 0){//empty case, rendercopy will get NULL NULL for bg tile

		} else { //if even, place in second column
			xloc = ((display.w / 2 )* .75) - tile_bag.tiles[c].get_size().width / 2;
			yloc = 30 + 30 * row + row * tile_bag.tiles[c].get_size().height;
			row++;//increment row counter every other tile
		}

		//use the base location + and the size info stored in the field to tell rendercopy where
		//to draw each tile
		SDL_Rect dest = {xloc+x_scroll,yloc+y_scroll,tile_bag.tiles[c].get_size().width,
					tile_bag.tiles[c].get_size().height};
		if(c == 0){ //special logic for the background tile
			SDL_RenderCopy(renderer,tex,NULL,NULL);//background should fill whole window

			//anti seg fault and double counting every frame
			if(tile_locations.size() != tile_bag.tiles.size()){
				tile_locations.push_back(dest); //save where background tile was drawn
			} else {
				tile_locations[0] = dest;
			}
		} else { //process normal (meaningful) tiles here
			if(sdl_test) cout << "To RenderCopy: xloc = " << xloc << " yloc= " << yloc
			     << " tile width = " << tile_bag.tiles[c].get_size().width
			     << " tile height = " << tile_bag.tiles[c].get_size().height << endl;
			//the NULL here means use the whole texture, if we animate textures with
			//sprite sheets, this will change
			if(tile_locations.size() != tile_bag.tiles.size()){
				//when I first wrote this I didn't account for the vector ALWAYS adding in
				//a new SDL_Rect, causing a funny seg fault where illegal bits were printed
				//to cout.....think it through!
				tile_locations.push_back(dest);//only push_back if vector doesn't have every
							       //tile in it yet
			} else {
				tile_locations[c] = dest;//if the vectors are the same size, then we need 
							 //to only update the position - hopefully no seg 
                                                         //fault and double positives now
			}
			SDL_RenderCopy(renderer,tex,NULL,&dest);
                }

		if( surf == NULL || tex == NULL || renderer == NULL){
			cout << "Error in draw_all(), an SDL pointer is null." << endl;

			cout << "surface | texture | renderer: " << surf << " | " << tex
			     << " | " << renderer << endl;

			cout << "Image path: " << path << endl;
		}
		if(sdl_test) cout << "xloc: " << xloc << " yloc: " << yloc << endl;
		if(!area_size_set){
			if(yloc > max_y) max_y = yloc;//save value if it's higher than previous record
			if(xloc > max_x) max_x = xloc;//save value if it's higher than previous record
		}
		SDL_DestroyTexture(tex);
		SDL_FreeSurface(surf); //this stops memory leaking?
		surf = NULL;
		tex = NULL;
	}//end of for loop
	if(!area_size_set){
		cout << "Set area size's dimensions." << max_x << "x" << max_y << endl;
		area.width = max_x;   //update sdl_help's area struct 
		area.height = max_y;  //with the maximum coords needed for all items,
		area_size_set = true;
	}
			   //even those that may be off screen
}//end of draw_all

void sdl_help::update_scroll(int x_scroll_in, int y_scroll_in){
	x_scroll = x_scroll + x_scroll_in;
	y_scroll = y_scroll + y_scroll_in;
}

void sdl_help::print_tile_locs(ostream& outs){
	for(unsigned int c = 0; c < tile_locations.size();c++){
		outs << "Corner= " << tile_locations[c].x << ":" << tile_locations[c].y 
		     << " tile dimensions= "<< tile_locations[c].w << ":" << tile_locations[c].h << endl;
	}
}
void sdl_help::click_detection(ostream& outs,int click_x, int click_y) const{
	for(unsigned int c = 0; c < tile_locations.size();c++){ //walk entire location vector
		if( in(click_x,click_y,tile_locations[c])){ //if the mouse click coordinates fall within
			tile_bag.tiles[c].clicked(outs);//a tile, enact that tiles clicked() member
		}//endif	
	}//end of for loop
}
bool sdl_help::in(int click_x, int click_y,const SDL_Rect& rect) const{
	if( click_x > rect.x && click_x < rect.x + rect.w &&
	    click_y > rect.y && click_y < rect.y + rect.h) return true;//return true if click falls within
								       //the tile's location
	return false; //return false otherwise
}






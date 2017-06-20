//! \file sdl_help.cc implements the functions declared in sdl_help.h
#include "sdl_help.h"
#include<iostream>
#include<string>
#include<cmath>
#include<algorithm>
#include<queue>

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
sdl_help::sdl_help(string name_in,string HF_input_file_in){
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS|SDL_INIT_VIDEO);//for now, timer,video and keyboard
	IMG_Init(IMG_INIT_PNG);//allows use of .png files
	if(TTF_Init() != 0){ //allows sdl to print text to the screen using .ttf files
		cout << "Error in TTF_Init()! " << endl;
	}
	window_name = name_in; //set window name
	image_p = "./Assets/Images/";
	font_p = "./Assets/fonts/";
	hf_input_p = "./HF_Input/";

	frame_count = 0;

	if(SDL_GetCurrentDisplayMode(0,&display) < 0){
		cout << "Get current display mode error" << endl;
		cout << SDL_GetError();
        };

	//cout << display.w << " " << display.h << endl;;
	window = SDL_CreateWindow(window_name.c_str(), 0, 0, display.w * .5, display.h * .75, 0);
	renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_PRESENTVSYNC);
 //(font_p + "LiberationSerif-Regular.tff").c_str()
	font = TTF_OpenFont( "./Assets/fonts/LiberationSerif-Regular.ttf", 22);//set up pointer to font from file
	if(font == NULL) {
		cout << "Error in opening font! " << SDL_GetError() << endl;
	}
        if(sdl_test) cout << "Enacting tile_bag update with values: " << display.w / 2 << " "
                          << display.h << endl;

	window_update(display.w / 2,display.h * .75);//this call updates sdl_help and manager's
					           // dimension window fields
	x_scroll = 0; y_scroll = 0; //set scrolling variables to 0

	tile_bag.init();
	//give vertical scroll bar the addresses of the info it needs from the sdl_help object
	vert_bar.init(&x_scroll,&y_scroll,&area.width,&area.height,
		      &window_s.width,&window_s.height, renderer,"v_ou_dark_green_quarter.png");
	//give horizontal scroll bar the address of the info it needs from the sdl_help object
	horiz_bar.init(&x_scroll,&y_scroll,&area.width,&area.height,
		      &window_s.width,&window_s.height, renderer,"h_ou_grey_quarter.png");

	vert_bar.print(cout);
	horiz_bar.print(cout);

	calc_corners(); //set up tile locations with the field's corner location 
	tile_bag.give_fields_renderer(renderer,image_p,&x_scroll,&y_scroll,font);//give fields rendering and font info

	io_handler.init();
	give_manager_io(&io_handler);
}

sdl_help::sdl_help(std::string name_in,string HF_input_file_in,
		   int width, int height){
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS|SDL_INIT_VIDEO);//for now, timer and keyboard
	IMG_Init(IMG_INIT_PNG);//allows use of .png files
	if(TTF_Init() != 0){//allows sdl to print text using .ttf files
		cout << "Error in TTF_Init()! " << endl;
	}
	window_name = name_in;
	image_p = "./Assets/Images/";
	hf_input_p = "./HF_Input/";
	font_p = "./Assets/fonts/";
	frame_count = 0;

	//this call is to make sure refresh_rate and driverdata are handled
	if(SDL_GetCurrentDisplayMode(0,&display) < 0){
		cout << "Get current display mode error" << endl;
		cout << SDL_GetError();
	}
	//initializing them twice is an obvious waste of time, consider a more elegant solution

	//init displaymode values
	display.w = width;
	display.h = height; //overrite previous height and width values to the user's preference

	window_update(display.w/2,display.h);//this call updates sdl_help and manager's window dimension 
                                             //fields tile_bag.update_win(display.w,display.h);

	window = SDL_CreateWindow(window_name.c_str(), 0, 0, display.w, display.h, 0); //set up the window pointer
	renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_PRESENTVSYNC); //set up the renderer pointer
	font = TTF_OpenFont( (font_p+"LiberationSerif-Regular.ttf").c_str(),22);//"Aller_Bd.ttf"
	if(font == NULL) {
		cout << "Error in opening font! " << SDL_GetError() << endl;
	}

	x_scroll = 0; y_scroll = 0; //set scrolling values to 0

	tile_bag.init();

	//give vertical scroll bar the addresses of the info it needs from the sdl_help object
	vert_bar.init(&x_scroll,&y_scroll,&area.width,&area.height,
		      &window_s.width,&window_s.height, renderer,"v_ou_dark_green_quarter.png");
	//give horizontal scroll bar the address of the info it needs from the sdl_help object
	horiz_bar.init(&x_scroll,&y_scroll,&area.width,&area.height,
		      &window_s.width,&window_s.height, renderer,"h_ou_grey_quarter.png");

	vert_bar.print(cout);
	horiz_bar.print(cout);

	calc_corners();//set up tile_locations with the field's corner locations
	tile_bag.give_fields_renderer(renderer,image_p,&x_scroll,&y_scroll,font);//give fields their rendering and
										 //font info

	//io_handler.init();
	give_manager_io(&io_handler);
}

sdl_help::~sdl_help(){
	SDL_DestroyRenderer(renderer);//stops memory leaks
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);//give back memory from the font pointer

	//cout << "I'm dying. You killed me. Final frame count= " << frame_count << endl;
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void sdl_help::quit(){
	this->~sdl_help();  
}
//#####################################################################################################


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

void sdl_help::give_manager_io(input_maker* input_maker_hook_in){
	tile_bag.set_input_maker_hook(input_maker_hook_in);
}

void sdl_help::present(){
	SDL_RenderPresent(renderer);
}
//arrange the tiles and draw their textures, for now just doing two tiles per row, with their locations
//being just a fraction of the screen size for testing
void sdl_help::draw_tiles(){
	SDL_RenderClear(renderer);//clear off the renderer, to prepare to re-draw

	vector<unsigned int> help_indices;
	for(unsigned int c = 0; c < tile_bag.tiles.size();c++){
		if(!tile_bag.tiles[c].is_help_mode()){ //draw tiles NOT in help mode first, so that the help mode boxes
			tile_bag.tiles[c].draw_me(); //are always on top
		} else {
			help_indices.push_back(c);//save this index, so that the field tiles in help mode can
						  //be drawn without re-iterating over the entire vector
		}
	}
	//loop over vector of indices that still need drawn, drawing those tiles with help boxes on top
	for(unsigned int c = 0; c < help_indices.size(); c++){
		tile_bag.tiles[help_indices[c]].draw_me();
	}

	frame_count++;//increment the frame counter

}//end of draw_tiles

void sdl_help::draw_sbars(){
	horiz_bar.draw_me();
	vert_bar.draw_me();
}
//**********************SCROLLING FUNCTIONS ***************************************************/

// walk the tile locations vector, and keep track of the rightmost, leftmost, bottommost, and upmost
// edges of all the tiles. This is used to make sure the user can't scroll in any direction past the point
// where no objects are visible, or are just barely visible - might fiddle with constants to make it stop
// scrolling a bit sooner or later
void sdl_help::most(int& rightmost,int& leftmost,int& upmost,int& downmost){
	//note that we are starting at 1 to avoid the massive background tile messing up this calculation
	for(unsigned int c = 1; c < tile_bag.tiles.size();c++){
		if(tile_bag.tiles[c].yloc + y_scroll < upmost){ //highest tile corner means LEAST Y value
			upmost = tile_bag.tiles[c].yloc + y_scroll;
		}
		if(tile_bag.tiles[c].yloc + y_scroll + tile_bag.tiles[c].get_size().height > downmost){
			//save lowest tile corner + that texture's height
			downmost = tile_bag.tiles[c].yloc + y_scroll + tile_bag.tiles[c].get_size().height;
		}
		if(tile_bag.tiles[c].xloc + x_scroll < leftmost){// save leftmost tile corner
			leftmost = tile_bag.tiles[c].xloc + x_scroll;
		}
		if(tile_bag.tiles[c].xloc + x_scroll +  tile_bag.tiles[c].get_size().width > rightmost){
			//save rightmost tile corner + texture width
			rightmost = tile_bag.tiles[c].xloc + x_scroll + tile_bag.tiles[c].get_size().width;
		}
	}
	//cout << "Rightmost: " << rightmost << " Leftmost: " << leftmost
	     //<< "Upmost: " << upmost << "Downmost: " << downmost << endl;
}

//can detect when we should stop scrolling, but allows no scrolling afterwards, not even in the opposite
//direction - fixed, but is there a better way?
void sdl_help::update_scroll(int x_scroll_in, int y_scroll_in){
	int rightmost = -2048, leftmost = 2048, upmost = 2048, downmost = -2048;
	most(rightmost,leftmost,upmost,downmost);

	if( (rightmost + x_scroll_in) <= 0){
		x_scroll = x_scroll + abs(0-rightmost);
		//cout << "Hit right scrolling barrier." << endl;
	}
	if( (leftmost + x_scroll_in) >= window_s.width){
		x_scroll = x_scroll - (/*window_s.width - leftmost*/ leftmost-window_s.width);
		//cout << "Hit left scrolling barrier." << endl;
	} //mid-statement commented regions are previous values, they were buggy so trying them switched
	  //seems to have fixed it, but commented regions stay until more strenuously tested
	if( (upmost + y_scroll_in) >= window_s.height){
		y_scroll = y_scroll - (/*window_s.height - upmost*/ upmost-window_s.height);
		//cout << "Hit up scrolling barrier." << endl;
	}
	if( (downmost + y_scroll_in) <= 0){
		y_scroll = y_scroll + abs(0-downmost);
		//cout << "Hit down scrolling barrier." << endl;
	}
	//it would make sense to be able to scroll like this 
		//cout << "x_scroll increased by " << x_scroll_in << "| " << x_scroll << "-> "
		//     << x_scroll + x_scroll_in << endl;
		//cout << "y_scroll increased by " << y_scroll_in << "| " << y_scroll << "-> "
		//     << y_scroll + y_scroll_in << endl;
		x_scroll = x_scroll + x_scroll_in;
		y_scroll = y_scroll + y_scroll_in;

	//let the scroll bars know what is going on
	vert_bar.update();
	horiz_bar.update();
	
}
//return user to the top of the page -currently called from a spacebar press
void sdl_help::reset_scroll(){
	x_scroll = 0; y_scroll = 0;

	//let scroll bars know that scrolling has been reset
	vert_bar.update();
	horiz_bar.update();
}

int sdl_help::scroll_clicked(ostream& outs,int click_x, int click_y) const{
	if(vert_bar.clicked(outs,click_x,click_y)) return 1;//call vbar's click detection member
							    //1 means vertical bar was clicked
	if(horiz_bar.clicked(outs,click_x,click_y)) return 2;//call hbar's click detection member
							     //2 means horizontal bar was clicked
	return 0;   //if we make it to this line return 'false' 0 (nothing was clicked)
		     //this returning false allows handle_mouseb_down to check the tiles
}
//********************************************************************************************/

void sdl_help::print_tile_locs(ostream& outs){
	for(unsigned int c = 0; c < tile_bag.tiles.size();c++){
		outs << "Corner= " << tile_bag.tiles[c].xloc << ":" << tile_bag.tiles[c].xloc
		     << " tile dimensions= " << tile_bag.tiles[c].get_size().width << ":"
		     << tile_bag.tiles[c].get_size().height << endl;
	}
}

void sdl_help::click_detection(ostream& outs,SDL_Event& event, int click_x, int click_y){
	for(unsigned int c = 0; c < tile_bag.tiles.size();c++){

		if( in(click_x,click_y, tile_bag.tiles[c].get_rect() ) ){
		//if the mouse click coordinates fall within a tile,
			if( tile_bag.tiles[c].text_box_clicked(outs,click_x,click_y) ){
				//if that click fell within the text box
				text_box_mini_loop(outs,event, tile_bag.tiles[c]);

			} else { //if that click didn't fall within the text box, enact clicked
				tile_bag.tiles[c].clicked(outs,event,click_x,click_y);//enact that tiles clicked() member

			}
		}//endif	

	}

}

//thanks to http://lazyfoo.net/tutorials/SDL/32_text_input_and_clipboard_handling/index.php
//which was used as a reference 
void sdl_help::text_box_mini_loop(ostream& outs, SDL_Event& event,field& current_tile){

	SDL_StartTextInput();//turn on the text input background functions

	//used to control text entry loop
	bool done = false;
	//int c = 0;
	bool text_was_changed = false;

	while(!done){
		//if(c >= 10) return;
		//do stuff
		//cout << " in text input mini loop " << c << endl;

		if( !SDL_PollEvent(&event) ){
			event.type = 1776; //dummy event to stop it from printing default message every frame
					   //where no event happens
		}

		switch(event.type){
		  case SDL_MOUSEMOTION:
			cout << "Mouse motion for some reason.... " << endl;
			break;

		  case SDL_MOUSEBUTTONDOWN:
			//if the click was within the text box, move the cursor maybe
		  	if( current_tile.text_box_clicked(outs,event.button.x,event.button.y) ){
				//cout << "Text box click at " << event.button.x << ":" << event.button.y << endl;
		  	} else { //elsewise exit text input mode, user clicked off the text box
		  		//cout << "Clicked outside of the text box, exiting mini-loop" << endl;
				SDL_PushEvent(&event);//doing this allows the user to 'hop' to another text box
						      //directly from editing another box
				done = true;
			}
		  	break;

		  case SDL_TEXTINPUT:
		  	cout << " I guess this was an SDL_TEXTINPUT event... " << endl;
			current_tile.update_temp_input(event);
			text_was_changed = true;
		  	//here this actually causes a loss of letters, so the event flooding is necessary, don't flush
			//SDL_FlushEvent(SDL_TEXTINPUT);
			break;

		  case SDL_KEYDOWN:
		  	//cout << " Key pressed: " << event.key.keysym.sym << endl;
			text_box_mini_loop_helper(event.key.keysym,current_tile,text_was_changed);

			SDL_FlushEvent(SDL_KEYDOWN); //prevent event flooding
		  	break;
		  case SDL_QUIT:
			//cout << "exiting from text entry" << endl;
			SDL_PushEvent(&event);//puts another sdl quit in the event queue, so program
					      //can be terminated while in "text entry" mode
			done = true;			
			break;

		  case 1776: //do nothing, event was not new
			break;

		  default:
			//outs << "Error finding case in text entry mini-loop" << endl;
			break;
		}



		//if something actually changed, re-draw
		//elsewise don't do it to try and save time
		if(text_was_changed){
			//cout << "HAVING TO REDRAW" << endl;
			//update picture
			draw_tiles();
			draw_sbars();
			text_was_changed = false;
			//show updated picture
			present();
		}

		//c++;
		SDL_Delay(50);
	}//end of loop
	SDL_StopTextInput();//stop text input functionality because it slows down the app

}

void sdl_help::text_box_mini_loop_helper(SDL_Keysym& key,field& current_tile,bool& text_was_changed){
	cout << key.sym << endl;
	switch( key.sym ){
	  case SDLK_BACKSPACE:
	  	//cout << "BACKSPACE" << endl;
		//delete last character, unless it's empty already than do nothing
		if( current_tile.temp_input.size() > 0 ){
			current_tile.back_space();//delete a character, update text's graphics
			text_was_changed = true;
		}
		break;

	  default:
	  	break;

	}
}


bool sdl_help::in(int click_x, int click_y,const SDL_Rect& rect) const{
	if( click_x > rect.x && click_x < rect.x + rect.w &&
	    click_y > rect.y && click_y < rect.y + rect.h) return true;//return true if click falls within
								       //the tile's location
	return false; //return false otherwise
}




void sdl_help::calc_corners(){
	//cout << tile_locations.size() << endl;
	vector<index_and_width> candidates;//will be filled up with the width of each tile in the tile bag
				       //and the index IN THE TILE BAG that corresponds to it

	//start counting at 1 because the background tile is taken care of by field constructor
	for(unsigned int c = 1; c < tile_bag.tiles.size();c++){
		index_and_width push_me;
		push_me.width = tile_bag.tiles[c].get_size().width;
		push_me.index = c;
		candidates.push_back(push_me);
	}
	sort(candidates.begin(),candidates.end(),compare_width);//sort the candidates such that they are in
								//ascending order based on width

	vector<int> processed(candidates.size(),0);//"boolean" vector, I don't want to use the optimized
						   //stl vector<bool> here, initialize to 0 = false

	queue<index_and_width> row;//temporarily hold the info of tiles about to be added to a row

	unsigned int curr_width = 0;//keep track of the current row's width
	unsigned int max_height = 0;//keep track of the current row's height

	unsigned int horiz_padding = 20; //distance between columns
	unsigned int vert_padding = 20; //distance between rows


	unsigned int width_limit = window_s.width;

	unsigned int done = 0;//keep track of how many tiles have their base corner location stored


	unsigned int prev_height = 0; //keep track of the height of the previous rows
	//cout << "done: " << done << " candidate size: " << candidates.size() << endl;
	while(done < candidates.size()){ //stop when every tile has been placed
		//cout << "DONE: " << done << endl;
		curr_width = 0;//each new row begins with 0 width, which is filled in as tiles are chosen
		max_height = 0;//reset this variable here, to be sure rows aren't affecting subsequent row's height

		int j = 0;
		while( (processed[j]) && j < processed.size()-1) j++ ;  //get index of lowest width tile that hasn't
								 //been processed yet


		while( (width_limit - curr_width) > candidates[j].width ){

			//cout << "WIDTH_LIMIT - CURR_WIDTH= " << width_limit - curr_width << endl;
			//cout << "SMALLEST TILE= " << candidates[j].width << endl;


			//start at the end of the array (where the highest values are) and walk backgrounds
			//until a tile that hasn't been processed has been found
			int i = candidates.size() - 1;
			while( i > -1 ){
			  //if this tile won't fit, move on
			  if( (!processed[i]) ){ //if it hasn't been processed yet, consider it
				//if it doesn't fit, move on
				if(curr_width + candidates[i].width + horiz_padding > width_limit){
					i--;
				} else break; //elsewise we can leave this loop, i is where it needs to be
			  } else i--; //keep going until an unprocessed candidate has been found
			}
			//cout << "I candidate that may work i= " << i << " width = " << candidates[i].width
			//     << endl;

			//the i index for the very last run of this loop is bad, so we need to leave early
			//the queue is set up at this point so the while(!row.empty()) loop will finish up
			if(i == -1){
				//cout << "row queue size when i = -1 : " << row.size() << endl;
				break; //we have placed all tiles, exit
			}
			//keep track of how much space has been used, account for padding
			curr_width = curr_width + candidates[i].width + horiz_padding;

			//similarly keep track of the height of this row , account for padding
			unsigned int temp_height = tile_bag.tiles[candidates[i].index].get_size().height +
					  + vert_padding;
			if(temp_height > max_height) {//save new max height
				max_height = temp_height;
				//cout << " NEW MAX HEIGHT: " << temp_height << endl;
			}
			//don't look at this tile again, set it to "true" in the "boolean" vector
			processed[i] = 1;

			row.push(candidates[i]);//add it on to the pending row
		} //loop for planning a row and filling in the queue
		//should be ready to add a row once the above mini loop is done


		int row_width = 5; //first tile starts off at the left edge of the screen - with some padding
		while( !row.empty() ){
			//cout << "QUEUE SIZE= " << row.size() << endl;
			index_and_width temp = row.front(); //copy element in the front of the queue
			row.pop();  //pop out biggest tile

			tile_bag.tiles[temp.index].xloc = row_width;
			tile_bag.tiles[temp.index].yloc = prev_height;


			//calc xloc for next tile
			row_width = row_width + tile_bag.tiles[temp.index].get_size().width + horiz_padding;
			done++;//increment our progress counter
		}//loop for putting the row into tile_locations

		prev_height = prev_height + max_height; //update prev_height with new row's max value
	}//loop until done
	//this call sets the area struct
	tile_bag.set_area(area.width,area.height);

}

//############################ NON-MEMBER HELPERS ##########################################################

bool compare_width(index_and_width& left, index_and_width& right){
	if(left.width < right.width) return true;
	return false;
}

















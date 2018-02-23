//! \file sdl_help.cc implements the functions declared in sdl_help.h
#include "sdl_help.h"
#include<iostream>
#include<string>
#include<cmath>
#include<algorithm>
#include<queue>

//button manager is included here so that sdl_help can access button_manager::draw_buttons()
//this include CAN'T be in the header file because it creates a circular dependency
//more pre-project planning on my part would have avoided such a misfortune
#include "button_manager.h"

#include "define.h"

#define SDL_HELP_ERROR "Attempted double free in ~sdl_help"
using namespace std;

extern asset_manager* asset_access;
extern manager* tile_access;
//######################### WIN SIZE STRUCT ###################################
//initialize window dimensions to bad values so they must
//be initialized elsewhere

win_size::win_size(){
	width = -1;
	height = -1;
}
void win_size::print(){
	error_logger.push_msg(to_string(width)+"x"+to_string(height));
}

win_size* sdl_help::get_win_size(){
	win_size* return_me = &window_s;
	return return_me;
}
//######################### WIN SIZE STRUCT ###################################

//######################### SDL_HELP CONSTRUCTORS/DESTRUCTORS #################
sdl_help::sdl_help(string name_in,string HF_input_file_in,
				   string bg_image_name_in){
	//for now, timer,video and keyboard
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS|SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);//allows use of .png files
	//allows sdl to print text to the screen using .ttf files
	if(TTF_Init() != 0){ 
		error_logger.push_error("Error in TTF_Init()!");
	}

	//set window name
	window_name = name_in;

	font_p = "./Assets/fonts/";
	hf_input_p = "./HF_Input/";

	frame_count = 0;

	error_logger.push_msg("this is merely a test of the logger");

	bg_image_name = bg_image_name_in;


	if(SDL_GetCurrentDisplayMode(0,&display) < 0){
		error_logger.push_error("Get current display mode error:");
		error_logger.push_error(SDL_GetError());
    }

	int temp_window_w = display.w * .9;
	int temp_window_h = display.h * .9;
	error_logger.push_msg("display width: " + to_string(display.w) +
						  "display height:" + to_string(display.h));
	window = SDL_CreateWindow(window_name.c_str(), 0, 0,temp_window_w,
							  temp_window_h, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_PRESENTVSYNC);
	if(renderer == NULL){
		//put message in error logger
		error_logger.push_error("MASSIVE ERROR, RENDERER",
								"COULD NOT BE CREATED");
		error_logger.make_error_file();//make the error logger output
		exit(1984);//throw a tantrum and close program
	}

	//set up pointer to font from file
	font = TTF_OpenFont( "./Assets/fonts/LiberationSerif-Regular.ttf", 22);
	if(font == NULL) {
		error_logger.push_error(SDL_GetError());
	}
        error_logger.push_msg("Enacting tile_bag update with values: " +
							  to_string(display.w / 2) + " " + 
                              to_string(display.h) );

	//this call updates sdl_help and manager's  dimension window fields
	window_update(temp_window_w,temp_window_h);

	x_scroll = 0; y_scroll = 0; //set scrolling variables to 0

}

sdl_help::~sdl_help(){

	if(renderer != NULL && window != NULL && font != NULL){
		SDL_DestroyRenderer(renderer);//stops memory leaks
		SDL_DestroyWindow(window);
		TTF_CloseFont(font);//give back memory from the font pointer
	} else error_logger.push_error(SDL_HELP_ERROR);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

//#################################################################################

void sdl_help::init(){

	//############## background image initialization ############################//
	string bg_loc = "Assets/Images/Backgrounds/"+bg_image_name;
	bg_texture = asset_access->get_texture(bg_loc);
	if(bg_texture == NULL) error_logger.push_error(SDL_GetError());
	SDL_RenderCopy(renderer,bg_texture,NULL,NULL);

	//give vertical scroll bar the addresses of the info it needs from the sdl_help object
	vert_bar.init(&x_scroll,&y_scroll, &window_s.width, &window_s.height, renderer,"v_ou_dark_green_quarter.png");

	//give horizontal scroll bar the address of the info it needs from the sdl_help object
	horiz_bar.init(&x_scroll,&y_scroll, &window_s.width, &window_s.height, renderer,"h_ou_grey_quarter.png");
}

void sdl_help::window_update(int width_in, int height_in){

	//update sdl class's window size variables
	window_s.width = width_in;
	window_s.height = height_in;
	vert_bar.handle_resize();
	horiz_bar.handle_resize();
	if(tile_access != NULL){
		tile_access->update_win(width_in,height_in);
	}
}

void sdl_help::draw(){

	//clear off the renderer, to prepare to re-draw
	SDL_RenderClear(renderer);

	//draw the background image to the screen
	SDL_RenderCopy(renderer,bg_texture,NULL,NULL);


	tile_access->draw();
	draw_sbars();

	button_access->draw_tray();
	button_access->draw_form_tray();
	button_access->draw_buttons();

	//increment the frame counter
	frame_count++;
}

//prints area window size and display 
void sdl_help::print_size_info(){

	error_logger.push_msg("Printing window size: ");
	window_s.print();
	error_logger.push_msg("Printing display info: "+to_string(display.w)+"x"+to_string(display.h));
}

void sdl_help::present(){
	SDL_RenderPresent(renderer);
}

void sdl_help::draw_sbars(){
	horiz_bar.draw_me();
	vert_bar.draw_me();
}
/*
void sdl_help::toggle_resizable(){
	if(resizable){
		//forbid resizing of the window while in form loop
		SDL_SetWindowResizable(&window,SDL_FALSE);
		resizable = false;
	} else {
		//allow the window to be resized
		SDL_SetWindowResizable(&window,SDL_TRUE);
		resizable = true;

	}
}*/
//**********************SCROLLING FUNCTIONS ***************************************************/

/* walk the tile locations vector, and keep track of the rightmost, leftmost,
* bottommost, and upmost edges of all the tiles. This is used to make sure that
*the user can't scroll in any direction past the point where no objects are
*visible, or are just barely visible - can fiddle with constants to make
*it stop scrolling a bit sooner or later */
void sdl_help::most(int& rightmost,int& leftmost,int& upmost,int& downmost){

	//shorthand reference to the tile vector in
	//the manager object
	vector<vector<field*>>& fields_order = tile_access->fields_order;

	for(uint line = 0; line < fields_order.size();line++){

		for(uint param = 0; param < fields_order[line].size();param++){


			int yloc   = fields_order[line][param]->yloc;
			int height = fields_order[line][param]->get_size().height;
			
			int xloc  = fields_order[line][param]->xloc;
			int width = fields_order[line][param]->get_size().width;


			//least Y value means it is the highest corner
			if(fields_order[line][param]->yloc + y_scroll < upmost){
				upmost = yloc + y_scroll;
			}

			//highest Y value means it is the lowest corner
			if(yloc + y_scroll + height > downmost){
				downmost = yloc + y_scroll + height; 
			}

			//lowest X value means that it is the left most corner
			if(xloc + x_scroll < leftmost){
				leftmost = xloc + x_scroll;
			}

			//highest X value means that it is the right most corner
			if(xloc + x_scroll + width > rightmost){
				rightmost = xloc + x_scroll + width;
			}

		}//parameters for loop
	}//lines for loop
}

void sdl_help::update_scroll(int x_scroll_in, int y_scroll_in){

	int rightmost = INT_MIN, leftmost = INT_MAX;
	int upmost = INT_MAX, downmost = INT_MIN;
	most(rightmost,leftmost,upmost,downmost);

	if( (rightmost + x_scroll_in) <= 0){
		x_scroll = x_scroll + abs(0-rightmost);
		error_logger.push_msg("Hit right scrolling barrier.");
	}

	if( (leftmost + x_scroll_in) >= window_s.width){
		x_scroll = x_scroll - (leftmost-window_s.width);
		error_logger.push_msg("Hit left scrolling barrier.");
	}

	if( (upmost + y_scroll_in) >= window_s.height){
		y_scroll = y_scroll - (upmost-window_s.height);
		error_logger.push_msg("Hit up scrolling barrier.");
	}

	if( (downmost + y_scroll_in) <= 0){
		y_scroll = y_scroll + abs(0-downmost);
		error_logger.push_msg("Hit down scrolling barrier.");
	}

	//it would make sense to be able to scroll like this
	string msg = "x_scroll increased by " + to_string(x_scroll_in);
	msg       += "| " + to_string(x_scroll) + "-> ";
	msg       += to_string(x_scroll + x_scroll_in);
	error_logger.push_msg(msg);

	msg  = "y_scroll increased by " + to_string(y_scroll_in);
	msg += "| " + to_string(y_scroll) + "-> " +to_string(y_scroll+y_scroll_in);

	error_logger.push_msg(msg);
	x_scroll = x_scroll + x_scroll_in;
	y_scroll = y_scroll + y_scroll_in;

	//let the scroll bars know what is going on
	vert_bar.update();
	horiz_bar.update();
	
}
//return user to the top of the page - currently called from a spacebar press
void sdl_help::reset_scroll(){
	x_scroll = 0; y_scroll = 0;

	//let scroll bars know that scrolling has been reset
	vert_bar.update();
	horiz_bar.update();
}

int sdl_help::scroll_clicked(int click_x, int click_y) const{
	//call vbar's click detection member
	//1 means vertical bar was clicked
	if(vert_bar.clicked(click_x,click_y)) return 1;

	//call hbar's click detection member
	//2 means horizontal bar was clicked
	if(horiz_bar.clicked(click_x,click_y)) return 2;

	//if we make it to this line return 'false' 0 (nothing was clicked)
	//this returning false allows handle_mouseb_down to check the tiles
	return 0;
}
//*****************************************************************************/

void sdl_help::click_detection(ostream& outs,SDL_Event& event,int click_x, int click_y){

	vector<vector<field*>>& fields_order = tile_access->fields_order;

	for(uint line = 0; line < fields_order.size();line++){

		for(uint param = 0; param < fields_order[line].size();param++){
			
			field* field_ptr = fields_order[line][param];

			if( in(click_x, click_y,field_ptr->get_rect()) ){

				//if they clicked the text box, allow them to
				//edit the parameter
				if(field_ptr->text_box_clicked(click_x,click_y)){

					if(!field_ptr->is_locked){
						text_box_mini_loop(outs,event,field_ptr);
					}

				//if they clicked the parameter name, give them
				//a brief explanation
				} else {

					field_ptr->clicked(event,click_x,click_y);

				}//end help dialogue click case
				
			
			}//end general click case

		}//end parameter for

	}//end line for 

}//end function

/*thanks to 
*http://lazyfoo.net/tutorials/SDL/32_text_input_and_clipboard_handling/index.php
which was used as a reference */
 
void sdl_help::text_box_mini_loop(ostream& outs, SDL_Event& event,
													field* current_tile){

	//turn on the text input background functions
	SDL_StartTextInput();

	//used to control text entry loop
	bool done = false;

	//int c = 0;
	bool text_was_changed = false;

	while(!done){


		if( !SDL_PollEvent(&event) ){
			//dummy event to stop it from printing default message every frame
			//where no event happens
			event.type = 1776; 
		}

		switch(event.type){
		  case SDL_MOUSEMOTION:
			break;

		  case SDL_MOUSEBUTTONDOWN:
			//if the click was within the text box, move the cursor maybe
		  	if( current_tile->text_box_clicked(event.button.x,event.button.y) ){

				string msg = "Text box click at " + to_string(event.button.x);
				msg       += ":" + to_string(event.button.y);
				error_logger.push_msg(msg);

			//elsewise exit text input mode, user clicked off the text box
		  	} else {
				//doing this allows the user to 'hop' to another text box
				//directly from editing another box
				SDL_PushEvent(&event);
				done = true;
			}
		  	break;

		  case SDL_TEXTINPUT:
			current_tile->update_temp_input(event);
			text_was_changed = true;
		  	//here this actually causes a loss of letters, so the event
			//flooding is necessary, don't flush SDL_FlushEvent(SDL_TEXTINPUT);
			break;

		  case SDL_KEYDOWN:
			text_box_mini_loop_helper(event.key.keysym,current_tile,
													text_was_changed);

			//prevent event flooding
			SDL_FlushEvent(SDL_KEYDOWN);
		  	break;
		  case SDL_QUIT:
			//puts another sdl quit in the event queue, so program
			//can be terminated while in "text entry" mode
			SDL_PushEvent(&event);
			done = true;			
			break;

		  //do nothing, event was not new
		  case 1776:
			break;

		  default:
			//outs << "Error finding case in text entry mini-loop" << endl;
			break;
		}



			//update picture
			sdl_access->draw();
			current_tile->draw_cursor();
			text_was_changed = false;
			present();

	}//end of loop
	//stop text input functionality because it slows down the app
	SDL_StopTextInput();
}

void sdl_help::text_box_mini_loop_helper(SDL_Keysym& key,field* current_tile,
											bool& text_was_changed){

	switch( key.sym ){
		case SDLK_BACKSPACE:
			//delete last character, unless it's empty already than do nothing
			if( current_tile->temp_input.size() > 0 ){
				//delete a character, update text's graphics
				current_tile->back_space();
				text_was_changed = true;
			}
			break;
	
		case SDLK_LEFT:
			//if we are not already at the very left of the text,
			//move the editing position one to the left
			if(current_tile->editing_location > 0){
				current_tile->editing_location--;
				text_was_changed = true;
			}
			break;
		case SDLK_RIGHT:
			//if we are not already at the very end of the text,
			//move the editing position one to the right
			if(current_tile->editing_location < current_tile->temp_input.size()){
				current_tile->editing_location++;
				text_was_changed = true;
			}
			break;
	  default:
	  	break;

	}
}


bool sdl_help::in(int click_x, int click_y,const SDL_Rect& rect) const{
	if( click_x > rect.x && click_x < rect.x + rect.w &&
	    click_y > rect.y && click_y < rect.y + rect.h){

		//return true if click falls within
		//the tile's location
		return true;
	}
	//return false otherwise
	return false;
}




void sdl_help::calc_corners(){
	error_logger.push_msg("######### IN CALC CORNERS ########################");
	//this variable keeps track of where the next line should
	//start being placed. The helper function should set it to be just
	//below the newly created section of tiles, and some padding value

	//5 pixel buffer from top of window
	uint row_height = 5;

	//this variable limits the width of the rows
	int row_limit;

	int widest_tile = tile_access->get_widest_tile_width();
	error_logger.push_msg("WIDEST TILE: "+to_string(widest_tile));
	if(widest_tile > window_s.width){
		error_logger.push_msg("USING WIDEST TILE TO LIMIT ROWS");
		//if a single tile is bigger than the window, use it for logic
		//because the window is likely so small that the normal
		//row placement logic won't work				
		row_limit = widest_tile;

	} else {

		error_logger.push_msg("USING WINDOW SIZE TO LIMIT ROWS");
		//else wise, fill up the window as best it can
		row_limit = window_s.width; 
	}


	for(uint c = 0; c < tile_access->fields_order.size();c++){
		calc_corners_helper(tile_access->fields_order[c],
							row_height,row_limit);

	}

	error_logger.push_msg("############# END CALC CORNERS #################");

}

void sdl_help::calc_corners_helper(vector<field*>& line_in,
								   uint& start_height,
								   int row_limit){

	//distance between two tiles horizontally
	int x_buffer = HORIZ_TILE_PADDING;

	int x_corner = x_buffer;

	//save the starting position to be used to set the corner
	//coordinates for fields but this variable can be changed if the current
	//line map takes up more than one row in the window
	int y_corner = start_height;
	

	//save the lowest ylocation + height value there is, so we can update
	//start_height when this function is done, to allow other rows
	//to know where to begin placing fields
	int lowest_point = start_height;

	for(uint c = 0; c < line_in.size();c++){

		//this is the case where the tile can stay in the current row 
		if(x_corner + line_in[c]->get_size().width < row_limit){

			line_in[c]->xloc = x_corner;
			line_in[c]->yloc = y_corner;


			x_corner = line_in[c]->xloc + line_in[c]->get_size().width + x_buffer;

			
			if(line_in[c]->yloc + line_in[c]->get_size().height + 5 > lowest_point){
				error_logger.push_msg("OLD lowest_point:"+to_string(lowest_point));
				lowest_point = line_in[c]->yloc + line_in[c]->get_size().height + 5;
				error_logger.push_msg(" NEW lowest_point:"+to_string(lowest_point));
			}
		//this is the case where the tile needs to be placed into
		//a new row (because there's not enough width left)
		} else {
			//place it on the left edge
			line_in[c]->xloc = x_buffer;

			//save it's leftmost edge + padding
			//to be used to place the next tile
			x_corner = line_in[c]->xloc + 
						line_in[c]->get_size().width + x_buffer;

			//place it just below the previous row
			line_in[c]->yloc = lowest_point;

			//set that lowest point as the new y coordinate for the
			//fields in this row
			y_corner = lowest_point;

			//save new lowest point
			lowest_point = line_in[c]->yloc +
							line_in[c]->get_size().height + 5;

		}		

		cout << "Paramter placement####################" << endl;
		cout << line_in[c]->xloc << ":" << line_in[c]->yloc << endl;
	}

	//save the the start location for the next row
	//start_height = lowest_point + 5;
	start_height = lowest_point + 15;
}

//##################### NON-MEMBER HELPERS #####################################

bool compare_width(names_and_width& left, names_and_width& right){
	if(left.width < right.width) return true;
	return false;
}














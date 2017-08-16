//! \file sdl_help.cc implements the functions declared in sdl_help.h
#include "sdl_help.h"
#include<iostream>
#include<string>
#include<cmath>
#include<algorithm>
#include<queue>

//button manager is included here so that sdl_help can access button_manager::draw_buttons()
//this include CAN'T be in the header file because it creates a circular dependency
//more pre-project planning on my part would have avoided such a misfortune, I am sorry
#include "button_manager.h"

using namespace std;


//######################### WIN SIZE STRUCT ################################################################
win_size::win_size(){//initialize window dimensions to bad values so they must be initialized elsewhere
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
//######################### WIN SIZE STRUCT ################################################################

//######################### SDL_HELP CONSTRUCTORS/DESTRUCTORS ##############################################
sdl_help::sdl_help(string name_in,string HF_input_file_in,string bg_image_name_in){
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS|SDL_INIT_VIDEO);//for now, timer,video and keyboard
	IMG_Init(IMG_INIT_PNG);//allows use of .png files
	if(TTF_Init() != 0){ //allows sdl to print text to the screen using .ttf files
		error_logger.push_error("Error in TTF_Init()!");
	}
	window_name = name_in; //set window name
	image_p = "./Assets/Images/";
	font_p = "./Assets/fonts/";
	hf_input_p = "./HF_Input/";

	frame_count = 0;

	error_logger.push_msg("this is merely a test of the logger");

	bg_image_name = bg_image_name_in;


	if(SDL_GetCurrentDisplayMode(0,&display) < 0){
		error_logger.push_error("Get current display mode error:");
		error_logger.push_error(SDL_GetError());
        };

	int temp_window_w = display.w *.5;
	int temp_window_h = display.h * .75;
	error_logger.push_msg( "display width: " + to_string(display.w) + "display height:" + to_string(display.h));
	window = SDL_CreateWindow(window_name.c_str(), 0, 0,temp_window_w,temp_window_h, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_PRESENTVSYNC);
 //(font_p + "LiberationSerif-Regular.tff").c_str()
	font = TTF_OpenFont( "./Assets/fonts/LiberationSerif-Regular.ttf", 22);//set up pointer to font from file
	if(font == NULL) {
		error_logger.push_error(SDL_GetError());
	}
        error_logger.push_msg("Enacting tile_bag update with values: " + to_string(display.w / 2) + " " + 
                              to_string(display.h) );

	window_update(temp_window_w,temp_window_h);//this call updates sdl_help and manager's
					           // dimension window fields
	x_scroll = 0; y_scroll = 0; //set scrolling variables to 0



	//################ background image initialization ############################//
	bg_surface = IMG_Load( ("Assets/Images/Backgrounds/"+bg_image_name).c_str() );
	if(bg_surface == NULL) error_logger.push_error(SDL_GetError());
	bg_texture = SDL_CreateTextureFromSurface(renderer,bg_surface);
	if(bg_surface == NULL) error_logger.push_error(SDL_GetError());



	tile_bag.init();

	//give vertical scroll bar the addresses of the info it needs from the sdl_help object
	vert_bar.init(&x_scroll,&y_scroll, &window_s.width, &window_s.height, renderer,"v_ou_dark_green_quarter.png");
	//give horizontal scroll bar the address of the info it needs from the sdl_help object
	horiz_bar.init(&x_scroll,&y_scroll, &window_s.width, &window_s.height, renderer,"h_ou_grey_quarter.png");

	calc_corners(); //set up tile locations with the field's corner location 
	tile_bag.give_fields_renderer(renderer,image_p,&x_scroll,&y_scroll,font);//give fields rendering and font info

	io_handler.init();
	give_manager_io(&io_handler);
}

sdl_help::~sdl_help(){

	SDL_FreeSurface(bg_surface);
	SDL_DestroyTexture(bg_texture);//free up the memory from the background image


	SDL_DestroyRenderer(renderer);//stops memory leaks
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);//give back memory from the font pointer

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
	vert_bar.handle_resize();
	horiz_bar.handle_resize();
	tile_bag.update_win(width_in,height_in);
}
//prints area window size and display 
void sdl_help::print_size_info(){

	error_logger.push_msg("Printing window size: ");
	window_s.print();
	error_logger.push_msg("Printing display info: "+to_string(display.w)+"x"+to_string(display.h));
}

void sdl_help::give_manager_io(input_maker* input_maker_hook_in){
	tile_bag.set_input_maker_hook(input_maker_hook_in);
}

void sdl_help::present(){
	SDL_RenderPresent(renderer);
}
//draws all of the fields. It makes sure to draw field's help dialog boxes on top of themselves or other fields
void sdl_help::draw_tiles(){
	SDL_RenderClear(renderer);//clear off the renderer, to prepare to re-draw
	tile_bag.check_locks();
	//draw the background image to the screen
	SDL_RenderCopy(renderer,bg_texture,NULL,NULL);

	vector<field*> drawn_second;

	for(map<string,map<string,field>>::iterator lines_it = tile_bag.fields.begin();
	    lines_it != tile_bag.fields.end();
	    lines_it++){
		for(map<string,field>::iterator fields_it = lines_it->second.begin();
		    fields_it != lines_it->second.end();	
		    fields_it++){

			if( !fields_it->second.is_help_mode() ){ //don't draw it if it's in help mode, help mode tiles need to be drawn second
								 //so they aren't overdrawn by other tiles not in help mode
				fields_it->second.draw_me();//have the field draw itself to the screen

			} else {
				drawn_second.push_back(&fields_it->second);

			}
		}
	}


	for(unsigned int c = 0; c < drawn_second.size();c++){
		drawn_second[c]->draw_me();//now the help mode tiles can be drawn
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


	for(map<string,map<string,field>>::iterator lines_it = tile_bag.fields.begin();
	    lines_it != tile_bag.fields.end();
	    lines_it++){
		for(map<string,field>::iterator params_it = lines_it->second.begin();
		    params_it != lines_it->second.end();
		    params_it++){
			if(params_it->second.yloc + y_scroll < upmost){ //least y value means it is the highest corner
				upmost = params_it->second.yloc + y_scroll;	
			}

			if(params_it->second.yloc + y_scroll + params_it->second.get_size().height > downmost ){
				//the lowest point will be the lowest corner + that texture's height
				downmost = params_it->second.yloc + y_scroll + params_it->second.get_size().height;
			}

			if(params_it->second.xloc + x_scroll < leftmost){
				leftmost = params_it->second.xloc + x_scroll;
			}

			if(params_it->second.xloc + x_scroll + params_it->second.get_size().width > rightmost){
				rightmost = params_it->second.xloc + x_scroll + params_it->second.get_size().width;
			}

		}

	}

}

//can detect when we should stop scrolling, but allows no scrolling afterwards, not even in the opposite
//direction - fixed, but is there a better way?
void sdl_help::update_scroll(int x_scroll_in, int y_scroll_in){
	int rightmost = -2048, leftmost = 2048, upmost = 2048, downmost = -2048;
	most(rightmost,leftmost,upmost,downmost);

	if( (rightmost + x_scroll_in) <= 0){
		x_scroll = x_scroll + abs(0-rightmost);
		error_logger.push_msg("Hit right scrolling barrier.");
	}
	if( (leftmost + x_scroll_in) >= window_s.width){
		x_scroll = x_scroll - (/*window_s.width - leftmost*/ leftmost-window_s.width);
		error_logger.push_msg("Hit left scrolling barrier.");
	} //mid-statement commented regions are previous values, they were buggy so trying them switched
	  //seems to have fixed it, but commented regions stay until more strenuously tested
	if( (upmost + y_scroll_in) >= window_s.height){
		y_scroll = y_scroll - (/*window_s.height - upmost*/ upmost-window_s.height);
		error_logger.push_msg("Hit up scrolling barrier.");
	}
	if( (downmost + y_scroll_in) <= 0){
		y_scroll = y_scroll + abs(0-downmost);
		error_logger.push_msg("Hit down scrolling barrier.");
	}
	//it would make sense to be able to scroll like this 
		error_logger.push_msg("x_scroll increased by " + to_string(x_scroll_in) + "| " + to_string(x_scroll)
				       + "-> " + to_string(x_scroll + x_scroll_in));
		error_logger.push_msg("y_scroll increased by " + to_string(y_scroll_in) + "| " + to_string(y_scroll)
				      + "-> " + to_string(y_scroll+y_scroll_in));
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

int sdl_help::scroll_clicked(int click_x, int click_y) const{
	if(vert_bar.clicked(click_x,click_y)) return 1;//call vbar's click detection member
							    //1 means vertical bar was clicked
	if(horiz_bar.clicked(click_x,click_y)) return 2;//call hbar's click detection member
							     //2 means horizontal bar was clicked
	return 0;   //if we make it to this line return 'false' 0 (nothing was clicked)
		     //this returning false allows handle_mouseb_down to check the tiles
}
//********************************************************************************************/

void sdl_help::print_tile_locs(ostream& outs){

	for(map<string,map<string,field>>::iterator lines_it = tile_bag.fields.begin();
	    lines_it != tile_bag.fields.end();
	    lines_it++){
		for(map<string,field>::iterator params_it = lines_it->second.begin();
		    params_it != lines_it->second.end();
		    params_it++){
			params_it->second.print();
		}
	}

}

void sdl_help::click_detection(ostream& outs,SDL_Event& event,button_manager* b_manager, int click_x, int click_y){
	for(map<string,map<string,field>>::iterator lines_it = tile_bag.fields.begin();
	    lines_it != tile_bag.fields.end();
	    lines_it++){
		for(map<string,field>::iterator params_it = lines_it->second.begin();
		    params_it != lines_it->second.end();
		    params_it++){

			//if the mouse click coordinates fall within a tile
			if( in( click_x,click_y, params_it->second.get_rect() ) ){

				if(params_it->second.text_box_clicked(click_x,click_y) ){
					//if the click fell within the text box
					//go into text entry loop
					if(!params_it->second.is_locked){
						text_box_mini_loop(outs,event,b_manager,params_it->second);
					}
				} else {
					//if the click was not on the text box, enact clicked()
				 	params_it->second.clicked(event,click_x,click_y);
				}

			}
		}
	}
}

//thanks to http://lazyfoo.net/tutorials/SDL/32_text_input_and_clipboard_handling/index.php
//which was used as a reference 
void sdl_help::text_box_mini_loop(ostream& outs, SDL_Event& event,button_manager* b_manager,field& current_tile){

	SDL_StartTextInput();//turn on the text input background functions

	//used to control text entry loop
	bool done = false;
	//int c = 0;
	bool text_was_changed = false;

	while(!done){


		if( !SDL_PollEvent(&event) ){
			event.type = 1776; //dummy event to stop it from printing default message every frame
					   //where no event happens
		}

		switch(event.type){
		  case SDL_MOUSEMOTION:
			break;

		  case SDL_MOUSEBUTTONDOWN:
			//if the click was within the text box, move the cursor maybe
		  	if( current_tile.text_box_clicked(event.button.x,event.button.y) ){
				error_logger.push_msg("Text box click at " + to_string(event.button.x) + ":"
						       + to_string(event.button.y) );
		  	} else { //elsewise exit text input mode, user clicked off the text box
				SDL_PushEvent(&event);//doing this allows the user to 'hop' to another text box
						      //directly from editing another box
				done = true;
			}
		  	break;

		  case SDL_TEXTINPUT:
			current_tile.update_temp_input(event);
			text_was_changed = true;
		  	//here this actually causes a loss of letters, so the event flooding is necessary, don't flush
			//SDL_FlushEvent(SDL_TEXTINPUT);
			break;

		  case SDL_KEYDOWN:
			text_box_mini_loop_helper(event.key.keysym,current_tile,text_was_changed);

			SDL_FlushEvent(SDL_KEYDOWN); //prevent event flooding
		  	break;
		  case SDL_QUIT:
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



			//update picture
			draw_tiles();
			draw_sbars();
			b_manager->draw_buttons();
			current_tile.draw_cursor();
			//text_was_changed = false;
			//show updated picture
			present();

	}//end of loop
	SDL_StopTextInput();//stop text input functionality because it slows down the app

}

void sdl_help::text_box_mini_loop_helper(SDL_Keysym& key,field& current_tile,bool& text_was_changed){
	switch( key.sym ){
		case SDLK_BACKSPACE:
			//delete last character, unless it's empty already than do nothing
			if( current_tile.temp_input.size() > 0 ){
				current_tile.back_space();//delete a character, update text's graphics
				text_was_changed = true;
			}
			break;
	
		case SDLK_LEFT:
			//if we are not already at the very left of the text, move the editing position
			//one to the left
			if(current_tile.editing_location > 0){
				current_tile.editing_location--;
				text_was_changed = true;
			}
			break;
		case SDLK_RIGHT:
			//if we are not already at the very end of the text, move the editing position
			//one to the right
			if(current_tile.editing_location < current_tile.temp_input.size()){
				current_tile.editing_location++;
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
	error_logger.push_msg("################# IN CALC CORNERS ##########################################");
	//this variable keeps track of where the next line should start being placed. The helper function
	//should set it to be just below the newly created section of tiles, and some padding value
	unsigned int row_height = 5;//5 pixel buffer from top of window

	int row_limit;//this variable limits the width of the rows

	int widest_tile = tile_bag.get_widest_tile_width();
	error_logger.push_msg("WIDEST TILE: "+to_string(widest_tile));
	if(widest_tile > window_s.width){
		error_logger.push_msg("USING WIDEST TILE TO LIMIT ROWS");
		row_limit = widest_tile;//if a single tile is bigger than the window, use it for logic
					//because the window is likely so small the normal row placement logic won't work				
	} else {
		error_logger.push_msg("USING WINDOW SIZE TO LIMIT ROWS");
		row_limit = window_s.width;//else wise, fill up the window as best it can 
	}

	//just traversing the maps causes line10 to be placed before line 2, because map contents
	//are traversed alphabetically by the iterator, so this won't work well

	//for(map<string,map<string,field>>::iterator line_it = tile_bag.fields.begin();
	    //line_it != tile_bag.fields.end();
	    //line_it++){

		//calc_corners_helper(line_it->first,line_it->second,row_height,row_limit);

	//}	

	//but, I saved the line names as they were read in my manager::init(), so we can just walk that
	//vector and ensure that lines are placed in the same order in which they were read
	for(unsigned int c = 0; c < tile_bag.line_order.size();c++){
		if(tile_bag.line_order[c] == "line_6"){
			//create a vector with the row parameter names in the order in which they should appear
			vector<string> line_6_order = {"ICNTRL1","ICNTRL2","ICNTRL3","ICNTRL4","ICNTRL5","ICNTRL6","ICNTRL7",
						       "ICNTRL8","ICNTRL9","ICNTRL10"};
			//and pass it into the alternative calc_corners helper function
			calc_corners_ordered(tile_bag.line_order[c],tile_bag.fields.at(tile_bag.line_order[c]),
						    row_height, row_limit,line_6_order);
		} else {
			//this is the standard calc_corners helper function
			calc_corners_helper(tile_bag.line_order[c],tile_bag.fields.at(tile_bag.line_order[c]),
				            row_height, row_limit);

		}
	}



	error_logger.push_msg("################# END CALC CORNERS ############################################");

}
void sdl_help::calc_corners_helper(const string line_in, map<std::string,field>& map_in, unsigned int& start_height,
				   int row_limit){
	error_logger.push_msg("In calc_corners_helper()! Line in progress is:" + line_in);



	int x_buffer = 5;//distance between the left edge and the tiles, and the distance between two tiles 
			  //horizontally

	int x_corner = x_buffer;

	int y_corner = start_height;
				    //save the starting position to be used to set the corner coordinates for fields
				    //but this variable can be changed if the current line map takes up more than
				    // one row in the window


	 //save the lowest ylocation + height value there is, so we can update start_height
	//when this function is done, to allow other rows to know where to begin placing fields
	int lowest_point = start_height;

	for(map<string,field>::iterator param_it = map_in.begin(); param_it != map_in.end();param_it++){
		error_logger.push_msg("PARAM:"+param_it->first);

		//this is the case where the tile can stay in the current row 
		if(x_corner + param_it->second.get_size().width < row_limit){

			param_it->second.xloc = x_corner;
			param_it->second.yloc = y_corner;


			x_corner = param_it->second.xloc + param_it->second.get_size().width + x_buffer;//literal 5 for 5 pixel offset

			

			if(param_it->second.yloc + param_it->second.get_size().height + 5 > lowest_point){
				error_logger.push_msg("OLD lowest_point:"+to_string(lowest_point));
				lowest_point = param_it->second.yloc + param_it->second.get_size().height + 5;
				error_logger.push_msg(" NEW lowest_point:"+to_string(lowest_point));
			}
		//this is the case where the tile needs to be placed into a new row (because there's not enough width left)
		} else {
			param_it->second.xloc = x_buffer;//place it on the left edge
			x_corner = param_it->second.xloc + param_it->second.get_size().width + x_buffer;
										      //save it's leftmost edge + padding
										      //to be used to place the next tile

			param_it->second.yloc = lowest_point; //place it just below the previous row

			y_corner = lowest_point; //set that lowest point as the new y coordinate for the
						 //fields in this row

			lowest_point = param_it->second.yloc + param_it->second.get_size().height + 5;//save new lowest point

		}		




	}

	start_height = lowest_point + 5;//save the the start location for the next row

}

//unfortunately, the map places "ICNTRL10" just after ICNTRL1, because they are stored in alphabetical order and not
//the order in which they were pushed into the map. So this function is used instead of the normal calc corners helper,
//to make sure that they are in logical order
void sdl_help::calc_corners_ordered(const string line_in,map<string,field>& map_in, unsigned int& start_height,int row_limit,
				    vector<string>& order){

	int lowest_point = start_height;//temp copy of the starting height
	int x_buffer = 5; //distance between tiles and the left edge
	int x_corner = x_buffer;//keep track of where the next tile should begin
	int y_corner = start_height;//keep track of there the tile's corner should be vertically


	for(vector<string>::iterator it = order.begin(); it != order.end();it++){

		//this is the case where the tile can stay in the current row 
		if(x_corner + map_in.at(*it).get_size().width < row_limit){

			map_in.at(*it).xloc = x_corner;
			map_in.at(*it).yloc = y_corner;


			x_corner = map_in.at(*it).xloc + map_in.at(*it).get_size().width + x_buffer;//literal 5 for 5 pixel offset

			

			if(map_in.at(*it).yloc + map_in.at(*it).get_size().height + 5 > lowest_point){
				error_logger.push_msg("OLD lowest_point:"+to_string(lowest_point));
				lowest_point = map_in.at(*it).yloc + map_in.at(*it).get_size().height + 5;
				error_logger.push_msg(" NEW lowest_point:"+to_string(lowest_point));
			}
		//this is the case where the tile needs to be placed into a new row (because there's not enough width left)
		} else {
			map_in.at(*it).xloc = x_buffer;//place it on the left edge
			x_corner = map_in.at(*it).xloc + map_in.at(*it).get_size().width + x_buffer;
										      //save it's leftmost edge + padding
										      //to be used to place the next tile

			map_in.at(*it).yloc = lowest_point; //place it just below the previous row

			y_corner = lowest_point; //set that lowest point as the new y coordinate for the
						 //fields in this row

			lowest_point = map_in.at(*it).yloc + map_in.at(*it).get_size().height + 5;//save new lowest point

		}		






	}



	start_height = lowest_point + 5;//save the start location for the next row
}



//############################ NON-MEMBER HELPERS ##########################################################

bool compare_width(names_and_width& left, names_and_width& right){
	if(left.width < right.width) return true;
	return false;
}














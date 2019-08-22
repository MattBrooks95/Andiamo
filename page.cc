#include "page.h"

#include "sdl_help.h"

using namespace std;

page::page(){
	num_columns = 0;
	num_rows = 0;
}

page::page(const page& other){
	num_columns = other.num_columns;
	num_rows = other.num_rows;
	column_labels = other.column_labels;
	row_labels = other.row_labels;
}


page::~page(){
	if(column_label_textures.size() == 0) return;

	for(unsigned int c = (column_label_textures.size() - 1) ; c > 0;c--){
		if(column_label_textures[c] != NULL){
			SDL_DestroyTexture(column_label_textures[c]);
			column_label_textures.pop_back();
		}
	}
}

void page::page_init(unsigned int num_columns_in, unsigned int rows_needed,
						const vector<string>& column_labels_in,
						const vector<string>& row_labels_in,
						const vector<int>& column_spacings){
	//abstracts some of the assignments to save space here
	init_local_var(num_columns_in, rows_needed, column_labels_in, row_labels_in);

	//turn true to make room for row labels
	bool row_labels_exist = false;

	//used to control where the text boxes start to be drawn
	int x_start_point = 0;


	if(num_columns_in != column_spacings.size()){
		logger_access->push_msg("Not been enough spacing values supplied to page init.");
		logger_access->push_msg("Column_spacings should have one less element than num_columns_in.");
		logger_access->push_msg("This could be by design, as is the case with ICNTRL6's parity form.");
	}


	//implement logics for setting row labels here
	//how does this affect the placing of the actual text boxes?
	if(row_labels_in.size() != 0){
		set_row_labels(row_labels_in,x_start_point);
		row_labels_exist = true;
	}


	set_text_boxes(x_start_point,column_spacings,row_labels_exist);

	init_column_labels(column_spacings,x_start_point,row_labels_exist);
}

void init_from_config(unsigned int num_columns_in, unsigned int rows_needed,
					  const vector<string>& column_labels_in,
					  const vector<string>& row_labels_in,
					  const vector<int>& column_spacings,
					  const vector<string>& init_values){
}

void page::init_local_var(uint num_columns_in, uint rows_needed,
						const vector<string>& column_labels_in,
						const vector<string>& row_labels_in){
	num_columns   = num_columns_in;
	num_rows      = rows_needed;
	column_labels = column_labels_in;
	row_labels    = row_labels_in;
}

void page::set_row_labels(const vector<string>& row_labels_in,
							int& x_start_point){
	SDL_Color black = {0,0,0,0};
	SDL_Surface* temp_surface = NULL;
	SDL_Texture* temp_texture = NULL;


	for(unsigned int c = 0; c < row_labels_in.size(); c++){

		//shove in null pointer
		row_label_textures.push_back(temp_texture);

		//create surface from the text
		temp_surface =
			TTF_RenderUTF8_Blended(sdl_access->font,row_labels[c].c_str(),black);

		int width;
		int height;


		if(TTF_SizeText(sdl_access->font,row_labels[c].c_str(),&width,&height) != 0){
			string err = "Error while making row labels in page,";
			err += " TTF_SizeText failure." + string(TTF_GetError());
			logger_access->push_error(err);
		}


			//make the previously shoved null pointer now point
			//to a texture created from the surface
			row_label_textures.back() =
				SDL_CreateTextureFromSurface(sdl_access->renderer,temp_surface);

			//calc location & use the size
			//explicit cast of c to int stops compiler warnings
			SDL_Rect temp_rect =
				 {TEXT_BOX_HORIZ_PADDING,80+25*int(c)+10*int(c), width, height};

			if(width > x_start_point){
				//update the starting point tracker
				x_start_point = width;
			}

			//push that info into the row label vector
			row_label_rects.push_back(temp_rect);
		}
}

void page::set_text_boxes(int& x_start_point,
							const vector<int>& column_spacings,
							bool& row_labels_exist){

	//give it some wiggle room
	if(x_start_point != 0) x_start_point += 10;

	//this variable declared here, because if row labels exist, the inner loop
	//should start at 1 instead of at 0
	unsigned int j = 0;
	if(row_labels_exist){
		j = 1;
	}

	//had a hard fight with the Rule of Three here....
	//implementing a copy constructor stopped the double free()
	// crash at the text_boxes.push_back
	int x_offset = 0;
	for(unsigned int i = 0; i < num_rows; i++){
		x_offset = 0;
		for( /*previously defined*/; j < num_columns; j++){
			text_box new_text_box;
			int x_val = x_start_point + x_offset + column_spacings[j];
			x_offset += column_spacings[j];
			int y_val = 80 + 25 * i + 10 * i;
			new_text_box.init(sdl_access->font,"",x_val,y_val,60,25);
			text_boxes.push_back(new_text_box);
		}
		//reset the j variable, because the for loop will no longer do it
		//since it was declared at the start of the function and not in the loop
		if(row_labels_exist) j = 1;
		else j = 0;
	}

	//but, I think another solution could have been to tell the vector
	//make 'x' text_boxes, and then init the copies that exist inside the vector
}

void page::init_column_labels(const vector<int>& column_spacings,
						 int& x_start_point,bool& row_labels_exist){


	//shorter reference to font
	TTF_Font* font = sdl_access->font;

	int x_offset = 0;
	//set up column labels
	SDL_Color black = {0,0,0,0};
	SDL_Surface* temp_surf = NULL;
	SDL_Texture* temp_texture = NULL;
	for(unsigned int c = 0; c < column_labels.size();c++){
		//shove in a null pointer
		column_label_textures.push_back(temp_texture);

		//render the text
		temp_surf = TTF_RenderUTF8_Blended(font,column_labels[c].c_str(),black);

		//make the pointer we shoved point at the desired texture
		column_label_textures.back() =
			SDL_CreateTextureFromSurface(sdl_access->renderer,temp_surf);

		//calculate drawing info for the column label
		SDL_Rect temp_rect = {x_start_point+x_offset+column_spacings[c],50,0,0};
		x_offset += column_spacings[c];

		//size the text and shove it into the vector
		TTF_SizeText(font,column_labels[c].c_str(),&temp_rect.w,&temp_rect.h);

		//shove it into the drawing info vector
		column_label_rects.push_back(temp_rect);

		//give memory back
		SDL_FreeSurface(temp_surf);
		//reset surf pointer for next item
		temp_surf = NULL;
	}

}

void page::draw_me(){
	SDL_Renderer* renderer = sdl_access->renderer;

	if( column_label_textures.size() != column_label_rects.size() ){
		string err = "The vector that saves drawing location for page";
		err += " column labels does not have the same size as";
		err += " the vector that contains the textures. Aborting drawing.";
		logger_access->push_error(err);
		return;
	}

	for(unsigned int c = 0; c < column_label_textures.size();c++){

		if(SDL_RenderCopy(renderer,column_label_textures[c],NULL,&column_label_rects[c]) != 0){
			logger_access->push_error("Could not draw column title.");
		}
	}

	if( row_label_textures.size() != row_label_rects.size() ){
		string err = "The vector that saves the drawing location for";
		err += " the page row labels does not have the same size";
		err += " as the vector that contains the textures. Aborting drawing.";
		logger_access->push_error(err);
	return;
	}

	for(unsigned int c = 0; c < row_label_textures.size();c++){

		if( SDL_RenderCopy(renderer,row_label_textures[c],NULL,&row_label_rects[c]) != 0 ){

		}

	}

	for(unsigned int c = 0; c < text_boxes.size();c++){
		text_boxes[c].draw_me();
	}
}

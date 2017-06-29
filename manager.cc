//! \file manager.cc implements the functions declared in manager.h
#include "manager.h"

#include<fstream>
#include<iostream>
#include<regex>
#include<algorithm> //for sorting the tiles based on width
using namespace std;

bool compare_width(field& left, field& right);//prototype for sorting function passed to algorithm::sort

manager::manager(){
	input_maker_hook = NULL; //should be overwritten when sdl_help's constructor calls give_manager_io
}

bool man_test = true;

void manager::init(){
	
	tile_input_p = "tile_Input/";
	fstream ins; //ins will refer to the stream to the input file for tile information

	ins.open((tile_input_p + "tiles.txt").c_str()); //open the file
	if(ins.fail()){ //if something went wrong, print an error message to console
 		cout << "Fatal error: Failed to open tile input file: " << tile_input_p + "tiles.txt";
	}
	

	string bg_name = "bad bg name"; //name of the bg tile that the user will see
	string bg_img = "bad bg image"; //name of the bg image file that will be loaded by sdl

	//ins >> bg_name;//read in background name
	//ins >> bg_img;//read in background image name
	getline(ins,bg_name);
	getline(ins,bg_img);




	field bg(bg_name,bg_img,0,0); //create a 'special' background tile that should always
					      //be the first one specified in the text file
	new_tile(bg);// pushes background tile into the vector

	string temp_string;//used to store unidentified line
	//ins >> temp_string;//read the first line pre-loop
	getline(ins,temp_string);
	if(temp_string != "andy"){ //background tile's info should have been ended with an 'andy' line
		cout << "Missing \"andy\" separator after background tile information." << endl;
	} //print an error if it was not there
	else {
		//ins >> temp_string;
		getline(ins,temp_string);//if it was formatted properly, read the next line which should
					 // be meaningful
	}

	regex img_pattern(".*\\.png"); //this line specifies an image name
	regex name_pattern("\\D[a-z0-9_A-Z]?+"); //this line specifies a tile name
	regex int_pattern("[0-9]+"); //these were causing an error b/c gcc 4.8 can't
				     //handle the [], I'm updating fedora + gcc 
				     //tomorrow 5/9 to resolve this issue

	regex desc_pattern("\\s*?c\\s+?.*");
		//describes a pattern for tile/input descriptors that starts with a 'c'
		//and is followed by any number of spaces, then contains any character




	while(!ins.eof()){
		if(ins.fail()) break;//get out on potentially erroneous last run

		string tile_name = "bad tile name";//names for generalized tiles
		string img_name = "bad image name";//names for tile's picture file
		string description; //description for this input tile
		vector<string> temp_descriptions;//save all lines for description of tile


		int tile_w = -1, tile_h = -1;//dimensions of given image for tile, -1 is bad input

		while(temp_string != "andy"){ //loop until separator 'andy' is found
			if(man_test) cout << "LINE:" << temp_string << "|" << endl;


			if( regex_match(temp_string,img_pattern) ){ //if this line has '.png' in it, 
								    //process it as an input picture name
				if(man_test) cout << "Found an image name!: " << temp_string << endl;
				img_name = temp_string;

			} else if( regex_match(temp_string,desc_pattern)){
				if(man_test) cout << "Found a description line.: " << temp_string << endl;
				description = temp_string.erase(0,2);//remove 'c ' at start of desc lines
				temp_descriptions.push_back(temp_string);

			} else if( regex_match(temp_string,name_pattern) ){
				if(man_test) cout << "Found a tile name!: " << temp_string << endl;
				tile_name = temp_string;

			} else if( tile_w == -1 && regex_match(temp_string,int_pattern) ){
				tile_w = stoi(temp_string,NULL,10);
				if(man_test) cout << "Found tile width!: " << tile_w << endl;

			} else if( tile_w != -1 && regex_match(temp_string,int_pattern) ){
				tile_h = stoi(temp_string,NULL,10);
				if(man_test) cout << "Found tile height!: " << tile_h << endl;
			} else {
				cout << "Error! This line failed to hit a case in the regex checks." << endl;
				cout << "It may be a missing 'Andy' separator in the tiles.txt config file." << endl;
				return;
				
			}

			getline(ins,temp_string);//read in again to update loop
			if(temp_string == "") getline(ins,temp_string);
		}
		//one tile will be filled and pushed into tile_bag vector per inner loop completion
                field temp_field(tile_name, img_name,tile_w,tile_h);
		for(unsigned int c = 0; c < temp_descriptions.size();c++){
			temp_field.descriptions.push_back(temp_descriptions[c]);
		}
		if(man_test){
			cout << "PUSHING THIS FIELD-----------------------" << endl;
			temp_field.print(cout);
		}
		new_tile(temp_field);
		ins >> temp_string;

	}
	ins.close(); //close the file
}

manager::~manager(){


}

void manager::set_area(int& sdl_max_width, int& sdl_max_height){
	unsigned int rightmost = 0;
	unsigned int downmost = 0;

	unsigned int temp_rightmost; //these integers prevent us from calcing the edges in the boolean if()
	unsigned int temp_downmost;  //and in the assignment

	//note, starting at 1 here so actual size logic doesn't account for the 2048+,2048+ background tile
	for(unsigned int c = 1; c < tiles.size(); c++){
		temp_rightmost = tiles[c].get_size().width + tiles[c].xloc; //calc right edge of tile
		temp_downmost = tiles[c].get_size().height + tiles[c].yloc; //calc bottom edge of tile

		if(temp_rightmost > rightmost){
			rightmost = temp_rightmost; //save newfound rightmost maximum
		}
		if(temp_downmost > downmost){
			downmost = temp_downmost; //save newfound bottommost maximum
		}
	}
	//all tiles have now been considered, save the maximums to sdl_helpers area struct
	sdl_max_width = rightmost;
	sdl_max_height = downmost;
}

void manager::set_input_maker_hook(input_maker* input_maker_hook_in){
	//seems to be working
	//cout <<"SETTING INPUT MAKER HOOK!" << endl;
	//cout <<"BEFORE: " << input_maker_hook << endl;
	//cout <<"PASSED HOOK: " << input_maker_hook_in << endl;
	input_maker_hook = input_maker_hook_in;
	//cout <<"AFTER: " << input_maker_hook_in << endl;
	give_fields_defaults();
}

void manager::give_fields_renderer(SDL_Renderer* sdl_help_renderer_in,string image_p_in,
				   int* xscroll_in, int* yscroll_in,TTF_Font* font_in){
	for(unsigned int c = 0; c < tiles.size();c++){
		tiles[c].graphics_init(sdl_help_renderer_in,image_p_in,xscroll_in,yscroll_in,font_in);
	}

}

void manager::give_fields_defaults(){
	//starting at 1 to skip over background tile


	bool found = false; //turn to true when we find the correct tile and set its default values and
			    //input maker hook. This way it doesn't bother continue searching


	//loop over every name in the name vector
	//and search all the parameter vectors for a parameter with the same name
	for(unsigned int c = 1; c < tiles.size();c++){
		found = false; //reset the found boolean per run of the outer loop

		//check the int4 params vector
		for(unsigned int i = 0; i < input_maker_hook->get_int4_params().size() && !found;i++){

			if(input_maker_hook->get_int4_params()[i].name == tiles[c].tile_name){
				tiles[c].int4_hook = &(input_maker_hook->get_int4_params()[i]);
				tiles[c].temp_input = to_string(input_maker_hook->get_int4_params()[i].value);
				cout << "Setting " << tiles[c].tile_name << "'s string field to "
				     << to_string(input_maker_hook->get_int4_params()[i].value) << endl;
				cout << "And its value has been set to: " << tiles[c].temp_input << endl;

				tiles[c].text_box_init();//now that input_maker hook is set, create surfaces and textures
				found = true;//found the tile, don't let the other sub loops run
				break;
			}
		}//inner for 1

		//check the real8_params vector
		for(unsigned int i = 0;i < input_maker_hook->get_real8_params().size() && !found;i++){

			if(input_maker_hook->get_real8_params()[i].name == tiles[c].tile_name){
				tiles[c].real8_hook = &(input_maker_hook->get_real8_params()[i]);
				tiles[c].temp_input = to_string(input_maker_hook->get_real8_params()[i].value);

				tiles[c].text_box_init();//now that input_maker hook is set, create surfaces and textures
				found = true;//found the tile, don't let the other sub loops run
				break;
			}
		}//inner for 2

		//check the string_params vector
		for(unsigned int i = 0;i < input_maker_hook->get_string_params().size() && !found;i++){

			cout << " PARAM VEC SIZE: " << input_maker_hook->get_string_params().size() << endl;
			if(input_maker_hook->get_string_params()[i].name == tiles[c].tile_name){
				cout << "STRING HOOK: " << &(input_maker_hook->get_string_params()[i]) << endl;
				tiles[c].string_hook = &(input_maker_hook->get_string_params()[i]);
				cout << tiles[c].string_hook << endl;
				tiles[c].temp_input = tiles[c].string_hook->value;
				cout << " STRING HOOK VALUE: " << input_maker_hook->get_string_params()[i].value << endl;
				tiles[c].text_box_init();//now that input_maker hook is set, create surfaces and textures
				cout << "SET STRING HOOK: " << tiles[c].string_hook << " = " << tiles[c].string_hook->value << endl;
				found = true;
				break;
			}
		}//inner for 3
		
		if(!found){
			//check the i4_array_params vector
			try {
				input_maker_hook->get_i4_array_params().at(tiles[c].tile_name);
				//set up the fields other values
			} catch (out_of_range& not_found){

				cout << "Tile name:" << tiles[c].tile_name << " not found in any input_maker vector.\n"
				     << "Check that names of parameters in HF_Config match names in tile_Input/tiles.txt"
				     << " exactly." << endl;
				cout << "Map is as follows:" << endl;
				map<string,param_int4_array>::iterator my_iterator;
				for(my_iterator = input_maker_hook->get_i4_array_params().begin();
				    my_iterator != input_maker_hook->get_i4_array_params().end();
				    my_iterator++){
					cout << "Key: " << my_iterator->first << "Name: "
					     << my_iterator->second.name << endl;

				}
			}
		}
	}//big for

}

void manager::update_io_maker(){
	for(unsigned int c = 1; c < tiles.size();c++){
		tiles[c].update_my_value();
	}

}

void manager::new_tile(field temp){
	tiles.push_back(temp);
}

void manager::update_win(int width_in,int height_in){
	win_w = width_in;
	win_h = height_in;
}


void manager::print_all(ostream& outs){
        cout << "\n Printing tile vector ########################################################" << endl;
	for(unsigned int c = 0; c < tiles.size();c++){
		tiles[c].print(outs);
	}
	cout << "###############################################################################\n"
	     << endl;
}










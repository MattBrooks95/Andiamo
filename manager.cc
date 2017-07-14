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


	string temp_string;//used to store unidentified line

	getline(ins,temp_string);//if it was formatted properly, read the next line which should
					 // be meaningful

	regex img_pattern("\\s*?.*\\.png\\s*?"); //this line specifies an image name
	regex field_size_pattern("\\s*?[0-9]+?\\s*?x\\s*?[0-9]+?\\s*?");//this recognizes lines that specify tile size
					 //lines should be of the form widthxheight EX:100x100

	regex name_pattern("\\s*?[a-z0-9_A-Z]?+\\s*?"); //this line specifies a tile name

	regex desc_pattern("c .*");
		//describes a pattern for tile/input descriptors that starts with a 'c'
		//and is followed by exactly one space, then contains any number of any characters

	regex line_separator("\\line_[0-9]+?[A-Z]?\\.*"); //this line recognizes the lines that separate
		//the parameters into lines that correspond with the input manual,
		//so they can be stored together and easily (and readably) printed to the HF file later

	/*  This map of maps needs to be filled by this subroutine
	std::map<std::string,std::map<std::string,field>> fields;//!< trying something new, to keep relevant tiles together
	*/

	//loop over the entire tile_Input/tiles.txt configuration file
	while(!ins.eof()){
		if(ins.fail()) break;//get out on potentially erroneous last run

		//reset new line container each run of loop
		map<string,field> new_line;
		string line_name;
		getline(ins,line_name);
		while( !ins.eof() && !regex_match(line_name,line_separator) ){ //read until a line start indicator/separator is found
			if( ins.fail() ){
				//we may not necessarily find this case in error, as the very last group in the config file won't
				//end with a line separator, because no line will come after it
				return;//ran out of file, get out of this function
			}
			getline(ins,line_name);
		}
		//once we've reached this point, we have found a line_name


		//outer loop runs over the # of grouped parameters (lines in HF input)
		while( !regex_match(temp_string,line_separator) ){

			//these parameter should be re-declared for each field
			string tile_name = "bad tile name";//names for generalized tiles
			string img_name = "bad image name";//names for tile's picture file
			string description; //description for this input tile
			vector<string> temp_descriptions;//save all lines for description of tile


			int tile_w = -1, tile_h = -1;//dimensions of given image for tile, -1 is bad input


			//inner loop runs name -> andy as many times as needed, until a line separator is found
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

				} else if( regex_match(temp_string,field_size_pattern) ){

					if(man_test) cout << "Found field size specification!: " << temp_string << endl;
					strip_char(temp_string,' '); //remove spaces
					vector<string> dimensions = split(temp_string,'x');   //split into a vector of strings
					tile_w = stoi(dimensions[0]); //first number in the line is the width
					tile_h = stoi(dimensions[1]); //second number in the line is the width

				} else {
					cout << "Error! This line failed to hit a case in the regex checks." << endl;
					cout << "It may be a missing 'Andy' separator in the tiles.txt config file." << endl;
					return;
				
				}

				getline(ins,temp_string);//read in again to update loop
				if(temp_string == "") getline(ins,temp_string);//skip empty lines
			}
			//field(std::string tile_name_in,std::string image_name_in, int width, int height);
			field temp_field(tile_name,img_name,tile_w,tile_h);
			new_line.emplace(line_name,temp_field);//push the field into the map for that parameter's line


		}
		//at this point, we have hit the separator for another group of parameters
		fields.emplace(line_name,new_line);//store the map of parameters in the map of lines, and give it the name we found earlier
	}
	ins.close(); //close the file
}

manager::~manager(){


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
		if(!found){
			for(unsigned int i = 0;i < input_maker_hook->get_real8_params().size() && !found;i++){

				if(input_maker_hook->get_real8_params()[i].name == tiles[c].tile_name){
					tiles[c].real8_hook = &(input_maker_hook->get_real8_params()[i]);
					tiles[c].temp_input = to_string(input_maker_hook->get_real8_params()[i].value);

					tiles[c].text_box_init();//now that input_maker hook is set, create surfaces and textures
					found = true;//found the tile, don't let the other sub loops run
					break;
				}
			}//inner for 2
		}
		if(!found){
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
		}
		if(!found){
			//check the i4_array_params vector
			try {
				//input_maker_hook->get_i4_array_params().at(tiles[c].tile_name);

				//following code is only ran if the map searching was successful
				//set up the field's other values
				tiles[c].int4_array_hook = &input_maker_hook->get_i4_array_params().at(tiles[c].tile_name);
				//cout << "NENT or LMAX loc:" << &input_maker_hook->get_i4_array_params().at(tiles[c].tile_name);
				//cout << "THAT TILE'S POINTER:" << tiles[c].int4_array_hook << endl;

				tiles[c].temp_input = tiles[c].int4_array_hook->get_string();				

				tiles[c].text_box_init(); //set up the text box surfaces and textures

			} catch (out_of_range& not_found){

				cout << "Tile name:" << tiles[c].tile_name << " not found in any input_maker vector.\n"
				     << "Check that names of parameters in HF_Config match names in tile_Input/tiles.txt"
				     << " exactly." << endl;
				cout << "Map is as follows:" << endl;
				map<string,param_int4_array>::iterator my_iterator;
				for(my_iterator = input_maker_hook->get_i4_array_params().begin();
				    my_iterator != input_maker_hook->get_i4_array_params().end();
				    my_iterator++){
					cout << "Key: " << my_iterator->first << " Name: "
					     << my_iterator->second.name << endl;
				} //testing for loop
			}
		}

		//check e_params map 
		if(!found){
			try{
				tiles[c].e_array_hook = &input_maker_hook->get_e_params().at(tiles[c].tile_name);
				tiles[c].temp_input = tiles[c].e_array_hook->get_string();
				tiles[c].text_box_init(); //set up text box's surfaces and textures

			} catch (out_of_range& not_found){
				cout << "Tile name:" << tiles[c].tile_name << " not found in any input_maker vector.\n"
				     << "Check that names of parameters in HF_Config match names in tile_Input/tiles.txt"
				     << "exactly." << endl;
				cout << "Map is as follows:" << endl;
				map<string,param_e_array>::iterator my_iterator;
				for(my_iterator = input_maker_hook->get_e_params().begin();
				    my_iterator != input_maker_hook->get_e_params().end();
				    my_iterator++){
				    cout << "Key: " << my_iterator->first << " Name: "
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

void manager::new_line(const string& line_name,const map<string,field>& line_map){
		//std::map<std::string,std::map<std::string,field>> fields;//!< trying something new, to keep relevant tiles together
		fields.emplace(line_name,line_map);
}

void manager::update_win(int width_in,int height_in){
	win_w = width_in;
	win_h = height_in;
}


void manager::print_all(ostream& outs){
        cout << "\n Printing line map ########################################################" << endl;

	//loop over every line map
	for(map<string,map<string,field>>::iterator big_it = fields.begin(); big_it != fields.end() ;big_it++){
		//loop over every parameter map
		for(map<string,field>::iterator small_it = fields.at(big_it->first).begin(); small_it != fields.at(big_it->first).end(); small_it++){
			small_it->second.print(outs);
		}

	} 

	cout << "###############################################################################\n"
	     << endl;
}










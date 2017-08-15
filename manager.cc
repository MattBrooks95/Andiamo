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

bool man_test = false;

void manager::init(){
	
	tile_input_p = "tile_Input/";
	fstream ins; //ins will refer to the stream to the input file for tile information

	ins.open((tile_input_p + "tiles.txt").c_str()); //open the file
	if(ins.fail()){ //if something went wrong, print an error message to console
 		error_logger.push_error("Fatal error: Failed to open tile input file: "+tile_input_p+"tiles.txt");
	}

	string temp_string;//used to store unidentified line

	regex img_pattern("\\s*?.*\\.png\\s*?"); //this line specifies an image name
	regex field_size_pattern("\\s*?[0-9]+?\\s*?x\\s*?[0-9]+?\\s*?");//this recognizes lines that specify tile size
					 //lines should be of the form widthxheight EX:100x100

	regex name_pattern("\\s*?([a-z0-9_A-Z]+?):?(.*)?\\s*?"); //this line specifies a tile name
	regex semi_pattern(":");//used to tell if the name line is of the form ->   HFvariable:EnglishVariable
	regex desc_pattern("c .*");
		//describes a pattern for tile/input descriptors that starts with a 'c'
		//and is followed by exactly one space, then contains any number of any characters

	regex line_separator("\\s*?line_[0-9]+?[A-Z]?.*"); //this line recognizes the lines that separate
		//the parameters into lines that correspond with the input manual,
		//so they can be stored together and easily (and readably) printed to the HF file later

	/*  This map of maps needs to be filled by this subroutine
	std::map<std::string,std::map<std::string,field>> fields;//!< trying something new, to keep relevant tiles together
	*/
	getline(ins,temp_string);//priming read
	//loop over the entire tile_Input/tiles.txt configuration file
	while(!ins.eof() ){
		if(ins.fail()) break;//get out on potentially erroneous last run

		//reset new line container each run of loop
		map<string,field> new_line;
		string line_name;

		while( !ins.eof() && !regex_match(temp_string,line_separator) ){ //read until a line start indicator/separator is found
			if( ins.fail() || temp_string.empty() ){
				//we may not necessarily find this case in error, as the very last group in the config file won't
				//end with a line separator, because no line will come after it
				return;//ran out of file, get out of this function
			}
			getline(ins,temp_string);
		}

		error_logger.push_msg("Found a line name:"+temp_string);
		strip_char(temp_string,'#');
		line_name = temp_string;//save line name for later
		getline(ins,temp_string);//grab a new line

		//outer loop runs over the # of grouped parameters (lines in HF input)
		while( !regex_match(temp_string,line_separator) && !ins.eof()){

			//these parameter should be re-declared for each field
			string tile_name = "bad tile name";//names for generalized tiles
			string display_name = "no display name";//display name for tiles, may or may not be used
			string img_name = "bad image name";//names for tile's picture file
			string description; //description for this input tile
			vector<string> temp_descriptions;//save all lines for description of tile


			int tile_w = -1, tile_h = -1;//dimensions of given image for tile, -1 is bad input


			//inner loop runs name -> andy as many times as needed, until a line separator is found
			while(temp_string != "andy" && !ins.fail()){ //loop until separator 'andy' is found
				error_logger.push_msg("LINE:"+temp_string+"|");


				if( regex_match(temp_string,img_pattern) ){ //if this line has '.png' in it, 
									    //process it as an input picture name
					error_logger.push_msg("Found an image name!: "+temp_string);
					img_name = temp_string;

				} else if( regex_match(temp_string,desc_pattern)){

					error_logger.push_msg("Found a description line.: "+temp_string);
					description = temp_string.erase(0,2);//remove 'c ' at start of desc lines
					temp_descriptions.push_back(temp_string);


				} else if( regex_match(temp_string,field_size_pattern) ){

					error_logger.push_msg("Found field size specification!: "+temp_string);
					strip_char(temp_string,' '); //remove spaces
					vector<string> dimensions = split(temp_string,'x');   //split into a vector of strings
					try{
					  tile_w = stoi(dimensions[0]); //first number in the line is the width
					  tile_h = stoi(dimensions[1]); //second number in the line is the width
					} catch (invalid_argument& error){
						error_logger.push_error("Error in manager::init(), tile given illegal size parameters: "
									+dimensions[0]+"x"+dimensions[1]);
					}
				}  else if( regex_match(temp_string,name_pattern) ){
					 error_logger.push_msg("Found a tile name!: "+temp_string);
					if( regex_search(temp_string,semi_pattern) ){
						vector<string> tokens = split(temp_string,':');
						tile_name = tokens[0];
						display_name = tokens[1];
					} else {

						tile_name = temp_string;
						display_name = temp_string;
					}
				} else {
					error_logger.push_error("Error! This line failed to hit a case in the regex checks:"
								+temp_string+"\nIt may be a missing 'Andy' separator in the tiles.txt config file.");
				
				}


				if( !ins.fail() ){
					getline(ins,temp_string);//read in again to update loop
				} else {
					break;
				}
			}
			field temp_field(tile_name,display_name,img_name,tile_w,tile_h);
			//copy the saved description lines to the new field before it is placed in the map
			for(unsigned int c = 0; c < temp_descriptions.size();c++){
				temp_field.descriptions.push_back(temp_descriptions[c]);
			}

			error_logger.push_msg("##########PUSHING FIELD###################");
			temp_field.print();
			error_logger.push_msg("##########################################");

			new_line.emplace(tile_name,temp_field);//push the field into the map for that parameter's line
			if( !ins.fail() ){
				getline(ins,temp_string);//"andy" is the current line, so go ahead and read the next one
			}
		}

		//at this point, we have hit the separator for another group of parameters
		line_order.push_back(line_name);//save the order of the lines, so they can be drawn in order to the screen
		fields.emplace(line_name,new_line);//store the map of parameters in the map of lines, and give it the name we found earlier
	}
	ins.close(); //close the file

	if(man_test){
		error_logger.push_msg("FIELD MAP AFTER MANAGER.init():");
		print_all();
		error_logger.push_msg("####################################################");
	}
}

manager::~manager(){


}


void manager::set_input_maker_hook(input_maker* input_maker_hook_in){
	//seems to be working
	error_logger.push_msg("SETTING INPUT MAKER HOOK!");
	error_logger.push_msg("BEFORE: "+to_string(size_t(input_maker_hook)) );
	input_maker_hook = input_maker_hook_in;
	error_logger.push_msg("AFTER: "+to_string(size_t(input_maker_hook_in)) );
	give_fields_defaults();
}

void manager::give_fields_renderer(SDL_Renderer* sdl_help_renderer_in,string image_p_in,
				   int* xscroll_in, int* yscroll_in,TTF_Font* font_in){
	//loop over each map in the map
	for(map<string,map<string,field>>::iterator big_it = fields.begin();
	    big_it != fields.end();
	    big_it++){
	    for(map<string,field>::iterator small_it = big_it->second.begin();
		small_it != big_it->second.end();
		small_it++){
		small_it->second.graphics_init(sdl_help_renderer_in,image_p_in,xscroll_in,
					       yscroll_in,font_in);
	    }

	}
}

int manager::get_widest_tile_width(){
	int max_width = 0;
	for(map<string,map<string,field>>::iterator lines_it = fields.begin();
	    lines_it != fields.end();
	    lines_it++){
		for(map<string,field>::iterator params_it = lines_it->second.begin();
		    params_it != lines_it->second.end();
		    params_it++){
			int candidate_width = params_it->second.get_size().width;
			if(candidate_width > max_width){
				max_width = candidate_width; 
			}

		}

	}
	return max_width;
}
void manager::give_fields_defaults(){

	//call helper function for connecting the int4 params with their graphical tiles
	give_int4_fields_defaults();

	//call helper function for connecting real8 params with their graphical tiles
	give_real8_fields_defaults();

	//call helper function connecting int4 arrays to their graphical tiles
	give_int4_array_fields_defaults();

	//call helper function connecting string params to their graphical tiles
	give_string_fields_defaults();

	//call helper function connecting 'e' parameters to their tiles
	give_r8_array_fields_defaults();

}
//################ GIVE_FIELDS_DEFAULTS() HELPERS #######################################################//
void manager::give_int4_fields_defaults(){
	//this looks dense, but is simple
	//the outer loop just runs over every integer in the integer map that needs output to the HF file,
	//and then runs through each line's map in the manager::fields map looking for the correct parameter name for that int4
	//param. It's better to go through input_maker's parameters and find them in the fields map than it is to loop through
	//the fields map and find the parameters. This is because the second way would involve checking each map in input_maker,
	//when the first way just checks the 2d map in the manager object - these may actually be the same thing now that I think about it
	for(map<string,param_int4>::iterator big_it = input_maker_hook->get_int4_params().begin();
		big_it != input_maker_hook->get_int4_params().end();
		big_it++){
		bool found = false; //start off false, turn to true if the desired parameter is found
		for(map<string,map<string,field>>::iterator line_it = fields.begin();
			line_it != fields.end();
			line_it++){
		  //try to find the desired parameter in this line
		  try {

		    	line_it->second.at(big_it->first).int4_hook = &big_it->second; //let the field reference it's place in the
										       //input_maker map, so it can output the new
										       //given to it by the user, to the HF output
			//-1804 means that this parameter shouldn't have a default value, so set the text box's text
			//to a message instead
			if(big_it->second.value == -1804) {
				line_it->second.at(big_it->first).init_temp_input("no default");
			} else {
				line_it->second.at(big_it->first).init_temp_input(to_string(big_it->second.value));
			}


			line_it->second.at(big_it->first).text_box_init();//set up the text box
			found = true; //we found it, so make the flag good

			break;//leave this loop early, we don't need to check the other line's maps

		  //if it fails, need to check a different line's map in the manager object
		  } catch (out_of_range& not_found){

			continue;//if it isn't found, just check the next line's map
		  }


		}
		if(!found){
			error_logger.push_error("Error! Failed to find parameter:"+big_it->first+"'s tile in the fields map."
			     		      +"\nPlease make sure that its entries in tile_Input/tiles.txt and HF_Config/config.txt"
			     		      +"\n have matching names.");

		}


	}
}

void manager::give_int4_array_fields_defaults(){

	for(map<string,param_int4_array>::iterator big_it = input_maker_hook->get_i4_array_params().begin();
	    big_it != input_maker_hook->get_i4_array_params().end();
	    big_it++){
		bool found = false;
		for(map<string,map<string,field>>::iterator line_it = fields.begin();
		    line_it != fields.end();
		    line_it++){
		  try{
			line_it->second.at(big_it->first).int4_array_hook = &big_it->second;
								// &input_maker_hook->get_i4_array_params().at(big_it->first).values;
			line_it->second.at(big_it->first).init_temp_input(big_it->second.get_string());
			//line_it->second.at(big_it->first).temp_input = big_it->second.get_string(); //let it point to it's value in input_maker
			line_it->second.at(big_it->first).text_box_init(); //run the text box's init function

			found = true;
			break;
		  } catch (out_of_range& not_found){
			continue;//check other line maps
		  }

		}
		if(!found){
			error_logger.push_error("Error! Failed to find parameter:"+big_it->first+"'s tile in the fields map."
			     		      +"\nPlease make sure that its entries in tile_Input/tiles.txt and HF_Config/config.txt"
			     		      +"\n have matching names.");
		}
	}

}

void manager::give_real8_fields_defaults(){

	for(map<string,param_real8>::iterator big_it = input_maker_hook->get_real8_params().begin();
	    big_it != input_maker_hook->get_real8_params().end();
	    big_it++){

		bool found = false;
		for(map<string,map<string,field>>::iterator line_it = fields.begin();
		    line_it != fields.end();
		    line_it++){
		  try{
			line_it->second.at(big_it->first).real8_hook = &big_it->second;//give field pointer access
										       //to it's value in input_maker

			//-180.4 is the float version of the no applicable default flag, so set text box
			//to be a message instead 
			if(big_it->second.value == -180.4){
				line_it->second.at(big_it->first).init_temp_input("no default");
			} else {
				line_it->second.at(big_it->first).init_temp_input(to_string(big_it->second.value));
			}

			line_it->second.at(big_it->first).text_box_init();//set up the text box

			found = true;//set the flag to true, because we found the param we were looking for
			break;
		  } catch (out_of_range& not_found){

			continue;//it wasn't found, so check next line's map

		  }
	  	  if(!found){
			error_logger.push_error("Error! Failed to find parameter:"+big_it->first+"'s tile in the fields map."
			     		      +"\nPlease make sure that its entries in tile_Input/tiles.txt and HF_Config/config.txt"
			     		      +"\n have matching names.");

		  }

		}	

	}
}

void manager::give_string_fields_defaults(){

	for(map<string,param_string>::iterator big_it = input_maker_hook->get_string_params().begin();
	    big_it != input_maker_hook->get_string_params().end();
	    big_it++){
		bool found = false;
		for(map<string,map<string,field>>::iterator line_it = fields.begin();
		    line_it != fields.end();
		    line_it++){
		  try{
			line_it->second.at(big_it->first).string_hook = &big_it->second;  //set up the pointer to the parameter in input_maker
			line_it->second.at(big_it->first).init_temp_input(big_it->second.value);
			//line_it->second.at(big_it->first).temp_input =  big_it->second.value; //set up the default value
			line_it->second.at(big_it->first).text_box_init();//set up the text box
			found = true;
			break;
		  } catch (out_of_range& not_found){
			continue;//check the other lines
		  }
		}
		if(!found){
			error_logger.push_error("Error! Failed to find parameter:"+big_it->first+"'s tile in the fields map."
			     		      +"\nPlease make sure that its entries in tile_Input/tiles.txt and HF_Config/config.txt"
			     		      +"\n have matching names.");

		}

	}

}

void manager::give_r8_array_fields_defaults(){
	for(map<string,param_r8_array>::iterator big_it = input_maker_hook->get_r8_array_params().begin();
	    big_it != input_maker_hook->get_r8_array_params().end();
	    big_it++){
		bool found = false;
		for(map<string,map<string,field>>::iterator lines_it = fields.begin();
		    lines_it != fields.end();
		    lines_it++){
		  try{
			lines_it->second.at(big_it->first).r8_array_hook = &big_it->second; //set pointer to parameter in input maker
			lines_it->second.at(big_it->first).init_temp_input(big_it->second.get_string());
			//lines_it->second.at(big_it->first).temp_input = big_it->second.get_string();//change array into comma separated list in a string
			lines_it->second.at(big_it->first).text_box_init(); //set up text box
			found = true;
			break;//stop checking lines
		  } catch (out_of_range& not_found){
			continue;//check other lines
		  }			

		}
		if(!found){
			error_logger.push_error("Error! Failed to find parameter:"+big_it->first+"'s tile in the fields map."
			     		      +"\nPlease make sure that its entries in tile_Input/tiles.txt and HF_Config/config.txt"
			     		      +"\n have matching names.");
		}

	}

}

//########################################################################################################//





bool manager::update_io_maker(vector<string>& bad_input_list){
	//for(unsigned int c = 1; c < tiles.size();c++){
	//	tiles[c].update_my_value();
	//}
	bool success = true;

	for(map<string,map<string,field>>::iterator big_it = fields.begin();
	    big_it != fields.end();
	    big_it++){
		for(map<string,field>::iterator small_it = big_it->second.begin();
		    small_it != big_it->second.end();
		    small_it++){

			//update the input_maker class with the user's entered values
			//if an error occurs, update_my_value() returns false, and the body is executed
			if(!small_it->second.update_my_value()){
				bad_input_list.push_back(small_it->first);//put tile name that caused error in list
				small_it->second.go_red();
				success = false;
			} else if(small_it->second.is_red){
				//if it worked and the tile was previously red, put it back to normal
				small_it->second.go_back();

			}
		}
	}
	return success;//let button manager know that errors occured
}

void manager::new_line(const string& line_name,const map<string,field>& line_map){
		//std::map<std::string,std::map<std::string,field>> fields;//!< trying something new, to keep relevant tiles together
		fields.emplace(line_name,line_map);
}

void manager::update_win(int width_in,int height_in){
	win_w = width_in;
	win_h = height_in;
}


void manager::print_all(){
        error_logger.push_msg("\n Printing line map ########################################################");

	//loop over every line map
	for(map<string,map<string,field>>::iterator big_it = fields.begin(); big_it != fields.end() ;big_it++){
		//loop over every parameter map
		for(map<string,field>::iterator small_it = big_it->second.begin(); small_it != big_it->second.end(); small_it++){
			small_it->second.print();
		}

	} 

	error_logger.push_msg("###############################################################################");
}

void manager::check_locks(){

	iench_locking();
	ilv1_locking();
	icntrl4_locking();

	//##### ICNTRL6 has a lot going on ##############
	icntrl6_locking();
	inm1_locking();
	inm2_locking();
}

void manager::iench_locking(){
  try{
	//do locking that pertains to IENCH
	regex iench_good("\\s*7\\s*");
	if(!regex_match(fields.at("line_2").at("IENCH").temp_input,iench_good)){
		//make IENCH's tile appear purple to indicate it is the reason some parameters are locked
		fields.at("line_2").at("IENCH").change_tile_background("purple_andy_tile.png");

		fields.at("line_4A").at("APAR").is_locked  = true;
		fields.at("line_4A").at("ZPAR").is_locked  = true;
		fields.at("line_4A").at("QIN").is_locked   = true;
		fields.at("line_4A").at("FJPAR").is_locked = true;
		fields.at("line_4A").at("FPRPAR").is_locked= true;
		fields.at("line_4A").at("NLIN").is_locked  = true;
		fields.at("line_4B").at("TIN").is_locked   = true;

	} else {//do the unlocking
		//switch the IENCH tile back to the default gray 
		fields.at("line_2").at("IENCH").change_tile_background("andy_tile.png");

		fields.at("line_4A").at("APAR").is_locked  = false;
		fields.at("line_4A").at("ZPAR").is_locked  = false;
		fields.at("line_4A").at("QIN").is_locked   = false;
		fields.at("line_4A").at("FJPAR").is_locked = false;
		fields.at("line_4A").at("FPRPAR").is_locked= false;
		fields.at("line_4A").at("NLIN").is_locked  = false;
		fields.at("line_4B").at("TIN").is_locked   = false;
	}
  } catch (out_of_range& map_error){
	error_logger.push_error("From: manager::iench_locking| Critical tiles associated with IENCH were not found,",
				"please check the tile and HF config files.");
  }


}

void manager::ilv1_locking(){
  try{
	//do locking that pertains to ILV1
	regex ilv1_good("\\s*6\\s*");
	if(!regex_match(fields.at("line_5").at("ILV1").temp_input,ilv1_good)){
		//make it purplish pink to indicate that it is locking other parameters
		fields.at("line_5").at("ILV1").change_tile_background("purple_andy_tile.png");

		fields.at("line_5A").at("ACON").is_locked = true;
		fields.at("line_5A").at("GAM").is_locked  = true;
		fields.at("line_5A").at("FCON").is_locked = true;
		fields.at("line_5A").at("C0").is_locked   = true;
		fields.at("line_5A").at("C10").is_locked  = true;
		fields.at("line_5A").at("C11").is_locked  = true;
		fields.at("line_5A").at("C12").is_locked  = true;
		fields.at("line_5A").at("C3").is_locked   = true;

	} else { //do the unlocking
		//set it back to the default gray
		fields.at("line_5").at("ILV1").change_tile_background("andy_tile.png");

		fields.at("line_5A").at("ACON").is_locked = false;
		fields.at("line_5A").at("GAM").is_locked  = false;
		fields.at("line_5A").at("FCON").is_locked = false;
		fields.at("line_5A").at("C0").is_locked   = false;
		fields.at("line_5A").at("C10").is_locked  = false;
		fields.at("line_5A").at("C11").is_locked  = false;
		fields.at("line_5A").at("C12").is_locked  = false;
		fields.at("line_5A").at("C3").is_locked   = false;
	}

  } catch (out_of_range& map_error){
	error_logger.push_error("From: manager::iench_locking| One of the critical tiles associated with ILV1 were not found,",
				"please check the tile and HF config files.");

  }

}

void manager::icntrl4_locking(){
  try{
	regex icntrl4_good("\\s*1\\s*");

	if( !regex_match(fields.at("line_6").at("ICNTRL4").temp_input,icntrl4_good) ){
		//make it purple to indicate it is locking other variables
		fields.at("line_6").at("ICNTRL4").change_tile_background("purple_andy_tile.png");

		fields.at("line_8").at("ICH4").is_locked  = true;
		fields.at("line_8").at("NCH4").is_locked  = true;

		fields.at("line_9").at("ECH4").is_locked  = true;
		fields.at("line_9").at("FJCH4").is_locked = true;
		fields.at("line_9").at("IPAR4").is_locked = true;
		fields.at("line_9").at("FIS4").is_locked  = true;


	} else { //do the unlocking

		//make it gray again, as it is no longer locking
		fields.at("line_6").at("ICNTRL4").change_tile_background("andy_tile.png");

		fields.at("line_8").at("ICH4").is_locked  = false;
		fields.at("line_8").at("NCH4").is_locked  = false;

		fields.at("line_9").at("ECH4").is_locked  = false;
		fields.at("line_9").at("FJCH4").is_locked = false;
		fields.at("line_9").at("IPAR4").is_locked = false;
		fields.at("line_9").at("FIS4").is_locked  = false;

	}

  } catch( out_of_range& map_error){
	error_logger.push_error("From: manager::icntrl4_locking| One of the critical tiles associated with ICNTRL4",
				" were not found, please check that tile and HF config files match."); 

  }

}


void manager::icntrl6_locking(){
  try{
	regex icntrl6_unlock("\\s*(1|2)\\s*");
	//if icntrl6 is 1 or 2, unlock
	if( regex_match(fields.at("line_6").at("ICNTRL6").temp_input,icntrl6_unlock) ){
		//unlock params now that icntrl6 > 0
		fields.at("line_6").at("ICNTRL6").change_tile_background("andy_tile.png");

		fields.at("line_10").at("ITER").is_locked = false;
		fields.at("line_10").at("INM1").is_locked = false;
		fields.at("line_10").at("INM2").is_locked = false;
	} else {
		//elsewise, lock them
		fields.at("line_6").at("ICNTRL6").change_tile_background("purple_andy_tile.png");

		fields.at("line_10").at("ITER").is_locked = true;
		fields.at("line_10").at("INM1").is_locked = true;
		fields.at("line_10").at("INM2").is_locked = true;
	}

  } catch (out_of_range& map_error){
	error_logger.push_error("From: manager::icntrl6_locking| One of the critical tiles associated with ICNTRL4",
				" were not found, please check that tile and HF config files match.");
  }

}

void manager::inm1_locking(){
  try{
	

  } catch (out_of_range& map_error){

  }

}

void manager::inm2_locking(){
  try{

  } catch (out_of_range& map_error){

  }

}



















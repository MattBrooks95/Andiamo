//! \file manager.cc implements the functions declared in manager.h
#include "manager.h"
#include "button_manager.h"


using namespace std;

bool compare_width(field& left, field& right);//prototype for sorting function passed to algorithm::sort

manager::manager(string image_p_in){
	//input_maker_hook = NULL; //should be overwritten when sdl_help's constructor calls give_manager_io
	image_p = image_p_in;
}

bool man_test = true;

void manager::init(){
	
	tile_input_p = "tile_Input/";

	//ins will refer to the stream to the input file for tile information
	fstream ins;

	//open the file
	ins.open((tile_input_p + "tiles.txt").c_str());

	//if something went wrong, print an error message to console
	if(ins.fail()){
 		error_logger.push_error("Fatal error: Failed to open tile input file: "+tile_input_p+"tiles.txt");
	}

	//used to store unidentified line
	string temp_string;

	//this line specifies an image name
	regex img_pattern("\\s*?.*\\.png\\s*?");

	//this recognizes lines that specify tile size lines should be of the
	//form widthxheight EX:100x100
	regex field_size_pattern("\\s*?[0-9]+?\\s*?x\\s*?[0-9]+?\\s*?");

	//this line specifies a tile name
	regex name_pattern("\\s*?([a-z0-9_A-Z]+?):?(.*)?\\s*?");

	//used to tell if the name line is of the form ->HFvariable:EnglishVariable
	regex semi_pattern(":");

	//describes a pattern for tile/input descriptors that starts with a 'c'
	//and is followed by exactly one space,
	//then contains any number of any characters
	regex desc_pattern("c .*");

	//this line recognizes the lines that separate
	//the parameters into lines that correspond with the input manual,
	//so they can be stored together and easily (and readably)
	//printed to the HF file later
	regex line_separator("\\s*?line_[0-9]+?[A-Z]?.*");


	getline(ins,temp_string);//priming read

	//loop over the entire tile_Input/tiles.txt configuration file
	while(!ins.eof() ){

		if(ins.fail()) break;//get out on potentially erroneous last run

		//reset new line container each run of loop
		map<string,field> new_line;
		string line_name;

	 	//read until a line start indicator/separator is found
		while( !ins.eof() && !regex_match(temp_string,line_separator) ){

			if( ins.fail() || temp_string.empty() ){
				//we may not necessarily find this case in error,
				//as the very last group in the config file won't
				//end with a line separator, because no line will come after it

				//ran out of file, get out of this function
				return;
			}
			getline(ins,temp_string);
		}

		error_logger.push_msg("Found a line name:"+temp_string);
		strip_char(temp_string,'#');

		//save line name for later
		line_name = temp_string;

		//grab a new line
		getline(ins,temp_string);

		//outer loop runs over the # of grouped parameters (lines in HF input)
		while( !regex_match(temp_string,line_separator) && !ins.eof()){

			//these parameter should be re-declared for each field
			//names for generalized tiles
			string tile_name = "bad tile name";

			//display name for tiles, may or may not be used
			string display_name = "no display name";

			//names for tile's picture file
			string img_name = "bad image name";

			//description for this input tile
			string description;

			//save all lines for description of tile
			vector<string> temp_descriptions;

			//dimensions of given image for tile, -1 is bad input
			int tile_w = -1, tile_h = -1;


			//inner loop runs name -> andy as many times as needed, until a
			//line separator is found

			//loop until separator 'andy' is found
			while(temp_string != "andy" && !ins.fail()){
				error_logger.push_msg("LINE:"+temp_string+"|");


				//if this line has '.png' in it,
				if( regex_match(temp_string,img_pattern) ){ 

					//process it as an input picture name
					error_logger.push_msg("Found an image name!: "+temp_string);
					img_name = temp_string;

				} else if( regex_match(temp_string,desc_pattern)){

					error_logger.push_msg("Found a description line.: "+temp_string);

					//remove 'c ' at start of desc lines
					description = temp_string.erase(0,2);
					temp_descriptions.push_back(temp_string);


				} else if( regex_match(temp_string,field_size_pattern) ){

					error_logger.push_msg("Found field size specification!: "+temp_string);

					//remove spaces
					strip_char(temp_string,' ');

					//split into a vector of strings
					vector<string> dimensions = split(temp_string,'x');
					try{
					  //first number in the line is the width
					  tile_w = stoi(dimensions[0]);

					  //second number in the line is the width
					  tile_h = stoi(dimensions[1]);
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
					//read in again to update loop
					getline(ins,temp_string);
				} else {
					break;
				}
			}
			field temp_field(tile_name,display_name,img_name,tile_w,tile_h);

			//copy the saved description lines to the new field
			//before it is placed in the map
			for(unsigned int c = 0; c < temp_descriptions.size();c++){
				temp_field.descriptions.push_back(temp_descriptions[c]);
			}

			error_logger.push_msg("##########PUSHING FIELD###################");
			temp_field.print();
			error_logger.push_msg("##########################################");

			//push the field into the map for that parameter's line
			new_line.emplace(tile_name,temp_field);

			if( !ins.fail() ){
				//"andy" is the current line, so go ahead and read the next one
				getline(ins,temp_string);
			}
		}

		//at this point, we have hit the separator for another group of parameters

		//save the order of the lines, so they can be drawn in order to the screen
		line_order.push_back(line_name);

		//store the map of parameters in the map of lines, and
		//give it the name we found earlier
		fields.emplace(line_name,new_line);
	}

	//close the file
	ins.close();

	if(man_test){
		error_logger.push_msg("FIELD MAP AFTER MANAGER.init():");
		print_all();
		error_logger.push_msg("##############################################");
	}
}

/*
manager::~manager(){


}
*/

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

	//call helper function for connecting the int4 params with
	//their graphical tiles
	give_int4_fields_defaults();

	//call helper function for connecting real8 params
	//with their graphical tiles
	give_real8_fields_defaults();

	//call helper function connecting int4 arrays to their graphical tiles
	give_int4_array_fields_defaults();

	//call helper function connecting string params to their graphical tiles
	give_string_fields_defaults();

	//call helper function connecting 'e' parameters to their tiles
	give_r8_array_fields_defaults();

}

void manager::init_fields_graphics(){

	for(map<string,map<string,field>>::iterator big_it = fields.begin();
	    big_it != fields.end();
	    big_it++){
	    for(map<string,field>::iterator small_it = big_it->second.begin();
			small_it != big_it->second.end();
			small_it++){
			small_it->second.graphics_init(image_p);
	    }

	}

}

void manager::draw(){

	check_locks();

	vector<field*> drawn_second;

	for(map<string,map<string,field>>::iterator lines_it = fields.begin();
	    lines_it != fields.end();
	    lines_it++){
		for(map<string,field>::iterator fields_it = lines_it->second.begin();
		    fields_it != lines_it->second.end();	
		    fields_it++){

			//don't draw it if it's in help mode, help mode tiles
			//need to be drawn second, so they aren't overdrawn
			//by other tiles not in help mode
			if( !fields_it->second.is_help_mode() ){

				//have the field draw itself to the screen
				fields_it->second.draw_me();

			} else {
				drawn_second.push_back(&fields_it->second);

			}
		}
	}


	for(unsigned int c = 0; c < drawn_second.size();c++){

		//now the help mode tiles can be drawn
		drawn_second[c]->draw_me();
	}

}

//############# GIVE_FIELDS_DEFAULTS() HELPERS ###############################//
void manager::give_int4_fields_defaults(){
	//this looks dense, but is simple
	//the outer loop just runs over every integer in the integer map that needs output to the HF file,
	//and then runs through each line's map in the manager::fields map looking for the correct parameter name for that int4
	//param. It's better to go through input_maker's parameters and find them in the fields map than it is to loop through
	//the fields map and find the parameters. This is because the second way would involve checking each map in input_maker,
	//when the first way just checks the 2d map in the manager object - these may actually be the same thing now that I think about it
	//for(map<string,param_int4>::iterator big_it = input_maker_hook->get_int4_params().begin();
	//	big_it != input_maker_hook->get_int4_params().end();
	for(map<string,param_int4>::iterator big_it = io_access->get_int4_params().begin();
		big_it != io_access->get_int4_params().end();
		big_it++){

		//start off false, turn to true if the desired parameter is found
		bool found = false;
		for(map<string,map<string,field>>::iterator line_it = fields.begin();
			line_it != fields.end();
			line_it++){
		  //try to find the desired parameter in this line
		  try {

			//let the field reference it's place in the input_maker map,
			//so it can output the new value given to it by the user,
			//to the HF output
		    line_it->second.at(big_it->first).int4_hook = &big_it->second;

			//-1804 means that this parameter shouldn't have a default value,
			//so set the text box's text to a message instead
			if(big_it->second.value == -1804) {
				line_it->second.at(big_it->first).init_temp_input("no default");
			} else {
				line_it->second.at(big_it->first).init_temp_input(to_string(big_it->second.value));
			}


			//set up the text box
			line_it->second.at(big_it->first).text_box_init();

			//we found it, so make the flag good
			found = true;

			//leave this loop early, we don't need to check
			//the other line's maps
			break;

		  //if it fails, need to check a different line's
		  // map in the manager object
		  } catch (out_of_range& not_found){

			//if it isn't found, just check the next line's map
			continue;
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

	for(map<string,param_int4_array>::iterator big_it = io_access->get_i4_array_params().begin();
	    big_it != io_access->get_i4_array_params().end();
	    big_it++){
		bool found = false;
		for(map<string,map<string,field>>::iterator line_it = fields.begin();
		    line_it != fields.end();
		    line_it++){
		  try{
			line_it->second.at(big_it->first).int4_array_hook = &big_it->second;
			line_it->second.at(big_it->first).init_temp_input(big_it->second.get_string());

			//run the text box's init function
			line_it->second.at(big_it->first).text_box_init();

			found = true;
			break;
		  } catch (out_of_range& not_found){
			//check other line maps
			continue;
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

	for(map<string,param_real8>::iterator big_it = io_access->get_real8_params().begin();
	    big_it != io_access->get_real8_params().end();
	    big_it++){

		bool found = false;
		for(map<string,map<string,field>>::iterator line_it = fields.begin();
		    line_it != fields.end();
		    line_it++){
		  try{

			//give field pointer access to it's value in input_maker
			line_it->second.at(big_it->first).real8_hook = &big_it->second;

			//-180.4 is the float version of the no applicable default flag,
			//so set text box to be a message instead 
			if(big_it->second.value == -180.4){
				line_it->second.at(big_it->first).init_temp_input("no default");
			} else {
				line_it->second.at(big_it->first).init_temp_input(to_string(big_it->second.value));
			}

			//set up the text box
			line_it->second.at(big_it->first).text_box_init();

			//set the flag to true, because we found the param we were looking for
			found = true;
			break;
		  } catch (out_of_range& not_found){

			//it wasn't found, so check next line's map
			continue;

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

	for(map<string,param_string>::iterator big_it = io_access->get_string_params().begin();
	    big_it != io_access->get_string_params().end();
	    big_it++){
		bool found = false;
		for(map<string,map<string,field>>::iterator line_it = fields.begin();
		    line_it != fields.end();
		    line_it++){
		  try{
			//set up the pointer to the parameter in input_maker
			line_it->second.at(big_it->first).string_hook = &big_it->second;
			line_it->second.at(big_it->first).init_temp_input(big_it->second.value);

			//set up the text box
			line_it->second.at(big_it->first).text_box_init();
			found = true;
			break;
		  } catch (out_of_range& not_found){
			//check the other lines
			continue;
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

	for(map<string,param_r8_array>::iterator big_it = io_access->get_r8_array_params().begin();
	big_it != io_access->get_r8_array_params().end();
	    big_it++){
		bool found = false;
		for(map<string,map<string,field>>::iterator lines_it = fields.begin();
		    lines_it != fields.end();
		    lines_it++){
		  try{
			//set pointer to parameter in input maker
			lines_it->second.at(big_it->first).r8_array_hook = &big_it->second;
			lines_it->second.at(big_it->first).init_temp_input(big_it->second.get_string());
			 //set up text box
			lines_it->second.at(big_it->first).text_box_init();
			found = true;

			//stop checking lines
			break;
		  } catch (out_of_range& not_found){
			//check other lines
			continue;
		  }			

		}
		if(!found){
			error_logger.push_error("Error! Failed to find parameter:"+big_it->first+"'s tile in the fields map."
			     		      +"\nPlease make sure that its entries in tile_Input/tiles.txt and HF_Config/config.txt"
			     		      +"\n have matching names.");
		}

	}

}

//############################################################################//





bool manager::update_io_maker(vector<string>& bad_input_list){

	bool success = true;

	for(map<string,map<string,field>>::iterator big_it = fields.begin();
	    big_it != fields.end();
	    big_it++){
		for(map<string,field>::iterator small_it = big_it->second.begin();
		    small_it != big_it->second.end();
		    small_it++){

			//update the input_maker class with the user's entered values
			//if an error occurs, update_my_value() returns false,
			//and the body is executed
			if(!small_it->second.update_my_value()){

				//put tile name that caused error in list
				bad_input_list.push_back(small_it->first);
				small_it->second.go_red();
				success = false;
			} else if(small_it->second.is_red){
				//if it worked and the tile was previously red,
				//put it back to normal
				small_it->second.go_back();
			}
		}
	}

	//let button manager know that errors occured
	return success;
}

void manager::new_line(const string& line_name,const map<string,field>& line_map){
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

	//simple cases
	iench_locking();
	ilv1_locking();

	//complex cases/form button cases
	icntrl10_locking();
	icntrl4_locking();
	icntrl8_locking();
	ilv3_ilv5_locking();
	//##### ICNTRL6 has a lot going on ##############
	icntrl6_locking();
	//###############################################
}

void manager::iench_locking(){
  try{
	//do locking that pertains to IENCH
	regex iench_good("\\s*7\\s*");
	if(!regex_match(fields.at("line_2").at("IENCH").temp_input,iench_good)){
		//make IENCH's tile appear purple to indicate it is the reason
		//some parameters are locked
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
		//if( !(b_manager_hook->get_icntrl_4().get_is_locked()) ){
		//	b_manager_hook->get_icntrl_4().toggle_lock();
		if( !(button_access->get_icntrl_4().get_is_locked()) ){
			button_access->get_icntrl_4().toggle_lock();
		} 


	} else { //do the unlocking

		//make it gray again, as it is no longer locking
		fields.at("line_6").at("ICNTRL4").change_tile_background("andy_tile.png");

		fields.at("line_8").at("ICH4").is_locked  = false;
		fields.at("line_8").at("NCH4").is_locked  = false;


		ich4_nch4_locking();

	}

  } catch( out_of_range& map_error){
	error_logger.push_error("From: manager::icntrl4_locking| One of the critical tiles associated with ICNTRL4",
				" were not found, please check that the tile and HF config files match."); 

  }



}
//this function is checked at the end of icntrl4 check lock, because the
//parameters icntrl4 unlocks must also be set up to unlock the form button
void manager::ich4_nch4_locking(){
  try{

	regex ich4_unlock("\\s*[1-6]\\s*");
	//if both of it's params are correctly set up

	string test_ich4 = fields.at("line_8").at("ICH4").temp_input;
	if( fields.at("line_8").at("ICH4").am_I_locking && regex_match(test_ich4,ich4_unlock) ){
		fields.at("line_8").at("ICH4").change_tile_background("andy_tile.png");
		fields.at("line_8").at("ICH4").am_I_locking = false;

	} else if( !regex_match(test_ich4,ich4_unlock) ){
		fields.at("line_8").at("ICH4").change_tile_background("purple_andy_tile.png");
		fields.at("line_8").at("ICH4").am_I_locking = true;
	}


	int test_nch4 = stoi(fields.at("line_8").at("NCH4").temp_input);
	if( fields.at("line_8").at("NCH4").am_I_locking && (test_nch4 > 0 && test_nch4 < 101 )){

		fields.at("line_8").at("NCH4").change_tile_background("andy_tile.png");
		fields.at("line_8").at("NCH4").am_I_locking = false;

	} else if( !(test_nch4 > 0 && test_nch4 < 101) ){

		fields.at("line_8").at("NCH4").change_tile_background("purple_andy_tile.png");
		fields.at("line_8").at("NCH4").am_I_locking = true;
	}

	if( button_access->get_icntrl_4().get_is_locked() && 
	    (!(fields.at("line_8").at("ICH4").am_I_locking) && !(fields.at("line_8").at("NCH4").am_I_locking)) ){

		//unlock the button, both are satisfied
		button_access->get_icntrl_4().toggle_lock();

	} else if( (fields.at("line_8").at("ICH4").am_I_locking || fields.at("line_8").at("NCH4").am_I_locking)
		   && !(button_access->get_icntrl_4().get_is_locked()) ){
		//lock the button
		button_access->get_icntrl_4().toggle_lock();
	}


  } catch( out_of_range& map_error){
	error_logger.push_error("From: manager::ich4_nch4_locking| One of the critical tiles associated with ICH4/NCH4",
				" were not found, please check that the tile and HF config files match.");

  } catch( invalid_argument& bad_arg){
	error_logger.push_msg("From manager::ich4_nch4_locking| NCH4 or ICH4 has a number in an illegal range");
	error_logger.push_msg(". The acceptable range for NCH4 is 0<=x<=100. The acceptable range for ICH4 is 0 <= ICH4 <= 6");
  }

}
void manager::icntrl8_locking(){
  try{
	regex icntrl8_unlock("\\s*[0-9]+?\\s*");

	string icntrl_8_str = fields.at("line_6").at("ICNTRL8").temp_input;
	int icntrl_8_val = stoi(fields.at("line_6").at("ICNTRL8").temp_input);
	//if it is currently in locking mode, and it shouldn't be, then change its mode
	if( (regex_match(icntrl_8_str,icntrl8_unlock) && (icntrl_8_val > 0 && icntrl_8_val < 332) ) &&
	    fields.at("line_6").at("ICNTRL8").am_I_locking ){

		//update background color
		fields.at("line_6").at("ICNTRL8").change_tile_background("andy_tile.png");

		//make it stop locking
		fields.at("line_6").at("ICNTRL8").am_I_locking = false;

	//if it failed the above check, and it's currently unlocked, then re-lock it
	} else if( !(icntrl_8_val > 0 && icntrl_8_val < 332) ){
		fields.at("line_6").at("ICNTRL8").change_tile_background("purple_andy_tile.png");
		fields.at("line_6").at("ICNTRL8").am_I_locking = true;

	}


  } catch (out_of_range& map_error){
	error_logger.push_error("From: manager::icntrl8_locking()| ICNTRL8 was not found in the map of parameter tiles",
				", please check that the tile and HF config files match.");

  } catch (invalid_argument& stoi_error){
	error_logger.push_msg("ICNTRL8 has an illegal string argument, it must be an integer in the range");
	error_logger.push_msg(" 0 <= ICNTRL8 < 332");
	fields.at("line_6").at("ICNTRL8").change_tile_background("purple_andy_tile.png");
	fields.at("line_6").at("ICNTRL8").am_I_locking = true;

  }

	//toggle if the conditions are met for unlocking, and it is currently
	//locked, or if the conditions are not met and it is unlocked
	if( ( !(fields.at("line_6").at("ICNTRL8").am_I_locking) && button_access->get_icntrl_8().get_is_locked() ) ||
	    (fields.at("line_6").at("ICNTRL8").am_I_locking && !button_access->get_icntrl_8().get_is_locked()) ){
		button_access->get_icntrl_8().toggle_lock();//
	}


}

void manager::icntrl10_locking(){
  try{

	regex icntrl10_unlock("\\s*[0-9]+?\\s*");
	int icntrl10_val = stoi(fields.at("line_6").at("ICNTRL10").temp_input);
	string icntrl10_str = fields.at("line_6").at("ICNTRL10").temp_input;

	//if icntrl10 tile is currently locking its button, and the conditions
	//for unlocking it are true, then have it start unlocking
	if( fields.at("line_6").at("ICNTRL10").am_I_locking &&
	    (regex_match(icntrl10_str,icntrl10_unlock) && icntrl10_val > 0) ){
		fields.at("line_6").at("ICNTRL10").change_tile_background("andy_tile.png");
		fields.at("line_6").at("ICNTRL10").am_I_locking = false;

	//otherwise, if it is not locking, and it's conditions are not met,
	//then have it start locking
	} else if( !(fields.at("line_6").at("ICNTRL10").am_I_locking) && 
		   !(regex_match(icntrl10_str,icntrl10_unlock) && icntrl10_val > 0) ){
		fields.at("line_6").at("ICNTRL10").change_tile_background("purple_andy_tile.png");
		fields.at("line_6").at("ICNTRL10").am_I_locking = true;

	}


  } catch (out_of_range& map_error){
	error_logger.push_error("From: manager::icntrl8_locking()| ICNTRL10 was not found in the map of parameter tiles,",
				" please check that the tile and HF config files match");

  } catch (invalid_argument& stoi_error){
	error_logger.push_msg("ICNTRL10 has an illegal string argument, it must be an integer in the range");
	error_logger.push_msg(" 0 <= ICNTRL10");
	fields.at("line_6").at("ICNTRL10").change_tile_background("purple_andy_tile.png");
	fields.at("line_6").at("ICNTRL10").am_I_locking = true;


  }

	if( ( !(fields.at("line_6").at("ICNTRL10").am_I_locking) && button_access->get_icntrl_10().get_is_locked() ) ||
	    ( fields.at("line_6").at("ICNTRL10").am_I_locking && !(button_access->get_icntrl_10().get_is_locked()) ) ){
		button_access->get_icntrl_10().toggle_lock();
	}


}

void manager::ilv3_ilv5_locking(){

  try{

	regex ilv3_ilv5_unlock("\\s*[0-9]+?\\s*");

	//do checks for ILV3
	ilv3_ilv5_locking_helper("ILV3",ilv3_ilv5_unlock);
	ilv3_ilv5_locking_helper("ILV5",ilv3_ilv5_unlock);

  } catch (out_of_range& map_error){
	error_logger.push_error("From: manager::ilv3_ilv5_locking()| ILV3 or ILV5was not found in the map of parameter"
				 " tiles, please check that the tile and HF config files match");

  } catch (invalid_argument& stoi_error){
	error_logger.push_msg("ILV3 or ILV5 has an illegal string argument, it must be an integer in the range");
	error_logger.push_msg(" 0 <= (ILV3 or ILV5)");
	fields.at("line_5").at("ILV3").change_tile_background("purple_andy_tile.png");
	fields.at("line_5").at("ILV3").am_I_locking = true;
	fields.at("line_5").at("ILV5").change_tile_background("purple_andy_tile.png");
	fields.at("line_5").at("ILV5").am_I_locking = true;


  }
	//store the truth values that we care about in local variables, so the logic below is slightly
	//more readable
	bool ilv3_locking = fields.at("line_5").at("ILV3").am_I_locking;
	bool ilv5_locking = fields.at("line_5").at("ILV5").am_I_locking;

    //if the form button is locked, and ILV3 XOR ILV5 (one is true, one is false), then unlock it (by toggling)
    //or, if the form button is not locked, and ILV3 XOR ILV5 is false (both are false, or both are true), then toggle
	if( (button_access->get_ilv3_ilv5().get_is_locked() && ((ilv3_locking && !ilv5_locking) || ((!ilv3_locking && ilv5_locking))))
	    || (!button_access->get_ilv3_ilv5().get_is_locked() && !((ilv3_locking && !ilv5_locking) || ((!ilv3_locking && ilv5_locking))))
          ){
		button_access->get_ilv3_ilv5().toggle_lock();
	}

}

void manager::ilv3_ilv5_locking_helper(const string& target_param,const regex& unlock_condition){

	//nab raw string from field parameter
	string target_str = fields.at("line_5").at(target_param).temp_input;

	//cast the string to an int
	int target_val = stoi(target_str);

	//if the target tile is currently locking its button, and the conditions
	//for unlocking it are true, then have it start unlocking
	if( fields.at("line_5").at(target_param).am_I_locking &&
	    (regex_match(target_str,unlock_condition) && target_val > 0) ){

		fields.at("line_5").at(target_param).change_tile_background("andy_tile.png");
		fields.at("line_5").at(target_param).am_I_locking = false;

	//if target_param is not currently locking it's form button,
	//and the conditions for it being unlocked are not met,
	//then make it start locking again
	} else if( !fields.at("line_5").at(target_param).am_I_locking &&
		   !(regex_match(target_str,unlock_condition) && target_val > 0) ){
		fields.at("line_5").at(target_param).change_tile_background("purple_andy_tile.png");
		fields.at("line_5").at(target_param).am_I_locking = true;
	}

}

void manager::icntrl6_locking(){
  try{
	regex icntrl6_unlock("\\s*[12]\\s*");
	//if icntrl6 is 1 or 2, unlock
	if( regex_match(fields.at("line_6").at("ICNTRL6").temp_input,icntrl6_unlock) ){
		//unlock params now that icntrl6 > 0
		fields.at("line_6").at("ICNTRL6").change_tile_background("andy_tile.png");
		fields.at("line_6").at("ICNTRL6").am_I_locking = false;

		fields.at("line_10").at("ITER").is_locked = false;
		fields.at("line_10").at("INM1").is_locked = false;
		fields.at("line_10").at("INM2").is_locked = false;
	} else {
		//elsewise, lock them
		fields.at("line_6").at("ICNTRL6").change_tile_background("purple_andy_tile.png");
		fields.at("line_6").at("ICNTRL6").am_I_locking = true;
		fields.at("line_10").at("ITER").is_locked = true;
		fields.at("line_10").at("INM1").is_locked = true;
		fields.at("line_10").at("INM2").is_locked = true;
	}

  } catch (out_of_range& map_error){
	error_logger.push_error("From: manager::icntrl6_locking| One of the critical tiles associated with ICNTRL4",
				" were not found, please check that tile and HF config files match.");
  }
	bool icntrl6_lock_status = fields.at("line_6").at("ICNTRL6").am_I_locking;
	if( !icntrl6_lock_status ){
		//these functions set the am_I_locking states for each
		//of the sub-parameters of ICNTRL6
		inm1_locking();
		inm2_locking();
		iter_locking();


		//the sub states are saved to local variables here so it's not
		//as long of a statement
		bool inm1_lock_status = fields.at("line_10").at("INM1").am_I_locking;
		bool inm2_lock_status = fields.at("line_10").at("INM2").am_I_locking;
		bool iter_lock_status = fields.at("line_10").at("ITER").am_I_locking;
		bool icntrl6_locked = button_access->get_icntrl_6().get_is_locked();

		//the following assumes that ITER must be greater than 0, and atleast one of INM1 or INM2 must be nonzero
		//if form_button is not locked, and all are locking, then lock it
		if( (icntrl6_locked && !iter_lock_status && (!inm1_lock_status || !inm2_lock_status)) ||
		    (!icntrl6_locked && iter_lock_status) ||
		    (!icntrl6_locked && !iter_lock_status && (inm1_lock_status && inm2_lock_status))
		     
		  ){
			button_access->get_icntrl_6().toggle_lock();
		}
	}


}

void manager::inm1_locking(){
  try{
	int inm1_val = stoi(fields.at("line_10").at("INM1").temp_input);
	string inm1_str = fields.at("line_10").at("INM1").temp_input;
	regex inm1_good("\\s*[0-9]+\\s*");

	if( fields.at("line_10").at("INM1").am_I_locking && (regex_match(inm1_str,inm1_good) && inm1_val > 0) ){
		fields.at("line_10").at("INM1").change_tile_background("andy_tile.png");
		fields.at("line_10").at("INM1").am_I_locking = false;

	} else if( !(fields.at("line_10").at("INM1").am_I_locking) && (!(inm1_val > 0) || !(regex_match(inm1_str,inm1_good))) ){
		fields.at("line_10").at("INM1").change_tile_background("purple_andy_tile.png");
		fields.at("line_10").at("INM1").am_I_locking = true;;
	}

  } catch (out_of_range& map_error){
	error_logger.push_error("From: manager: inm1_locking| INM1 was not found in the parameter map",
				", please check that the tile and HF config files match");
  } catch (invalid_argument& stoi_error){
	error_logger.push_msg("INM1 has an illegal string argument, it must be an integer in the range ");
	error_logger.push_msg(" 0 <= INM1");
	fields.at("line_10").at("INM1").change_tile_background("purple_andy_tile.png");
	fields.at("line_10").at("INM1").am_I_locking = true;
  }

}

void manager::inm2_locking(){
  try{
	int inm2_val = stoi(fields.at("line_10").at("INM2").temp_input);

	if( fields.at("line_10").at("INM2").am_I_locking && inm2_val > 0 ){
		fields.at("line_10").at("INM2").change_tile_background("andy_tile.png");
		fields.at("line_10").at("INM2").am_I_locking = false;

	} else if( !(fields.at("line_10").at("INM2").am_I_locking) && !(inm2_val > 0) ){
		fields.at("line_10").at("INM2").change_tile_background("purple_andy_tile.png");
		fields.at("line_10").at("INM2").am_I_locking = true;;
	}	

  } catch (out_of_range& map_error){
	error_logger.push_error("From: manager: inm2_locking| INM2 was not found in the parameter map",
				", please check that the tile and HF config files match");
  } catch (invalid_argument& stoi_error){
	error_logger.push_msg("INM2 has an illegal string argument, it must be an integer in the range ");
	error_logger.push_msg(" 0 <= INM2");
	fields.at("line_10").at("INM2").change_tile_background("purple_andy_tile.png");
	fields.at("line_10").at("INM2").am_I_locking = true;
  }

}

void manager::iter_locking(){
  try{
	int iter_val = stoi(fields.at("line_10").at("ITER").temp_input);

	if( fields.at("line_10").at("ITER").am_I_locking && iter_val > 0 ){
		fields.at("line_10").at("ITER").change_tile_background("andy_tile.png");
		fields.at("line_10").at("ITER").am_I_locking = false;

	} else if( !(fields.at("line_10").at("ITER").am_I_locking) && !(iter_val > 0) ){
		fields.at("line_10").at("ITER").change_tile_background("purple_andy_tile.png");
		fields.at("line_10").at("ITER").am_I_locking = true;;
	}	

  } catch (out_of_range& map_error){
	error_logger.push_error("From: manager: iter_locking| ITER was not found in the parameter map",
				", please check that the tile and HF config files match");
  } catch (invalid_argument& stoi_error){
	error_logger.push_msg("ITER has an illegal string argument, it must be an integer in the range ");
	error_logger.push_msg(" 0 <= ITER");

	fields.at("line_10").at("ITER").change_tile_background("purple_andy_tile.png");
	fields.at("line_10").at("ITER").am_I_locking = true;
  }




}

//void manager::gain_bmanager_access(button_manager* b_manager_hook_in){
//	b_manager_hook = b_manager_hook_in;
//}















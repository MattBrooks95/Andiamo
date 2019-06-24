//! \file manager.cc implements the functions declared in manager.h
#include "manager.h"

#include "button_manager.h"
#include "define.h"
#include "regex_patterns.h"

using namespace std;

//prototype for sorting function passed to algorithm::sort
bool compare_width(field& left, field& right);

manager::manager(const string& config_folder_name){
	cout << "In manager constructor. Config folder:" << config_folder_name << endl;

	//relative to the asset_manager's asset root
	image_path = "Images/";

	configuration_folder_path = system_access->get_andiamo_root() + CONFIGURATION_PATH;
	cout << "Config folder path:" << configuration_folder_path << endl;

	configuration_folder_name = config_folder_name;

	init_regular_expressions();
	init_parameter_configurations();
	init_parameter_graphics();
	cout << "array size: " << fields_order.size() << " map size: " << fields.size() << endl;
	// cout << "array:" << endl;
	// for(vector<vector<field*>>::iterator it = fields_order.begin();
	// 	it != fields_order.end();
	// 	++it){
	// 	cout << it[0][0]->get_string() << endl;
	// }
	// cout << "map:" << endl;
	// for (map<string,map<string,field*>*>::iterator it = fields.begin();
	// 	it != fields.end();
	// 	++it){
	// 	cout << "line: " << it->first << endl;
	// }
}

string manager::get_configuration_file_path(const string& file_name){
	string configuration_total_path = configuration_folder_path + configuration_folder_name + '/';
	return configuration_total_path + file_name;
}

void manager::fill_vector_with_configuration_lines_from_file(const string& file_name,vector<string>& file_lines){
	string total_file_path = get_configuration_file_path(file_name);
	cout << "path to file:" << total_file_path << endl;

	system_access->get_file_as_lines(total_file_path, file_lines);

	// for(uint c = 0; c < file_lines.size(); c++){
	// 	cout << file_lines[c] << endl;
	// }
}

void manager::init_regular_expressions(){
	cout << "Init regular expressions" << endl;

	string file_name = REGEX_FILE_NAME;
	vector<string> file_lines;
	fill_vector_with_configuration_lines_from_file(file_name,file_lines);

	for(uint c = 0; c < file_lines.size(); c++){
		if(file_lines[c].empty() || file_lines[c] == "\n"){
			continue;
		}

		vector<string> parts = split(file_lines[c],':');

		string parameter_name = parts[0];
		regex* regular_expression = regex_access->get_regular_expression(parts[1]);
		cout << "inserting regexp for parameter:" << parameter_name << endl;
		pair<string, regex*> parameter_regex(parameter_name,regular_expression);
		parameter_regexps.insert(parameter_regex);
	}
}

void manager::init_parameter_configurations(){
	cout << "Init parameter default values" << endl;

	string file_name = HF_FILE_NAME;
	vector<string> file_lines;
	fill_vector_with_configuration_lines_from_file(file_name,file_lines);

	regex* comment_line       = regex_access->get_regular_expression(RE_COMMENT_LINE);
	regex* default_value_line = regex_access->get_regular_expression(RE_INITIAL_VALUE);

	for(uint c = 0; c < file_lines.size(); c++){
		bool empty_line = file_lines[c].empty();
		bool newline    = file_lines[c] == "\n";
		bool comment = regex_match(file_lines[c],*comment_line);
		if( empty_line || newline || comment){
			continue;
		}

		//note that the first match group is the entire match
		smatch capture_groups;

		regex_match(file_lines[c],capture_groups,*default_value_line);

		if(capture_groups.size() != 3){
			logger_access->push_error("invalid parameter default line", file_lines[c]);
		}

		string parameter_name           = capture_groups[1];
		string parameter_default_string = capture_groups[2];

		pair<string,string> line_info(parameter_name,parameter_default_string);
		parameter_defaults.insert(line_info);
	}

}

void manager::insert_line_pointer(const string& line_name, map<string, field*>* line_map, vector<field*>& this_lines_parameters_in_order){
	cout << "Inserting line of params! line name: " << line_name << endl;
	fields.insert(std::pair<string,map<string,field*>*>(line_name,line_map));
	cout << "Pushing line of params into array!" << endl;
	fields_order.push_back(this_lines_parameters_in_order);
}

void manager::init_parameter_graphics(){
	string file_name = PARAMETER_GRAPHICS_FILE_NAME;
	vector<string> file_lines;
	fill_vector_with_configuration_lines_from_file(file_name,file_lines);

	//this line specifies an image name
	regex* img_pattern = regex_access->get_regular_expression(RE_IMG);

	//this recognizes lines that specify tile size lines should be of the
	//form widthxheight EX:100x100
	regex* field_size_pattern = regex_access->get_regular_expression(RE_FIELD_SIZE);

	//this line specifies a tile name
	regex* name_pattern = regex_access->get_regular_expression(RE_TILE_NAME);

	/*describes a pattern for tile/input descriptors that starts with a 'c'
	 *and is followed by exactly one space,
	 *then contains any number of any characters */
	regex* desc_pattern = regex_access->get_regular_expression(RE_DESCRIPTION);

	/*this line recognizes the lines that separate
	 *the parameters into lines that correspond with the input manual,
	 *so they can be stored together*/
	regex* line_separator = regex_access->get_regular_expression(RE_LINE_SEPARATOR);

	vector<field*> this_lines_parameters_in_order;
	map<string, field*>* line_map         = NULL;
	field* currently_processing_parameter = NULL;
	string line_name;

	string tile_name;
	string display_name;
	string image_name;
	int width  = 0;
	int height = 0;

	regex* regular_expression = NULL;
	string parameter_default;

	vector<string>* temp_descriptions = NULL;

	smatch groups_from_line_match;

	uint line_number = 1;

	for(vector<string>::iterator line_iterator = file_lines.begin();
		line_iterator != file_lines.end();
		++line_iterator){

		if(regex_match(*line_iterator,groups_from_line_match,*line_separator)){
			cout << "found a line indicator!:" << *line_iterator << endl;
			string new_line_name = groups_from_line_match[1];
			if (line_map == NULL){
				line_map = new map<string, field*>();
				this_lines_parameters_in_order.clear();
				continue;
			}

			insert_line_pointer(line_name, line_map, this_lines_parameters_in_order);

			line_name = new_line_name;
			line_map = new map<string, field*>();
			this_lines_parameters_in_order.clear();

		} else if(line_iterator->compare("andy") == 0){

			//end current parameter, push into current line map
			field* new_field = new field(tile_name,display_name,image_name,width,height,temp_descriptions);

			new_field->set_regular_expression(regular_expression);
			new_field->set_image_path(image_path);

			//x and y coordinates calculated later, by calc_corners
			new_field->get_text_box().init(sdl_access->font,parameter_default,0,0,new_field->get_size().width,25);
			new_field->graphics_init();
			// new_field->update_text(parameter_default);
			new_field->text_init();

			if (line_map == NULL){
				cout << "Found an andy separator, when the current line's map is NULL! line: " << *line_iterator << "\nline number: " << line_number << endl;
				exit(-1);
			}
			line_map->insert(std::pair<string,field*>(tile_name,new_field));
			this_lines_parameters_in_order.push_back(new_field);

			vector<string>::iterator next_run = line_iterator + 1;
			if(next_run == file_lines.end()){
				cout << "this is the last run!" << endl;
				insert_line_pointer(line_name, line_map, this_lines_parameters_in_order);
			}

			temp_descriptions  = NULL;
			regular_expression = NULL;

			tile_name.clear();
			display_name.clear();
			image_name.clear();
			parameter_default.clear();

			width  = 0;
			height = 0;

		} else {
			if (regex_match(*line_iterator,groups_from_line_match, *desc_pattern)){
				handle_description_line(temp_descriptions,groups_from_line_match);
			//should use a regular expression to check the image type instead...
			//also it makes sense to allow JPGS and GIFS as well
			} else if(regex_match(*line_iterator,groups_from_line_match,*img_pattern)){
				image_name = handle_image_name_line(groups_from_line_match);
			} else if(regex_match(*line_iterator,groups_from_line_match,*field_size_pattern)){
				handle_field_size_match(groups_from_line_match,width,height);
			//used to tell if the name line is of the form ->HFvariable:EnglishVariable
			} else if(regex_match(*line_iterator,groups_from_line_match,*name_pattern)){
				//assigns to tile & display name strings
				handle_name_line_match(groups_from_line_match,tile_name,display_name);

				regular_expression = get_map_entry(parameter_regexps,tile_name,logger_access->get_message_vector());
				parameter_default  = get_map_entry(parameter_defaults,tile_name,logger_access->get_message_vector());

			} else {

				logger_access->push_msg("Parameter content line didn't match a regular expression!");

			}
		}
		++line_number;
	}
}

void handle_description_line(vector<string>* temp_descriptions, const smatch& groups_from_line_match){
	string description_line = groups_from_line_match[1];

	if (temp_descriptions == NULL){
		temp_descriptions = new vector<string>();
	}
	temp_descriptions->push_back(description_line);
}

string handle_image_name_line(const smatch& groups_from_line_match){
	int expected_matches = 2;
	if (groups_from_line_match.size() != expected_matches){
		print_matching_message("tile image",expected_matches);
	}

	string image_name = groups_from_line_match[1];

	return image_name;
}

void handle_field_size_match(const smatch& groups_from_line_match, int& width, int& height){
	width  = stoi(groups_from_line_match[1]);
	height = stoi(groups_from_line_match[2]);
}

void handle_name_line_match(const smatch& groups_from_line_match, string& tile_name, string& display_name){
	// need to set the display name and the actual HF parameter name
	uint matches_for_just_parameter_name = 2;
	uint matches_for_parameter_and_display_names = 3;

	uint match_group_size = groups_from_line_match.size();

	if (match_group_size < matches_for_just_parameter_name
		|| match_group_size > matches_for_parameter_and_display_names){
		print_matching_message("parameter_name",matches_for_just_parameter_name);
		return;
	}

	if (match_group_size >= matches_for_just_parameter_name){
		tile_name = groups_from_line_match[1];
	}

	if(match_group_size >= matches_for_parameter_and_display_names){
		display_name = groups_from_line_match[2];
	}
}

manager::~manager(){
	//clear out the objects that the vector points to
	for(uint line = 0; line < fields_order.size();line++){

		for(uint param = 0; param < fields_order[line].size();param++){
			delete(fields_order[line][param]);
		}

	}

	//because the map & the vector have pointers to the same objects,
	//calling delete on the pointers in the map will cause issues

}

int manager::get_widest_tile_width(){

	int max_width = 0;

	for(uint line = 0; line < fields_order.size();line++){

		for(uint param = 0; param < fields_order[line].size();param++){
			int candidate_width = fields_order[line][param]->get_size().width;
			if(candidate_width > max_width){
				max_width = candidate_width;
			}

		}

	}

	//cout << "Max width: " << max_width << endl;

	return max_width;
}

// void manager::init_fields_graphics(){

// 	for(uint line = 0; line < fields_order.size();line++){

// 		for(uint param = 0; param < fields_order[line].size();param++){

// 			if(fields_order[line][param] == NULL){
// 				cout << "NULL POINTER IN TILE VECTOR."
// 					 << "Line: " << line << " Param: " << param << endl;
// 			} else {
// 				fields_order[line][param]->graphics_init(image_path);
// 			}
// 		}

// 	}

// }

void manager::draw(){
	check_locks();

	vector<field*> drawn_second;

	for(uint line = 0; line < fields_order.size();line++){

		for(uint param = 0; param < fields_order[line].size();param++){

			if(fields_order[line][param] == NULL){
				logger_access->push_error("NULL pointer in manager::draw()");
				continue;
			}
			if(!fields_order[line][param]->is_help_mode()){
				fields_order[line][param]->draw_me();
			} else {
				drawn_second.push_back(fields_order[line][param]);
			}

		}
	}

	//now the help mode tiles can be drawn
	for(uint c = 0; c < drawn_second.size();c++){
		drawn_second[c]->draw_me();
	}

}

field* manager::get_param(const string& target_param){


	for(FIELDS_MAP::iterator line_it = fields.begin(); line_it != fields.end(); line_it++){

		field* this_field = get_param_from_line(target_param,*line_it->second);
		if(this_field == NULL){
			continue;
		} else {
			return this_field;
		}
	}
	logger_access->push_error("Couldn't find param:"+target_param+" in the fields map.");
	return NULL;
}

field* manager::get_param_from_line(const string& target_param,map<string,field*>& target_line){

	try{

		return target_line.at(target_param);

	} catch(out_of_range& failure) {

		return NULL;
	}

}

void manager::update_win(int width_in,int height_in){
	win_w = width_in;
	win_h = height_in;
}

void manager::update_locks_in_line(map<string,field*>& target_line, bool lock_value){
	for(map<string,field*>::iterator line_it = target_line.begin(); line_it != target_line.end(); line_it++){
		field* this_field = line_it->second;
		if(this_field == NULL){
			continue;
		} else {
			this_field->is_locked = lock_value;
		}
	}
}

void manager::lock_line(map<string,field*>& target_line){
	update_locks_in_line(target_line,true);
}

void manager::unlock_line(map<string,field*>& target_line){
	update_locks_in_line(target_line,false);
}


void manager::print_all(){

	logger_access->push_msg("\n Printing parameters in order #################");

	for(uint line = 0; line < fields_order.size();line++){
		logger_access->push_msg("LINE VECTOR INDEX: "+to_string(line));
		uint num_params = fields_order[line].size();
		for(uint param = 0; param < num_params; param++){

			fields_order[line][param]->print();

		}

	}

	logger_access->push_msg("\n End params order print #######################");

}

void manager::check_locks(){
	//simple cases
	//iench_locking();//can't handle iench = 7
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

/* we can't do iench = 7
void manager::iench_locking(){
  try{
	//do locking that pertains to IENCH
	regex iench_good("\\s*7\\s*");
	if(!regex_match(fields.at("line_2")->at("IENCH").get_text(),iench_good)){
		//make IENCH's tile appear purple to indicate it is the reason
		//some parameters are locked
		fields.at("line_2")->at("IENCH").change_tile_background("purple_andy_tile.png");
		lock_line(fields.at("line_4A"));

	} else {//do the unlocking
		//switch the IENCH tile back to the default gray
		fields.at("line_2")->at("IENCH").change_tile_background("andy_tile.png");
		unlock_line(fields.at("line_4A"));
	}
  } catch (out_of_range& map_error){
	logger_access->push_error("From: manager::iench_locking| Critical tiles associated with IENCH were not found,",
				"please check the tile and HF config files.");
  }


}*/

void manager::ilv1_locking(){
	try{
		field* ilv1_field = fields.at("line_5")->at("ILV1");
		string ilv1_text  = ilv1_field->get_text();
		bool ilv1_locking = ilv1_field->am_I_locking;

		//do locking that pertains to ILV1
		regex ilv1_good(RE_ILV1);
		if(!regex_match(ilv1_text,ilv1_good) && !ilv1_locking){
			//make it purplish pink to indicate that it is locking other parameters
			ilv1_field->change_tile_background("purple_andy_tile.png");
			ilv1_field->am_I_locking = true;

			// need to loop over the map instead
			lock_line(*fields.at("line_5A"));

		//do the unlocking
		} else if(regex_match(ilv1_text,ilv1_good) && ilv1_locking){
			//set it back to the default gray
			ilv1_field->change_tile_background("andy_tile.png");
			ilv1_field->am_I_locking = false;

			unlock_line(*fields.at("line_5A"));
		}

	} catch (out_of_range& map_error){
	string err = "From: manager::iench_locking| One of the critical";
	err       += " tiles associated with ILV1 were not found,";
	err       += " please check the tile and HF config files.";
	logger_access->push_error(err);
  }
}

void manager::icntrl4_locking(){
  try{
	regex icntrl4_pattern(RE_ICNTRL4);

	field* icntrl4_field    = fields.at("line_6")->at("ICNTRL4");
	bool icntrl4_is_locking = icntrl4_field->am_I_locking;

	bool icntrl4_good = regex_match(icntrl4_field->get_text(),icntrl4_pattern);

	if(!icntrl4_good && !icntrl4_is_locking){
		//make it purple to indicate it is locking other variables
		icntrl4_field->change_tile_background("purple_andy_tile.png");
		icntrl4_field->am_I_locking = true;

		lock_line(*fields.at("line_8"));

		if( !(button_access->get_icntrl_4().get_is_locked()) ){
			button_access->get_icntrl_4().toggle_lock();
		}

	} else if(icntrl4_good && icntrl4_is_locking){ //do the unlocking

		//make it gray again, as it is no longer locking
		icntrl4_field->change_tile_background("andy_tile.png");
		icntrl4_field->am_I_locking = false;

		unlock_line(*fields.at("line_8"));

		ich4_nch4_locking();
	}

  } catch( out_of_range& map_error){
	string err = "From: manager::icntrl4_locking| One of the critical";
	err       += " tiles associated with ICNTRL4, were not found";
	err       += ", please check that the tile and HF config files match.";
	logger_access->push_error(err);
  }
}

//this function is checked at the end of icntrl4 check lock, because the
//parameters icntrl4 unlocks must also be set up to unlock the form button
void manager::ich4_nch4_locking(){
  try{

	regex ich4_unlock(RE_ICH4_UNLOCK);
	//if both of it's params are correctly set up

	field* ich4_field = fields.at("line_8")->at("ICH4");

	string test_ich4 = ich4_field->get_text();
	if(ich4_field->am_I_locking && regex_match(test_ich4,ich4_unlock)){
		ich4_field->change_tile_background("andy_tile.png");
		ich4_field->am_I_locking = false;

	} else if(!regex_match(test_ich4,ich4_unlock)){
		ich4_field->change_tile_background("purple_andy_tile.png");
		ich4_field->am_I_locking = true;
	}

	field* nch4_field = fields.at("line_8")->at("NCH4");

	int test_nch4 = stoi(nch4_field->get_text());
	if( nch4_field->am_I_locking && (test_nch4 > 0 && test_nch4 < 101 )){

		nch4_field->change_tile_background("andy_tile.png");
		nch4_field->am_I_locking = false;

	} else if( !(test_nch4 > 0 && test_nch4 < 101) ){

		nch4_field->change_tile_background("purple_andy_tile.png");
		nch4_field->am_I_locking = true;
	}

	if( button_access->get_icntrl_4().get_is_locked() &&
		(!(ich4_field->am_I_locking) && !(nch4_field->am_I_locking) )){

		//unlock the button, both are satisfied
		button_access->get_icntrl_4().toggle_lock();

	} else if( (ich4_field->am_I_locking || nch4_field->am_I_locking)
		   && !(button_access->get_icntrl_4().get_is_locked()) ){
		//lock the button
		button_access->get_icntrl_4().toggle_lock();
	}


  } catch( out_of_range& map_error){
	string msg = "From: manager::ich4_nch4_locking| One of the critical tiles";
	msg       += "associated with ICH4/NCH4, were not found,";
	msg       +=  " please check that the tile and HF config files match.";
	logger_access->push_msg(msg);

  } catch( invalid_argument& bad_arg){
	string msg = "From manager::ich4_nch4_locking| NCH4 or ICH4 has a number";
	msg       += " in an illegal range";
	logger_access->push_msg(msg);

	msg = ". The acceptable range for NCH4 is 0<=x<=100. The acceptable";
	msg = " range for ICH4 is 0 <= ICH4 <= 6.";
	logger_access->push_msg(msg);
  }
}

void manager::icntrl8_locking(){
  try{
	regex icntrl8_unlock(RE_ICNTRL8_UNLOCK);

	field* icntrl8_field = fields.at("line_6")->at("ICNTRL8");

	string icntrl8_str = icntrl8_field->get_text();
	int icntrl8_val    = stoi(icntrl8_field->get_text());

	bool icntrl8_in_range = (icntrl8_val > 0 && icntrl8_val < 332);

	bool icntrl8_locking = icntrl8_field->am_I_locking;

	//if it is currently in locking mode, and it shouldn't be, then change its mode
	if( (regex_match(icntrl8_str,icntrl8_unlock) && icntrl8_in_range) &&
		icntrl8_field->am_I_locking ){

		//update background color
		icntrl8_field->change_tile_background("andy_tile.png");

		//make it stop locking
		icntrl8_field->am_I_locking = false;

	//if it failed the above check, and it's currently unlocked, then re-lock it
	} else if( !icntrl8_in_range && !icntrl8_locking){
		icntrl8_field->change_tile_background("purple_andy_tile.png");
		icntrl8_field->am_I_locking = true;
	}

	//toggle if the conditions are met for unlocking, and it is currently
	//locked, or if the conditions are not met and it is unlocked
	if( ( !(icntrl8_field->am_I_locking) && button_access->get_icntrl_8().get_is_locked() ) ||
		(icntrl8_field->am_I_locking && !button_access->get_icntrl_8().get_is_locked()) ){
		button_access->get_icntrl_8().toggle_lock();
	}

  } catch (out_of_range& map_error){
	string err = "From: manager::icntrl8_locking()| ICNTRL8 was not found";
	err       += " in the map of parameter tiles, please check that";
	err       += "the tile and HF config files match.";
	logger_access->push_error(err);

  } catch (invalid_argument& stoi_error){
	logger_access->push_msg("ICNTRL8 has an illegal string argument, it must be an integer in the range");
	logger_access->push_msg(" 0 <= ICNTRL8 < 332");
	fields.at("line_6")->at("ICNTRL8")->change_tile_background("purple_andy_tile.png");
	fields.at("line_6")->at("ICNTRL8")->am_I_locking = true;
  }
}

void manager::icntrl10_locking(){
  try{

	regex icntrl10_unlock(RE_ICNTRL10_UNLOCK);

	field * icntrl10_field = fields.at("line_6")->at("ICNTRL10");

	int icntrl10_val    = stoi(icntrl10_field->get_text());
	string icntrl10_str = icntrl10_field->get_text();

	field* nnsig_field = fields.at("line_11")->at("NNSIG");

	int nnsig_val      = stoi(nnsig_field->get_text());
	bool nnsig_locking = nnsig_field->am_I_locking;

	if(!nnsig_field->is_locked && nnsig_locking && nnsig_val > 0){
		nnsig_field->am_I_locking = false;
		nnsig_field->change_tile_background("andy_tile.png");
	} else if((nnsig_field->is_locked || !(nnsig_val > 0)) && !nnsig_locking){
		nnsig_field->am_I_locking = true;
		nnsig_field->change_tile_background("purple_andy_tile.png");
	}

	bool icntrl10_locking     = icntrl10_field->am_I_locking;
	bool icntrl10_valid_input = regex_match(icntrl10_str,icntrl10_unlock);

	//if icntrl10 tile is currently locking its button, and the conditions
	//for unlocking it are true, then have it start unlocking
	if(icntrl10_locking && nnsig_field->is_locked &&
	   icntrl10_valid_input && icntrl10_val > 0){

		icntrl10_field->change_tile_background("andy_tile.png");
		icntrl10_field->am_I_locking = false;
		nnsig_field->is_locked = false;

	//otherwise, if it is not locking, and it's conditions are not met,
	//then have it start locking
	} else if(!icntrl10_locking && !nnsig_field->is_locked &&
		   !(icntrl10_valid_input && icntrl10_val > 0)){

		icntrl10_field->change_tile_background("purple_andy_tile.png");
		icntrl10_field->am_I_locking = true;
		nnsig_field->is_locked       = true;

	}

	bool icntrl10_button_locked = button_access->get_icntrl_10().get_is_locked();

	if( ( !(icntrl10_field->am_I_locking) &&
		  !nnsig_field->am_I_locking &&
		  icntrl10_button_locked) ||
		  (icntrl10_field->am_I_locking &&
		  !(icntrl10_button_locked) ) ){
			button_access->get_icntrl_10().toggle_lock();

	} else if( !icntrl10_button_locked &&
			   (nnsig_field->is_locked || nnsig_field->am_I_locking) ){

		button_access->get_icntrl_10().toggle_lock();
	}

  } catch (out_of_range& map_error){
	logger_access->push_error("From: manager::icntrl8_locking()| ICNTRL10 was not found in the tiles map,",
				" please check that the tile and HF config files match");

  } catch (invalid_argument& stoi_error){
	logger_access->push_msg("ICNTRL10 has an illegal string argument, it must be an integer in the range");
	logger_access->push_msg(" 0 <= ICNTRL10");
	field * icntrl10_field = fields.at("line_6")->at("ICNTRL10");
	icntrl10_field->change_tile_background("purple_andy_tile.png");
	icntrl10_field->am_I_locking = true;
  }
}

void manager::ilv3_ilv5_locking(){

  try{
	regex ilv3_ilv5_unlock(RE_ILV3_ILV5_UNLOCK);

	field* ilv3_field = fields.at("line_5")->at("ILV3");
	field* ilv5_field = fields.at("line_5")->at("ILV5");

	//do checks for ILV3
	ilv3_ilv5_locking_helper(ilv3_field,ilv3_ilv5_unlock);
	ilv3_ilv5_locking_helper(ilv5_field,ilv3_ilv5_unlock);

	//store the truth values that we care about in local variables,
	//so the logic below is slightly more readable
	bool i3_locking = ilv3_field->am_I_locking;
	bool i5_locking = ilv5_field->am_I_locking;
	bool i3_i5_locked = button_access->get_ilv3_ilv5().get_is_locked();

	/*if the form button is locked, and
	 *ILV3 XOR ILV5 (one is true, one is false), then unlock it (by toggling)
	 *or, if the form button is not locked, and ILV3 XOR ILV5 is false
	 *(both are false, or both are true), then toggle */
	if( (i3_i5_locked && ((i3_locking && !i5_locking) || ((!i3_locking && i5_locking))))
		|| (!i3_i5_locked && !((i3_locking && !i5_locking) || ((!i3_locking && i5_locking))))){
		button_access->get_ilv3_ilv5().toggle_lock();
	}

  } catch (out_of_range& map_error){
	logger_access->push_error("From: manager::ilv3_ilv5_locking()| ILV3 or ILV5was not found in the tiles map,",
				 " ensure that the tile and HF config files match");

  } catch (invalid_argument& stoi_error){
	logger_access->push_msg("ILV3 or ILV5 has an illegal string argument, it must be an integer in the range");
	logger_access->push_msg(" 0 <= (ILV3 or ILV5)");
	field* ilv3_field = fields.at("line_5")->at("ILV3");
	field* ilv5_field = fields.at("line_5")->at("ILV5");
	ilv3_field->change_tile_background("purple_andy_tile.png");
	ilv3_field->am_I_locking = true;
	ilv5_field->change_tile_background("purple_andy_tile.png");
	ilv5_field->am_I_locking = true;
  }

}

void manager::ilv3_ilv5_locking_helper(field* this_field,const regex& unlock_condition){

	if (this_field == NULL){
		logger_access->push_error("ILV3/ILV5 helper given NULL pointer");
	}

	//nab raw string from field parameter
	string field_text = this_field->get_text();

	//cast the string to an int
	int field_val = stoi(field_text);

	//if the target tile is currently locking its button, and the conditions
	//for unlocking it are true, then have it start unlocking
	if( this_field->am_I_locking &&
		(regex_match(field_text,unlock_condition) && field_val > 0) ){

		this_field->change_tile_background("andy_tile.png");
		this_field->am_I_locking = false;

	//if target_param is not currently locking it's form button,
	//and the conditions for it being unlocked are not met,
	//then make it start locking again
	} else if( !this_field->am_I_locking &&
		   !(regex_match(field_text,unlock_condition) && field_val > 0) ){
		this_field->change_tile_background("purple_andy_tile.png");
		this_field->am_I_locking = true;
	}
}

void manager::icntrl6_locking(){
  try{
	regex icntrl6_unlock(RE_ICNTRL6_UNLOCK);

	field* icntrl6_field = fields.at("line_6")->at("ICNTRL6");

	bool icntrl6_locking = fields.at("line_6")->at("ICNTRL6")->am_I_locking;

	string icntrl6_str = icntrl6_field->get_text();

	bool valid_input = regex_match(icntrl6_str,icntrl6_unlock);

	//if icntrl6 is 1 or 2, unlock
	if(valid_input && icntrl6_locking){
		//unlock params now that icntrl6 > 0
		icntrl6_field->change_tile_background("andy_tile.png");
		icntrl6_field->am_I_locking = false;

		unlock_line(*fields.at("line_10"));
	} else if(!valid_input && !icntrl6_locking){
		//elsewise, lock them
		icntrl6_field->change_tile_background("purple_andy_tile.png");
		icntrl6_field->am_I_locking = true;

		lock_line(*fields.at("line_10"));
	}

	//######### block originally below the try/catch#################

	if(!icntrl6_locking){
		//these functions set the am_I_locking states for each
		//of the sub-parameters of ICNTRL6
		inm1_locking();
		inm2_locking();
		iter_locking();

		//the sub states are saved to local variables here so it's not
		//as long of a statement
		bool inm1_lock_status = fields.at("line_10")->at("INM1")->am_I_locking;
		bool inm2_lock_status = fields.at("line_10")->at("INM2")->am_I_locking;
		bool iter_lock_status = fields.at("line_10")->at("ITER")->am_I_locking;
		bool icntrl6_locked = button_access->get_icntrl_6().get_is_locked();

		//the following assumes that ITER must be greater than 0, and atleast
		//one of INM1 or INM2 must be nonzero if form_button is not locked,
		// and all are locking, then lock it
		if( (icntrl6_locked && !iter_lock_status && (!inm1_lock_status || !inm2_lock_status)) ||
			(!icntrl6_locked && iter_lock_status) ||
			(!icntrl6_locked && !iter_lock_status && (inm1_lock_status && inm2_lock_status))

		  ){
			button_access->get_icntrl_6().toggle_lock();
		}
	}
	//######### block originally below the try/catch#################

  } catch (out_of_range& map_error){
	logger_access->push_error("From: manager::icntrl6_locking| One of the critical tiles associated with ICNTRL4",
				" were not found, please check that tile and HF config files match.");
  }

}

void manager::inm1_locking(){
  try{

	field* inm1_field = fields.at("line_10")->at("INM1");

	int inm1_val    = stoi(inm1_field->get_text());
	string inm1_str = inm1_field->get_text();
	regex inm1_good(RE_INM1_UNLOCK);

	if( inm1_field->am_I_locking && (regex_match(inm1_str,inm1_good) && inm1_val > 0) ){
		inm1_field->change_tile_background("andy_tile.png");
		inm1_field->am_I_locking = false;

	} else if( !(inm1_field->am_I_locking) && (!(inm1_val > 0) || !(regex_match(inm1_str,inm1_good))) ){
		inm1_field->change_tile_background("purple_andy_tile.png");
		inm1_field->am_I_locking = true;;
	}

  } catch (out_of_range& map_error){
	logger_access->push_error("From: manager: inm1_locking| INM1 was not found in the parameter map",
				", please check that the tile and HF config files match");
  } catch (invalid_argument& stoi_error){
	logger_access->push_msg("INM1 has an illegal string argument, it must be an integer in the range ");
	logger_access->push_msg(" 0 <= INM1");
	fields.at("line_10")->at("INM1")->change_tile_background("purple_andy_tile.png");
	fields.at("line_10")->at("INM1")->am_I_locking = true;
  }

}

void manager::inm2_locking(){
  try{

  	field* inm2_field = fields.at("line_10")->at("INM2");
	int inm2_val      = stoi(inm2_field->get_text());

	if( inm2_field->am_I_locking && inm2_val > 0 ){
		inm2_field->change_tile_background("andy_tile.png");
		inm2_field->am_I_locking = false;

	} else if( !(inm2_field->am_I_locking) && !(inm2_val > 0) ){
		inm2_field->change_tile_background("purple_andy_tile.png");
		inm2_field->am_I_locking = true;;
	}

  } catch (out_of_range& map_error){
	logger_access->push_error("From: manager: inm2_locking| INM2 was not found in the parameter map",
				", please check that the tile and HF config files match");
  } catch (invalid_argument& stoi_error){
	logger_access->push_msg("INM2 has an illegal string argument, it must be an integer in the range ");
	logger_access->push_msg(" 0 <= INM2");
	fields.at("line_10")->at("INM2")->change_tile_background("purple_andy_tile.png");
	fields.at("line_10")->at("INM2")->am_I_locking = true;
  }

}

void manager::iter_locking(){
  try{
	field* iter_field = fields.at("line_10")->at("ITER");
	int iter_val      = stoi(iter_field->get_text());

	if( iter_field->am_I_locking && iter_val > 0 ){
		iter_field->change_tile_background("andy_tile.png");
		iter_field->am_I_locking = false;

	} else if( !(iter_field->am_I_locking) && !(iter_val > 0) ){
		iter_field->change_tile_background("purple_andy_tile.png");
		iter_field->am_I_locking = true;;
	}

  } catch (out_of_range& map_error){
	logger_access->push_error("From: manager: iter_locking| ITER was not found in the parameter map",
				", please check that the tile and HF config files match");
  } catch (invalid_argument& stoi_error){
	logger_access->push_msg("ITER has an illegal string argument, it must be an integer in the range ");
	logger_access->push_msg(" 0 <= ITER");

	fields.at("line_10")->at("ITER")->change_tile_background("purple_andy_tile.png");
	fields.at("line_10")->at("ITER")->am_I_locking = true;
  }
}


// non-member helpers
void print_matching_message(const string& match_case, int expected_groups){
	string logger_msg_base = "Potential parsing error! Incorrect number of regex matches for ";

	string logger_msg = logger_msg_base + match_case + " " + to_string(expected_groups);

	logger_access->push_msg(logger_msg);
}

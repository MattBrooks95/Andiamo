//! \file manager.cc implements the functions declared in manager.h
#include "manager.h"
#include "button_manager.h"

#include "define.h"
#include "regex_patterns.h"
using namespace std;

//prototype for sorting function passed to algorithm::sort
bool compare_width(field& left, field& right);

manager::manager(string image_p_in){
    image_p = image_p_in;
}

bool man_test = true;

void manager::init(const string& graphical_config_file){

    tile_input_p = HOME + "/Andiamo/tile_Input/";

    //ins will refer to the stream to the input file for tile information
    fstream ins;

    //open the file

    if(graphical_config_file.size() == 0){
        ins.open((tile_input_p + "tiles.txt").c_str());
    } else {
        ins.open((tile_input_p + graphical_config_file));
    }

    //if something went wrong, print an error message to console
    if(ins.fail()){
        string err = "Fatal error: Failed to open tile input file: ";
        err       += tile_input_p+"tiles.txt";
        error_logger.push_error(err);
    }

    //used to store unidentified line
    string temp_string;

    //this line specifies an image name
    regex img_pattern(RE_IMG);

    //this recognizes lines that specify tile size lines should be of the
    //form widthxheight EX:100x100
    regex field_size_pattern(RE_FIELD_SIZE);

    //this line specifies a tile name
    regex name_pattern(RE_TILE_NAME);

    //used to tell if the name line is of the form ->HFvariable:EnglishVariable
    regex semi_pattern(RE_SEMI);

    //describes a pattern for tile/input descriptors that starts with a 'c'
    //and is followed by exactly one space,
    //then contains any number of any characters
    regex desc_pattern(RE_DESCRIPTION);

    //this line recognizes the lines that separate
    //the parameters into lines that correspond with the input manual,
    //so they can be stored together
    regex line_separator(RE_LINE_SEPARATOR);


    getline(ins,temp_string);//priming read

    //loop over the entire tile_Input/tiles.txt configuration file
    while(!ins.eof() ){

        //get out on potentially erroneous last run
        if(ins.fail()) break;

        //reset new line container each run of loop
        map<string,field*> new_line;
        vector<field*> params_vector;


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

        strip_char(temp_string,'#');
        //cout << "Found a line name:"+temp_string << endl;
        error_logger.push_msg("Found a line name:"+temp_string);
        line_names_read_order.push_back(temp_string);

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

                    error_logger.push_msg("Found a description line.: "+
                                          temp_string);

                    //remove 'c ' at start of desc lines
                    description = temp_string.erase(0,2);
                    temp_descriptions.push_back(temp_string);


                } else if( regex_match(temp_string,field_size_pattern) ){

                    error_logger.push_msg("Found field size specification!: "+
                                          temp_string);

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
                        string err = "Error in manager::init(), tile given:";
                        err       +=    "illegal size parameters: ";
                        err       += dimensions[0]+"x"+dimensions[1];
                        error_logger.push_error(err);
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

                    string err = "Error! This line failed to hit a case in the";
                    err       += " regex checks:"+temp_string;
                    err       += "\nIt may be a missing 'Andy' separator";
                    err       += " in the tiles.txt config file.";
                    error_logger.push_error(err);
                
                }


                if( !ins.fail() ){
                    //read in again to update loop
                    getline(ins,temp_string);
                } else {
                    return;
                }
            }
            field* temp_field = NULL;
            temp_field = new field(tile_name,display_name,img_name,tile_w,tile_h);

            //copy the saved description lines to the new field
            //before it is placed in the map
            for(uint c = 0; c < temp_descriptions.size();c++){
                temp_field->descriptions.push_back(temp_descriptions[c]);
            }

            error_logger.push_msg("##########PUSHING FIELD###################");
            temp_field->print();
            //cout << temp_field->tile_name << endl;
            error_logger.push_msg("##########################################");

            //push the field into the lookup map for that parameter's line
            new_line.emplace(tile_name,temp_field);

            //also push the field into the parameter vector
            params_vector.push_back(temp_field);            

            if( !ins.fail() ){
                //"andy" is the current line, so go ahead and read the next one
                getline(ins,temp_string);
            }

            temp_field = NULL;
        }

        //at this point, we have hit the separator for another
        //group of parameters

        //save the order of the lines, so they can be drawn in order to
        //the screen line_order.push_back(line_name);

        //store the map of parameters in the map of lines, and
        //give it the name we found earlier
        fields.emplace(line_name,new_line);
        fields_order.push_back(params_vector);
    }

    //close the file
    ins.close();

    if(man_test){
        error_logger.push_msg("FIELD MAP AFTER MANAGER.init():");
        print_all();
        error_logger.push_msg("##############################################");
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

void manager::init_fields_graphics(){



    for(uint line = 0; line < fields_order.size();line++){

        for(uint param = 0; param < fields_order[line].size();param++){

            if(fields_order[line][param] == NULL){
                cout << "NULL POINTER IN TILE VECTOR."
                     << "Line: " << line << " Param: " << param << endl;
            } else {
                fields_order[line][param]->graphics_init(image_p);
            }
        }

    }

}

void manager::draw(){

    check_locks();

    vector<field*> drawn_second;


    for(uint line = 0; line < fields_order.size();line++){

        for(uint param = 0; param < fields_order[line].size();param++){

            if(fields_order[line][param] == NULL){
                cout << "NULL pointer in manager::draw()" << endl;
                continue;
            }
            if(!fields_order[line][param]->is_help_mode()){
                fields_order[line][param]->draw_me();
            } else {
                drawn_second.push_back(fields_order[line][param]);
            }

        }
    }

    for(uint c = 0; c < drawn_second.size();c++){

        //now the help mode tiles can be drawn
        drawn_second[c]->draw_me();
    }

}

void manager::update_win(int width_in,int height_in){
    win_w = width_in;
    win_h = height_in;
}


void manager::print_all(){

    error_logger.push_msg("\n Printing parameters in order #################");

    for(uint line = 0; line < fields_order.size();line++){
        error_logger.push_msg("LINE VECTOR INDEX: "+to_string(line));
        uint num_params = fields_order[line].size();
        for(uint param = 0; param < num_params; param++){

            fields_order[line][param]->print();

        }

    }

    error_logger.push_msg("\n End params order print #######################");

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
    if(!regex_match(fields.at("line_2").at("IENCH").my_text_box.text,iench_good)){
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


}*/


void manager::ilv1_locking(){
  try{
    //do locking that pertains to ILV1
    regex ilv1_good(RE_ILV1);
    if(!regex_match(fields.at("line_5").at("ILV1")->my_text_box.text,ilv1_good)){
        //make it purplish pink to indicate that it is locking other parameters
        fields.at("line_5").at("ILV1")->change_tile_background("purple_andy_tile.png");

        fields.at("line_5A").at("ACON")->is_locked = true;
        fields.at("line_5A").at("GAM")->is_locked  = true;
        fields.at("line_5A").at("FCON")->is_locked = true;
        fields.at("line_5A").at("C0")->is_locked   = true;
        fields.at("line_5A").at("C10")->is_locked  = true;
        fields.at("line_5A").at("C11")->is_locked  = true;
        fields.at("line_5A").at("C12")->is_locked  = true;
        fields.at("line_5A").at("C3")->is_locked   = true;

    } else { //do the unlocking
        //set it back to the default gray
        fields.at("line_5").at("ILV1")->change_tile_background("andy_tile.png");

        fields.at("line_5A").at("ACON")->is_locked = false;
        fields.at("line_5A").at("GAM")->is_locked  = false;
        fields.at("line_5A").at("FCON")->is_locked = false;
        fields.at("line_5A").at("C0")->is_locked   = false;
        fields.at("line_5A").at("C10")->is_locked  = false;
        fields.at("line_5A").at("C11")->is_locked  = false;
        fields.at("line_5A").at("C12")->is_locked  = false;
        fields.at("line_5A").at("C3")->is_locked   = false;
    }

  } catch (out_of_range& map_error){
    string err = "From: manager::iench_locking| One of the critical";
    err       += " tiles associated with ILV1 were not found,";
    err       += " please check the tile and HF config files.";
    error_logger.push_error(err);

  }

}

void manager::icntrl4_locking(){
  try{
    regex icntrl4_good(RE_ICNTRL4);

    if( !regex_match(fields.at("line_6").at("ICNTRL4")->my_text_box.text,icntrl4_good) ){
        //make it purple to indicate it is locking other variables
        fields.at("line_6").at("ICNTRL4")->change_tile_background("purple_andy_tile.png");

        fields.at("line_8").at("ICH4")->is_locked  = true;
        fields.at("line_8").at("NCH4")->is_locked  = true;
        if( !(button_access->get_icntrl_4().get_is_locked()) ){
            button_access->get_icntrl_4().toggle_lock();
        } 


    } else { //do the unlocking

        //make it gray again, as it is no longer locking
        fields.at("line_6").at("ICNTRL4")->change_tile_background("andy_tile.png");

        fields.at("line_8").at("ICH4")->is_locked  = false;
        fields.at("line_8").at("NCH4")->is_locked  = false;


        ich4_nch4_locking();

    }

  } catch( out_of_range& map_error){
    string err = "From: manager::icntrl4_locking| One of the critical";
    err       += " tiles associated with ICNTRL4, were not found";
    err       += ", please check that the tile and HF config files match.";
    error_logger.push_error(err); 

  }

}

//this function is checked at the end of icntrl4 check lock, because the
//parameters icntrl4 unlocks must also be set up to unlock the form button
void manager::ich4_nch4_locking(){
  try{

    regex ich4_unlock(RE_ICH4_UNLOCK);
    //if both of it's params are correctly set up

    string test_ich4 = fields.at("line_8").at("ICH4")->my_text_box.text;
    if( fields.at("line_8").at("ICH4")->am_I_locking && regex_match(test_ich4,ich4_unlock) ){
        fields.at("line_8").at("ICH4")->change_tile_background("andy_tile.png");
        fields.at("line_8").at("ICH4")->am_I_locking = false;

    } else if( !regex_match(test_ich4,ich4_unlock) ){
        fields.at("line_8").at("ICH4")->change_tile_background("purple_andy_tile.png");
        fields.at("line_8").at("ICH4")->am_I_locking = true;
    }


    int test_nch4 = stoi(fields.at("line_8").at("NCH4")->my_text_box.text);
    if( fields.at("line_8").at("NCH4")->am_I_locking && (test_nch4 > 0 && test_nch4 < 101 )){

        fields.at("line_8").at("NCH4")->change_tile_background("andy_tile.png");
        fields.at("line_8").at("NCH4")->am_I_locking = false;

    } else if( !(test_nch4 > 0 && test_nch4 < 101) ){

        fields.at("line_8").at("NCH4")->change_tile_background("purple_andy_tile.png");
        fields.at("line_8").at("NCH4")->am_I_locking = true;
    }

    if( button_access->get_icntrl_4().get_is_locked() && 
        (!(fields.at("line_8").at("ICH4")->am_I_locking) && !(fields.at("line_8").at("NCH4")->am_I_locking)) ){

        //unlock the button, both are satisfied
        button_access->get_icntrl_4().toggle_lock();

    } else if( (fields.at("line_8").at("ICH4")->am_I_locking || fields.at("line_8").at("NCH4")->am_I_locking)
           && !(button_access->get_icntrl_4().get_is_locked()) ){
        //lock the button
        button_access->get_icntrl_4().toggle_lock();
    }


  } catch( out_of_range& map_error){
    string msg = "From: manager::ich4_nch4_locking| One of the critical tiles";
    msg       += "associated with ICH4/NCH4, were not found,";
    msg       +=  " please check that the tile and HF config files match.";
    error_logger.push_msg(msg);

  } catch( invalid_argument& bad_arg){
    string msg = "From manager::ich4_nch4_locking| NCH4 or ICH4 has a number";
    msg       += " in an illegal range";
    error_logger.push_msg(msg);

    msg = ". The acceptable range for NCH4 is 0<=x<=100. The acceptable";
    msg = " range for ICH4 is 0 <= ICH4 <= 6.";
    error_logger.push_msg(msg);
  }

}

void manager::icntrl8_locking(){
  try{
    regex icntrl8_unlock(RE_ICNTRL8_UNLOCK);

    string icntrl_8_str = fields.at("line_6").at("ICNTRL8")->my_text_box.text;
    int icntrl_8_val = stoi(fields.at("line_6").at("ICNTRL8")->my_text_box.text);
    //if it is currently in locking mode, and it shouldn't be, then change its mode
    if( (regex_match(icntrl_8_str,icntrl8_unlock) && (icntrl_8_val > 0 && icntrl_8_val < 332) ) &&
        fields.at("line_6").at("ICNTRL8")->am_I_locking ){

        //update background color
        fields.at("line_6").at("ICNTRL8")->change_tile_background("andy_tile.png");

        //make it stop locking
        fields.at("line_6").at("ICNTRL8")->am_I_locking = false;

    //if it failed the above check, and it's currently unlocked, then re-lock it
    } else if( !(icntrl_8_val > 0 && icntrl_8_val < 332) ){
        fields.at("line_6").at("ICNTRL8")->change_tile_background("purple_andy_tile.png");
        fields.at("line_6").at("ICNTRL8")->am_I_locking = true;

    }

    //toggle if the conditions are met for unlocking, and it is currently
    //locked, or if the conditions are not met and it is unlocked
    if( ( !(fields.at("line_6").at("ICNTRL8")->am_I_locking) && button_access->get_icntrl_8().get_is_locked() ) ||
        (fields.at("line_6").at("ICNTRL8")->am_I_locking && !button_access->get_icntrl_8().get_is_locked()) ){
        button_access->get_icntrl_8().toggle_lock();
    }

  } catch (out_of_range& map_error){
    string err = "From: manager::icntrl8_locking()| ICNTRL8 was not found";
    err       += " in the map of parameter tiles, please check that";
    err       += "the tile and HF config files match.";
    error_logger.push_error(err);

  } catch (invalid_argument& stoi_error){
    error_logger.push_msg("ICNTRL8 has an illegal string argument, it must be an integer in the range");
    error_logger.push_msg(" 0 <= ICNTRL8 < 332");
    fields.at("line_6").at("ICNTRL8")->change_tile_background("purple_andy_tile.png");
    fields.at("line_6").at("ICNTRL8")->am_I_locking = true;

  }


}

void manager::icntrl10_locking(){
  try{

    regex icntrl10_unlock(RE_ICNTRL10_UNLOCK);
    int icntrl10_val = stoi(fields.at("line_6").at("ICNTRL10")->my_text_box.text);
    string icntrl10_str = fields.at("line_6").at("ICNTRL10")->my_text_box.text;


    int nnsig_val = stoi(fields.at("line_11").at("NNSIG")->my_text_box.text);

    if( !fields.at("line_11").at("NNSIG")->is_locked &&
        nnsig_val > 0){

        fields.at("line_11").at("NNSIG")->am_I_locking = false;
        fields.at("line_11").at("NNSIG")->change_tile_background("andy_tile.png");
    } else {

        fields.at("line_11").at("NNSIG")->am_I_locking = true;
        fields.at("line_11").at("NNSIG")->change_tile_background("purple_andy_tile.png");
    }

    //if icntrl10 tile is currently locking its button, and the conditions
    //for unlocking it are true, then have it start unlocking
    if( fields.at("line_6").at("ICNTRL10")->am_I_locking &&
        (regex_match(icntrl10_str,icntrl10_unlock) && icntrl10_val > 0) ){

        fields.at("line_6").at("ICNTRL10")->change_tile_background("andy_tile.png");
        fields.at("line_6").at("ICNTRL10")->am_I_locking = false;

        fields.at("line_11").at("NNSIG")->is_locked = false;
        



    //otherwise, if it is not locking, and it's conditions are not met,
    //then have it start locking
    } else if( !(fields.at("line_6").at("ICNTRL10")->am_I_locking) && 
           !(regex_match(icntrl10_str,icntrl10_unlock) && icntrl10_val > 0) ){

        fields.at("line_6").at("ICNTRL10")->change_tile_background("purple_andy_tile.png");
        fields.at("line_6").at("ICNTRL10")->am_I_locking = true;
        fields.at("line_11").at("NNSIG")->is_locked = true;

    }


    if( ( !(fields.at("line_6").at("ICNTRL10")->am_I_locking) &&
          !fields.at("line_11").at("NNSIG")->am_I_locking &&
          button_access->get_icntrl_10().get_is_locked() ) ||
          (fields.at("line_6").at("ICNTRL10")->am_I_locking &&
          !(button_access->get_icntrl_10().get_is_locked()) ) ){
            button_access->get_icntrl_10().toggle_lock();

    } else if( !button_access->get_icntrl_10().get_is_locked() &&
               (fields.at("line_11").at("NNSIG")->is_locked ||
               fields.at("line_11").at("NNSIG")->am_I_locking) ){

        button_access->get_icntrl_10().toggle_lock();

    }

  } catch (out_of_range& map_error){
    error_logger.push_error("From: manager::icntrl8_locking()| ICNTRL10 was not found in the map of parameter tiles,",
                " please check that the tile and HF config files match");

  } catch (invalid_argument& stoi_error){
    error_logger.push_msg("ICNTRL10 has an illegal string argument, it must be an integer in the range");
    error_logger.push_msg(" 0 <= ICNTRL10");
    fields.at("line_6").at("ICNTRL10")->change_tile_background("purple_andy_tile.png");
    fields.at("line_6").at("ICNTRL10")->am_I_locking = true;


  }




}

void manager::ilv3_ilv5_locking(){

  try{

    regex ilv3_ilv5_unlock(RE_ILV3_ILV5_UNLOCK);

    //do checks for ILV3
    ilv3_ilv5_locking_helper("ILV3",ilv3_ilv5_unlock);
    ilv3_ilv5_locking_helper("ILV5",ilv3_ilv5_unlock);



    //store the truth values that we care about in local variables,
    //so the logic below is slightly more readable
    bool i3_locking = fields.at("line_5").at("ILV3")->am_I_locking;
    bool i5_locking = fields.at("line_5").at("ILV5")->am_I_locking;
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
    error_logger.push_error("From: manager::ilv3_ilv5_locking()| ILV3 or ILV5was not found in the map of parameter"
                 " tiles, please check that the tile and HF config files match");

  } catch (invalid_argument& stoi_error){
    error_logger.push_msg("ILV3 or ILV5 has an illegal string argument, it must be an integer in the range");
    error_logger.push_msg(" 0 <= (ILV3 or ILV5)");
    fields.at("line_5").at("ILV3")->change_tile_background("purple_andy_tile.png");
    fields.at("line_5").at("ILV3")->am_I_locking = true;
    fields.at("line_5").at("ILV5")->change_tile_background("purple_andy_tile.png");
    fields.at("line_5").at("ILV5")->am_I_locking = true;


  }

}

void manager::ilv3_ilv5_locking_helper(const string& target_param,const regex& unlock_condition){

    //nab raw string from field parameter
    string target_str = fields.at("line_5").at(target_param)->my_text_box.text;

    //cast the string to an int
    int target_val = stoi(target_str);

    //if the target tile is currently locking its button, and the conditions
    //for unlocking it are true, then have it start unlocking
    if( fields.at("line_5").at(target_param)->am_I_locking &&
        (regex_match(target_str,unlock_condition) && target_val > 0) ){

        fields.at("line_5").at(target_param)->change_tile_background("andy_tile.png");
        fields.at("line_5").at(target_param)->am_I_locking = false;

    //if target_param is not currently locking it's form button,
    //and the conditions for it being unlocked are not met,
    //then make it start locking again
    } else if( !fields.at("line_5").at(target_param)->am_I_locking &&
           !(regex_match(target_str,unlock_condition) && target_val > 0) ){
        fields.at("line_5").at(target_param)->change_tile_background("purple_andy_tile.png");
        fields.at("line_5").at(target_param)->am_I_locking = true;
    }

}

void manager::icntrl6_locking(){
  try{
    regex icntrl6_unlock(RE_ICNTRL6_UNLOCK);
    //if icntrl6 is 1 or 2, unlock
    if( regex_match(fields.at("line_6").at("ICNTRL6")->my_text_box.text,icntrl6_unlock) ){
        //unlock params now that icntrl6 > 0
        fields.at("line_6").at("ICNTRL6")->change_tile_background("andy_tile.png");
        fields.at("line_6").at("ICNTRL6")->am_I_locking = false;

        fields.at("line_10").at("ITER")->is_locked = false;
        fields.at("line_10").at("INM1")->is_locked = false;
        fields.at("line_10").at("INM2")->is_locked = false;
    } else {
        //elsewise, lock them
        fields.at("line_6").at("ICNTRL6")->change_tile_background("purple_andy_tile.png");
        fields.at("line_6").at("ICNTRL6")->am_I_locking = true;
        fields.at("line_10").at("ITER")->is_locked = true;
        fields.at("line_10").at("INM1")->is_locked = true;
        fields.at("line_10").at("INM2")->is_locked = true;
    }

    //######### block originally below the try/catch#################
    bool icntrl6_lock_status = fields.at("line_6").at("ICNTRL6")->am_I_locking;
    if( !icntrl6_lock_status ){
        //these functions set the am_I_locking states for each
        //of the sub-parameters of ICNTRL6
        inm1_locking();
        inm2_locking();
        iter_locking();


        //the sub states are saved to local variables here so it's not
        //as long of a statement
        bool inm1_lock_status = fields.at("line_10").at("INM1")->am_I_locking;
        bool inm2_lock_status = fields.at("line_10").at("INM2")->am_I_locking;
        bool iter_lock_status = fields.at("line_10").at("ITER")->am_I_locking;
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
    error_logger.push_error("From: manager::icntrl6_locking| One of the critical tiles associated with ICNTRL4",
                " were not found, please check that tile and HF config files match.");
  }


}

void manager::inm1_locking(){
  try{
    int inm1_val = stoi(fields.at("line_10").at("INM1")->my_text_box.text);
    string inm1_str = fields.at("line_10").at("INM1")->my_text_box.text;
    regex inm1_good(RE_INM1_UNLOCK);

    if( fields.at("line_10").at("INM1")->am_I_locking && (regex_match(inm1_str,inm1_good) && inm1_val > 0) ){
        fields.at("line_10").at("INM1")->change_tile_background("andy_tile.png");
        fields.at("line_10").at("INM1")->am_I_locking = false;

    } else if( !(fields.at("line_10").at("INM1")->am_I_locking) && (!(inm1_val > 0) || !(regex_match(inm1_str,inm1_good))) ){
        fields.at("line_10").at("INM1")->change_tile_background("purple_andy_tile.png");
        fields.at("line_10").at("INM1")->am_I_locking = true;;
    }

  } catch (out_of_range& map_error){
    error_logger.push_error("From: manager: inm1_locking| INM1 was not found in the parameter map",
                ", please check that the tile and HF config files match");
  } catch (invalid_argument& stoi_error){
    error_logger.push_msg("INM1 has an illegal string argument, it must be an integer in the range ");
    error_logger.push_msg(" 0 <= INM1");
    fields.at("line_10").at("INM1")->change_tile_background("purple_andy_tile.png");
    fields.at("line_10").at("INM1")->am_I_locking = true;
  }

}

void manager::inm2_locking(){
  try{
    int inm2_val = stoi(fields.at("line_10").at("INM2")->my_text_box.text);

    if( fields.at("line_10").at("INM2")->am_I_locking && inm2_val > 0 ){
        fields.at("line_10").at("INM2")->change_tile_background("andy_tile.png");
        fields.at("line_10").at("INM2")->am_I_locking = false;

    } else if( !(fields.at("line_10").at("INM2")->am_I_locking) && !(inm2_val > 0) ){
        fields.at("line_10").at("INM2")->change_tile_background("purple_andy_tile.png");
        fields.at("line_10").at("INM2")->am_I_locking = true;;
    }   

  } catch (out_of_range& map_error){
    error_logger.push_error("From: manager: inm2_locking| INM2 was not found in the parameter map",
                ", please check that the tile and HF config files match");
  } catch (invalid_argument& stoi_error){
    error_logger.push_msg("INM2 has an illegal string argument, it must be an integer in the range ");
    error_logger.push_msg(" 0 <= INM2");
    fields.at("line_10").at("INM2")->change_tile_background("purple_andy_tile.png");
    fields.at("line_10").at("INM2")->am_I_locking = true;
  }

}

void manager::iter_locking(){
  try{
    int iter_val = stoi(fields.at("line_10").at("ITER")->my_text_box.text);

    if( fields.at("line_10").at("ITER")->am_I_locking && iter_val > 0 ){
        fields.at("line_10").at("ITER")->change_tile_background("andy_tile.png");
        fields.at("line_10").at("ITER")->am_I_locking = false;

    } else if( !(fields.at("line_10").at("ITER")->am_I_locking) && !(iter_val > 0) ){
        fields.at("line_10").at("ITER")->change_tile_background("purple_andy_tile.png");
        fields.at("line_10").at("ITER")->am_I_locking = true;;
    }   

  } catch (out_of_range& map_error){
    error_logger.push_error("From: manager: iter_locking| ITER was not found in the parameter map",
                ", please check that the tile and HF config files match");
  } catch (invalid_argument& stoi_error){
    error_logger.push_msg("ITER has an illegal string argument, it must be an integer in the range ");
    error_logger.push_msg(" 0 <= ITER");

    fields.at("line_10").at("ITER")->change_tile_background("purple_andy_tile.png");
    fields.at("line_10").at("ITER")->am_I_locking = true;
  }

}












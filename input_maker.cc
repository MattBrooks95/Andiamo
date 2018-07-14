/*! \file input_maker.cc \brief implements the functions
 *defined in input_maker.h */

#include<regex>
#include<iomanip>

#include "input_maker.h"
#include "button_manager.h"
#include "regex_patterns.h"

#define F << setw(8) <<
#define F10 << setw(10) <<
#define F5 << setw(5) <<
#define I << setw(5) <<
#define I10 << setw(10) <<

using namespace std;

extern button_manager* button_access;
extern string HOME;

input_maker::input_maker(string output_file_name_in,string config_file_name_in){
    config_p  = HOME;
    config_p += "/Andiamo/config/";
    output_p  = HOME;
    output_p += "/Andiamo/output/";
    output_file_name = output_file_name_in;
    file_name = config_file_name_in;

    //start off false, become positive when output() is ran
    output_was_made = false;

}


void input_maker::check_map(){
    error_logger.push_msg("####### INPUT MAKER INT4 PARAMS MAP ##############");
    for(INT4_MAP::iterator int_it = int4_params.begin();
        int_it != int4_params.end();
        int_it++){
        error_logger.push_msg_no_nl(int_it->second.get_string()+"  ");
    }
    error_logger.push_msg("\n######### INPUT MAKER R8 PARAMS MAP ##########\n");
    for(REAL8_MAP::iterator r8_it = real8_params.begin();
        r8_it != real8_params.end();
        r8_it++){
        error_logger.push_msg_no_nl(r8_it->second.get_string()+":");
    }
    error_logger.push_msg("\n######### INPUT MAKER STRING PARAMS MAP ######\n");
    for(STR_MAP::iterator string_it = string_params.begin();
        string_it != string_params.end();
        string_it++){
        string msg = string_it->first + ":" + string_it->second.value+"   ";
        error_logger.push_msg_no_nl(msg);
    }
    error_logger.push_msg("\n####### INPUT MAKER INT4 ARRAY MAP ###########\n");
    for(INT4_ARR_MAP::iterator int4_array_it = int4_array_params.begin();
        int4_array_it != int4_array_params.end();
        int4_array_it++){
        string msg = int4_array_it->first + ":"
                     + int4_array_it->second.get_string() + "  ";
        error_logger.push_msg_no_nl(msg);
    }
    error_logger.push_msg("\n####### INPUT MAKER R8_ARRAY MAP #############\n");
    for(R8_ARR_MAP::iterator r8_array_it = r8_array_params.begin();
        r8_array_it != r8_array_params.end();
        r8_array_it++){
        string msg = r8_array_it->first + ":"
                     + r8_array_it->second.get_string() + "  ";
        error_logger.push_msg_no_nl(msg);
    }

}



void input_maker::init(const string& alternate_config){

    error_logger.push_msg("############ INPUT_MAKER INIT ####################");

    ifstream ins;
    if(alternate_config.size() == 0){
        ins.open( (config_p+file_name).c_str() );
    } else {
        ins.open( (config_p+"/custom_configs/"+alternate_config).c_str() );
    }

    if(ins.fail()){
        string err;
        err = "Error in input_maker::init(), couldn't find the input file!";
        error_logger.push_error(err);
        return;
    }

    //set up regex matches
    regex re_comment(RE_COMMENT);
    regex re_i4(RE_INT4);
    regex re_i4_array(RE_INT4_ARRAY);
    regex re_string(RE_STRING);
    regex re_real8(RE_REAL8);

    regex form_init(RE_FORM_INIT);

    regex string_array_size_pattern(RE_STRING_ARRAY_SIZE);
    regex int_array_size_pattern(RE_INT_ARRAY_SIZE);

    regex r8_array(RE_R8_ARRAY);
    regex nad_flag(RE_NAD_FLAG);

    string temp_string;
    string temp_name;

    getline(ins,temp_string);
    while(!ins.eof()){

        //get out, file stream is done
        if(ins.fail()) break;


        /*some sort of bad or meaningless input, try again until something of
         *value is found thanks to
         *https://stackoverflow.com/questions/9235296/
         *how-to-detect-empty-lines-while-reading-from-istream-object-in-c
         *for convincing me to check string.empty() instead of
         *"does string equal " ", "\n" or "", this check allows for "spacer"
         *empty lines in the input file to make it easier to read */
        while( temp_string.empty() ){
            if(!ins.fail()){
                getline(ins,temp_string);
            } else break;
        }

        error_logger.push_msg("LINE: |"+temp_string+"|");


        if( regex_match(temp_string,re_comment) ){
            error_logger.push_msg("Is a comment line!");
            //comment line, don't do anything

        //logics for reading in fortran real 8's
        } else if( regex_match(temp_string,re_real8) ){
            error_logger.push_msg("Is an R8 line!");

            vector<string> tokens = split(temp_string,' ');
            string var_name = tokens[1];
            if(tokens[2] != "="){
                error_logger.push_error("Missing '=' in R8 param declaration!");
            } else if( !regex_search(temp_string,nad_flag) ){
                double value;
                try {
                  value = stod(tokens[3]);

                } catch (invalid_argument& error){
                  string err = "Error in input_maker::init(), real 8 parameter";
                  err       +=" given bad initial value:" + tokens[3];
                  error_logger.push_error(err);
                  value = -180.4;
                }
                param_real8 new_param(var_name,value);
                real8_params.emplace(var_name,new_param);

            } else {
                string msg = "String:" + temp_string;
                msg       += " has no default set. This is likely intentional.";
                error_logger.push_msg(msg);
                param_real8 new_param(var_name,-180.4);
                real8_params.emplace(var_name,new_param);


            }

        //logics for reading in fortran integer 4's
        } else if( regex_match(temp_string,re_i4) ){
            error_logger.push_msg("Is an int4 line!");

            vector<string> tokens = split(temp_string,' ');
            string var_name = tokens[1];
            if(tokens[2] != "="){
                error_logger.push_msg("Missing '=' in I4 param declaration!");
            } else if( !regex_search(temp_string,nad_flag) ){
                int value;
                try{
                  value = stoi(tokens[3]);
                } catch(invalid_argument& error){
                  string err = "Error in input_maker::init()! Illegal value";
                  err += " in int4 declaration!:"+tokens[3];
                  error_logger.push_error(err);
                  //give it a bad value as an indication that
                  //something went wrong
                  value = -1804;
                }


                param_int4 new_param(var_name,value);
                int4_params.emplace(var_name,new_param);

            } else {
                string err = "String" + temp_string;
                err       += " has no default set. This is likely intentional.";
                error_logger.push_error(err);
                param_int4 new_param(var_name,-1804);
                int4_params.emplace(var_name,new_param);

            }

        //logics for reading in fortran strings
        //and their size
        } else if( regex_match(temp_string,re_string) ){
                error_logger.push_msg("Is a string line!");
                error_logger.push_msg("This is that line split along spaces:");
                //split across spaces, except for spaces within ""
                vector<string> tokens = split(temp_string,' ');
                for(unsigned int c = 0; c < tokens.size() ;c++){
                    error_logger.push_msg("\t:"+ tokens[c]);
                }


            if(tokens[2] != "="){
                string err = "Error! Missing '=' in string declaration!";
                error_logger.push_error(err);
            } else {

                //will contain the number matched from the
                //found label|SIZE| portion
                smatch number_matches;
                //match with SIZE number
                regex_search(tokens[1],number_matches,
                                string_array_size_pattern);

                //put it into a string
                //convert the string to an integer
                string temp_string = number_matches[0].str();
                int size;
                try {
                  size = stoi( temp_string.substr(1,temp_string.length()-1));
                } catch(invalid_argument& error){
                  string bad_size_msg = "Error! Illegal string size value in";
                  bad_size_msg += " input_maker::init() :";
                  bad_size_msg += temp_string.substr(1,temp_string.length()-1);
                  error_logger.push_error(bad_size_msg);
                  size = 0;
                }

                //name can be found by token label|some-number| minus
                //the |some-number| part
                string name;
                name = tokens[1].substr(0,tokens[1].size() - temp_string.size());

                //make sure bookkeeping vector knows what is going on
                //names_in_order.push_back(name);

                //create new ftran struct

                param_string push_me(name,tokens[tokens.size()-1],size);
                //save this new param value in its vector
                string_params.emplace(name,push_me);
            }

        } else if( regex_match(temp_string,re_i4_array) ){
            string int_arr_msg = "Is an array of integers! This is that";
            int_arr_msg += " line split along spaces:";
            error_logger.push_msg(int_arr_msg);

            vector<string> tokens = split(temp_string,' ');

            for(unsigned int c = 0; c < tokens.size() ;c++){
                error_logger.push_msg("\t"+tokens[c]);
            }
            //store numerical result of grabbing the array's size
            smatch size_match;

            //have regex search out the integer size value
            regex_search(tokens[0],size_match,int_array_size_pattern);

            //if a match was found
            if( size_match.ready() ){

                //create a string that contains just the size of the array
                string temp_size_string;
                temp_size_string = size_match[0].str().substr(1,
                                                size_match[0].str().size()-2);
                int array_size;
                try {
                  array_size = stoi(temp_size_string);
                } catch(invalid_argument& error){

                  string i4_err = "Error in input_maker::init(), i4 array";
                  i4_err += " size given illegal value:";
                  i4_err += temp_size_string;
                  error_logger.push_error(i4_err);
                  array_size = 0;
                }

                //create the int4 array that will be pushed into input_maker's
                // containing vector tokens[1] should be the string name given,
                //array_size was determined by the size_match regex should
                //not be satisfied from the start, although it would appear
                // that it is OK for NENT
                //to not have 7 values depending on some other variable.....
                param_int4_array i4_array_push_me(tokens[1],array_size,false);

                handle_i4_array(tokens[3],i4_array_push_me.values);
                error_logger.push_msg("VECTOR OF INTS AS FOLLOWS: ");
                for(unsigned int c = 0; c < i4_array_push_me.values.size();c++){
                    error_logger.push_msg("\t"+to_string(i4_array_push_me.values[c]));
                }
                int4_array_params.insert(std::pair<string,param_int4_array>(i4_array_push_me.name,i4_array_push_me));

            } else {
                //if there was no match for some reason, print an error message
                string err = "Error! Could not determine array size from";
                err       += " int4 array declaration line.";
                error_logger.push_error(err);
            }


        } else if( regex_match(temp_string,r8_array) ){
            error_logger.push_msg("LINE:"+temp_string+"is an E array!");

            //split across spaces
            vector<string> tokens = split(temp_string,' ');
            for(unsigned int c = 0; c < tokens.size(); c++){
                error_logger.push_msg("\t"+tokens[c]);

            }

            //token 1 should be just the variable name
            string name = tokens[1];

            //fill this value later
            int array_size = 0;
            //fill this with what's in the comma separated list
            vector<double> values;

            //rip array size out of type declaration part E(size)
            //"E(8) some array name = " " "
            //contain result of regex search for size
            smatch size_match;
            regex_search(tokens[0],size_match,int_array_size_pattern);
            if(size_match.ready()){
                string temp_size_string;
                temp_size_string =
                    size_match[0].str().substr(1,size_match[0].str().size()-2);

                try {
                  array_size = stoi(temp_size_string);
                } catch (invalid_argument& error){
                  error_logger.push_error("Error in input_maker::init, real 8",
                                " array given illegal size:"+temp_size_string);
                }

            } else {

                string bad_size_msg = "Error! Could not determine array size";
                bad_size_msg += "of R8 array (TIN?) declaration line.";
                error_logger.push_error(bad_size_msg);
            }

            //create object to be shoved into the map for E arrays
            param_r8_array r8_array_push_me(name,array_size,false);
            handle_r8_array(tokens[3],r8_array_push_me.values);
            error_logger.push_msg("Vector of R8 Doubles? As follows:");
            for(unsigned int c = 0; c < r8_array_push_me.values.size();c++){
                error_logger.push_msg(to_string(r8_array_push_me.values[c]));
            }

            //shove object into the map for E arrays
            r8_array_params.insert(
                       std::pair<string,param_r8_array>(name,r8_array_push_me));

        } else if( regex_match(temp_string,form_init) ){

            //cout << "Line: " << temp_string << " describes a form "
            //     << "initialization list!" << endl;
            stringstream form_init_list(temp_string);

            string first_part;

            form_init_list >> first_part;
            cout << "First part: " << first_part << endl;

            string form_name = split(first_part,':')[1];
            cout << "Form name: " << form_name << endl;

            //if this form_name is already in the map,
            //the user has two initializer lists with the same name
            //we don't know which to use, so abort and ask them to
            //fix the file
            try{
                form_init_arrays.at(form_name);
                error_logger.push_error("Error while parsing config file,",
                    " redundant form button initializer list. Exiting.");
                exit(-1);

            //elsewise, if this form_name is not already in the map,
            //go ahead and insert it
            } catch(out_of_range& not_found){

                vector<string> values;
                pair<string,vector<string>> push_me(form_name,values);
                form_init_arrays.emplace(push_me);
            }

            //need a special case here, the ICNTRL10 initialization list
            //has separators besides spaces. So, it's best to just
            //send over an array of size 1, so that icntrl10 can do the
            //processing. | separates 'pages', and ',' separates
            //text boxes. ' ' separates values
            if(form_name.compare("ICNTRL10") == 0){


                string entire_list = form_init_list.str();
                string without_name = entire_list.substr(14,entire_list.size()-13);
                cout << "icntrl10 without name:" << without_name << endl;
                form_init_arrays.at(form_name).push_back(without_name);

            } else if(form_name.compare("ICNTRL6") == 0){

                cout << "Processing icntrl6 init values." << endl;
                //get entirety of input
                string entire_list = form_init_list.str();
                cout << entire_list << endl;
                //cut off the form name
                string without_name = entire_list.substr(13,entire_list.size()-12);
                //split the big string along '\', separating it into 3 lists
                //of values
                cout << without_name << endl;
                vector<string> each_form = split(without_name,'|');
                for(UINT c = 0; c < each_form.size();c++){
                    cout << each_form[c] << endl;
                }
                //push the first list into the form_init_arrays map
                //as normal, icntrl6 can use form_button::init_values_helper
                //to fill out this one
                vector<string> first_form = split(each_form[0],' ');
                form_init_arrays.at(form_name) = first_form;

                //but, we also need to fill in the init values for the
                //other two forms: search_spectra, and cross_sections

                //search_spectra's list of values is found by splitting
                //its entire string along commas
                vector<string> search_spectra = split(each_form[1],' ');

                //do the same for cross_sections
                vector<string> cross_sections = split(each_form[2],' ');

                //push both of the lists into icntrl6's vector
                //note that the order is important here:
                //search_spectra then cross_sections
                icntrl6_extra_init_arrays.push_back(search_spectra);
                icntrl6_extra_init_arrays.push_back(cross_sections);

            } else {
                //read all of the values into the vector string
                while(!form_init_list.eof()){

                    if(form_init_list.fail()){
                        break;
                    }
                    string this_bit;
                    form_init_list >> this_bit;
                    form_init_arrays.at(form_name).push_back(this_bit);
                }
            }
            cout << form_name << "'s value list:\n";
            for(uint c = 0; c < form_init_arrays.at(form_name).size();c++){
                cout << form_init_arrays.at(form_name)[c] << endl;
            }

        } else {
            error_logger.push_error("Error! Line type wasn't determined:");
            error_logger.push_error(temp_string);
        }

        getline(ins,temp_string);
    }
    string im_end_msg = "########## END INPUT MAKER INIT() ##################";
    error_logger.push_error(im_end_msg);
    ins.close();
}

bool input_maker::output(vector<string>& form_bad_inputs){

    //set to true to more easily identify specific
    //lines from the HF manual, false to actually make FOP inputs
    bool console_test = true;
    //note that this will not yet be properly formatted for HF input,
    //mostly here for testing field input and this class's output logic
    ofstream outs;
    outs.open( (output_p+output_file_name).c_str(),std::fstream::trunc );
    if(outs.fail()) {
        error_logger.push_error("Error! Can not open/create output file: |"+
                                output_p+output_file_name+
                                "| Attempting output directory creation.");
        system("mkdir output");
        outs.open( (output_p+output_file_name).c_str(),std::fstream::trunc);
        if(outs.fail()){
            error_logger.push_error("Opening output stream failed again.");
            return false;
        }

    }

    //note that in these functions, the variables that are being printed are
    //found in the order they are found in the HF_config/config.txt
    //file, based off of the input manual that I was given
    //so if a new line is inserted, care to adjust the indices.
    //Adding new lines to the end shouldn't change the lines above them, though


    //SET UP LINE 1#############################################################
    if(console_test) outs << "LINE_1###################################" << endl;
    do_line1(outs,string_params);
    if(console_test) outs << "#########################################" << endl;
    //##########################################################################

    //SET UP LINE 2#############################################################
    if(console_test) outs << "LINE_2###################################" << endl;
    do_line2(outs,real8_params, int4_params);
    if(console_test) outs << "#########################################" << endl;
    //##########################################################################

    //SET UP LINE 3#############################################################
    if(console_test) outs << "LINE_3###################################" << endl;
    do_TC_coefficients(real8_params,int4_array_params,outs);
    //##########################################################################

    if(console_test) outs << "LINE_4###################################" << endl;
    /*from what I understand from the input manual, line 4
     *is always needed, even though Zach doesn't want 4A and 4B
     *in the Andiamo! project, something about IENCH = 7 not being
     *doable */
    do_line4(outs,real8_params,int4_params);
    /* we can't handle IENCH = 7, this is disabled
    //IF IENCH = 7
    //bool do_4a_4b = false;

    if(int4_params.at("IENCH").value == 7){
        do_4a_4b = true;
    }



    //only do the following lines if IENCH == 7, per the input manual
    if(do_4a_4b){
        //SET UP LINE 4#########################################################

        do_line4A(outs,real8_params,int4_params);

        do_line4B(outs,r8_array_params);
        //######################################################################
    }
    //################### IENCH = 7 LINES ######################################
    */
    if(console_test) outs << "########################################" << endl;

    //do line 5
    if(console_test) outs << "LINE_5###################################" << endl;
    do_line5(outs,int4_params);
    if(console_test) outs << "#########################################" << endl;

    //do line 5A
    if(console_test) outs << "LINE_5A##################################" << endl;
    if(int4_params.at("ILV1").value == 6){
        do_line5A(outs,real8_params);
    }
    if(console_test) outs << "#########################################" << endl;


    //do line 5D or 5E
    if(console_test) outs << "LINE_5D or LINE 5E#######################" << endl;
    std::vector<index_value> ilv3_ilv5_bad_inputs;
    int ilv3_val = int4_params.at("ILV3").value;
    int ilv5_val = int4_params.at("ILV5").value;

    if( !( ilv3_val == 0 && ilv5_val == 0) && !(ilv3_val > 0 && ilv5_val > 0)){


        ilv3_ilv5_form_button& ilv3_ilv5 = button_access->get_ilv3_ilv5();

        if( !ilv3_ilv5.make_output(outs,ilv3_ilv5_bad_inputs) ){

        string header = "##############Ilv3/ilv5 error list##############\n";
        string msg    = "Distinct Residual Level Density - Ilv3 OR";
        msg          += " Distinct Level Density Model form\n";

        form_bad_inputs.push_back(header);
        form_bad_inputs.push_back(msg);
            for(unsigned int c = 0; c < ilv3_ilv5_bad_inputs.size(); c++){
                string temp_error;
                temp_error = "Index: " +to_string(ilv3_ilv5_bad_inputs[c].index)
                                + "  Argument: " + ilv3_ilv5_bad_inputs[c].value
                                + "\n";
                form_bad_inputs.push_back(temp_error);
            }
        }
    }
    if(console_test) outs << "#########################################" << endl;

    //do line 6
    if(console_test) outs << "LINE_6###################################" << endl;
    do_line6(outs,int4_params);
    if(console_test) outs << "#########################################" << endl;

    //do line 7
    if(console_test) outs << "LINE_7###################################" << endl;
    do_line7(outs,real8_params);
    if(console_test) outs << "#########################################" << endl;

    //do line 8 and the line 9 output from form
    if(console_test) outs << "LINE_8 and LINE_9#######################" << endl;
    //if the conditions from the input manual are met
    if( int4_params.at("ICNTRL4").value != 0 ){
        //static line 8
        do_line8(outs,int4_params);

        //dynamic line 9
        //do the form's output
        std::vector<index_value> icntrl4_bad_inputs;
        if(!button_access->get_icntrl_4().make_output(outs,icntrl4_bad_inputs)){
            form_bad_inputs.push_back("###########Icntrl4 error list#######\n");
            form_bad_inputs.push_back("Resolved Level + Continuum form\n");
            for(unsigned int c = 0; c < icntrl4_bad_inputs.size(); c++){
                string temp_error =  "Index: "
                                     + to_string(icntrl4_bad_inputs[c].index)
                                     + "  Argument: "
                                     + icntrl4_bad_inputs[c].value
                                     + "\n";
                form_bad_inputs.push_back(temp_error);
            }
        }
    }//elsewise, don't do lines 8&9
    if(console_test) outs << "#########################################" << endl;

    //do line 10
    if(console_test) outs << "LINE_10##################################" << endl;
    if( int4_params.at("ICNTRL6").value > 0){
        do_line10(outs,int4_params);
    }
    if(console_test) outs << "#########################################" << endl;


    //#########MAKE OUTPUTS FROM FORM_BUTTONS ################################//
    if(console_test) outs << "ICNTRL6_FORM#############################" << endl;
    vector<index_value> icntrl6_bad_inputs;
    int icntrl6_value = int4_params.at("ICNTRL6").value;
    do_icntrl6(outs,icntrl6_value,form_bad_inputs,icntrl6_bad_inputs);
    if(console_test) outs << "#########################################" << endl;

    if(console_test) outs << "ICNTRL8_FORM#############################" << endl;
    vector<index_value> icntrl8_bad_inputs;
    do_icntrl8(outs,form_bad_inputs,icntrl8_bad_inputs);
    if(console_test) outs << "$########################################" << endl;

    if(console_test) outs << "ICNTRL10_FORM############################" << endl;
    vector<index_value> icntrl10_bad_inputs;
    do_icntrl10(outs,form_bad_inputs,icntrl10_bad_inputs);
    if(console_test) outs << "#########################################" << endl;

    //########################################################################//

    //if the vector of error message's size is not 0, don't make the output file
    //and present a message to the user
    if(form_bad_inputs.size() != 0){

        return false;

    } else {

        //push changes to file, if this is not here C++ will wait
         // to do the writing until the program is ends
        outs.flush();
        //close the output file stream
        outs.close();
        //make this boolean true, so that during the closing
        //process we know that we don't need to remind the user
        // to generate an input file first
        output_was_made = true;
        check_map();
        return true;
    }
}

//################# Initialzation of Fields ####################################
void input_maker::initialize_fields(){

    give_int4_defaults();
    // give_int4_array_defaults();

    give_r8_defaults();
    // give_r8_array_defaults();

    give_string_defaults();
}

string input_maker::get_string_init_error_message(const string& param_name){

    string err;
    err  = "Error! Failed to find parameter:"+param_name;
    err += "'s tile in the fields map.\nPlease make sure that";
    err += " its entries in tile_Input/tiles.txt";
    err += "and HF_Config/config.txt \n have matching names.";
    return err;
}

void input_maker::give_int4_defaults(){

    //the outer loop just runs over every integer in the integer map
    //that needs output to the HF file, and then runs through each line's
    //map in the manager::fields map looking for the correct parameter
    //name for that int4 param.
    for(INT4_MAP::iterator i4_it = int4_params.begin();
        i4_it != int4_params.end();
        i4_it++){


        field* this_params_field = tile_access->get_param(i4_it->first);


        if(this_params_field != NULL){

            if(i4_it->second.value == -1804){

                this_params_field->my_text_box.update_text("no default", NULL);

            } else {

                string set_string = to_string(i4_it->second.value);
                this_params_field->my_text_box.update_text(set_string,NULL);

            }

        } else {

            string err = get_string_init_error_message(i4_it->first);
            error_logger.push_error(err);

        }
    }
}

//void input_maker::give_int4_array_defaults(){

//     for(INT4_ARR_MAP::iterator big_it = io_access->get_i4_array_params().begin();
//         big_it != io_access->get_i4_array_params().end();
//         big_it++){
//         bool found = false;
//         for(FIELDS_MAP::iterator line_it = fields.begin();
//             line_it != fields.end();
//             line_it++){
//           try{

//             string default_val;
//             default_val = big_it->second.get_string();

//             line_it->second.at(big_it->first)->int4_array_hook = &big_it->second;
//             line_it->second.at(big_it->first)->my_text_box.update_text(default_val,NULL);
//             found = true;
//             break;
//           } catch (out_of_range& not_found){
//             //check other line maps
//             continue;
//           }

//         }
//         if(!found){

//             string err = "Error! Failed to find parameter:";
//             err += big_it->first +"'s tile in the fields map.";
//             err += "\nPlease make sure that its entries in";
//             err += "tile_Input/tiles.txt and HF_Config/config.txt";
//             err += "\n have matching names.";

//             error_logger.push_error(err);
//         }
//     }

//}

void input_maker::give_r8_defaults(){

    //the outer loop just runs over every integer in the integer map
    //that needs output to the HF file, and then runs through each line's
    //map in the manager::fields map looking for the correct parameter
    //name for that int4 param.
    for(REAL8_MAP::iterator r8_it = real8_params.begin();
        r8_it != real8_params.end();
        r8_it++){


        field* this_params_field = tile_access->get_param(r8_it->first);


        if(this_params_field != NULL){

            if(r8_it->second.value == -1804){

                this_params_field->my_text_box.update_text("no default", NULL);

            } else {

                string set_string = to_string(r8_it->second.value);
                this_params_field->my_text_box.update_text(set_string,NULL);

            }

        } else {

            string err = get_string_init_error_message(r8_it->first);
            error_logger.push_error(err);

        }
    }

}

//void input_maker::give_r8_array_defaults(){

//     for(R8_ARR_MAP::iterator big_it = io_access->get_r8_array_params().begin();
//     big_it != io_access->get_r8_array_params().end();
//         big_it++){
//         bool found = false;
//         for(FIELDS_MAP::iterator lines_it = fields.begin();
//             lines_it != fields.end();
//             lines_it++){
//           try{

//             string temp_str;
//             temp_str = big_it->second.get_string();

//             //set pointer to parameter in input maker
//             lines_it->second.at(big_it->first)->r8_array_hook = &big_it->second;
//             lines_it->second.at(big_it->first)->my_text_box.update_text(temp_str,NULL);
//             found = true;

//             //stop checking lines
//             break;
//           } catch (out_of_range& not_found){
//             //check other lines
//             continue;
//           }

//         }
//         if(!found){
//             string err = "Error! Failed to find parameter:";
//             err += big_it->first + "'s tile in the fields map.";
//             err += "\nPlease make sure that its entries in";
//             err += " tile_Input/tiles.txt and HF_Config/config.txt";
//             err += "\n have matching names.";
//             error_logger.push_error(err);
//         }

//     }

//}

void input_maker::give_string_defaults(){

    //the outer loop just runs over every integer in the integer map
    //that needs output to the HF file, and then runs through each line's
    //map in the manager::fields map looking for the correct parameter
    //name for that int4 param.
    for(STR_MAP::iterator str_it = string_params.begin();
        str_it != string_params.end();
        str_it++){

            field* this_params_field = tile_access->get_param(str_it->first);

        if(this_params_field != NULL){

            this_params_field->my_text_box.update_text(str_it->second.value,NULL);

        } else {

            string err = get_string_init_error_message(str_it->first);
            error_logger.push_error(err);

        }
    }

}
//##############################################################################

bool input_maker::grab_values(vector<string>& bad_input_list){

    bool success = true;

    for(INT4_MAP::iterator it = int4_params.begin(); it != int4_params.end();it++){

        string field_value = tile_access->get_param(it->first)->my_text_box.text;

            it->second.value = str_to_integer(field_value);

    }

    for(REAL8_MAP::iterator it = real8_params.begin(); it != real8_params.end();it++){

        string field_value = tile_access->get_param(it->first)->my_text_box.text;

        it->second.value = str_to_double(field_value);

    }

    // for(INT4_ARR_MAP::iterator it = int4_array_params.begin(); it != int4_array_params.end();it++){

    //     cout << it->first << endl;
    //     // it->second.value = tile_access->get_param(it->first).my_text_box.text;

    // }

    // for(R8_ARR_MAP::iterator it = r8_array_params.begin();it != r8_array_params.end();it++){

    //     cout << it->first << endl;
    //     // it->second.value = tile_access->get_param(it->first).my_text_box.text;

    // }
    return success;
}

//################# CONTEXT SAVING     #########################################
void input_maker::save_context(ofstream& outs){

    save_fields(outs);
    save_forms(outs);

}

void input_maker::save_fields(ofstream& context_out){


// void save_context_button::save_fields(ofstream& context_out){
//     cout << "In save_context's save_fields() helper function" << endl;
//     FIELDS_VEC* fields_ref = &tile_access->fields_order;
//     for(uint line = 0; line < fields_ref->size();line++){
//         for(uint param = 0; param < (*fields_ref)[line].size(); param++){
//             if(((*fields_ref)[line][param])->int4_hook != NULL){
//                 context_out << "I4 ";
//             } else if(((*fields_ref)[line][param])->real8_hook != NULL){
//                 context_out << "R8 ";
//             } else if(((*fields_ref)[line][param])->string_hook != NULL){
//                 context_out << "C* ";
//             } else {
//                 context_out << "Tile arrays should be removed" << endl;
//             }
//             context_out << ((*fields_ref)[line][param])->tile_name;
//             if(((*fields_ref)[line][param])->string_hook != NULL){
//                 context_out << "|"
//                     << ((*fields_ref)[line][param])->my_text_box.text.size()
//                     << "|";
//             }
//             context_out << " = "
//                         << ((*fields_ref)[line][param])->my_text_box.text
//                         << endl;
//         }

//      }
// }
}

void input_maker::save_forms(ofstream& context_out){

    // cout << "In save_context's save_forms() helper function" << endl;

    // //save icntrl8's information to the new config file
    // button_access->get_icntrl_8().save_information(context_out);
    // //context_out << endl;

    // //save icntrl6's information to the new config file
    // button_access->get_icntrl_6().save_information(context_out);
    // //context_out << endl;

    // //save icntrl10's information to the new config file
    // button_access->get_icntrl_10().save_information(context_out);

    // //save icntrl4's information to the new config file
    // button_access->get_icntrl_4().save_information(context_out);
    // //context_out << endl;

    // //save ilv3/ilv5's information to the new config file
    // button_access->get_ilv3_ilv5().save_information(context_out);
    // //context_out << endl;

}
//##############################################################################

//################# NON MEMBER HELPERS #########################################
void output_string(ofstream& outs,const unsigned int& size,
                    const string& string_in){

    //set up output flags
    outs << setw(size) << left;
    //if string is in quotation marks, don't print them
    if(string_in[0] == '\"' && string_in[string_in.size()-1] == '\"'){
        //take out the "" that were used for the regular expression matching
        string output_me = string_in.substr(1,string_in.size()-2);
        //print to the file
        outs << output_me;
    } else {//elsewise, just print the string

        outs << string_in;

    }

}

void do_line1(ofstream& outs,const map<string,param_string>& string_params){

    //if(string_params[0].name != "label" || string_params.size() == 0){
    try{
        uint size = string_params.at("label").size;
        string value = string_params.at("label").value;
        output_string(outs,size,value);
        outs << "\n";

    } catch( out_of_range& not_found ){


        string err = "Error in input_maker, 'label' parameter not found";
        err       += " in vector string_params as it should be.";
        error_logger.push_error(err);
    }

}

void do_line2(ofstream& outs,const REAL8_MAP& real8_params,
              const INT4_MAP& int4_params){

    //note, setw(something) needs to be called before every item is printed
    //this is really annoying, so I have a macro up top
    //where F = "<< setw(8) << " for printing the real 8 values

    outs << right;//right justify data in it's width field

    //set up decimal place precision
    outs << fixed << setprecision(1);

    try{
        // ELAB A Z FNRME1 FNRMM1
        outs F real8_params.at("ELAB").value F real8_params.at("A").value
             F real8_params.at("Z").value
             F real8_params.at("FNRME1").value
             F real8_params.at("FNRMM1").value;

        //I = "<< setw(5) <<" macro up top
        //IENCH, ICM, NZ3, TCPR
        outs I int4_params.at("IENCH").value I int4_params.at("ICM").value
             I int4_params.at("NZ3").value I int4_params.at("TCPR").value;

        //make field width 8 again, FNRME2
        outs F real8_params.at("FNRME2").value;

        //put width back to 5 for NGF
        outs I int4_params.at("NGF").value << endl;
    } catch(out_of_range& not_found) {



    }

}

void do_TC_coefficients(const map<string,param_real8>& real8_params,
                        const map<string,param_int4_array>& array_map,
                        ofstream& outs){
    /*ifstream ins;
    string TC_path = HOME;
    TC_path       += "/Andiamo/TC_files/";
    TC_path       += alt_TC_dir;
    ins.open(TC_path.c_str());
    if(ins.fail()){
        string err = "Error! File:~/Andiamo/TC_files/" + alt_TC_dir;
        err       += " could not be found.";
        error_logger.push_error(err);
    }

    vector<string> lines_in;

    //handy temporary string
    string temp_string;


    while(!ins.eof()){
        if(ins.fail()){
            //leave loop, file is tapped out
            break;
        }

        //yank line from file
        getline(ins,temp_string);

        //shove it into the array
        lines_in.push_back(temp_string);
    }

    //this will be the case where the exact TC file was given, just mirror it
    error_logger.push_msg("############# PRINTING TC ########################");
    outs << right;
    for(unsigned int c = 0; c < lines_in.size();c++){
        error_logger.push_msg(lines_in[c]);
        outs << lines_in[c];

        if(c < lines_in.size() - 1){
            outs << endl;
        }

    }
    error_logger.push_msg("############ DONE PRINTING TC ####################");

    //this is the case where the FOP wrapper has been run

    //###################################################

    ins.close();
*/
}

void do_line4(ofstream& outs,const map<string,param_real8>& real8_params,
                const map<string,param_int4>& int4_params){
  try{
    outs << right;
    outs F10 real8_params.at("FJTAR").value F10 real8_params.at("FCMJMAX").value
    F10 real8_params.at("FRESIDMAX").value
    I int4_params.at("ITARPR").value I int4_params.at("NG").value << endl;

  } catch (out_of_range& not_found){
    string err_msg = "Error in do_line4: parameter not found in the map!";
    error_logger.push_error(err_msg);
  }


}

void do_line4A(ofstream& outs,const map<string,param_real8>& real8_params,
                const map<string,param_int4>& int4_params){
  try{
    outs << fixed << setprecision(2);
    outs F5 real8_params.at("APAR").value F5 real8_params.at("ZPAR").value
         F5 real8_params.at("QIN").value
         F5 real8_params.at("FJPAR").value F5 real8_params.at("FPRPAR").value;
    outs I int4_params.at("NLIN").value << endl;
  } catch (out_of_range& not_found){
    error_logger.push_error("Error in do_line4A: parameter not found in the map!");
  }
}
void do_line4B(ofstream& outs,const map<string, param_r8_array>& r8_array_params){
  try{
    outs << right;//set orientation
    outs << setprecision(3);//set # of decimal places
    for(unsigned int c = 0; c < r8_array_params.at("TIN").values.size();c++){
        outs F10 r8_array_params.at("TIN").values[c];
        if(c == r8_array_params.at("TIN").values.size()-1){
        outs << endl;
        }

    }
  } catch (out_of_range& not_found){
    error_logger.push_error("Error! Parameter in do_line4B not found in the map!");
  }
}

void do_line5(ofstream& outs, const map<string,param_int4>& int4_params){

    int ilv_1 = int4_params.at("ILV1").value;
    int ilv_2 = 0;
    int ilv_3 = int4_params.at("ILV3").value;
    int ilv_4 = int4_params.at("ILV4").value;
    int ilv_5 = int4_params.at("ILV5").value;
    outs I ilv_1 I ilv_2 I ilv_3 I ilv_4 I ilv_5 << endl;

}

void do_line5A(ofstream& outs, const map<string,param_real8>& real8_params){

    double acon_val,gam_val,fcon_val,c0_val,c10_val,c11_val,c12_val,c3_val;
    acon_val = real8_params.at("ACON").value;
    gam_val  = real8_params.at("GAM").value;
    fcon_val = real8_params.at("FCON").value;
    c0_val   = real8_params.at("C0").value;
    c10_val  = real8_params.at("C10").value;
    c11_val  = real8_params.at("C11").value;
    c12_val  = real8_params.at("C12").value;
    c3_val   = real8_params.at("C3").value;

    outs << setprecision(2);
    outs F5 acon_val F5 gam_val F5 fcon_val F5 c0_val F5 c10_val;
    outs F5 c11_val  F5 c12_val F5 c3_val << endl;



}

void do_line5E_D(ofstream& outs,const map<string,param_int4>& int4_params,
                 const map<string,param_real8>& real8_params){



}

void do_line6(ofstream& outs,const map<string,param_int4>& int4_params){
    outs << right;
  try{
    outs I int4_params.at("ICNTRL1").value I int4_params.at("ICNTRL2").value
         I int4_params.at("ICNTRL3").value
         I int4_params.at("ICNTRL4").value I int4_params.at("ICNTRL5").value
         I int4_params.at("ICNTRL6").value
         I int4_params.at("ICNTRL7").value I int4_params.at("ICNTRL8").value
         I int4_params.at("ICNTRL9").value I int4_params.at("ICNTRL10").value
         << endl;
  } catch (out_of_range& not_found){
    error_logger.push_error("Error! Parameter in do_line6 not found in the map!");
  }
}



void do_line7(ofstream& outs,const map<string,param_real8>& real8_params){
  try{

    //set the justification
    outs << right;
    //set precision for 2 points past the decimal
    outs << setprecision(2);
    outs F5 real8_params.at("FMU").value F5 real8_params.at("FCON_").value
         << endl;
  } catch (out_of_range& not_found){
    error_logger.push_error("Error! Parameter in do_line7 not found in the map!");
  }

}

void do_line8(ofstream& outs,const map<string,param_int4>& int4_params){
  try{
    //set the justification
    outs << right;

    outs I int4_params.at("ICH4").value I int4_params.at("NCH4").value << endl;
  } catch(out_of_range& not_found){
    error_logger.push_error("Error! Parameter in do_line8 not found in the map!");
  }
}

void do_line9(ofstream& outs,const map<string,param_int4>& int4_params,
              const map<string,param_real8>& real8_params){
  try{
    outs << right;
    //set precision to 2 points past decimal
    outs << setprecision(2);
    //5 literal spaces as described in input manual pg 8
    outs F5 real8_params.at("ECH4").value << "     ";
    outs << setprecision(1);
    //next few real8's need only 1 digit past decimal
    outs F5 real8_params.at("FJCH4").value << "     ";
    outs I int4_params.at("IPAR4").value << "     ";
    //no spaces afterwards, end this line
    outs F5 real8_params.at("FIS4").value << endl;
  } catch(out_of_range& not_found){
    error_logger.push_error("Error! Parameter in do_line9 not found in the map!");
  }
}

void do_line10(ofstream& outs, const map<string,param_int4>& int4_params){
    outs I int4_params.at("ITER").value I int4_params.at("INM1").value
         I int4_params.at("INM2").value << endl;
}

void do_icntrl6(ofstream& outs,int icntrl6_value,
                vector<string>& form_bad_inputs,
                vector<index_value>& icntrl6_bad_inputs){

    if( icntrl6_value > 0 &&
        !button_access->get_icntrl_6().make_output(outs,icntrl6_bad_inputs)){

        form_bad_inputs.push_back("########## Icntrl6 error list###########\n");
        form_bad_inputs.push_back("Parameter Search form\n");
        for(unsigned int c = 0; c < icntrl6_bad_inputs.size(); c++){
            string temp_error =  "Index: "
                                 +to_string(icntrl6_bad_inputs[c].index)
                                 + "  Argument: " + icntrl6_bad_inputs[c].value
                                 + "\n";
            form_bad_inputs.push_back(temp_error);
        }
    }
}

void do_icntrl8(ofstream& outs,
                vector<string>& form_bad_inputs,
                vector<index_value>& icntrl8_bad_inputs){

    if(!button_access->get_icntrl_8().get_is_locked() &&
       !button_access->get_icntrl_8().make_output(outs,icntrl8_bad_inputs)){

        form_bad_inputs.push_back("########## Icntrl8 error list ##########\n");
        form_bad_inputs.push_back("Residual Level Threshold Count form\n");
        for(unsigned int c = 0; c < icntrl8_bad_inputs.size(); c++){
            string temp_error =  "Index: "
                                 + to_string(icntrl8_bad_inputs[c].index)
                                 + "  Argument: " + icntrl8_bad_inputs[c].value
                                 + "\n";
            form_bad_inputs.push_back(temp_error);
        }
    }
}

void do_icntrl10(ofstream& outs,
                 vector<string>& form_bad_inputs,
                 vector<index_value>& icntrl10_bad_inputs){

    //only print the output for icntrl10 if it is activated & satisfied
    if(!button_access->get_icntrl_10().get_is_locked()){

        std::vector<index_value> icntrl10_bad_inputs;
        if(!button_access->get_icntrl_10().make_output(outs,icntrl10_bad_inputs)){

            form_bad_inputs.push_back("##### ICNTRL10 ERRORS ######");
            form_bad_inputs.push_back("Special Sigma");
            for(unsigned int c = 0; c < icntrl10_bad_inputs.size();c++){

                string push_me = "Value: " + icntrl10_bad_inputs[c].value;
                push_me       += " Index: " + to_string(icntrl10_bad_inputs[c].index);
                form_bad_inputs.push_back(push_me);
            }

        }

    }
}
//##############################################################################

//! \file input_maker.cc \brief implements the functions defined in input_maker.h

#include<regex>
#include<iomanip>

#include "input_maker.h"
#include "button_manager.h"

#define F << setw(8) <<
#define F10 << setw(10) <<
#define F5 << setw(5) <<
#define I << setw(5) <<
#define I10 << setw(10) <<

using namespace std;

input_maker::input_maker(string output_file_name_in,string config_file_name_in){
	config_p = "./config/";
	output_p = "./output/";
	output_file_name = output_file_name_in;
	file_name = config_file_name_in;

	output_was_made = false;//start off false, become positive when output() is ran

	//init();//parse config file
}

/*
input_maker::~input_maker(){
	output();
}*/

void input_maker::check_map(){
	error_logger.push_msg("########### INPUT MAKER INT4 PARAMS MAP ##############################");
	for(map<string,param_int4>::iterator int_it = int4_params.begin();
	    int_it != int4_params.end();
	    int_it++){
		error_logger.push_msg_no_nl(int_it->second.get_string()+"   ");
	}
	error_logger.push_msg("\n######### INPUT MAKER R8 PARAMS MAP #################################\n");
	for(map<string,param_real8>::iterator r8_it = real8_params.begin();
	    r8_it != real8_params.end();
	    r8_it++){
		error_logger.push_msg_no_nl(r8_it->second.get_string()+":");
	}
	error_logger.push_msg("\n######### INPUT MAKER STRING PARAMS MAP #############################\n");
	for(map<string,param_string>::iterator string_it = string_params.begin();
	    string_it != string_params.end();
	    string_it++){
		error_logger.push_msg_no_nl(string_it->first+":"+string_it->second.value+"   ");
	}
	error_logger.push_msg("\n######### INPUT MAKER INT4 ARRAY MAP ###############################\n");
	for(map<string,param_int4_array>::iterator int4_array_it = int4_array_params.begin();
	    int4_array_it != int4_array_params.end();
	    int4_array_it++){
		error_logger.push_msg_no_nl(int4_array_it->first+":"+int4_array_it->second.get_string()+"   ");
	}
	error_logger.push_msg("\n######### INPUT MAKER R8_ARRAY MAP ##################################\n");
	for(map<string,param_r8_array>::iterator r8_array_it = r8_array_params.begin();
	    r8_array_it != r8_array_params.end();
	    r8_array_it++){
		error_logger.push_msg_no_nl(r8_array_it->first+":"+r8_array_it->second.get_string()+"   ");
	}

}


void input_maker::init(){

	error_logger.push_msg("########################### INPUT_MAKER INIT ################################");

	ifstream ins;
	ins.open( (config_p+file_name).c_str() );
	if(ins.fail()){
		error_logger.push_error("Error in input_maker::init(), couldn't find the input file!");
		return;
	}

	//set up regex matches
	regex re_comment("\\s*?#.*");
	regex re_i4("\\s*?I4\\s+?[A-Za-z0-9_]+?\\s+?=\\s+?-?(([0-9]*)|(nodef))\\s*");
	regex re_i4_array("\\s*?I4\\(\\s*?[0-9]+?\\s*?\\)\\s*?[A-Za-z0-9_]+?\\s*?=\\s*?\"(\\s*?-?[0-9]*?\\s*?,?)+?\"\\s*");
	regex re_string("\\s*?C\\*\\s*?[A-Za-z_]+?\\|[0-9]+?\\|\\s*?=\\s*?\".+?\"\\s*");
	regex re_real8("\\s*?R8\\s+?[A-Za-z0-9_]+?\\s+?=\\s+?((-?[0-9]*?\\.[0-9]*?)|(nodef))\\s*");

	regex string_array_size_pattern("\\|\\d+?\\|");
	regex int_array_size_pattern("\\([0-9]+?\\)");

	regex r8_array("\\s*?R8\\(\\s*[0-9]+?\\s*?\\)\\s*?[A-Za-z0-9_]+?\\s*?=\\s*?\"(\\s*?[0-9]+?\\.[0-9]+?,?)+?\"");
	regex nad_flag("nodef");

	string temp_string;
	string temp_name;

	getline(ins,temp_string);
	while(!ins.eof()){
		if(ins.fail()) break; // get out, file stream is done


		//some sort of bad or meaningless input, try again until something of value is found
		//thanks to https://stackoverflow.com/questions/9235296/how-to-detect-empty-lines-while-reading-from-istream-object-in-c
		//for convincing me to check string.empty() instead of "does string equal " ", "\n" or ""
		//this check allows for "spacer" empty lines in the input file to make it easier to read
		while( temp_string.empty() ){
			if(!ins.fail()){
				getline(ins,temp_string);
			} else break;
		}

		error_logger.push_msg("LINE: |"+temp_string+"|");


		if( regex_match(temp_string,re_comment) ){
			error_logger.push_msg("Is a comment line!");
			//comment line, don't do anything

		} else if( regex_match(temp_string,re_real8) ){//logics for reading in fortran real 8's
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
				  error_logger.push_error("Error in input_maker::init(), real 8 parameter given bad initial value:"+tokens[3]);
				  value = -180.4;
				}
				param_real8 new_param(var_name,value);
				//names_in_order.push_back(var_name);//update bookkeeping vector
				real8_params.emplace(var_name,new_param);
				//real8_params.push_back(new_param);//push into real 8 vector 
			} else {
				error_logger.push_msg("String:"+temp_string+" has no default set. This is likely intentional.");
				param_real8 new_param(var_name,-180.4);
				real8_params.emplace(var_name,new_param);


			}

		} else if( regex_match(temp_string,re_i4) ){//logics for reading in fortran integer 4's
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
				  error_logger.push_error("Error in input_maker::init()! Illegal value in int4 declaration!:"+tokens[3]);
				  value = -1804;//give it a bad value as an indication that something went wrong				
				}


				param_int4 new_param(var_name,value);
				int4_params.emplace(var_name,new_param);
				//names_in_order.push_back(var_name);//update bookkeeping vector
				//int4_params.push_back(new_param);//push into the i4 vector

			} else {
				error_logger.push_error("String"+temp_string+" has no default set. This is likely intentional.");
				param_int4 new_param(var_name,-1804);//give it a -1
				int4_params.emplace(var_name,new_param);

			}

		} else if( regex_match(temp_string,re_string) ){//logics for reading in fortran strings
						  //and their size
				error_logger.push_msg("Is a string line!");
				error_logger.push_msg("This is that line split along spaces:");
			//split across spaces, except for spaces within ""
			vector<string> tokens = split(temp_string,' ');
				for(unsigned int c = 0; c < tokens.size() ;c++){
					error_logger.push_msg("\t:"+ tokens[c]);
				}


			if(tokens[2] != "="){
				error_logger.push_error("Error! Missing '=' in string declaration!");
			} else {
				//will contain the number matched from the found label|SIZE| portion
				smatch number_matches;
				//match with SIZE number
				regex_search(tokens[1],number_matches,string_array_size_pattern);

				string temp_string = number_matches[0].str(); //put it into a string
				//convert the string to an integer
				int size;
				try {
				  size = stoi( temp_string.substr(1,temp_string.length()-1));
				} catch(invalid_argument& error){
				  error_logger.push_error("Error! Illegal string size value in input_maker::init() :"+temp_string.substr(1,temp_string.length()-1));
				  size = 0;
				}

				//name can be found by token label|some-number| minus the |some-number| part
				string name = tokens[1].substr(0,tokens[1].size()- temp_string.size());

				//make sure bookkeeping vector knows what is going on
				//names_in_order.push_back(name);

				//create new ftran struct

				param_string push_me(name,tokens[tokens.size()-1],size);
				//save this new param value in its vector
				//string_params.push_back(push_me);	
				string_params.emplace(name,push_me);
			}
			
		} else if( regex_match(temp_string,re_i4_array) ){
			error_logger.push_msg("Is an array of integers! This is that line split along spaces:");
			
			vector<string> tokens = split(temp_string,' ');
			
			for(unsigned int c = 0; c < tokens.size() ;c++){
				error_logger.push_msg("\t"+tokens[c]);
			}
			smatch size_match;//store numerical result of grabbing the array's size

			//have regex search out the integer size value
			regex_search(tokens[0],size_match,int_array_size_pattern);

			//if a match was found
			if( size_match.ready() ){
				
				//create a string that contains just the size of the array
				string temp_size_string = size_match[0].str().substr(1,size_match[0].str().size()-2);
				int array_size;
				try {
				  array_size = stoi(temp_size_string);
				} catch(invalid_argument& error){
				  error_logger.push_error("Error in input_maker::init(), i4 array size given illegal value:"+temp_size_string);
				  array_size = 0;
				}
				
				//create the int4 array that will be pushed into input_maker's containing vector
				//tokens[1] should be the string name given, array_size was determined by the size_match regex
				//should not be satisfied from the start, although it would appear that it is OK for NENT
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
				error_logger.push_error("Error! Could not determine array size from int4 array declaration line.");
			}


		} else if( regex_match(temp_string,r8_array) ){
			error_logger.push_msg("LINE:"+temp_string+"is an E array!"); 
			vector<string> tokens = split(temp_string,' ');//split across spaces
			for(unsigned int c = 0; c < tokens.size(); c++){
				error_logger.push_msg("\t"+tokens[c]);

			}
			string name = tokens[1];//token 1 should be just the variable name 
			int array_size = 0;//fill this value later
			vector<double> values; //fill this with what's in the comma separated list

			//rip array size out of type declaration part E(size) "E(8) some array name = " " "
			smatch size_match;//contain result of regex search for size
			regex_search(tokens[0],size_match,int_array_size_pattern);
			if(size_match.ready()){
				string temp_size_string = size_match[0].str().substr(1,size_match[0].str().size()-2);
				try {
				  array_size = stoi(temp_size_string);
				} catch (invalid_argument& error){
				  error_logger.push_error("Error in input_maker::init, real 8 array given illegal size:"+temp_size_string);
				}
			} else {	

				error_logger.push_error("Error! Could not determine array size of R8 array (TIN?) declaration line.");
			}
			param_r8_array r8_array_push_me(name,array_size,false);//create object to be shoved into the map for E arrays
			handle_r8_array(tokens[3],r8_array_push_me.values);
			error_logger.push_msg("Vector of R8 Doubles? As follows:");
			for(unsigned int c = 0; c < r8_array_push_me.values.size();c++){
				error_logger.push_msg(to_string(r8_array_push_me.values[c]));
			}

			r8_array_params.insert(std::pair<string,param_r8_array>(name,r8_array_push_me));//shove object into the map for E arrays
		} else {
			error_logger.push_error("Error! Line type wasn't determined.");
		}

		getline(ins,temp_string);
	}

	error_logger.push_error("##################### END INPUT MAKER INIT() ###############################");
	ins.close();
}

void input_maker::output(){
	//note that this will not yet be properly formatted for HF input, mostly here for testing field input
	//and this class's output logic

	ofstream outs;
	outs.open( (output_p+output_file_name).c_str(),std::fstream::trunc );
	if(outs.fail()) {
		error_logger.push_error("Error! Can not open/create output file: |"+output_p+output_file_name+"| Attempting output directory creation.");
		system("mkdir output");
		outs.open( (output_p+output_file_name).c_str(),std::fstream::trunc);
		if(outs.fail()){
			error_logger.push_error("Opening output stream failed again.");
			return;
		} 

	}

	//note that in these functions, the variables that are being printed are found in the order they are
	//found in the HF_config/config.txt file, based off of the input manual that I was given
	//so if a new line is inserted, care to adjust the indices. Adding new lines to the end shouldn't
	//change the lines above them, though


	//SET UP LINE 1##########################################################################################
	do_line1(outs,string_params);
	//#######################################################################################################

	//SET UP LINE 2##########################################################################################
	do_line2(outs,real8_params, int4_params);
	//#######################################################################################################

	//SET UP LINE 3##########################################################################################
	do_TC_coefficients(real8_params,int4_array_params,TC_input_file_name,outs);
	//#######################################################################################################

	//IF IENCH = 7 ##########################################################################################
	bool do_4a_4b = false;
	/*for(unsigned int c = 0; c < int4_params.size();c++){
		if(int4_params[c].name == "IENCH" && int4_params[c].value == 7){
			do_4_5 = true;
			break;
		}
	}*/

	if(int4_params.at("IENCH").value == 7){
		do_4a_4b = true;
	}



	//only do the following lines if IENCH == 7, per the input manual
	if(do_4a_4b){
		//SET UP LINE 4##################################################################################
		do_line4(outs,real8_params,int4_params);

		do_line4A(outs,real8_params,int4_params);

		do_line4B(outs,r8_array_params);
		//###############################################################################################

	}
	//####################### IENCH = 7 LINES ###############################################################

    //do line 5
    do_line5(outs,int4_params);	

    //do line 5A
    if(int4_params.at("ILV1").value == 6){
        do_line5A(outs,real8_params);
    }

    //do line 5D or 5E
	std::vector<index_value> ilv3_ilv5_bad_inputs;
    if(!b_manager->get_ilv3_ilv5().make_output(outs,ilv3_ilv5_bad_inputs)){
		cout << "DO SOMETHING ABOUT BAD INPUTS HERE" << endl;
	}

	//do line 6
	do_line6(outs,int4_params);

	//do line 7
	do_line7(outs,real8_params);

	//if the conditions from the input manual are met
	if( int4_params.at("ICNTRL4").value != 0 ){
		do_line8(outs,int4_params);
		//do line 8
		//do the form's output
		std::vector<index_value> icntrl4_bad_inputs;
		if(!b_manager->get_icntrl_4().make_output(outs,icntrl4_bad_inputs)){
			cout << "DO SOMETHING ABOUT BAD INPUTS HERE" << endl;
		}
	}//elsewise, don't do lines 8&9


	if( int4_params.at("ICNTRL6").value > 0){
		do_line10(outs,int4_params);
	}


	//#########MAKE OUTPUTS FROM FORM_BUTTONS ##############################################################//
	std::vector<index_value> icntrl6_bad_inputs;
	if(!b_manager->get_icntrl_6().make_output(outs,icntrl6_bad_inputs)){
		cout << "DO SOMETHING ABOUT ICNTRL6 BAD INPUTS HERE" << endl;
	}

	std::vector<index_value> icntrl8_bad_inputs;
	if(!b_manager->get_icntrl_8().make_output(outs,icntrl8_bad_inputs)){
		cout << "DO SOMETHING ABOUT ICNTRL8 BAD INPUTS HERE" << endl;
		cout << "Icntrl8 error list: " << endl;
		for(unsigned int c = 0; c < icntrl8_bad_inputs.size(); c++){
			cout << "Text: " << icntrl8_bad_inputs[c].value
				 << " Index: " << icntrl8_bad_inputs[c].index << endl;
		}
	}

	std::vector<index_value> icntrl10_bad_inputs;
	if(!b_manager->get_icntrl_10().make_output(outs,icntrl10_bad_inputs)){
		cout << "DO SOMETHING ABOUT ICNTRL10 BAD INPUTS HERE" << endl;
	}
	//######################################################################################################//


	outs.flush();//push changes to file, if this is not here C++ will wait to do the writing until
		     //the program is terminated
	//close the output file stream
	outs.close();
	output_was_made = true;//make this boolean true, so that during the closing process we know that we don't
			       //need to remind the user to generate an input file first
	check_map();
}
//########################## NON MEMBER HELPERS #################################################################
void output_string(ofstream& outs,const unsigned int& size,const string& string_in){
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
		output_string(outs,string_params.at("label").size,string_params.at("label").value);
		outs << "\n";

	} catch( out_of_range& not_found ){

		error_logger.push_error("Error in input_maker, 'label' parameter not found in vector string_params as it should be.");


	}


}

void do_line2(ofstream& outs,const map<string,param_real8>& real8_params,const map<string,param_int4>& int4_params){

	//note, setw(something) needs to be called before every item is printed
	//this is really annoying, so I have a macro up top where F = "<< setw(8) << " for
	//printing the real 8 values

	outs << right;//right justify data in it's width field

	//set up decimal place precision
	outs << fixed << setprecision(1);

	try{
		// ELAB A Z FNRME1 FNRMM1
		outs F real8_params.at("ELAB").value F real8_params.at("A").value F real8_params.at("Z").value
		     F real8_params.at("FNRME1").value F real8_params.at("FNRMM1").value;

		//I = "<< setw(5) <<" macro up top
		//IENCH, ICM, NZ3, TCPR
		outs I int4_params.at("IENCH").value I int4_params.at("ICM").value I int4_params.at("NZ3").value
		     I int4_params.at("TCPR").value;

		//make field width 8 again, FNRME2
		outs F real8_params.at("FNRME2").value;

		//put width back to 5 for NGF
		outs I int4_params.at("NGF").value << endl;
	} catch(out_of_range& not_found) {



	}

}

void do_TC_coefficients(const map<string,param_real8>& real8_params, const map<string,param_int4_array>& array_map,
			string TC_input_file_name,ofstream& outs){
	ifstream ins;
	ins.open("./TC_files/"+TC_input_file_name);
	if(ins.fail()){
		error_logger.push_error("Error! File:./TC_files/"+TC_input_file_name+" could not be found.");
	}

	vector<string> lines_in;
	string temp_string;//handy temporary string

	
	while(!ins.eof()){
		if(ins.fail()){
			break;//leave loop, file is tapped out
		}

		getline(ins,temp_string);//yank line from file
		lines_in.push_back(temp_string);//shove it into the array
	}
	
	//this will be the case where the exact TC file was given, just mirror it
	error_logger.push_msg("#################### PRINTING TC ##################################");
	outs << right;
	for(unsigned int c = 0; c < lines_in.size();c++){
		error_logger.push_msg(lines_in[c]);
		outs << lines_in[c];

		if(c < lines_in.size() - 1){
			outs << endl;
		}

	}
	error_logger.push_msg("#################### DONE PRINTING TC #############################");



	//#######################################################################

	//here be the case where we have to use ESIS to do it ###################



	//#######################################################################
	

	ins.close();

}

void do_line4(ofstream& outs,const map<string,param_real8>& real8_params,const map<string,param_int4>& int4_params){
  try{
	outs << right;
	//     FJTAR               FCMJMAX          FRESIDEMAX         ITARPR               NG             
	outs F10 real8_params.at("FJTAR").value F10 real8_params.at("FCMJMAX").value F10 real8_params.at("FRESIDMAX").value
	     I int4_params.at("ITARPR").value I int4_params.at("NG").value << endl;
  } catch (out_of_range& not_found){
	error_logger.push_error("Error in do_line4: parameter not found in the map!");
  }


}

void do_line4A(ofstream& outs,const map<string,param_real8>& real8_params,const map<string,param_int4>& int4_params){
  try{
	outs << fixed << setprecision(2);
	outs F5 real8_params.at("APAR").value F5 real8_params.at("ZPAR").value F5 real8_params.at("QIN").value 
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
    int ilv_2 = int4_params.at("ILV2").value;
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
	outs I int4_params.at("ICNTRL1").value I int4_params.at("ICNTRL2").value I int4_params.at("ICNTRL3").value
	     I int4_params.at("ICNTRL4").value I int4_params.at("ICNTRL5").value I int4_params.at("ICNTRL6").value
	     I int4_params.at("ICNTRL7").value I int4_params.at("ICNTRL8").value
	     I int4_params.at("ICNTRL9").value I int4_params.at("ICNTRL10").value << endl;
  } catch (out_of_range& not_found){
	error_logger.push_error("Error! Parameter in do_line6 not found in the map!");
  }
}



void do_line7(ofstream& outs,const map<string,param_real8>& real8_params){
  try{
	outs << right;	//set the justification
	outs << setprecision(2);//set precision for 2 points past the decimal
	outs F5 real8_params.at("FMU").value F5 real8_params.at("FCON_").value << endl;
  } catch (out_of_range& not_found){
	error_logger.push_error("Error! Parameter in do_line7 not found in the map!");
  }

}

void do_line8(ofstream& outs,const map<string,param_int4>& int4_params){
  try{
	outs << right;	//set the justification

	outs I int4_params.at("ICH4").value I int4_params.at("NCH4").value << endl;
  } catch(out_of_range& not_found){
	error_logger.push_error("Error! Parameter in do_line8 not found in the map!");
  }
}

void do_line9(ofstream& outs,const map<string,param_int4>& int4_params,const map<string,param_real8>& real8_params){
  try{
	outs << right;
	outs << setprecision(2);                         //set precision to 2 points past decimal
	outs F5 real8_params.at("ECH4").value << "     ";//5 literal spaces as described in input manual pg 8
	outs << setprecision(1);                         //next few real8's need only 1 digit past decimal
	outs F5 real8_params.at("FJCH4").value << "     ";
	outs I int4_params.at("IPAR4").value << "     ";
	outs F5 real8_params.at("FIS4").value << endl;    //no spaces afterwards, end this line
  } catch(out_of_range& not_found){
	error_logger.push_error("Error! Parameter in do_line9 not found in the map!");
  }
}

void do_line10(ofstream& outs, const map<string,param_int4>& int4_params){
	outs I int4_params.at("ITER").value I int4_params.at("INM1").value I int4_params.at("INM2").value << endl;
}
//#################################################################################################################






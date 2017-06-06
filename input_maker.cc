//! \file input_maker.cc \brief implements the functions defined in input_maker.h

#include "input_maker.h"
#include<regex>
using namespace std;

input_maker::input_maker(string output_file_name_in,string config_file_name_in){
	config_p = "./config/";
	output_p = "./output/";
	output_file_name = output_file_name_in;
	file_name = config_file_name_in;

	init();//parse config file
}
input_maker::~input_maker(){
	output();
}

void input_maker::init(){
	bool init_test = false;

	ifstream ins;
	ins.open( (config_p+file_name).c_str() );
	if(ins.fail()){
		cout << "Error in input_maker::init(), couldn't find the input file!" << endl;
		return;
	}

	//set up regex matches
	regex re_comment("\\s*?#.*");
	regex re_i4("\\s*?I4\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*");
	regex re_i4_array("\\s*?I4\\(\\s*?[0-9]+?\\s*?\\)\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*");
	regex re_string("\\s*?C\\*\\s*?[A-Za-z]+?\\|[0-9]+?\\|\\s*?=\\s*?\".+?\"");
	regex re_real8("\\s*?R8\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*?\\.[0-9]*?");

	regex string_array_size_pattern("\\|\\d+?\\|");
	regex int_array_size_pattern("");



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
			//cout << "JUNK" << endl;
			if(!ins.fail()){
				getline(ins,temp_string);
			} else break;
		}
		if(init_test){
			cout << "LINE: |" << temp_string << "|" << endl;;
			cout << "\t";
		}

		if( regex_match(temp_string,re_comment) ){
			if(init_test) cout << "Is a comment line!" << endl;
			//comment line, don't do anything

		} else if( regex_match(temp_string,re_real8) ){//logics for reading in fortran real 8's
			if(init_test) cout << "Is an R8 line!" << endl;

			vector<string> tokens = split(temp_string,' ');
			string var_name = tokens[1];
			if(tokens[2] != "="){
				cout << "Missing '=' in R8 param declaration!" << endl;
			} else {
				double value = stod(tokens[3]);
				param_real8 new_param(var_name,value);
				names_in_order.push_back(var_name);//update bookkeeping vector
				real8_params.push_back(new_param);//push into real 8 vector 
			}

		} else if( regex_match(temp_string,re_i4) ){//logics for reading in fortran integer 4's
			if(init_test) cout << "Is an int4 line!" << endl;

			vector<string> tokens = split(temp_string,' ');			
			string var_name = tokens[1];
			if(tokens[2] != "="){
				cout << "Missing '=' in I4 param declaration!" << endl;
			} else {
				int value = stoi(tokens[3]);
				param_int4 new_param(var_name,value);

				names_in_order.push_back(var_name);//update bookkeeping vector
				int4_params.push_back(new_param);//push into the i4 vector
			}

		} else if( regex_match(temp_string,re_string) ){//logics for reading in fortran strings
						  //and their size
			if(init_test){
				cout << "Is a string line!" << endl;
				cout << "This is that line split along spaces: " << endl;
			}
			//split across spaces, except for spaces within ""
			vector<string> tokens = split(temp_string,' ');
			if(init_test){
				for(unsigned int c = 0; c < tokens.size() ;c++){
					cout << tokens[c] << endl;
				}
			}


			if(tokens[2] != "="){
				cout << "Error! Missing '=' in string declaration!" << endl;
			} else {
				//will contain the number matched from the found label|SIZE| portion
				smatch number_matches;
				//match with SIZE number
				regex_search(tokens[1],number_matches,string_array_size_pattern);

				string temp_string = number_matches[0].str(); //put it into a string
				//convert the string to an integer
				int size = stoi( temp_string.substr(1,temp_string.length()-1));

				//name can be found by token label|some-number| minus the |some-number| part
				string name = tokens[1].substr(0,tokens[1].size()- temp_string.size());

				//make sure bookkeeping vector knows what is going on
				names_in_order.push_back(name);

				//create new ftran struct
				param_string push_me(name,tokens[tokens.size()-1],size);
				//save this new param value in its vector
				string_params.push_back(push_me);	
			}
			
		} else if( regex_match(temp_string,re_i4_array) ){
			if(init_test) cout << "Is an array of integers!" << endl;
			/*
			cout << "This is that line split along spaces: " << endl;
			vector<string> tokens = split(temp_string,' ');
			for(unsigned int c = 0; c < tokens.size() ;c++){
				cout << tokens[c] << endl;
			}*/

		} else {
			cout << "Error! Line type wasn't determined." << endl;
		}

		getline(ins,temp_string);
	}


	ins.close();
}

void input_maker::output(){
	//note that this will not yet be properly formatted for HF input, mostly here for testing field input
	//and this class's output logic

	ofstream outs;
	outs.open( (output_p+output_file_name).c_str() );
	if(outs.fail()) {
		cout << "Error! Can not open/create output file: |" << output_p+output_file_name << "|" << endl;
		return;
	}



	unsigned int int_index = 0; //keep track of the 'front' of the variable vectors
	unsigned int r8_index = 0; //as their index likely won't line up with names_in_order's
	unsigned int str_index = 0;

	for(unsigned int c = 0; c < names_in_order.size();c++){

		if( int4_params[int_index].name == names_in_order[c] && !int4_params.empty() ){
			//this matches the name of the parameter whose turn it is to be output to the file
			outs << int4_params[int_index].name << "=" ;
			outs.width(5);
			outs<< int4_params[int_index].value << endl;
			int_index++;

		} else if( real8_params[r8_index].name == names_in_order[c] && !real8_params.empty() ) {
			//or this does
			outs << real8_params[r8_index].name << "=";
			outs.setf(std::ios_base::fixed);
			outs.precision(2); outs.width(8); 
			outs << real8_params[r8_index].value << endl;
			r8_index++;

			outs.unsetf(std::ios_base::fixed);

		} else if( string_params[str_index].name == names_in_order[c] && !string_params.empty() ) {
			//or this does
			outs << string_params[str_index].name << "=" << string_params[str_index].value << endl;
			str_index++;

		} else {
			//woops, error handling
			cout << "Error in input_maker's output() function." 
			     << " Variable " << names_in_order[c] 
			     << " not found by name, file is likely erroneous." << endl;
		}

	}
	outs.close();
}










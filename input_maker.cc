//! \file input_maker.cc \brief implements the functions defined in input_maker.h

#include "input_maker.h"
#include<regex>
using namespace std;

input_maker::input_maker(string file_name_in){
	config_p = "./config/";
	file_name = file_name_in;
}

void input_maker::init(){
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





	string temp_string;
	string temp_name;

	getline(ins,temp_string);
	while(!ins.eof()){
		if(ins.fail()) break; // get out, file stream is done

		if( regex_match(temp_string,re_comment) ){
			//comment line, don't do anything

		} else if( regex_match(temp_string,re_real8) ){//logics for reading in fortran real 8's
			/*
			double real_value;
			cin >> temp_name;
			cin >> temp_string; //get var name
			if(temp_string != "=") { //check for equality sign
				cout << "Error in config parsing, lack of '=' in"
				     << " real 8 declaration line." << endl;
			}
			cin >> real_value;
			param_real8 temp_r8(temp_name,temp_string);
			real8_params.push_back(temp_r8);
			*/

		} else if( regex_match(temp_string,re_i4) ){//logics for reading in fortran integer 4's
			/*
			int temp_int;
			ins >> temp_name; //get variable name
			ins >> temp_string;//check for '=' in the line
			if(temp_string != '='){
				cout << "Error! Missing '=' in int4 assignment in HF_config" << endl;
			}
			ins >> temp_int;

			param_int4(temp_name,temp_int);
			int4_params.push_back(new_i4);
			*/

		} else if( regex_match(temp_string,re_string) ){//logics for reading in fortran strings
						  //and their size
			/*
			string temp_char_array;
			cin >> temp_name;//grab name of parameter
			cin >> temp_string;
			if(temp_string != "="){
				cout << "Error in config parsing, lack of '=' in character string declaration."
				     << endl;
			}
			cin >> temp_char_array;//do I need to chop off the " marks here?
			*/
		}

	}


	ins.close();
}

void input_maker::output(std::ostream& outs){




}










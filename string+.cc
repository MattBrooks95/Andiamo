//! \file string+.cc \brief string+.cc implements the functions declared in string+.h
#include "string+.h"
#include<iostream>
using namespace std;

vector<string> split(string split_me, char delim){
	bool split_test = false;

	vector<string> return_me;
	if(split_me.empty()) return return_me;

	unsigned int index = 0;
	char temp = split_me[index];

	string push_me;
	//control logics in regards to ignoring delimiting characters within
	// a constant string, like label = "this shouldn't be split"	
	bool constant_string = false;

	//go until end of string is reached
	while( index < split_me.size() ){

		//reset new word container
		push_me = "";

		while( temp == delim && index < split_me.size() ){
			index++;
			if(index >= split_me.size()) break;
			temp = split_me[index];
		}

		while( (temp != delim || constant_string) && index < split_me.size() ){
			//turn on literal meaning mode
			if(temp == '\"' && !constant_string){
				if(split_test) cout << "Toggling constant_string true, char: " << temp << endl;
				constant_string = true;

			//turn off literal meaning mode
			} else if(temp == '\"' && constant_string){
				if(index == split_me.size() ){
					if(split_test) cout << "Error in String+!, Failure to end a string literal "
							   << "with \", returned vector may be bad." << endl;
					if(split_test) cout << "index: " << index << " string size: "
							    << split_me.size()<< endl;
					return_me.push_back(push_me);
					return return_me;
				}
				if(split_test) cout << "Toggling constant_string false, char: " << temp << endl;
				constant_string = false;
			}
			push_me.push_back(temp);
			index++;
			if(index >= split_me.size()) break;
			temp = split_me[index];
		}

		if(split_test) cout << "Pushing: |" << push_me << "|" << endl;
		return_me.push_back(push_me);
	}

	return return_me;
}

void trim(std::string& base_string,unsigned int remove_num){
	//cout << "Base string before: " << base_string << endl;
	if(remove_num > base_string.length()){
		return;
	} else {
		
		base_string.erase(base_string.length() - remove_num, remove_num);

	}

	//cout << "Base string after: " << base_string << endl;
}

void handle_i4_array(std::string& string_in,vector<int>& fill_me){

	if(!fill_me.empty()){
		cout << "Error, handle_i4_array given a non-empty array as a parameter." << endl;
	}

	//cout << "handle_i4_array string in:" << string_in << endl;

	//remove the quotation marks
	string split_me = string_in.substr(1,string_in.length()-2);

	//split the line into a vector of strings
	vector<string> string_vec = split(split_me,',');

	//cast the strings into integers, and fill the return vector
	for(unsigned int c = 0; c < string_vec.size();c++){
		fill_me.push_back( atoi( string_vec[c].c_str() ) );
	}

}

void handle_r8_array(string& string_in,vector<double>& fill_me){
	if(!fill_me.empty()){
		cout << "Error, handle_e_array given a non-empty array as a parameter." << endl;
	}
	//remove quotes
	string split_me = string_in.substr(1,string_in.length()-2);

	//split the line into a vector of strings
	vector<string> string_vec = split(split_me,',');

	for(unsigned int c = 0; c < string_vec.size();c++){
		fill_me.push_back( atof(string_vec[c].c_str()) ) ;
	}
}

void strip_char(std::string& string_in,char strip_me){
	//cout << "String passed to strip_me:" << string_in << endl;
		//cout << "String passed to strip_me:" << string_in << endl;
	string temp_string;
	for(unsigned int c = 0; c < string_in.size();c++){
		if( string_in[c] != strip_me){
			temp_string += string_in[c];
		}
	}
	//cout << "String at end of function:" << string_in << endl;
	string_in = temp_string;
	//cout << "String at end of function:" << string_in << endl;
}














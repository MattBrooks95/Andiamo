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
	bool constant_string = false; //control logics in regards to ignoring delimiting characters within
				      // a constant string, like label = "this shouldn't be split"	
	while( index < split_me.size() ){ // go until end of string is reached
		push_me = "";//reset new word container


		while( temp == delim && index < split_me.size() ){
			index++;
			if(index >= split_me.size()) break;
			temp = split_me[index];
		}

		while( (temp != delim || constant_string) && index < split_me.size() ){
			if(temp == '\"' && !constant_string){//turn on literal meaning mode
				if(split_test) cout << "Toggling constant_string true, char: " << temp << endl;
				constant_string = true;
			} else if(temp == '\"' && constant_string){//turn off literal meaning mode
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

	/* old version
	bool split_test = false;

	vector<string> return_me;
	if(split_me.empty()) return return_me;

	unsigned int index = 0;
	char temp = split_me[index];

	string push_me;
	
	while( index < split_me.size() ){ // go until end of string is reached
		push_me = "";//reset new word container

		while(temp == delim && index < split_me.size()){
			index++;
			if(index >= split_me.size()) break;
			temp = split_me[index];
		}

		while(temp != delim && index < split_me.size() ){
			push_me.push_back(temp);
			index++;
			if(index >= split_me.size()) break;
			temp = split_me[index];
		}

		if(split_test) cout << "Pushing: |" << push_me << "|" << endl;
		return_me.push_back(push_me);
	}

	return return_me; */

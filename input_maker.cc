//! \file input_maker.cc \brief implements the functions defined in input_maker.h

#include<regex>
#include<iomanip>

#include "input_maker.h"


#define F << setw(8) <<
#define I << setw(5) <<
using namespace std;

input_maker::input_maker(string output_file_name_in,string config_file_name_in){
	config_p = "./config/";
	output_p = "./output/";
	output_file_name = output_file_name_in;
	file_name = config_file_name_in;

	//init();//parse config file
}
input_maker::~input_maker(){
	output();
}

void input_maker::init(){
	bool init_test = true;
	if(init_test) cout << "########################### INPUT_MAKER INIT ################################" << endl;

	ifstream ins;
	ins.open( (config_p+file_name).c_str() );
	if(ins.fail()){
		cout << "Error in input_maker::init(), couldn't find the input file!" << endl;
		return;
	}

	//set up regex matches
	regex re_comment("\\s*?#.*");
	regex re_i4("\\s*?I4\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*");
	//regex re_i4_array("\\s*?I4\\(\\s*?[0-9]+?\\s*?\\)\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*");
	regex re_i4_array("\\s*?I4\\(\\s*?[0-9]+?\\s*?\\)\\s*?[A-Za-z0-9]+?\\s*?=\\s*?\"(\\s*?[0-9]*?\\s*?,?)+?\"");
	regex re_string("\\s*?C\\*\\s*?[A-Za-z]+?\\|[0-9]+?\\|\\s*?=\\s*?\".+?\"");
	regex re_real8("\\s*?R8\\s+?[A-Za-z0-9]+?\\s+?=\\s+?[0-9]*?\\.[0-9]*?");

	regex string_array_size_pattern("\\|\\d+?\\|");
	regex int_array_size_pattern("\\([0-9]+?\\)");



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
					cout << c << ": " << tokens[c] << endl;
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
				//cout << "CHARACTER ARRAY SIZE = " << size << endl;
				//cout << "CHARACTER ARRAY NAME = " << tokens[1].substr(0,tokens[1].size()-
				//temp_string.size()) << endl;
				//name can be found by token label|some-number| minus the |some-number| part
				string name = tokens[1].substr(0,tokens[1].size()- temp_string.size());

				//make sure bookkeeping vector knows what is going on
				names_in_order.push_back(name);

				//create new ftran struct
				//cout << "CHARACTER ARRAY VALUE = " << tokens[tokens.size()-1] << endl; 
				param_string push_me(name,tokens[tokens.size()-1],size);
				//save this new param value in its vector
				string_params.push_back(push_me);	
			}
			
		} else if( regex_match(temp_string,re_i4_array) ){
			if(init_test) cout << "Is an array of integers!" << endl;
			
			if(init_test) cout << "This is that line split along spaces: " << endl;
			vector<string> tokens = split(temp_string,' ');
			
			if(init_test){
				for(unsigned int c = 0; c < tokens.size() ;c++){
					cout << tokens[c] << endl;
				}
			}
			smatch size_match;//store numerical result of grabbing the array's size

			//have regex search out the integer size value
			regex_search(tokens[0],size_match,int_array_size_pattern);

			//if a match was found
			if( size_match.ready() ){
				
				//create a string that contains just the size of the array
				string temp_size_string = size_match[0].str().substr(1,size_match[0].str().size()-2);
				int array_size = stoi(temp_size_string);
				if(init_test) cout << temp_size_string << endl;
				
				//create the int4 array that will be pushed into input_maker's containing vector
				//tokens[1] should be the string name given, array_size was determined by the size_match regex
				//should not be satisfied from the start, although it would appear that it is OK for NENT
				//to not have 7 values depending on some other variable.....
				param_int4_array i4_array_push_me(tokens[1],array_size,false);
				
				handle_i4_array(tokens[3],i4_array_push_me.values);
				if(init_test){
					cout << "VECTOR OF INTS AS FOLLOWS: " << endl;
					for(unsigned int c = 0; c < i4_array_push_me.values.size();c++){
						cout << i4_array_push_me.values[c] << " ";
					}
				}
				if(init_test) cout << endl;
				int4_array_params.insert(std::pair<string,param_int4_array>(i4_array_push_me.name,i4_array_push_me));

			} else {
				//if there was no match for some reason, print an error message
				cout << "Error! Could not determine array size from int4 array"
				     << " declaration line." << endl;
			}


		} else {
			cout << "Error! Line type wasn't determined." << endl;
		}

		getline(ins,temp_string);
	}

	if(init_test) cout << "##################### END INPUT MAKER INIT() ###############################" << endl;
	ins.close();
}

void input_maker::output(){
	//note that this will not yet be properly formatted for HF input, mostly here for testing field input
	//and this class's output logic

	ofstream outs;
	outs.open( (output_p+output_file_name).c_str(),std::fstream::trunc );
	if(outs.fail()) {
		cout << "Error! Can not open/create output file: |" << output_p+output_file_name << "|" << endl;
		cout << "Attemping output directory creation." << endl;
		system("mkdir output");
		outs.open( (output_p+output_file_name).c_str(),std::fstream::trunc);
		if(outs.fail()){
			cout << "Opening output stream failed again." << endl;
			return;
		} 

	}

	//SET UP LINE 1##########################################################################################
	do_line1(string_params,outs);
	//#########################################################################################################

	//SET UP LINE 2############################################################################################
	do_line2(real8_params, int4_params, outs);
	//#########################################################################################################


	//close the output file stream
	outs.close();
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

void do_line1(const vector<param_string>& string_params,ofstream& outs){

	if(string_params[0].name != "label" || string_params.size() == 0){
		cout << "Error in input_maker, 'label' parameter not found in vector string_params "
		     << "as it should be." << endl;

		if( string_params.size() != 0 ){
			cout << "Furthermore, the string_params vector is empty." << endl;
			cout << "exiting" << endl;
			return;
		}

	} else{

		output_string(outs,string_params[0].size,string_params[0].value);
		outs << "\n";
	}

}

void do_line2(const vector<param_real8>& real8_params,const vector<param_int4> int4_params, ofstream& outs){

	//note, setw(something) needs to be called before every item is printed
	//this is really annoying, so I have a macro up top where F = "<< setw(8) << " for
	//printing the real 8 values

	//set up decimal place precision
	outs << fixed << setprecision(1);
	// ELAB A Z FNRME1 FNRMM1
	outs F real8_params[0].value F real8_params[1].value F real8_params[2].value F real8_params[3].value
	     F real8_params[4].value;

	//I = "<< setw(5) <<" macro up top
	//IENCH, ICM, NZ3, TCPR
	outs I int4_params[0].value I int4_params[1].value I int4_params[2].value I int4_params[3].value;

	//make field width 8 again, FNRME2
	outs F real8_params[5].value;

	//put width back to 5 for NGF
	outs I int4_params[4].value;
}


//#################################################################################################################






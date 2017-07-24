//! \file input_maker.cc \brief implements the functions defined in input_maker.h

#include<regex>
#include<iomanip>

#include "input_maker.h"


#define F << setw(8) <<
#define F10 << setw(10) <<
#define F5 << setw(5) <<
#define I << setw(5) <<
#define I10 << setw(10) <<
#define E << setw(10) <<
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

void input_maker::check_map(std::ostream& outs){
	cout << "########### INPUT MAKER INT4 PARAMS MAP ##############################" << endl;
	for(map<string,param_int4>::iterator int_it = int4_params.begin();
	    int_it != int4_params.end();
	    int_it++){
		outs << int_it->second << "   ";
	}
	outs << "\n######### INPUT MAKER R8 PARAMS MAP #################################\n";
	for(map<string,param_real8>::iterator r8_it = real8_params.begin();
	    r8_it != real8_params.end();
	    r8_it++){
		outs << r8_it->second << ":";
	}
	outs << "\n######### INPUT MAKER STRING PARAMS MAP #############################\n";
	for(map<string,param_string>::iterator string_it = string_params.begin();
	    string_it != string_params.end();
	    string_it++){
		outs << string_it->first << ":" << string_it->second.value << "   ";
	}
	outs << "\n######### INPUT MAKER INT4 ARRAY MAP ###############################\n";
	for(map<string,param_int4_array>::iterator int4_array_it = int4_array_params.begin();
	    int4_array_it != int4_array_params.begin();
	    int4_array_it++){
		outs << int4_array_it->first << ":" << int4_array_it->second.get_string() << "   ";
	}
	outs << "\n######### INPUT MAKER E_ARRAY MAP ##################################\n";
	for(map<string,param_e_array>::iterator e_array_it = e_params.begin();
	    e_array_it != e_params.end();
	    e_array_it++){
		outs << e_array_it->first << ":" << e_array_it->second.get_string() << "   " << endl;
	}

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
	regex re_i4("\\s*?I4\\s+?[A-Za-z0-9_]+?\\s+?=\\s+?-?[0-9]*\\s*");
	regex re_i4_array("\\s*?I4\\(\\s*?[0-9]+?\\s*?\\)\\s*?[A-Za-z0-9_]+?\\s*?=\\s*?\"(\\s*?-?[0-9]*?\\s*?,?)+?\"\\s*");
	regex re_string("\\s*?C\\*\\s*?[A-Za-z_]+?\\|[0-9]+?\\|\\s*?=\\s*?\".+?\"\\s*");
	regex re_real8("\\s*?R8\\s+?[A-Za-z0-9_]+?\\s+?=\\s+?-?[0-9]*?\\.[0-9]*?\\s*");

	regex string_array_size_pattern("\\|\\d+?\\|");
	regex int_array_size_pattern("\\([0-9]+?\\)");

	regex e_array("\\s*?E\\(\\s*[0-9]+?\\s*?\\)\\s*?[A-Za-z0-9_]+?\\s*?=\\s*?\"(\\s*?[0-9]+?\\.[0-9]+?,?)+?\"");


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
				//names_in_order.push_back(var_name);//update bookkeeping vector
				real8_params.emplace(var_name,new_param);
				//real8_params.push_back(new_param);//push into real 8 vector 
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
				int4_params.emplace(var_name,new_param);
				//names_in_order.push_back(var_name);//update bookkeeping vector
				//int4_params.push_back(new_param);//push into the i4 vector
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
				//names_in_order.push_back(name);

				//create new ftran struct
				//cout << "CHARACTER ARRAY VALUE = " << tokens[tokens.size()-1] << endl; 
				param_string push_me(name,tokens[tokens.size()-1],size);
				//save this new param value in its vector
				//string_params.push_back(push_me);	
				string_params.emplace(name,push_me);
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


		} else if( regex_match(temp_string,e_array) ){
			cout << "LINE:" << temp_string << "is an E array!" << endl; 
			vector<string> tokens = split(temp_string,' ');//split across spaces
			/*for(unsigned int c = 0; c < tokens.size(); c++){
				cout << tokens[c] << endl;

			}*/
			string name = tokens[1];//token 1 should be just the variable name 
			int array_size = 0;//fill this value later
			vector<double> values; //fill this with what's in the comma separated list

			//rip array size out of type declaration part E(size) "E(8) some array name = " " "
			smatch size_match;//contain result of regex search for size
			regex_search(tokens[0],size_match,int_array_size_pattern);
			if(size_match.ready()){
				string temp_size_string = size_match[0].str().substr(1,size_match[0].str().size()-2);
				array_size = stoi(temp_size_string);
			} else {

				cout << "Error! Could not determine array size of E array (TIN?)"
				     << "declaration line." << endl;
			}
			param_e_array e_array_push_me(name,array_size,false);//create object to be shoved into the map for E arrays
			handle_e_array(tokens[3],e_array_push_me.values);
			/*cout << "Vector of Es? Doubles? As follows:" << endl;
			for(unsigned int c = 0; c < e_array_push_me.values.size();c++){
				cout << e_array_push_me.values[c] << endl;
			}*/

			e_params.insert(std::pair<string,param_e_array>(name,e_array_push_me));//shove object into the map for E arrays
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

	//note that in these functions, the variables that are being printed are found in the order they are
	//found in the HF_config/config.txt file, based off of the input manual that I was given
	//so if a new line is inserted, care to adjust the indices. Adding new lines to the end shouldn't
	//change the lines above them, though


	//SET UP LINE 1##########################################################################################
	do_line1(string_params,outs);
	//#######################################################################################################

	//SET UP LINE 2##########################################################################################
	do_line2(real8_params, int4_params, outs);
	//#######################################################################################################

	//SET UP LINE 3##########################################################################################
	do_TC_coefficients(real8_params,int4_array_params,TC_input_file_name,outs);
	//#######################################################################################################

	//IF IENCH = 7 ##########################################################################################
	bool do_4_5 = false;
	/*for(unsigned int c = 0; c < int4_params.size();c++){
		if(int4_params[c].name == "IENCH" && int4_params[c].value == 7){
			do_4_5 = true;
			break;
		}
	}*/

	if(int4_params.at("IENCH").value == 7){
		do_4_5 = true;
	}


	/*if(do_4_5){
		cout << "IENCH VALUE: " << 7 << endl;	
		cout << "Lines 4 and 5 will be done." << endl;
	} else {
		cout << "IENCH VALUE: not 7" << endl;
		cout << "Lines 4 and 5 will not be done." << endl;

	}*/
	//only do the following lines if IENCH == 7, per the input manual
	if(do_4_5){
		//SET UP LINE 4##################################################################################
		do_line4(real8_params,int4_params,outs);

		do_line4A(real8_params,int4_params,outs);

		do_line4B(e_params,outs);
		//###############################################################################################

		//SET UP LINE 5##################################################################################
		//do_line

		//###############################################################################################

	}
	//####################### IENCH = 7 LINES ###############################################################
	

	//do line 6
	do_line6(int4_params,outs);

	//do line 7
	do_line7(real8_params,outs);

	//if the conditions from the input manual are met
	if( int4_params.at("ICNTRL4").value != 0 ){
		//do line 8
		do_line8(int4_params,outs);
	}//elsewise, don't do line 8


	outs.flush();//push changes to file, if this is not here C++ will wait to do the writing until
		     //the program is terminated
	//close the output file stream
	outs.close();
	output_was_made = true;//make this boolean true, so that during the closing process we know that we don't
			       //need to remind the user to generate an input file first
	check_map(cout);
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

void do_line1(const map<string,param_string>& string_params,ofstream& outs){

	//if(string_params[0].name != "label" || string_params.size() == 0){
	try{
		output_string(outs,string_params.at("label").size,string_params.at("label").value);
		outs << "\n";

	} catch( out_of_range& not_found ){

		cout << "Error in input_maker, 'label' parameter not found in vector string_params "
		     << "as it should be." << endl;


	}


}

void do_line2(const map<string,param_real8>& real8_params,const map<string,param_int4>& int4_params, ofstream& outs){

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
		cout << "Error! File:" << "./TC_files/"+TC_input_file_name << " could not be found." << endl;
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
	cout << "#################### PRINTING TC ##################################" << endl;
	cout << right;
	outs << right;
	for(unsigned int c = 0; c < lines_in.size();c++){
		cout << lines_in[c];
		outs << lines_in[c];

		if(c < lines_in.size() - 1){
			cout << endl;
			outs << endl;
		}

	}
	cout << "#################### DONE PRINTING TC #############################" << endl;



	//#######################################################################

	//here be the case where we have to use ESIS to do it ###################



	//#######################################################################
	

	ins.close();

}

void do_line4(const map<string,param_real8>& real8_params,const map<string,param_int4>& int4_params,ofstream& outs){
  try{
	outs << right;
	//     FJTAR               FCMJMAX          FRESIDEMAX         ITARPR               NG             
	outs F10 real8_params.at("FJTAR").value F10 real8_params.at("FCMJMAX").value F10 real8_params.at("FRESIDMAX").value
	     I int4_params.at("ITARPR").value I int4_params.at("NG").value << endl;
  } catch (out_of_range& not_found){
	cout << "Error in do_line4: parameter not found in the map!" << endl;
  }


}

void do_line4A(const map<string,param_real8>& real8_params,const map<string,param_int4>& int4_params,ofstream& outs){
  try{
	outs << fixed << setprecision(2);
	outs F5 real8_params.at("APAR").value F5 real8_params.at("ZPAR").value F5 real8_params.at("QIN").value 
	     F5 real8_params.at("FJPAR").value F5 real8_params.at("FPRPAR").value;
	outs I int4_params.at("NLIN").value << endl;
  } catch (out_of_range& not_found){
	cout << "Error in do_line4A: parameter not found in the map!" << endl;
  }
}
void do_line4B(const map<string, param_e_array>& e_params,std::ofstream& outs){
  try{
	outs << right;//set orientation
	outs << setprecision(3);//set # of decimal places
	for(unsigned int c = 0; c < e_params.at("TIN").values.size();c++){
		outs E e_params.at("TIN").values[c];
		if(c == e_params.at("TIN").values.size()-1){
		outs << endl;
		}

	}
  } catch (out_of_range& not_found){
	cout << "Error! Parameter in do_line4B not found in the map!" << endl;
  }
}

void do_line6(const map<string,param_int4>& int4_params,ofstream& outs){
	outs << right;
  try{
	outs I int4_params.at("ICNTRL1").value I int4_params.at("ICNTRL2").value I int4_params.at("ICNTRL3").value
	     I int4_params.at("ICNTRL4").value I int4_params.at("ICNTRL5").value I int4_params.at("ICNTRL6").value
	     I int4_params.at("ICNTRL7").value I int4_params.at("ICNTRL8").value
	     I int4_params.at("ICNTRL9").value I int4_params.at("ICNTRL10").value << endl;
  } catch (out_of_range& not_found){
	cout << "Error! Parameter in do_line6 not found in the map!" << endl;
  }
}



void do_line7(const map<string,param_real8>& real8_params,ofstream& outs){
  try{
	outs << right;	//set the justification
	outs << setprecision(2);//set precision for 2 points past the decimal
	outs F5 real8_params.at("FMU").value F5 real8_params.at("FCON_").value << endl;
  } catch (out_of_range& not_found){
	cout <<"Error! Parameter in do_line7 not found in the map!" << endl;
  }

}

void do_line8(const map<string,param_int4>& int4_params,ofstream& outs){
  try{
	outs << right;	//set the justification

	outs I int4_params.at("ICH4").value I int4_params.at("NCH4").value << endl;
  } catch(out_of_range& not_found){
	cout << "Error! Parameter in do_line8 not found in the map!" << endl;
  }
}

//#################################################################################################################





